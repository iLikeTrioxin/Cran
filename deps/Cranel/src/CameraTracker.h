#pragma once

#include "Aliases.h"

#include <opencv2/opencv.hpp>

#include "CranelCamera.h"
#include "TrackingDevice.h"
#include "PointFinder.h"

namespace Cranel {

    class CameraTracker {
    public:
        int solver = cv::SOLVEPNP_P3P;
        CranelCamera *camera;
        Vector<cv::Point2f> points;
        std::vector<cv::Vec3d> angles;
        cv::Matx33d cameraMatrix{cv::Matx33d::zeros()};
        cv::Matx<double, 8, 1> distCoeffsMatrix;

        struct {
            double fx;
            double fy;
            double P_x;
            double P_y;
        } cameraInfo;


        cv::Mat frame_old;
        cv::Mat frame_now;
        Vector<cv::Scalar> colors;
        Vector<cv::Point2f> p0, p1;

        void upd() {
            cv::RNG rng;
            for (int i = 0; i < 100; i++) {
                int r = rng.uniform(0, 256);
                int g = rng.uniform(0, 256);
                int b = rng.uniform(0, 256);

                colors.emplace_back(r, g, b);
            }

            frame_now = camera->getFrame();

            if (frame_old.empty()) return;

            Vector<uchar> status;
            Vector<float> err;
            cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
            calcOpticalFlowPyrLK(frame_old, frame_now, p0, p1, status, err, cv::Size(15, 15), 2, criteria);

            Vector<cv::Point2f> good_new;
            for (uint i = 0; i < p0.size(); i++) {
                if (status[i] == 1) {
                    good_new.push_back(p1[i]);
                    // draw the tracks
//                line(mask,p1[i], p0[i], colors[i], 2);
//                circle(frame, p1[i], 5, colors[i], -1);
                }
            }


        }

        CameraTracker(CranelCamera *&camera) : camera(camera) {
            pointFinder = PointFinder();
            this->camera = camera;
            camera = nullptr;
        }

        void CreateCameraIntrinsicsMatrices() {
            // Create our camera matrix
            cameraMatrix(0, 0) = cameraInfo.fx;
            cameraMatrix(1, 1) = cameraInfo.fy;
            cameraMatrix(0, 2) = cameraInfo.P_x;
            cameraMatrix(1, 2) = cameraInfo.P_y;
            cameraMatrix(2, 2) = 1;

            // Create distortion cooefficients
            distCoeffsMatrix = cv::Matx<double, 8, 1>::zeros();
            // As per OpenCV docs they should be thus: k1, k2, p1, p2, k3, k4, k5, k6
            // 0 - Radial first order
            // 1 - Radial second order
            // 2 - Tangential first order
            // 3 - Tangential second order
            // 4 - Radial third order
            // 5 - Radial fourth order
            // 6 - Radial fifth order
            // 7 - Radial sixth order
            //
            // SL: Using distortion coefficients in this way is breaking our face tracking output.
            // Just disable them for now until we invest time and effort to work it out.
            // For our face tracking use case not having proper distortion coefficients ain't a big deal anyway
            // See issues #1141 and #1020
            //for (unsigned k = 0; k < 8; k++)
            //    iDistCoeffsMatrix(k) = (double)iCameraInfo.dist_c[k];
        }

        // Compute Euler angles from rotation matrix
        void getEulerAngles(cv::Mat &rotCamerMatrix, cv::Vec3d &eulerAngles) {
            cv::Mat cameraMatrix, rotMatrix, transVect, rotMatrixX, rotMatrixY, rotMatrixZ;
            double *_r = rotCamerMatrix.ptr<double>();
            double projMatrix[12] = {_r[0], _r[1], _r[2], 0,
                                     _r[3], _r[4], _r[5], 0,
                                     _r[6], _r[7], _r[8], 0};

            cv::decomposeProjectionMatrix(cv::Mat(3, 4, CV_64FC1, projMatrix),
                                          cameraMatrix,
                                          rotMatrix,
                                          transVect,
                                          rotMatrixX,
                                          rotMatrixY,
                                          rotMatrixZ,
                                          eulerAngles);
        }

