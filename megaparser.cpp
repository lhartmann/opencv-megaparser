#include "megaparser.h"
#include <cmath>
#include <QDebug>

megaparser::megaparser(const cv::Mat &s)
    : src(s.clone())
{
    cv::subtract(255, src, src);
    xoff = NAN;
}

megaparser::contours_t megaparser::getContours()
{
    if (_c.size())
        return _c;

    contours_t contours;

    cv::Mat tmp;
    cv::Mat el0 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
    cv::Mat el1 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5));
    cv::erode(src, tmp, el0);
    cv::dilate(tmp, tmp, el1);
    cv::findContours(tmp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    _c = contours;
    return contours;
}

megaparser::contours_t megaparser::getContoursConvex()
{
    if (_cc.size())
        return _cc;

    contours_t contours = getContours();

    for (auto &c : contours)
        cv::convexHull(c,c);

    _cc = contours;
    return contours;
}

megaparser::rectList megaparser::getContoursRectangular()
{
    if (_cr.size())
        return  _cr;

    rectList markers;

    // Consider contours rectangular if the fill over 95% of the bounding area.
    // A circle would fill pi/4 = 78%
    for (auto c : getContours()) {
        cv::Rect r = cv::boundingRect(c);

        cv::Mat region = src.rowRange(r.tl().y, r.br().y).colRange(r.tl().x, r.br().x);

        cv::Moments m = cv::moments(region, true);

        if (m.m00 >= 0.5 * r.width * r.height)
            markers.push_back(r);
    }

    _cr = markers;
    return markers;
}

megaparser::pointList megaparser::getMarkers()
{
    if (_m.size())
        return _m;

    // Consider markers the leftmost rectangular contours on the image.
    // "Leftmost" means the very first one, and up 3% of the image wdth later.

    std::vector<cv::Point> markers;

    // Get the center coordinate of every rectangular shape
    for (auto b : getContoursRectangular()) {
        markers.push_back((b.tl() + b.br()) / 2);
    }

    // Code below presumes non-empty list, so give up on issues
    if (!markers.size())
        return pointList();

    // Sort by x
    std::sort(markers.begin(), markers.end(),
        [](cv::Point &l, cv::Point &r) -> bool {
            return l.x < r.x;
        }
    );

    // Select the leftmost
    int xlim = markers[0].x + src.size().width * 0.03;
    int i=0;
    while (markers[i].x < xlim) ++i;
    markers.resize(i);

    // Sort selected by y
    std::sort(markers.begin(), markers.end(),
        [](const cv::Point l, const cv::Point r) -> bool {
            return l.y < r.y;
        }
    );

    _m = markers;
    return markers;
}

cv::Mat megaparser::getVisual()
{
    if (_visual.data)
        return _visual;

    cv::cvtColor(src, _visual, CV_GRAY2BGR);
    cv::drawContours(_visual, getContours(), -1, cv::Scalar(0,0,255));
//    cv::drawContours(_visual, getContoursConvex(), -1, cv::Scalar(0,0,128));

    for (auto r : getContoursRectangular())
        cv::rectangle(_visual, r, cv::Scalar(255,0,0));

    for (auto m : getMarkers())
        cv::drawMarker(_visual, m, cv::Scalar(180,0,0));

    return _visual;
}

void megaparser::updateMapping()
{
    if (!std::isnan(xoff)) {
        qDebug("HELLO!");
        return;
    }

    // Find the row with the column markers
    auto ref = getMarkers()[0];
    int step = getMarkers()[1].y-ref.y;

    // Get the column markers
    pointList cm;
    for (auto r : getContoursRectangular()) {
        cv::Point p = (r.tl() + r.br()) / 2;

        if (abs(ref.y - p.y) < step/2)
            cm.push_back(p);
    }

    // Megasena tickets always have 5 column markers on the reference row
    if (cm.size() != 5) {
        qDebug("Not 5 col markers found");
        return;
    }

    // Sort by x
    std::sort(cm.begin(), cm.end(),
        [](const cv::Point &l, const cv::Point &r) -> bool {
            return l.x < r.x;
        }
    );

    // Find the displacement for columns
    cv::Point p0 = cm[2];
    cv::Point p3 = cm[4];

    xoff = p0.x - ref.x;
    yoff = p0.y - ref.y;
    dy = (p3.y - p0.y) / 3.;
    dx = (p3.x - p0.x) / 3.;
}

megaparser::data_t megaparser::getData()
{
    data_t data;

    updateMapping();
    if (std::isnan(xoff))
        return data;

    for (auto ref : getMarkers()) {
        std::vector<bool> row;
        for (int i=0; i<cols(); ++i) {
            double y0 = ref.y + yoff + i*dy;
            double x0 = ref.x + xoff + i*dx;
            double w2 = dx/6;
            double h2 = dx/10;

            cv::Mat region = src.rowRange(y0-h2, y0+h2).colRange(x0-w2, x0+w2);
            auto m = cv::moments(region, true);

            row.push_back(m.m00 > 0.5*region.size().width * region.size().height);
        }
        data.push_back(row);
    }

    return data;
}
