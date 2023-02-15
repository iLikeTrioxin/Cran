#include "OpencvCamera.h"
namespace Cranel {

    OpenCVCamera::OpenCVCamera(const std::string &path) {
        this->name = path;

        camera = cv::VideoCapture(path);
//        camera.set(cv::CAP_PROP_EXPOSURE, 0);
    }

    void OpenCVCamera::nextFrame() {
        camera >> this->frame;
        if(this->frame.empty()) return;
        cvtColor(this->frame, this->frame, cv::COLOR_BGR2GRAY);
    }
}