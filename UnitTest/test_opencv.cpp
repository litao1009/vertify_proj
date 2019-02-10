#include "gtest/gtest.h"
#include "glog/logging.h"

#include "Eigen/Eigen"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/eigen.hpp"

#include <vector>

TEST(opencv, jacobi)
{
    auto cvRaw = cv::imread("data/dag.png", cv::IMREAD_ANYCOLOR);
    ASSERT_EQ(cvRaw.empty(), false);
    cv::resize(cvRaw, cvRaw, cvRaw.size() / 4);

    std::vector<cv::Mat> splits;
    cv::split(cvRaw, splits);
    auto src = splits[0];
    cv::Mat cvSrc;
    src.convertTo(cvSrc, CV_64F);

    using   EigenMatrixType = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    Eigen::Map<EigenMatrixType> eigenSrc(cvSrc.ptr<double>(), cvSrc.rows, cvSrc.cols);

    //Eigen::JacobiSVD<EigenMatrixType> svd(eigenSrc, Eigen::ComputeThinU | Eigen::ComputeThinV);
    auto svd = eigenSrc.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);
    EigenMatrixType v = svd.matrixV();
    auto vr = v.rows();
    auto vc = v.cols();

    EigenMatrixType u = svd.matrixU();
    auto ur = u.rows();
    auto uc = u.cols();

    auto a = svd.singularValues();
    auto ar = a.rows();
    auto ac = a.cols();

    EigenMatrixType aD = a.asDiagonal();
    for ( auto i = 0; i < 100; ++i )
    {
        aD(ar - 1 - i, ar - 1 - i) = 0.0;
    }

    EigenMatrixType B = u * aD * v.transpose();

    cv::Mat bImg(B.rows(), B.cols(), CV_64FC1, B.data());
}

TEST(opencv, bdc)
{
    auto cvRaw = cv::imread("data/dag.png", cv::IMREAD_ANYCOLOR);
    ASSERT_EQ(cvRaw.empty(), false);
    cv::resize(cvRaw, cvRaw, cvRaw.size() / 4);

    std::vector<cv::Mat> splits;
    cv::split(cvRaw, splits);
    auto src = splits[0];
    cv::Mat cvSrc;
    src.convertTo(cvSrc, CV_64F);

    using   EigenMatrixType = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    Eigen::Map<EigenMatrixType> eigenSrc(cvSrc.ptr<double>(), cvSrc.rows, cvSrc.cols);

    auto svd = eigenSrc.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV);
    EigenMatrixType v = svd.matrixV();
    auto vr = v.rows();
    auto vc = v.cols();

    EigenMatrixType u = svd.matrixU();
    auto ur = u.rows();
    auto uc = u.cols();

    auto a = svd.singularValues();

    auto ar = a.rows();
    auto ac = a.cols();

    EigenMatrixType aD = a.asDiagonal();
    for ( auto i = 0; i < 100; ++i )
    {
        aD(ar - 1 - i, ar - 1 - i) = 0.0;
    }

    EigenMatrixType B = u * aD * v.transpose();

    cv::Mat bImg(B.rows(), B.cols(), CV_64FC1, B.data());
}