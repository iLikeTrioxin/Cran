#pragma once

#include "CranelCamera.h"
namespace Cranel {

    class OpenCVCamera : public CranelCamera {
    public:
        explicit OpenCVCamera(const std::string &file);

        explicit OpenCVCamera(int id);

        void nextFrame() override;

        ~OpenCVCamera() override = default;

        cv::VideoCapture camera;
    };
}