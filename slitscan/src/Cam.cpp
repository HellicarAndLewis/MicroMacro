//
//  Cam.cpp
//  slitscan
//
//  Created by Chris on 09/03/2015.
//
//


#include "Cam.h"

Cam::Cam() {
    // defaults
    isCapturing = false;
    useBlackmagic = false;
    useVideoPlayer = false;
    isFrameNew = false;
    doFlow = true;
    camWidth = BLACKMAGIC_W;
	camHeight = BLACKMAGIC_H;
    cvRatio = .3;
    flowSize = 1;
    blur = 3;
    opticalFlowSensitivity = .99;
    opticalFlowSmoothing = .5;
    vidGrabberDeviceId = 0;
}
Cam::~Cam() {
    blackmagic.close();
}

void Cam::startCapture(){
    bool success = false;
    // try the blackmagic first, always grab in HD
    if (isCapture720) {
        success = blackmagic.setup(BLACKMAGIC720_W, BLACKMAGIC720_H, 60);
        camWidth = BLACKMAGIC720_W;
        camHeight = BLACKMAGIC720_H;
    }
    else {
        success = blackmagic.setup(BLACKMAGIC_W, BLACKMAGIC_H, 30);
        camWidth = BLACKMAGIC_W;
        camHeight = BLACKMAGIC_H;
    }
    if (success) {
        ofLogNotice() << "\n\nUsing blackmagic!";
        useBlackmagic = true;
    }
    else {
        useBlackmagic = false;
        ofLogNotice() << "\n\nBlackmagic not present, using ofVideoGrabber instead";
        vector<ofVideoDevice> devices = vidGrabber.listDevices();
        for (int i = 0; i < devices.size(); i++) {
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
            if ( devices[i].bAvailable ) {
                ofLogNotice() << endl;
            }
            else {
                ofLogNotice() << " - unavailable " << endl;
            }
        }
        vidGrabber.setDeviceID(int(vidGrabberDeviceId));
        camWidth = WEBCAM_W;
        camHeight = WEBCAM_H;
        success = vidGrabber.initGrabber(WEBCAM_W, WEBCAM_H);
    }
    isCapturing = true;
}
void Cam::stopCapture(){
    if (useBlackmagic) blackmagic.close();
    else vidGrabber.close();
    isCapturing = false;
    colorImage.clear();
    thisGrayImage.clear();
    lastGrayImage.clear();
    opticalFlowLk.reset();
    flowX.clear();
    flowY.clear();
    flowCombined.clear();
}

void Cam::setup(bool isCapture720, float cvRatio){
    
    this->isCapture720 = isCapture720;
    
    // Start capturing first to work out camera resultion
    startCapture();
    
    
    ofLogNotice() << "Setting up camera with h:" << camWidth << " h:" << camHeight << " cvRatio:" << cvRatio << " using black magic: " << useBlackmagic;
    
    this->cvRatio = cvRatio;
    cvWidth = camWidth*cvRatio;
    cvHeight = camHeight*cvRatio;
    
    // CV images
    colorImage.allocate(camWidth,camHeight);
	thisGrayImage.allocate(camWidth,camHeight);
    lastGrayImage.allocate(camWidth,camHeight);
    
    delayMap.allocate(camWidth, camHeight);
    
    // optical flow
    opticalFlowLk.allocate(cvWidth, cvHeight);
    flowX.allocate(cvWidth, cvHeight);
    flowY.allocate(cvWidth, cvHeight);
    flowCombined.allocate(cvWidth, cvHeight);
    flow.allocate(camWidth, camHeight);
    
}

