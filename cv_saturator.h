#ifndef CV_SATURATOR_H
#define CV_SATURATOR_H

#include <opencv2/core.hpp>

class cv_saturator
{
    int low, high;
    int dist;
    cv::Mat src;
    cv::Mat mw, mb, mg, mbw;

    // Restores m to src.size
    void regrow(cv::Mat &m);
    void blur(cv::Mat &m, int d=0);

public:
    cv_saturator(const cv::Mat &s, int low=0, int high=255, int dist=0);
    cv::Mat white(); // Returns the reference white level image
    cv::Mat black(); // Returns the refeference black level image
    cv::Mat gray();  // Returns a grayscale relative to reference levels
    cv::Mat blackAndWhite(); // Returns the saturated BW image
};

#endif // CV_SATURATOR_H
