//
//  Cam.h
//  slitscan
//
//  Created by Chris on 09/03/2015.
//
//

#pragma once
#include "ofxBlackmagic.h"
#include "ofxCvOpticalFlowLK.h"
#include "ofxOpenCv.h"

#define BLACKMAGIC_W 1920
#define BLACKMAGIC_H 1080
#define BLACKMAGIC720_W 1280
#define BLACKMAGIC720_H 720
#define WEBCAM_W 1280
#define WEBCAM_H 720

// Cam class handles switching between blackmagic input and standard video grabber
// It also generates optical flow textures and data
class Cam {
public:
    
    Cam();
    ~Cam();
    
	void setup(bool isCapture1080=false, float cvRatio=0.3);
	void update();
	void draw(int x=0, int y=0);
    void drawFlow();
    void drawFlowDebug(int w, int h);
    void drawGrey();
    void drawDebug();
	void keyPressed(int key);
    
    // Cam specific
    void startCapture();
    void stopCapture();
    ofPixels& getImage();
    ofColor getColourAt(int x, int y);
    float getFlowGreyAt(int x, int y);
    
    
    // Blackmagic grabber
    ofxBlackMagic blackmagic;
    
    // Standard oF video grabber
    ofVideoGrabber vidGrabber;
    float vidGrabberDeviceId;
    
    // grab paramters
    bool isCapture1080;
    float camWidth, camHeight, cvWidth, cvHeight, cvRatio;
    bool isCapturing, useBlackmagic;
    
    
    // Open CV / Optical flow
    ofxCvGrayscaleImage thisGrayImage;
	ofxCvGrayscaleImage lastGrayImage;
	ofxCvColorImage opticalFlowDraw;
	ofxCvOpticalFlowLK opticalFlowLk;
    ofxCvFloatImage flowX;
    ofxCvFloatImage flowY;
    ofxCvFloatImage flowCombined;
    ofFbo flow;
	ofxCvColorImage colorImage;
    bool doFlow;
    bool isFrameNew;
    
    // optical flow settings
    int flowSize, blur;
    float opticalFlowSensitivity, opticalFlowSmoothing;
    
    ofxCvGrayscaleImage delayMap;
    
    ofVideoPlayer videoPlayer;
    bool useVideoPlayer;
    
    
private:
    
};