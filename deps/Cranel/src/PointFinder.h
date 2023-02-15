#pragma once

#include "Aliases.h"
#include <opencv2/opencv.hpp>
namespace Cranel {

    class PointFinder {
    public:
        void update(cv::Mat frame, cv::Mat *preview, Vector<cv::Point2f> &points) {
//        cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            threshold(frame, frame, 60, 255, cv::THRESH_BINARY);

            Vector<Vector<cv::Point>> contours;

            cv::imshow("t", frame);

            findContours(frame, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

            // Workout which countours are valid points
            for (size_t i = 0; i < contours.size(); i++) {
                if (preview)
                    cv::drawContours(*preview, contours, (int) i, CV_RGB(255, 0, 0), 2);

                cv::Rect bBox = cv::boundingRect(contours[i]);

                // Make sure bounding box matches our criteria
                if (bBox.width >= minPointSize && bBox.height >= minPointSize && bBox.width <= maxPointSize &&
                    bBox.height <= maxPointSize) {
                    // Do a mean shift or cam shift, it's not bringing much though
                    //cv::CamShift(iFrameGray, bBox, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1));
                    cv::Point center;

                    center.x = bBox.x + bBox.width / 2;
                    center.y = bBox.y + bBox.height / 2;

                    points.push_back(center);

                    //if (preview)
                    //    cv::rectangle(*preview, bBox, CV_RGB(0, 255, 0), 2);
                }
            }
            std::cout << "points: " << points.size() << std::endl;
        }

        int minPointSize = 1;
        int maxPointSize = 12;
    };
}