#ifndef KAK
#define KAK

#pragma once

#include "CranelCamera.h"
#include "libfreenect/libfreenect.h"
#include <iostream>
#include <cmath>
#include <thread>
namespace Cranel {

    void video_cb(freenect_device *dev, void *data, uint32_t timestamp);
    void depth_cb(freenect_device *dev, void *data, uint32_t timestamp);

    class KinectCamera : public CranelCamera {
    public:
        std::thread t;
        bool running = true;

        void nextFrame() override {
            pthread_mutex_lock(&gl_backbuf_mutex);

            memcpy(this->frame.data, video_back, 640 * 480);

            pthread_mutex_unlock(&gl_backbuf_mutex);
        }

        void update() {
            while (running && freenect_process_events(fn_ctx) >= 0) {
                // Do nothing.
            }
        }

        KinectCamera(std::string path) {
            this->name = path;

            video_back = (uint8_t *) malloc(640 * 480 * 3);
            depth_back = (uint8_t *) malloc(640 * 480 * 3);

            this->frame = cv::Mat(480, 640, CV_8UC1);

            // Initialize libfreenect.
            int ret = freenect_init(&fn_ctx, NULL);
            if (ret < 0) return;

            // Show debug messages and use camera only.
            freenect_set_log_level(fn_ctx, FREENECT_LOG_SPEW);
            freenect_select_subdevices(fn_ctx, FREENECT_DEVICE_CAMERA);

            // Find out how many devices are connected.
            int num_devices = ret = freenect_num_devices(fn_ctx);
            if (ret < 0) return;

            if (num_devices == 0) {
                printf("No device found!\n");
                freenect_shutdown(fn_ctx);
                return;
            }

            // Open the first device.
            ret = freenect_open_device(fn_ctx, &fn_dev, 0);
            if (ret < 0) {
                freenect_shutdown(fn_ctx);
                return;
            }

            freenect_set_video_buffer(fn_dev, video_back);
            freenect_set_tilt_degs(fn_dev, 0);
            freenect_set_led(fn_dev, LED_RED);
            freenect_set_depth_callback(fn_dev, depth_cb);
            freenect_set_video_callback(fn_dev, video_cb);
            freenect_set_video_mode(fn_dev,
                                    freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_IR_8BIT));
            freenect_set_depth_mode(fn_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));
            freenect_set_video_buffer(fn_dev, video_back);

            freenect_start_depth(fn_dev);
            freenect_start_video(fn_dev);

            t = std::thread(&KinectCamera::update, this);
        }

        ~KinectCamera() {
            running = false;
            t.join();

            freenect_stop_depth(fn_dev);
            freenect_stop_video(fn_dev);

            freenect_close_device(fn_dev);
            freenect_shutdown(fn_ctx);
        }

    public:
        static uint16_t t_gamma[2048];

        static pthread_mutex_t gl_backbuf_mutex;
        static pthread_cond_t gl_frame_cond;

        static uint8_t *depth_back;
        static uint8_t *video_back;

        static freenect_context *fn_ctx;
        static freenect_device *fn_dev;
    };

}
#endif