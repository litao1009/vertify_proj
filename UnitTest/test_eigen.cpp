#include "gtest/gtest.h"
#include "glog/logging.h"

#include "Eigen/Eigen"

TEST(eigen, LinearEquation)
{
    Eigen::MatrixXd A = Eigen::MatrixXd::Random(3, 3);
    LOG(INFO) << "A:" << A;

    Eigen::VectorXd B = Eigen::VectorXd::Random(3);
    LOG(INFO) << "B:" << B;

    Eigen::VectorXd x = A.colPivHouseholderQr().solve(B);
    LOG(INFO) << "x:" << x;

    auto testB = A * x;
    LOG(INFO) << "testB:" << testB;
}

TEST(eigen, SVD)
{
    Eigen::MatrixXd A = Eigen::MatrixXd::Random(3, 3);
    LOG(INFO) << "A:" << A;

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
    auto v = svd.matrixV();
    LOG(INFO) << "v:" << v;

    auto u = svd.matrixU();
    LOG(INFO) << "u:" << u;

    auto a = svd.singularValues();
    LOG(INFO) << "a:" << a;

    Eigen::MatrixXd aD = a.asDiagonal();
    auto r = aD.rows();
    auto c = aD.cols();
    LOG(INFO) << "aD:" << aD;

    auto pA = u * aD * v.transpose();
    LOG(INFO) << "pA:" << pA;
}