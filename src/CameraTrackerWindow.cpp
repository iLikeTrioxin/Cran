//
// Created by samuel on 2/12/23.
//

#include "CameraTrackerWindow.h"

QPixmap cvMatToQPixmap(const cv::Mat &mat) {
    if(mat.empty()) return QPixmap();

    QImage::Format format;
    switch (mat.type()) {
        case CV_8UC1:
            format = QImage::Format_Grayscale8;
            break;
        case CV_8UC3:
            format = QImage::Format_RGB888;
            break;
        default:
            format = QImage::Format_Invalid;
            break;
    }

    QImage image(
            mat.data,
            mat.cols,
            mat.rows,
            static_cast<int>(mat.step),
            format
    );

    if (format == QImage::Format_Invalid) {
        // Handle the error
        return QPixmap();
    }

    if (format == QImage::Format_RGB888) {
        return QPixmap::fromImage(image.rgbSwapped());
    } else {
        return QPixmap::fromImage(image);
    }
}

CameraTrackerWindow::CameraTrackerWindow(QWidget* parent, Cranel::CameraTracker* tracker) : QMainWindow(parent) {
    setupUi(this);
    this->tracker = tracker;
}

void CameraTrackerWindow::closeEvent(QCloseEvent *event) {
    closed = true;
    emit destroyedd(this);
    QMainWindow::closeEvent(event);
}

void CameraTrackerWindow::update() {
    if(closed) return;
    this->cameraPreview->setPixmap(cvMatToQPixmap(tracker->camera->getFrame(true)));
}