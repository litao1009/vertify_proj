#include <gtest/gtest.h>
#include <glog/logging.h>

#include <hiredis/hiredis.h>

#define CHECK_REDIS_ERR                                 \
if (rc->err)                                            \
{                                                       \
    LOG(ERROR) << "Connection error: " << rc->errstr;   \
    freeReplyObject(reply);                             \
    redisFree(rc);                                      \
    FAIL();                                             \
}

TEST(hiredis, case)
{
    //auto rc = redisConnectWithTimeout("127.0.0.1", 6379, timeval{ 10,0 });
    auto rc = redisConnectWithTimeout("172.17.0.2", 6379, timeval{ 10,0 });

    if (!rc)
    {
        FAIL() << "Connection error: can't allocate redis context";
    }

    if (rc->err)
    {
        FAIL() << "Connection error: " << rc->errstr;
        redisFree(rc);
    };

    /* PING server */
    auto reply = (redisReply*)redisCommand(rc, "PING");
    CHECK_REDIS_ERR;
    LOG(INFO) << "PING: " << reply->str;
    freeReplyObject(reply);

    /* Set a key */
    reply = (redisReply*)redisCommand(rc, "SET %s %s", "foo", "hello world");
    CHECK_REDIS_ERR;
    LOG(INFO) << "SET: " << reply->str;
    freeReplyObject(reply);

    /* Set a key using binary safe API */
    reply = (redisReply*)redisCommand(rc, "SET %b %b", "bar", (size_t)3, "hello", (size_t)5);
    CHECK_REDIS_ERR;
    LOG(INFO) << "SET (binary API): " << reply->str;
    freeReplyObject(reply);

    /* Try a GET and two INCR */
    reply = (redisReply*)redisCommand(rc, "GET foo");
    CHECK_REDIS_ERR;
    LOG(INFO) << "GET foo: " << reply->str;
    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(rc, "INCR counter");
    CHECK_REDIS_ERR;
    LOG(INFO) << "INCR counter: " << reply->integer;
    freeReplyObject(reply);
    /* again ... */
    reply = (redisReply*)redisCommand(rc, "INCR counter");
    CHECK_REDIS_ERR;
    LOG(INFO) << "INCR counter: " << reply->integer;
    freeReplyObject(reply);

    /* Create a list of numbers, from 0 to 9 */
    reply = (redisReply*)redisCommand(rc, "DEL mylist");
    freeReplyObject(reply);

    for (auto j = 0; j < 10; j++)
    {
        char buf[64];

        snprintf(buf, 64, "%u", j);
        reply = (redisReply*)redisCommand(rc, "LPUSH mylist element-%s", buf);
        freeReplyObject(reply);
    }

    /* Let's check what we have inside the list */
    reply = (redisReply*)redisCommand(rc, "LRANGE mylist 0 -1");
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (auto j = 0; j < reply->elements; j++)
        {
            LOG(INFO) << j << " " << reply->element[j]->str;
        }
    }
    freeReplyObject(reply);

    /* Disconnects and frees the context */
    redisFree(rc);
}