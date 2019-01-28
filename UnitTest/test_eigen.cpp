#include "gtest/gtest.h"

#include "Eigen/Eigen"

TEST(eigen, LinearEquation)
{
    Eigen::MatrixXd A = Eigen::MatrixXd::Random(3, 3);
    std::cout << "A:" << A << std::endl << std::endl;

    Eigen::VectorXd B = Eigen::VectorXd::Random(3);
    std::cout << "B:" << B << std::endl << std::endl;

    Eigen::VectorXd x = A.colPivHouseholderQr().solve(B);
    std::cout << "x:" << x << std::endl << std::endl;

    auto testB = A * x;
    std::cout << "testB:" << testB << std::endl;
}

TEST(eigen, SVD)
{
    Eigen::MatrixXd A = Eigen::MatrixXd::Random(3, 3);
    std::cout << "A:" << A << std::endl << std::endl;

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
    auto v = svd.matrixV();
    std::cout << "v:" << v << std::endl << std::endl;

    auto u = svd.matrixU();
    std::cout << "u:" << u << std::endl << std::endl;

    auto a = svd.singularValues();
    std::cout << "a:" << a << std::endl << std::endl;

    Eigen::MatrixXd aD = a.asDiagonal();
    auto r = aD.rows();
    auto c = aD.cols();
    std::cout << "aD:" << aD << std::endl << std::endl;

    auto pA = u * aD * v.transpose();
    std::cout << "pA:" << pA << std::endl << std::endl;
}