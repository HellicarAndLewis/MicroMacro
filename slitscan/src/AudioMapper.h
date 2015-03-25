//
//  AudioMapper.h
//  slitscan
//
//  Created by Chris on 11/03/2015.
//
//

#pragma once
#include "Mic.h"
#include "ofxRemoteUIServer.h"

// Takes audio input and visualises it in different ways
class AudioMapper {
public:
    
    // visual states
    enum Layout {
        LEFT_RIGHT, RIGHT_LEFT, UP_DOWN, DOWN_UP, MIRROR_SIDE_V, MIRROR_SIDE_H, MIRROR_CENTRE_V, MIRROR_CENTRE_H, SOLID_V, SOLID_H
    };
    Layout layout;
    bool isFadeOn;
    
    // mic provides actual sound input
    Mic mic;
    
    // sound input is mapped to visual levels
    // number of levels is determined directly
    // or by specifying a per level thickness and gap
    vector<float> levels;
    int levelCount;
    bool useLevelCount;
    int width;
    int height;
    float thick;
    float gap;
    float easeIn;
    float easeOut;
    ofColor colour;
    
    // audio sample settings
    float mapMin, mapMax;
    float audioThreshold;
    float audioPeakDecay;
    float audioMaxDecay;
    bool audioMirror;
    
    void setup();
    void update();
    void draw();
    void resetLevels();
    bool getIsLayoutVertical();
    void clientDidSomething(RemoteUIServerCallBackArg & arg);
    
    
};