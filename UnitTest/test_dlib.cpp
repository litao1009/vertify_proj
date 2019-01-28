#include "gtest/gtest.h"

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"

#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing.h"
#include "dlib/image_io.h"
#include "dlib/opencv.h"

TEST(dlib, detection)
{
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize("/var/lib/dlib/shape_predictor_68_face_landmarks.dat") >> sp;

    auto img = cv::imread("data/0_female_bgr.png", cv::IMREAD_ANYCOLOR);
    
    dlib::cv_image<dlib::rgb_pixel> dlibImg(img);
    
    auto dets = detector(dlibImg);
    ASSERT_EQ(dets.empty(), false);

    auto shape = sp(dlibImg, dets[0]);

    std::vector<cv::Point2f> landmark;
    for ( int i = 0; i < shape.num_parts(); i++ )
    {
        landmark.emplace_back(shape.part(i).x(), shape.part(i).y());
    }
}