#ifndef MEGAPARSER_H
#define MEGAPARSER_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

class megaparser
{
public:
    typedef std::vector<std::vector<cv::Point> > contours_t;
    typedef std::vector<cv::Rect>  rectList;
    typedef std::vector<cv::Point> pointList;
    typedef std::vector<std::vector<bool> > data_t;

private:
    cv::Mat src;

    // Cached results:
    contours_t _c, _cc;
    rectList   _cr;
    pointList  _m;

    // marker grid to image coordinate mapping:
    double xoff, yoff, dx, dy;

    // Debug image
    cv::Mat _visual;

public:
    megaparser(const cv::Mat &s);

    // Returns all external contours
    contours_t getContours();

    contours_t getContoursConvex();

    rectList getContoursRectangular();

    // Returns only contours that look like reference markers
    pointList getMarkers();

    // Returns a drawing for debugging purposed
    cv::Mat getVisual();

    //
    void updateMapping();

    // Grid size information
    int rows() { return getMarkers().size(); }
    int cols() { return 10; }

    data_t getData();
};

#endif // MEGAPARSER_H
