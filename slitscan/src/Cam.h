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

// Cam class handles switching between blackmagic input and standard video grabber
// It also generates optical flow textures and data
// TODO: extend and tidy optical flow stuff, test blackmagic!
class Cam {
public:
    
    Cam();
    ~Cam();
    
	void setup();
	void setup(int w, int h, float cvRatio);
	void update();
	void draw(int x=0, int y=0);
	void draw(int x, int y, int w, int h);
    void drawFlow();
    void drawFlow(int w, int h);
    void drawGrey();
    void drawDebug();
	void keyPressed(int key);
    
    // Cam specific
    void startCapture();
    void stopCapture();
    ofColor getColourAt(int x, int y);
    float getFlowGreyAt(int x, int y);
    
    
    // Blackmagic grabber
    ofxBlackMagic blackmagic;
    bool defaultBlackmagic;
    
    // Standard oF video grabber
    ofVideoGrabber vidGrabber;
    float vidGrabberDeviceId;
    
    // grab paramters
    float camWidth, camHeight, cvWidth, cvHeight, cvRatio;
    bool doDrawGrey, doDrawFlow, doDrawDebug;
    bool isCapturing, useBlackmagic, doHMirror;
    
    
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
    float flowSize, blur, opticalFlowSensitivity, opticalFlowSmoothing;
	bool firstFrame;
    bool doFlow, doFlowLk;
    bool isFrameNew;
    
    
    // Open CV / countour analysis
    ofxCvContourFinder 	contourFinder;
    ofxCvGrayscaleImage greyBG;
    ofxCvGrayscaleImage greyDiff;
    float threshold, minBlob, maxBlob;
    bool doContour;
    bool bLearnBakground;
    bool doDrawContour, doDrawBg;
    
    ofVideoPlayer videoPlayer;
    bool useVideoPlayer;
    
    
private:
    
};