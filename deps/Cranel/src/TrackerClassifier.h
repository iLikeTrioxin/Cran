#pragma once

#include "Aliases.h"
#include "TrackingDevice.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
namespace Cranel {

    class TrackerClassifier {
    public:
        double groupDistanceX = 50;
        double groupDistanceY = 50;
        std::vector<TrackingDevice> devices;

        void update(std::vector<cv::Point2f> points) {
            leftRightPoints.clear();
            topBottomPoints.clear();

            leftRightPoints.resize(points.size());
            topBottomPoints.resize(points.size());

            // sort points by x
            std::iota(leftRightPoints.begin(), leftRightPoints.end(), 0u);
            std::sort(leftRightPoints.begin(), leftRightPoints.end(), [=](const int &a, const int &b) {
                return points[a].x < points[b].x;
            });

            // sort points by y
            std::iota(topBottomPoints.begin(), topBottomPoints.end(), 0u);
            std::sort(topBottomPoints.begin(), topBottomPoints.end(), [=](const int &a, const int &b) {
                return points[a].y < points[b].y;
            });


            // group points by x
            std::vector<std::vector<int>> xGroups = {{0}};

            for (int i = 1; i < points.size(); i++) {
                if (points[leftRightPoints[i]].x - points[leftRightPoints[i - 1]].x > groupDistanceX)
                    xGroups.emplace_back();

                xGroups.back().push_back(leftRightPoints[i]);
            }
            std::cout << "xGroups: " << xGroups.size() << std::endl;

            // group points by y
            std::vector<std::vector<int>> yGroups = {{0}};

            for (int i = 1; i < points.size(); i++) {
                if (points[topBottomPoints[i]].x - points[topBottomPoints[i - 1]].x > groupDistanceY)
                    yGroups.emplace_back();

                yGroups.back().push_back(topBottomPoints[i]);
            }
            std::cout << "yGroups: " << yGroups.size() << std::endl;

            // group points by x and y
            std::vector<std::vector<int>> xyGroups;
            for (int i = 0; i < xGroups.size(); i++) {
                for (int j = 0; j < yGroups.size(); j++) {
                    std::vector<int> intersection;

                    std::set_intersection(xGroups[i].begin(), xGroups[i].end(), yGroups[j].begin(), yGroups[j].end(),
                                          std::back_inserter(intersection));

                    if (!intersection.empty())
                        xyGroups.push_back(intersection);
                }
            }

            std::cout << "xyGroups: " << xyGroups.size() << std::endl;
            // correct points

        }

    private:
        std::vector<int> leftRightPoints;
        std::vector<int> topBottomPoints;
    };
}