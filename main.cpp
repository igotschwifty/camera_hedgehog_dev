#include <hedgehog/hedgehog.h>
#include <EGrabber.h>
#include <EGenTL.h>
#include <filesystem>
#include <string>
#include "abstract_priority_task.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/mat.hpp"
#include "RealTimeCameraTask.h"
#include <opencv2/imgcodecs.hpp>
#include <FormatConverter.h>
#include "unordered_map"

using namespace Euresys;
using namespace gc;

// TODO: Get time to add result

class MyGrabber : public EGrabber<CallbackOnDemand> {

    void startGrab() {
        int width = getInteger<Euresys::RemoteModule>("Width");
        int height = getInteger<Euresys::RemoteModule>("Height");
        reallocBuffers(10,  width*height);
        start();
    }

    void config() {
        if (std::filesystem::exists("../config.js")) {
            runScript("../config.js");
        }
    }

    public:
    explicit MyGrabber(EGenTL &genTL) : EGrabber<CallbackOnDemand>(genTL) {
        this->execute<RemoteModule>("AcquisitionStop");
        config();
        startGrab();
    }
};

class Genicam: public RealTimeCameraTask {
public:
    int frames_popped = 0;
private:
    EGenTL genTL;
    FormatConverter converter = FormatConverter(genTL);
    MyGrabber grabber = MyGrabber(genTL);
    int64_t height;
    int64_t width;
//    uint8_t* lcl_img;

    /**
     * Checks if a frame is currently available to grab
     * @return true if there is a frame, false otherwise
     */
     virtual bool oneFrameAvailable() override {
        uint64_t difference = grabber.getInfo<StreamModule, uint64_t>(GenTL::STREAM_INFO_NUM_AWAIT_DELIVERY);
//        int loops = 0;
//        while(difference != 1 and loops < 100) {
//            if(difference > 1) {
//                grabber.pop();
//            }
//            loops++;
//            difference = grabber.getInfo<StreamModule, uint64_t>(GenTL::STREAM_INFO_NUM_AWAIT_DELIVERY);
//        }
//        if(difference != 1) {
//            std::cout << difference << "\n";
//        }
        while(difference != 1) {
            if(difference > 1) {
                grabber.pop();
            }
            difference = grabber.getInfo<StreamModule, uint64_t>(GenTL::STREAM_INFO_NUM_AWAIT_DELIVERY);
        }
        return true;
    }

    /**
     * Grabs a single frame
     * @return pointer to the image
     */
    unsigned char* grabFrame() override {
        uint64_t difference = grabber.getInfo<StreamModule, uint64_t>(GenTL::STREAM_INFO_NUM_AWAIT_DELIVERY);
        if(difference != 1) {
            std::cout << difference << "\n";
        }
        ScopedBuffer buffer(grabber);
//        auto imagePointer = buffer.getInfo<uint8_t*>(GenTL::BUFFER_INFO_BASE);
//        const std::string format = (genTL.imageGetPixelFormat(buffer.getInfo<uint64_t>(gc::BUFFER_INFO_PIXELFORMAT)));
////        converter.copy(lcl_img, imagePointer, format, width, height);
//        auto timestamp = buffer.getInfo<uint64_t>(GenTL::BUFFER_INFO_TIMESTAMP);
//        loggingData logData;
//        logData.data["Timestamp"] = timestamp;
        return 0;
    }

    /**
     * Uses image pointer and converts to cv matrix
     * @param imagePointer
     * @return cv matrix pointer for the image
     */
    std::shared_ptr<cv::Mat> getImage(unsigned char* imagePointer) const override {
        std::vector<int> viewDimsInt = {static_cast<int>(height), static_cast<int>(width)};
        int type = (int) CV_MAKETYPE(cv::traits::Type<uint8_t>::value, 1);
        cv::Mat grayImage, colorImage, thresh_img;
        grayImage = cv::Mat(viewDimsInt, type, imagePointer);
        return std::make_shared<cv::Mat>(grayImage);
    }

    virtual void logInfo(int i) override {
        logData.get()[i].data["DroppedFrames"] = grabber.getInfo<Euresys::StreamModule, uint64_t>(GenTL::STREAM_INFO_NUM_UNDERRUN);
    };

