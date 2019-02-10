#include "gtest/gtest.h"
#include "glog/logging.h"

#include "grpcpp/grpcpp.h"

#include "helloworld.grpc.pb.h"

#include <boost/date_time.hpp>

#include <thread>
#include <iostream>
#include <mutex>
#include <map>

using namespace std::literals;

std::mutex g_mutex;

class	SyncServerImp : public helloworld::Greeter::Service
{
public:

    virtual ::grpc::Status SayHello(::grpc::ServerContext* context, const ::helloworld::HelloRequest* request, ::helloworld::HelloReply* response) override
    {
        auto curTime = boost::posix_time::microsec_clock::local_time();
        auto timeStr = boost::posix_time::to_iso_string(curTime);
        auto message = request->name() + " " + timeStr;
        response->set_message(message);
        {
            std::unique_lock<std::mutex> lock(g_mutex);
            LOG(INFO) << std::this_thread::get_id() << ">>>>Server:Recv[" << message << "]" << std::endl;
        }
        return grpc::Status::OK;
    }

};

void	RunSyncServer(grpc::Server** serv)
{
    SyncServerImp service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort("127.0.0.1:54545", grpc::InsecureServerCredentials());

    builder.RegisterService(&service);

    {
        std::unique_lock<std::mutex> lock(g_mutex);
        LOG(INFO) << ">>>>Server:Start" << std::endl;
    }
    auto server = builder.BuildAndStart();
	*serv = server.get();

    server->Wait();
    {
        std::unique_lock<std::mutex> lock(g_mutex);
        LOG(INFO) << ">>>>Server:Stop" << std::endl;
    }
}

void	RunSyncClient()
{
    auto channel = grpc::CreateChannel("localhost:54545", grpc::InsecureChannelCredentials());
    auto stub = helloworld::Greeter::NewStub(channel);

    helloworld::HelloRequest req;
    req.set_name("sync");

    grpc::ClientContext context;

    helloworld::HelloReply reply;

    {
        std::unique_lock<std::mutex> lock(g_mutex);
        LOG(INFO) << ">>SyncClint:PreSend" << std::endl;
    }
    auto status = stub->SayHello(&context, req, &reply);
    {
        std::unique_lock<std::mutex> lock(g_mutex);
        LOG(INFO) << ">>SyncClint:PostSend" << std::endl;
    }
}

class   AsyncClientContext
{
public:

    class   Task
    {
    public:

        helloworld::HelloReply  Reply;
        grpc::ClientContext     Context;
        grpc::Status            Status;
        std::unique_ptr<grpc::ClientAsyncResponseReader<helloworld::HelloReply>> ResponseHeader;
    };

private:

    std::atomic<bool>               Run_{ false };
    std::shared_ptr<grpc::Channel>  ServerChannel_;
    std::unique_ptr<helloworld::Greeter::Stub>  ServerStub_;

    std::vector<std::thread>    WaitThreadList_;
    std::map<Task*, std::unique_ptr<Task>> TaskList_;
    std::mutex                      TaskMutex_;
    std::unique_ptr<grpc::CompletionQueue>           CQ_;

public:

    static AsyncClientContext&  GetInstance()
    {
        static AsyncClientContext sIns;
        return sIns;
    }

public:

    void    Init()
    {
        ServerChannel_ = grpc::CreateChannel("localhost:54545", grpc::InsecureChannelCredentials());
        ServerStub_ = helloworld::Greeter::NewStub(ServerChannel_);
    }

    void    Run()
    {
        if ( Run_ )
        {
            return;
        }

        Run_ = true;
        CQ_ = std::make_unique<grpc::CompletionQueue>();

        for ( auto i = 0; i < 4; ++i )
        {
            WaitThreadList_.emplace_back([this]()
            {
                void* tag;
                auto ok = false;

                while ( Run_ && CQ_->Next(&tag, &ok) )
                {
                    auto ptask = static_cast<Task*>(tag);

                    if ( ok )
                    {
                        auto successTag = ptask->Status.ok() ? "[OK]" : ptask->Status.error_message();
                        std::unique_lock<std::mutex> lock(g_mutex);
                        LOG(INFO) << std::this_thread::get_id() << " Client:" << successTag << " " << ptask->Reply.message() << std::endl;
                    }

                    {
                        std::unique_lock<std::mutex> lock(TaskMutex_);
                        TaskList_.erase(ptask);
                    }
                }
            });
        }
    }

    void    Stop()
    {
        CQ_->Shutdown();
        for ( auto& cur : WaitThreadList_ )
        {
            cur.join();
        }
        WaitThreadList_.clear();
        CQ_.reset();
    }

    void Send(const helloworld::HelloRequest& req)
    {
        auto task = std::make_unique<Task>();
        auto pTask = task.get();
        {
            std::unique_lock<std::mutex> lock(TaskMutex_);
            TaskList_.emplace(pTask, std::move(task));
        }

        pTask->ResponseHeader = ServerStub_->AsyncSayHello(&pTask->Context, req, CQ_.get());
        pTask->ResponseHeader->Finish(&pTask->Reply, &pTask->Status, pTask);
    }
};

