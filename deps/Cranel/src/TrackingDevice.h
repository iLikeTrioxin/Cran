#pragma once

#include "Aliases.h"
#include <opencv2/opencv.hpp>
namespace Cranel {

    struct TrackingDevice {
    public:
        // gyroscope data
        double gyroX;
        double gyroY;
        double gyroZ;

        // accelerometer data
        double accelX;
        double accelY;
        double accelZ;

        const std::vector<cv::Point3f> &getPoints() const { return points; }

        virtual void update() = 0;

    protected:
        // cords for each ir led in 3d space
        std::vector<cv::Point3f> points;
    };
}