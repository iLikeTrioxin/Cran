#pragma once

#include <opencv2/opencv.hpp>

namespace Cranel {

    class CranelCamera {
    public:
        std::string name;

        const cv::Mat &getFrame(bool nextFrame = true) {
            if (nextFrame) this->nextFrame();
            return frame;
        }

        virtual void nextFrame() = 0;

        virtual ~CranelCamera() = default;

    protected:
        cv::Mat frame;
    };
}