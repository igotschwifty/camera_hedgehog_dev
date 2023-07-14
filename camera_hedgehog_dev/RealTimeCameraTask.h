//
// Created by soc2 on 7/5/23.
//

#ifndef CAMERA_HEDGEHOG_DEV_REALTIMECAMERATASK_H
#define CAMERA_HEDGEHOG_DEV_REALTIMECAMERATASK_H


#include <opencv2/core/mat.hpp>
#include <any>
#include "abstract_priority_task.h"

using namespace hh;

class loggingData{

public:
    std::unordered_map<std::string, float> data;
    std::shared_ptr<cv::Mat> img;
};

class RealTimeCameraTask: public hh::AbstractPriorityTask<1, void, loggingData[]> {
    public:
        bool flag = true;
        int numFrames = 100;
        std::shared_ptr<loggingData[]> logData = std::shared_ptr<loggingData[]> (new loggingData[numFrames], std::default_delete<loggingData[]>());
    private:
        virtual bool oneFrameAvailable() {};
        virtual unsigned char* grabFrame() {};
        virtual std::shared_ptr<cv::Mat> getImage(unsigned char* imagePointer) const {};
        virtual std::shared_ptr<cv::Mat> processImage(const std::shared_ptr<cv::Mat>& img) {};
        virtual void logInfo(int i) {};
        void execute(std::shared_ptr<void>) override {

            // Configure the grabber with a script
            std::chrono::time_point<std::chrono::system_clock> timeStart = std::chrono::system_clock::now();
            while(flag) {
                for(int i = 0; i < numFrames; i++) {
                    // Put another timer here
                    std::chrono::time_point<std::chrono::system_clock> timeStartWaiting = std::chrono::system_clock::now();
                    oneFrameAvailable();
                    std::chrono::time_point<std::chrono::system_clock> totalTimeStart = std::chrono::system_clock::now();
                    std::chrono::time_point<std::chrono::system_clock> timeGrabStart = std::chrono::system_clock::now();
                    unsigned char* imagePointer = grabFrame();
                    std::chrono::time_point<std::chrono::system_clock> timeGrabEnd = std::chrono::system_clock::now();
                    std::chrono::time_point<std::chrono::system_clock> timeProcessStart = std::chrono::system_clock::now();
//                    std::shared_ptr<cv::Mat> img = getImage(imagePointer);
//                    processImage(img);
//                    logData.get()[i].img = img;
                    std::chrono::time_point<std::chrono::system_clock> totalTimeEnd = std::chrono::system_clock::now();
                    float timeTotalNoWait = std::chrono::duration<float, std::micro>(totalTimeEnd- totalTimeStart).count();
                    float timeWithWait = std::chrono::duration<float, std::micro>(totalTimeEnd - timeStartWaiting).count();
                    float timeProcess = std::chrono::duration<float, std::micro>(totalTimeEnd - timeProcessStart).count();
                    float timeGrab = std::chrono::duration<float, std::micro>(timeGrabEnd - timeGrabStart).count();
                    float timeWaiting = std::chrono::duration<float, std::micro>(totalTimeStart - timeStartWaiting).count();
                    logData.get()[i].data["TimeWithWait"] = timeWithWait;
                    logData.get()[i].data["TimeTotalNoWait"] = timeTotalNoWait;
                    logData.get()[i].data["TimeWaiting"] = timeWaiting;
                    logData.get()[i].data["TimeProcess"] = timeProcess;
                    logData.get()[i].data["TimeGrab"] = timeGrab;
                    logInfo(i);
                }
                std::chrono::time_point<std::chrono::system_clock> timeStartAdd = std::chrono::system_clock::now();
                this->addResult(logData);
                std::chrono::time_point<std::chrono::system_clock> timeStopAdd = std::chrono::system_clock::now();
                logData.get()->data.clear();
            }
            std::cout << "Process complete \n";
        }

public:

    RealTimeCameraTask(std::basic_string<char> name, size_t numberThreads, Policy policy, const struct sched_param *param, bool const automaticStart = false):
            hh::AbstractPriorityTask<1, void, loggingData[]>(name, numberThreads, policy, param, automaticStart){
        initialize_prio_task();
    };

    void initialize_prio_task() override {
        initialize_camera();
    }

    virtual void initialize_camera() {}

};


#endif //CAMERA_HEDGEHOG_DEV_REALTIMECAMERATASK_H