        void update(TrackingDevice *device) {
            points.clear();
            cv::Mat frame = camera->getFrame();

            pointFinder.update(frame, nullptr, points);

//        TrackerClassifier classifier;
//        classifier.update(points);

//        drawPoints(contourImage, points);
            std::cout << "Points: " << points.size() << std::endl;
            const auto &deviceModelPoints = device->getPoints();
            return;
            // TODO: sort points for PnP
            // MatchVertices(topPointIndex, rightPointIndex, leftPointIndex, centerPointIndex, topRightPointIndex, topLeftPointIndex);

            int solutionCount;
            if (deviceModelPoints.size() == 6) {
                solutionCount = cv::solveP3P(deviceModelPoints, points, cameraMatrix, distCoeffsMatrix, rotations,
                                             translations, solver);
            } else {
                //Guess extrinsic boolean is only for ITERATIVE method, it will be set to false for all other method
                cv::Mat rotation, translation;
                // Init only needed for iterative, it's also useless as it is
                rotation = cv::Mat::zeros(3, 1, CV_64FC1);
                translation = cv::Mat::zeros(3, 1, CV_64FC1);
                rotation.setTo(cv::Scalar(0));
                translation.setTo(cv::Scalar(0));
                /////
                rotations.clear();
                translations.clear();
                bool solved = cv::solvePnP(deviceModelPoints, points, cameraMatrix, distCoeffsMatrix, rotation,
                                           translation, true, solver);
                if (solved) {
                    solutionCount = 1;
                    rotations.push_back(rotation);
                    translations.push_back(translation);
                }
            }

            // Reset best solution index
            int iBestSolutionIndex = -1;

            if (solutionCount > 0) {
                std::cout << "Solution count: " << solutionCount << "\n";
                int minPitch = std::numeric_limits<int>::max();
                // Find the solution we want amongst all possible ones
                for (int i = 0; i < solutionCount; i++) {
                    std::cout << "Translation:\n";
                    std::cout << translations.at(i);
                    std::cout << "\n";
                    std::cout << "Rotation:\n";
                    //dbgout << rvecs.at(i);
                    cv::Mat rotationCameraMatrix;
                    cv::Rodrigues(rotations[i], rotationCameraMatrix);
                    cv::Vec3d angles1;
                    getEulerAngles(rotationCameraMatrix, angles1);
                    angles.push_back(angles1);

                    // Check if pitch is closest to zero
                    int absolutePitch = (int) std::abs(angles1[0]);
                    if (minPitch > absolutePitch) {
                        // The solution with pitch closest to zero is the one we want
                        minPitch = absolutePitch;
                        iBestSolutionIndex = i;
                    }

                    std::cout << angles1;
                    std::cout << "\n";
                }

                std::cout << "\n";
            }

            if (iBestSolutionIndex != -1) {
                // Best translation
                cv::Vec3d translation = translations[iBestSolutionIndex];
                // Best angles
                cv::Vec3d angles = angles[iBestSolutionIndex];

                // Pass solution through our kalman filter
//            iKf.Update(translation[0], translation[1], translation[2], angles[2], angles[0], angles[1]);

                // Check if our solution makes sense
                // For now, just discard solutions with extrem pitch
                if (std::abs(angles[0]) > solutionMaxPitch) {
                    std::cout << "WARNING: discarding solution!";
                    badSolutionCount++;
                } else {
                    goodSolutionCount++;
                    // iBestAngles = angles;
                    // iBestTranslation = translation;
                }

            }

        }

        int solutionMaxPitch = 50;
        int goodSolutionCount = 0;
        int badSolutionCount = 0;
        int frameCount = 0;
        bool doPreview = false;
        cv::Mat preview;

        ~CameraTracker() {
            delete camera;
        }

    private:
        std::vector<cv::Mat> translations;
        std::vector<cv::Mat> rotations;

        PointFinder pointFinder;
    };
}