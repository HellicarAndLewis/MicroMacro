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
// so far using left channel buffer to draw bars horizontally or vertically
// TODO: add debug draws full rects, add colour setting, add mode using mic volume instead of left channel?
class AudioMapper {
public:
    
    enum Layout {
        LEFT_RIGHT, RIGHT_LEFT, UP_DOWN, DOWN_UP, MIRROR_SIDE_V, MIRROR_SIDE_H, MIRROR_CENTRE_V, MIRROR_CENTRE_H, SOLID_V, SOLID_H
    };
    Layout layout;
    
    bool isFadeOn;
    
	void setup();
    void update();
    void draw();
    void resetLevels();
    void clientDidSomething(RemoteUIServerCallBackArg & arg);
    
    Mic mic;
    int bufferSize;
    vector<float> levels;
    int width;
    int height;
    float thick;
    float gap;
    float easeIn;
    float easeOut;
    ofColor colour;
    
    float mapMin, mapMax;
    
};