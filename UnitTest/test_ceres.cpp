#include "gtest/gtest.h"
#include "glog/logging.h"

#include "ceres/ceres.h"

#include <vector>

struct Coincident
{
    Coincident(int p0id, int p1id)
    {
        p0id_ = p0id;
        p1id_ = p1id;
    }

    int p0id_{}, p1id_{};

    template <typename T> bool operator()(T const* const* parameters, T* residual) const
    {
        auto xList = parameters[0];
        auto yList = parameters[1];

        residual[0] = xList[p0id_] - xList[p1id_];
        residual[1] = yList[p0id_] - yList[p1id_];

        return true;
    }
};

struct Perpendicular
{
    Perpendicular(int l0p0id, int l0p1id, int l1p0id, int l1p1id)
    {
        l0p0id_ = l0p0id;
        l0p1id_ = l0p1id;
        l1p0id_ = l1p0id;
        l1p1id_ = l1p1id;
    }

    int l0p0id_{}, l0p1id_{};
    int l1p0id_{}, l1p1id_{};

    template <typename T> bool operator()(const T* const xList, const T* const yList, T* residual) const
    {
        auto v1x = xList[l0p1id_] - xList[l0p0id_];
        auto v1y = yList[l0p1id_] - yList[l0p0id_];
        auto v2x = xList[l1p1id_] - xList[l1p0id_];
        auto v2y = yList[l1p1id_] - yList[l1p0id_];

        residual[0] = v1x * v2x + v1y * v2y;

        return true;
    }
};

struct DistanceP2P
{
    DistanceP2P(int p0id, int p1id, double length)
    {
        p0id_ = p0id;
        p1id_ = p1id;
        length_ = length;
    }

    int p0id_{}, p1id_{};
    double length_{};

    template <typename T> bool operator()(const T* const xList, const T* const yList, T* residual) const
    {
        auto xDiff = xList[p1id_] - xList[p0id_];
        auto yDiff = yList[p1id_] - yList[p0id_];

        residual[0] = T(length_ * length_) - xDiff * xDiff - yDiff * yDiff;

        return true;
    }
};

struct SetConstant
{
    SetConstant(int pid, double val)
    {
        pid_ = pid;
        value_ = val;
    }

    int pid_{};
    double value_{};

    template <typename T> bool operator()(const T* const pList, T* residual) const
    {
        residual[0] = T(value_) - pList[pid_];
        return true;
    }
};


TEST(ceres, solve_rect)
{
    double x0{ 0.0 }, y0{ 1.f };
    double x1{ 12.0 }, y1{ -1.f };
    double x2{ 10.0 }, y2{ 2.f };
    double x3{ 12.0 }, y3{ 30.f };
    double x4{ 11.0 }, y4{ 28.f };
    double x5{ -1.0 }, y5{ 31.f };
    double x6{ 3.0 }, y6{ 26.f };
    double x7{ -2.0 }, y7{ -2.f };

    std::vector<double> xList{ x0, x1, x2, x3, x4, x5, x6, x7 }, yList{ y0, y1, y2, y3, y4, y5, y6, y7 };

    ceres::Problem problem;

    auto createFunc = [&problem, &xList, &yList](int p0, int p1)
    {
        auto constraint = new Coincident(p0, p1);
        auto costFunc = new ceres::DynamicAutoDiffCostFunction<Coincident>(constraint);
        costFunc->SetNumResiduals(2);
        costFunc->AddParameterBlock(xList.size());
        costFunc->AddParameterBlock(yList.size());
        problem.AddResidualBlock(costFunc, nullptr, xList.data(), yList.data());
    };

    createFunc(1, 2);
    createFunc(3, 4);
    createFunc(5, 6);
    createFunc(7, 0);
    //problem.AddResidualBlock(new ceres::AutoDiffCostFunction<Coincident, 2, 8, 8>(new Coincident(1, 2)), NULL, xList.data(), yList.data());
    //problem.AddResidualBlock(new ceres::AutoDiffCostFunction<Coincident, 2, 8, 8>(new Coincident(3, 4)), NULL, xList.data(), yList.data());
    //problem.AddResidualBlock(new ceres::AutoDiffCostFunction<Coincident, 2, 8, 8>(new Coincident(5, 6)), NULL, xList.data(), yList.data());
    //problem.AddResidualBlock(new ceres::AutoDiffCostFunction<Coincident, 2, 8, 8>(new Coincident(7, 0)), NULL, xList.data(), yList.data());

    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<Perpendicular, 1, 8, 8>(new Perpendicular(1, 0, 2, 3)), NULL, xList.data(), yList.data());
    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<Perpendicular, 1, 8, 8>(new Perpendicular(3, 2, 4, 5)), NULL, xList.data(), yList.data());
    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<Perpendicular, 1, 8, 8>(new Perpendicular(5, 4, 6, 7)), NULL, xList.data(), yList.data());

    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<DistanceP2P, 1, 8, 8>(new DistanceP2P(0, 1, 10)), NULL, xList.data(), yList.data());
    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<DistanceP2P, 1, 8, 8>(new DistanceP2P(2, 3, 30)), NULL, xList.data(), yList.data());

    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<SetConstant, 1, 8>(new SetConstant(0, 0)), NULL, xList.data());
    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<SetConstant, 1, 8>(new SetConstant(0, 0)), NULL, yList.data());

    ceres::Solver::Options options;
    options.max_num_iterations = 1000;
    //options.num_threads = 4;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;


    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);
    x0 = xList[0]; y0 = yList[0];
    x1 = xList[1]; y1 = yList[1];
    x2 = xList[2]; y2 = yList[2];
    x3 = xList[3]; y3 = yList[3];
    x4 = xList[4]; y4 = yList[4];
    x5 = xList[5]; y5 = yList[5];
    x6 = xList[6]; y6 = yList[6];
    x7 = xList[7]; y7 = yList[7];
    LOG(INFO) << summary.FullReport() << "\n";


    auto tv1x = x0 - x1;
    auto tv1y = y0 - y1;
    auto tv2x = x3 - x2;
    auto tv2y = y3 - y2;
    auto t = tv1x * tv2x + tv1y * tv2y;
}