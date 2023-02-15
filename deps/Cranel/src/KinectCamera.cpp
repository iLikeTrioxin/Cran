#include "KinectCamera.h"
#include <pthread.h>

namespace Cranel {

    uint16_t KinectCamera::t_gamma[2048];

    pthread_mutex_t KinectCamera::gl_backbuf_mutex;
    pthread_cond_t  KinectCamera::gl_frame_cond;

    uint8_t *KinectCamera::depth_back;
    uint8_t *KinectCamera::video_back;

    freenect_context *KinectCamera::fn_ctx;
    freenect_device *KinectCamera::fn_dev;

    void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp) {
        int i;
        uint16_t *depth = (uint16_t *) v_depth;

        pthread_mutex_lock(&KinectCamera::gl_backbuf_mutex);
        for (i = 0; i < 640 * 480; i++) {
            int pval = KinectCamera::t_gamma[depth[i]];
            int lb = pval & 0xff;
            switch (pval >> 8) {
                case 0:
                    KinectCamera::depth_back[3 * i + 0] = 255;
                    KinectCamera::depth_back[3 * i + 1] = 255 - lb;
                    KinectCamera::depth_back[3 * i + 2] = 255 - lb;
                    break;
                case 1:
                    KinectCamera::depth_back[3 * i + 0] = 255;
                    KinectCamera::depth_back[3 * i + 1] = lb;
                    KinectCamera::depth_back[3 * i + 2] = 0;
                    break;
                case 2:
                    KinectCamera::depth_back[3 * i + 0] = 255 - lb;
                    KinectCamera::depth_back[3 * i + 1] = 255;
                    KinectCamera::depth_back[3 * i + 2] = 0;
                    break;
                case 3:
                    KinectCamera::depth_back[3 * i + 0] = 0;
                    KinectCamera::depth_back[3 * i + 1] = 255;
                    KinectCamera::depth_back[3 * i + 2] = lb;
                    break;
                case 4:
                    KinectCamera::depth_back[3 * i + 0] = 0;
                    KinectCamera::depth_back[3 * i + 1] = 255 - lb;
                    KinectCamera::depth_back[3 * i + 2] = 255;
                    break;
                case 5:
                    KinectCamera::depth_back[3 * i + 0] = 0;
                    KinectCamera::depth_back[3 * i + 1] = 0;
                    KinectCamera::depth_back[3 * i + 2] = 255 - lb;
                    break;
                default:
                    KinectCamera::depth_back[3 * i + 0] = 0;
                    KinectCamera::depth_back[3 * i + 1] = 0;
                    KinectCamera::depth_back[3 * i + 2] = 0;
                    break;
            }
        }

        pthread_cond_signal(&KinectCamera::gl_frame_cond);
        pthread_mutex_unlock(&KinectCamera::gl_backbuf_mutex);
    }

    void video_cb(freenect_device *dev, void *rgb, uint32_t timestamp) {
        pthread_mutex_lock(&KinectCamera::gl_backbuf_mutex);

        freenect_set_video_buffer(dev, KinectCamera::video_back);

        pthread_cond_signal(&KinectCamera::gl_frame_cond);
        pthread_mutex_unlock(&KinectCamera::gl_backbuf_mutex);
    }
}