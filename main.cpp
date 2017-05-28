#include <iostream>
#include <iomanip>
#include <string>
#include "cv_saturator.h"
#include "megaparser.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cout << "Use: " << argv[0] << " <inamge_file>" << endl;
        return 0;
    }

    string fn = argv[1];
    cv::Mat src = cv::imread(fn);
    if (!src.data) {
        cout << "File open failed: " << fn << endl;
        return 1;
    }

    // The real work: Extract red channel, saturate, then parse
    // Parsed data is returnes as a 2D bool vector: data[rows][columns]
    cv::extractChannel(src, src, 2);
    cv_saturator s(src, 0, 256-64, (src.rows+src.cols) * 0.2);
    megaparser mp(s.blackAndWhite());
    auto data = mp.getData();

    // Everything from here on is just for demonstration.

    // Optional: Save saturator and parser partial images for debugging.
    cv::imwrite(fn+"_01_src.png",   src);
    cv::imwrite(fn+"_02_white.png", s.white());
    cv::imwrite(fn+"_03_black.png", s.black());
    cv::imwrite(fn+"_04_gray.png",  s.gray());
    cv::imwrite(fn+"_05_B&W.png",   s.blackAndWhite());
    cv::imwrite(fn+"_06_mp.png",    mp.getVisual());

    // Iterate over the data and print.
    for (auto row : data) {
        for (auto col : row) {
            cout << (col ? "### " : "    ");
        }
        cout << endl;
    }

    return 0;
}