void Cam::update() {
    
    if (!isCapturing) return;
    
    if (useVideoPlayer) {
        if (!videoPlayer.isLoaded()) {
            videoPlayer.loadMovie("test.mp4");
            camWidth = videoPlayer.getWidth();
            camHeight = videoPlayer.getHeight();
            setup();
            videoPlayer.play();
        }
        videoPlayer.update();
        isFrameNew = videoPlayer.isFrameNew();
    }
    else if (useBlackmagic) {
        isFrameNew = blackmagic.update();
    }
    else {
        vidGrabber.update();
        isFrameNew = vidGrabber.isFrameNew();
    }
	
    
	if ((doFlow) && isFrameNew){
        
        if (useVideoPlayer) colorImage.setFromPixels(videoPlayer.getPixels(), camWidth, camHeight);
        else if (useBlackmagic) colorImage.setFromPixels(blackmagic.getColorPixels());
        else colorImage.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight);
        
		lastGrayImage = thisGrayImage;
		thisGrayImage = colorImage;
        ofxCvGrayscaleImage tempGrey = thisGrayImage;
        ofxCvGrayscaleImage tempGreyLast = lastGrayImage;
        tempGrey.resize(cvWidth, cvHeight);
        tempGreyLast.resize(cvWidth, cvHeight);
        
        // optical flow processing
        if (doFlow && flowSize > 0) {
            if(flowSize % 2  == 0) --flowSize;
            opticalFlowLk.calc(tempGreyLast, tempGrey, (int)flowSize);

            cvAddWeighted(
                          opticalFlowLk.vel_x.getCvImage(), (1 - opticalFlowSmoothing)*opticalFlowSensitivity,
                          flowX.getCvImage(), opticalFlowSmoothing, 0,
                          flowX.getCvImage()
                          );
            flowX.flagImageChanged();
            
            cvAddWeighted(
                          opticalFlowLk.vel_y.getCvImage(), (1 - opticalFlowSmoothing)*opticalFlowSensitivity,
                          flowY.getCvImage(), opticalFlowSmoothing, 0,
                          flowY.getCvImage()
                          );
            flowY.flagImageChanged();
            
            cvAddWeighted(
                          flowX.getCvImage(), 1,
                          flowY.getCvImage(), 1, 0,
                          flowCombined.getCvImage()
                          );
            //flowCombined.dilate();
            flowCombined.blur((int)blur);
            //flowCombined.erode();
            //flowCombined.erode();
            flowCombined.flagImageChanged();
            
            flow.begin();
            flowCombined.draw(0, 0, camWidth, camHeight);
            flow.end();
            
            ofxCvFloatImage tempFlow = flowCombined;
            tempFlow.resize(camWidth, camHeight);
            delayMap.setFromFloatImage(tempFlow);
        }
        
        // update textures for drawing
        colorImage.updateTexture();
        thisGrayImage.updateTexture();
        delayMap.updateTexture();
	}
    
}

void Cam::draw(int x, int y) {
    if (useBlackmagic) blackmagic.drawColor();
    else vidGrabber.draw(x, y);
}

void Cam::drawFlow(){
    flow.draw(0, 0);
}

void Cam::drawFlowDebug(int w, int h){
    if (doFlow) {
        flowX.draw(0, 0, w/3, h/3);
        flowY.draw(w/3, 0, w/3, h/3);
        flow.draw(0, h/3, w/3, h/3);
    }
    
}

void Cam::drawGrey(){
    thisGrayImage.draw(0, 0);
}

void Cam::drawDebug(){
    drawFlowDebug(camWidth, camHeight);
    ofSetColor(255);
}


ofPixels& Cam::getImage(){
    if (useBlackmagic) {
        return blackmagic.getColorPixels();
    }
    else {
        return vidGrabber.getPixelsRef();
    }
}

ofColor Cam::getColourAt(int x, int y){
    if (!isCapturing || colorImage.getWidth()<1) return;
    unsigned char * pixels = colorImage.getPixels();
    ofColor c;
    // find the smallest value out of those two so we don't crash if we click outside of the camera image
    x = MIN(x,colorImage.width-1);
    y = MIN(y,colorImage.height-1);
    int index = y*colorImage.width*3 + x*3;
    if (index < 0) index = 0;
    c.r = pixels[index];
    c.g = pixels[index+1];
    c.b = pixels[index+2];
    return c;
}

float Cam::getFlowGreyAt(int x, int y){
    if (!isCapturing || flowX.getWidth()<1) return;
    unsigned char * pixels = flowX.getPixels();
    // find the smallest value out of those two so we don't crash if we click outside of the camera image
    x = MIN(x,flowX.getWidth()-1);
    y = MIN(y,flowX.getHeight()-1);
    int index = y*flowX.getWidth() + x;
    if (index < 0) index = 0;
    float grey = pixels[index];
    return grey;
}

//////////////////////////////////////////////////////////////////////////////////
// oF event handlers
//////////////////////////////////////////////////////////////////////////////////

void Cam::keyPressed (int key) {
}

//////////////////////////////////////////////////////////////////////////////////
// private
//////////////////////////////////////////////////////////////////////////////////
