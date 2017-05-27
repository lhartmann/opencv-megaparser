#include "cv_saturator.h"
#include <opencv2/imgproc.hpp>

#include <cmath>
using std::max;
using std::min;

cv_saturator::cv_saturator(const cv::Mat &s, int low, int high, int dist)
    : low(low), high(high), dist(dist), src(s)
{
}

cv::Mat cv_saturator::white() {
    if (mw.data)
        return mw;

    if (!dist) dist = max(src.cols, src.rows) * 15 / 100;

    if (dist > src.cols || dist > src.rows || !src.data)
        return cv::Mat();

    // Find the maximum values by region. Matrix dimensions get smaller.
    mw = src.clone();
    blur(mw, 4);
    for (int off=1; off<dist/2; off*=2) {
        cv::Mat tmp = cv::max(mw.rowRange(0, mw.rows-off), mw.rowRange(off,mw.rows));
        mw = tmp;
    }
    for (int off=1; off<dist/2; off*=2) {
        cv::Mat tmp = cv::max(mw.colRange(0, mw.cols-off), mw.colRange(off,mw.cols));
        mw = tmp;
    }

    regrow(mw); // Restores dimensions to src.size
    blur(mw);

    return mw;
}

cv::Mat cv_saturator::black() {
    if (mb.data)
        return mb;

    if (!dist) dist = max(src.cols, src.rows) * 15 / 100;

    if (dist > src.cols || dist > src.rows || !src.data)
        return cv::Mat();

    // Find the maximum values by region. Matrix dimensions get smaller.
    mb = src.clone();
    blur(mb, 4);
    for (int off=1; off<dist/2; off*=2) {
        cv::Mat tmp = cv::min(mb.rowRange(0, mb.rows-off), mb.rowRange(off,mb.rows));
        mb = tmp;
    }
    for (int off=1; off<dist/2; off*=2) {
        cv::Mat tmp = cv::min(mb.colRange(0, mb.cols-off), mb.colRange(off,mb.cols));
        mb = tmp;
    }

    regrow(mb); // Restores dimensions to src.size
    blur(mb);

    // Black must be below white, subtract 1
    mb = cv::min(white(), mb);
    cv::subtract(mb, 1, mb);

    return mb;
}

// Restores m to src.size
void cv_saturator::regrow(cv::Mat &m) {
    // Where to place m
    int left   = (src.cols - m.cols)/2;
    int top    = (src.rows - m.rows)/2;
    int right  = left + m.cols;
    int bottom = top + m.rows;

    // Allocate space for the final image
    cv::Mat tmp;
    tmp.create(src.rows, src.cols, src.type());

    // Copy small image centered
    m.copyTo(tmp.rowRange(top, bottom).colRange(left,right));

    // Replicate left edge
    for (int x=0; x<left; ++x)
        tmp.col(left).copyTo(tmp.col(x));

    // Replicate right edge
    for (int x=right; x<src.cols; ++x)
        tmp.col(right-1).copyTo(tmp.col(x));

    // Replicate top edge
    for (int y=0; y<top; ++y)
        tmp.row(top).copyTo(tmp.row(y));

    // Replicate bottom edge
    for (int y=bottom; y<src.rows; ++y)
        tmp.row(bottom-1).copyTo(tmp.row(y));

    // Done, save
    m = tmp;
}

cv::Mat cv_saturator::gray() {
    if (mg.data)
        return mg;

    if (!src.data)
        return cv::Mat();

    cv::Mat tmp;
    cv::subtract(white(), black(), tmp);

    cv::subtract(src, black(), mg);
    cv::divide(mg, tmp, mg, 255);

    cv::subtract(mg, low, mg);
    cv::divide(mg, high-low, mg, 255);

    return mg;
}

cv::Mat cv_saturator::blackAndWhite() {
    if (mbw.data)
        return mbw;

    if (!src.data)
        return cv::Mat();

    cv::threshold(gray(), mbw, 256/3, 255, cv::THRESH_BINARY);

    return mbw;
}

void cv_saturator::blur(cv::Mat &m, int d) {
    cv::Size2i sz;
    sz.width  = d ? d : dist;
    sz.height = d ? d : dist;

    cv::boxFilter(m,m,-1,sz);
    return;

    cv::Mat tmp;
    cv::GaussianBlur(m, tmp, sz, dist/2, dist/2);
    m = tmp;
}
