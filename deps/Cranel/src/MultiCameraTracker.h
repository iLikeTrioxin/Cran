#pragma once

#include "CameraTracker.h"

#include <vector>
#include <thread>

namespace Cranel {

    class MultiCameraTracker {
    public:
        std::vector<CameraTracker *> trackers;
        std::vector<std::thread> threads;
        std::vector<int> threadsResults;
        std::vector<char> threadsDone;
        std::vector<char> runningThreads;

        void trackerLoop(int i) {
            while (runningThreads[i]) {
                if (threadsDone[i]) return;

                trackers[i]->upd();

                threadsDone[i] = true;
            }
        }

        CameraTracker *addTracker(CameraTracker *&tracker) {
            trackers.push_back(tracker);
            threadsResults.push_back({});
            threadsDone.push_back(false);
            runningThreads.push_back(true);
            threads.emplace_back(&MultiCameraTracker::trackerLoop, this, trackers.size() - 1);
            tracker = nullptr;
            return trackers.back();
        }

        void removeTracker(int i) {
            delete trackers[i];
            runningThreads[i] = false;
            threads[i].join();

            runningThreads.erase(runningThreads.begin() + i);
            threads.erase(threads.begin() + i);
            trackers.erase(trackers.begin() + i);
            threadsResults.erase(threadsResults.begin() + i);
            threadsDone.erase(threadsDone.begin() + i);
        }

        void update() {
            for (auto &done: threadsDone) done = false;

        }

        ~MultiCameraTracker() {
            for (auto &runningThread: runningThreads) runningThread = false;
            for (auto &thread: threads) thread.join();
            for (auto tracker: trackers) delete tracker;
        }

    private:

    };
}