void RunAsyncClent()
{
    AsyncClientContext::GetInstance().Init();
    AsyncClientContext::GetInstance().Run();

    for ( auto i = 0; i < 100; ++i )
    {
        helloworld::HelloRequest req;
        req.set_name(std::to_string(i));
        AsyncClientContext::GetInstance().Send(req);
        //std::this_thread::sleep_for(50ms);
    }
}

class   AsyncServer
{
    helloworld::Greeter::AsyncService Service_;
    std::unique_ptr<grpc::ServerCompletionQueue>  CQ_;
    std::unique_ptr<grpc::Server>   Server_;
    std::atomic<bool>               Run_{ false };
    std::vector<std::thread>        ThreadList_;

    class   Task
    {
    public:

        enum EState
        {
            ES_Accept,
            ES_Request,
            ES_Reply
        };
        EState                      Status{ ES_Accept };
        grpc::ServerContext         Context;
        helloworld::HelloRequest    Request;
        helloworld::HelloReply      Reply;
        std::unique_ptr<grpc::ServerAsyncResponseWriter<helloworld::HelloReply>> Responder;
    };

    std::map<Task*, std::unique_ptr<Task>> TaskList_;
    std::mutex  TaskMutex_;

public:

    static AsyncServer& GetInstance()
    {
        static AsyncServer sIns;
        return sIns;
    }

public:

    void    Join()
    {
        if ( Run_ )
        {
            Server_->Wait();
        }
    }

    void    Init()
    {
        if ( Run_ )
        {
            return;
        }

        grpc::ServerBuilder builder;
        builder.AddListeningPort("127.0.0.1:54545", grpc::InsecureServerCredentials());
        builder.RegisterService(&Service_);
        CQ_ = builder.AddCompletionQueue();
        Server_ = builder.BuildAndStart();

        {
            std::unique_lock<std::mutex> lock(g_mutex);
            LOG(INFO) << std::this_thread::get_id() << ">>>>Server:Init Finish" << std::endl;
        }
    }

    Task*   CreateTask()
    {
        auto task = std::make_unique<Task>();
        auto pTask = task.get();
        {
            std::unique_lock<std::mutex> lock(TaskMutex_);
            TaskList_.emplace(pTask, std::move(task));
        }

        return pTask;
    }

    void    Process(Task* const task)
    {
        switch ( task->Status )
        {
        case Task::ES_Accept:
        {
            task->Responder = std::make_unique<grpc::ServerAsyncResponseWriter<helloworld::HelloReply>>(&task->Context);

            task->Status = Task::ES_Request;
            Service_.RequestSayHello(&task->Context, &task->Request, task->Responder.get(), CQ_.get(), CQ_.get(), task);

            {
                std::unique_lock<std::mutex> lock(g_mutex);
                LOG(INFO) << std::this_thread::get_id() << ">>>>Server:ES_Accept" << std::endl;
            }
        }
        break;
        case Task::ES_Request:
        {
            auto curTime = boost::posix_time::microsec_clock::local_time();
            auto timeStr = boost::posix_time::to_iso_string(curTime);
            auto message = task->Request.name() + " " + timeStr;
            task->Reply.set_message(message);
            {
                std::unique_lock<std::mutex> lock(g_mutex);
                LOG(INFO) << std::this_thread::get_id() << ">>>>Server:ES_Request[" << message << "]" << std::endl;
            }

            task->Status = Task::ES_Reply;
            task->Responder->Finish(task->Reply, grpc::Status::OK, task);

            auto newTask = CreateTask();
            Process(newTask);
        }
        break;
        case Task::ES_Reply:
        {
            {
                std::unique_lock<std::mutex> lock(g_mutex);
                LOG(INFO) << std::this_thread::get_id() << ">>>>Server:ES_Reply[" << task->Reply.message() << "]" << std::endl;
            }

            std::unique_lock<std::mutex> lock(TaskMutex_);
            TaskList_.erase(task);
        }
        break;
        default:
            break;
        }
    }

    void    Run()
    {
        if ( Run_ )
        {
            return;
        }

        Run_ = true;
        for ( auto i = 0; i < 4; ++i )
        {
            ThreadList_.emplace_back([this]()
            {
                void* tag;
                auto ok = false;

                auto task = CreateTask();
                Process(task);

                while ( Run_ && CQ_->Next(&tag, &ok) )
                {
                    if ( ok )
                    {

                    }

                    auto pTask = static_cast<Task*>(tag);
                    Process(pTask);
                }
            });
        }
    }

    void    Stop()
    {
        if ( !Run_ )
        {
            return;
        }

        Run_ = false;
        Server_->Shutdown();
        CQ_->Shutdown();
        for ( auto& cur : ThreadList_ )
        {
            cur.join();
        }
        ThreadList_.clear();

        {
            std::unique_lock<std::mutex> lock(g_mutex);
            LOG(INFO) << std::this_thread::get_id() << ">>>>Server:Stop" << std::endl;
        }
    }
};


TEST(grpc, case)
{
	grpc::Server* serv {};
	std::thread serverTD([&serv]()
	{
		RunSyncServer(&serv);
	});

    RunSyncClient();

	serv->Shutdown();
	serverTD.join();
}