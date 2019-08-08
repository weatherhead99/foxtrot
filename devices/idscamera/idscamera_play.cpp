#include <iostream>
#include "idscamera.h"
#include <foxtrot/Logging.h>
#include <chrono>
#include <thread>
#include <ueye.h>

using std::cout;
using std::endl;

int main(int argc,char** argv)
{
    foxtrot::setLogFilterLevel(sl::trace);
    
    cout << "idscamera test..." << endl;
    
    const uint32_t cameraID = 4; // This ID appears in the ids camera manager when the camera is connected.
    foxtrot::devices::idscamera idscam(&cameraID);
    
    idscam.setExposure(0.01);
    cout << "Exposure time = " << idscam.getExposure() << endl;
    //idscam.setPixelClock(20);
    cout << "Pixel Clock = " << idscam.getPixelClock() << endl;
    //idscam.setFrameRate(16);
    
    cout << "Taking an image..." << endl;
    
    //idscam.waitEvent(100,IS_SET_EVENT_FRAME);
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // THIS CODE WORKS BUT IT IS USEFUL FOR VIDEOS, NOT FOR SINGLE PHOTOS
    /*char ** charpp;
    int seqID = 0;

    for(int i=0 ; i < 10; i++)
    {
        //idscam.waitEvent(1000);
        auto imptr = std::make_shared<foxtrot::devices::Image>(1280, 1024, 8);
        idscam.AddImageToSequence(imptr);
        
    };
    foxtrot::devices::Image im_single = idscam.getSingleImage();
    idscam.camImage = foxtrot::devices::Image(im_single);
    idscam.printoutImage();*/
    
    //USING JUST THE SINGLE IMAGE
    /*idscam.getSingleImageAlone();
    idscam.printoutImage();*/
    
    
    //USING THE METADATA AND RAWDATA FUNCTIONS FOR FOXTROT
    /*idscam.getSingleImageAlone();
    foxtrot::devices::metadata meta = idscam.getImageMetadata();
    cout << meta.width << endl;
    
    std::vector<int> raw = idscam.getImageRawData();
    cout << raw[100] << endl;*/
    
    cout << "Test finished." << endl;
}
