#pragma once

#include "TrackingDevice.h"
#include "NodeMcu.h"
#include <cmath>
#include <string>
namespace Cranel {

    std::vector<std::string> split(const std::string &s, char seperator);

    class MyBraclet : public TrackingDevice, public NodeMcuV3 {
    public:
        MyBraclet() {
            for (int i = 0; i < 6; i++) {
                points.emplace_back(std::sin(i * 60.0), 0, std::cos(i * 60.0));
            }

        }

        void update() override {
            sendCommand("get");
            std::string resp = this->getResponse();
            std::vector<std::string> args = split(resp, ' ');

            if (args.size() == 6) {
                gyroX = std::stod(args[0]);
                gyroY = std::stod(args[1]);
                gyroZ = std::stod(args[2]);
                accelX = std::stod(args[3]);
                accelY = std::stod(args[4]);
                accelZ = std::stod(args[5]);
            }
        }
    };
}