    /**
     * Processes the image, runs findcontours
     * @param img image to be processed
     */
    std::shared_ptr<cv::Mat> processImage(const std::shared_ptr<cv::Mat>& img) override {
        cv::Mat grayImage = *img;
        std::vector<int> viewDimsInt = {static_cast<int>(height), static_cast<int>(width)};
        cv::Mat colorImage, thresh_img;

        cv::threshold(grayImage,thresh_img,20, 255,cv::THRESH_BINARY );

        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;

        cv::findContours(thresh_img,contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);
        cv::Scalar red(0,0,255);
//        // Create a 3-channel color image
//
//        // Calculate the bounding box for the current contour
//
//        // Assign grayscale values to each channel
        cv::cvtColor(grayImage, colorImage, cv::COLOR_GRAY2BGR);
//        cv::drawContours(colorImage ,contours,-1,red,2);
        return std::make_shared<cv::Mat>(colorImage);
    }

public:

    Genicam(std::basic_string<char> name, size_t numberThreads, hh::Policy policy, const struct sched_param *param)
            : RealTimeCameraTask(name, numberThreads, policy, param, true) {
        width = grabber.getInteger<Euresys::RemoteModule>("Width");
        height = grabber.getInteger<Euresys::RemoteModule>("Height");
    }

    void initialize_camera() override {
//        lcl_img = new uint8_t[width * height];
    }


    ~Genicam() override {
        grabber.stop();
    }
};

class LoggingTask: public hh::AbstractTask<1, loggingData[], void> {

    std::ofstream outfile;
    int index = 0;

public:

    LoggingTask(const std::basic_string<char>& name, size_t numberThreads)
            : AbstractTask<1, loggingData[], void>(name, numberThreads) {
        outfile.open ("output.bin", std::ios::binary);
    }

    void execute(std::shared_ptr<loggingData[]> logData) override {
        for(int i = 0; i < 100; i++) {
            outfile.write(reinterpret_cast<char*>(&(logData[i].data["TimeWithWait"])), sizeof(float));
            outfile.write(reinterpret_cast<char*>(&(logData[i].data["TimeTotalNoWait"])), sizeof(float));
            outfile.write(reinterpret_cast<char*>(&(logData[i].data["TimeWaiting"])), sizeof(float));
            outfile.write(reinterpret_cast<char*>(&(logData[i].data["TimeProcess"])), sizeof(float));
            outfile.write(reinterpret_cast<char*>(&(logData[i].data["TimeGrab"])), sizeof(float));
            cv::Mat out;
//            cv::threshold(*logData[i].img,out,150, 255,cv::THRESH_BINARY );
//            if(cv::countNonZero(out) != 0) {
//            cv::imwrite(std::string("../result/").append(std::to_string(index)).append(".jpg"), *logData[i].img);
//            }
            index++;
        }
        outfile.flush();
        sync();

        std::ofstream ofs("/proc/sys/vm/drop_caches");
        ofs << "3" << std::endl;
    }
};

void init() {
    EGenTL genTL;
    gc::TL_HANDLE hTL = genTL.tlOpen();
    gc::IF_HANDLE hIF = genTL.tlOpenInterface(hTL, genTL.tlGetInterfaceID(hTL, 0));
    genTL.genapiExecuteCommand(hIF, "CxpPoCxpTurnOff");
    // Wait for device to turn on
    std::string devID = genTL.ifGetDeviceID(hIF, 0);
    gc::DEV_HANDLE hDEV = genTL.ifOpenDevice(hIF, devID);
    bool devLoaded = false;
    while (!devLoaded) {
        try {
            genTL.devGetInfo<std::string>(hDEV, DEVICE_INFO_VENDOR);
            devLoaded = true;
        } catch (GC_ERROR_LIST) {}
    }
}

int main() {
    cpu_set_t set;
    CPU_ZERO(&set);        // clear cpu mask
    CPU_SET(0, &set); // set cpu 0
    CPU_SET(1, &set); // set cpu 1
    cv::setNumThreads(0);
    sched_setaffinity(0, sizeof(cpu_set_t), &set);
    hh::Graph<1, void, void> graphCamera("Run camera");
    struct sched_param param{};
// Commented out assuming no real time OS
//    param.sched_priority = 50;
    init();
    auto runCamera = std::make_shared<Genicam>("Run camera", 1, hh::Policy::SchedFIFO, &param);
    auto log = std::make_shared<LoggingTask>("Log info", 1);
    graphCamera.inputs(runCamera);
    graphCamera.outputs(log);
    graphCamera.edges(runCamera, log);
    graphCamera.executeGraph(true);
    graphCamera.finishPushingData();
    while(true) {
        if(std::cin.get() == 'q') {
            runCamera->flag = false;
            break;
        }
    }
    graphCamera.waitForTermination();
}
