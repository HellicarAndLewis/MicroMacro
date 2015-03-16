//
//  AudioMapper.cpp
//  slitscan
//
//  Created by Chris on 11/03/2015.
//
//

#include "AudioMapper.h"

void AudioMapper::setup(){
    
    layout = DOWN_UP;
    thick = 20;
    gap = 20;
    levelCount = 20;
    useLevelCount = false;
    easeIn = 0.9;
    easeOut = 0.1;
    mapMin = 0.001;
    mapMax = 0.1;
    resetLevels();
    mic.setup();
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
    // share controls
    string modeLabels[] = {"LEFT_RIGHT", "RIGHT_LEFT", "UP_DOWN", "DOWN_UP", "MIRROR_SIDE_V", "MIRROR_SIDE_H", "MIRROR_CENTRE_V", "MIRROR_CENTRE_H", "SOLID_V", "SOLID_H"};
	RUI_NEW_GROUP("Audio Mapper");
    ofAddListener(RUI_GET_OF_EVENT(), this, &AudioMapper::clientDidSomething);
	RUI_SHARE_ENUM_PARAM(layout, LEFT_RIGHT, SOLID_H, modeLabels);
    RUI_SHARE_PARAM(isFadeOn);
    
    RUI_SHARE_PARAM(mapMin, 0, 1);
    RUI_SHARE_PARAM(mapMax, 0, 1);
    
    RUI_SHARE_PARAM(useLevelCount);
    RUI_SHARE_PARAM(levelCount, 1, 99);
    RUI_SHARE_PARAM(thick, 1, 100);
    RUI_SHARE_PARAM(gap, 0, 100);
	RUI_SHARE_PARAM(easeIn, 0.0, 1.0);
	RUI_SHARE_PARAM(easeOut, 0.0, 1.0);
    RUI_SHARE_COLOR_PARAM(colour);
}
void AudioMapper::update(){
    mic.update();
    for (unsigned int i = 0; i < levels.size(); i++){
        int index = ofMap(i, 0, levels.size(), 0, mic.left.size());
        float rate = (mic.left[index] > levels[i]) ? easeIn : easeOut;
        float level = ofMap(mic.left[index], mapMin, mapMax, 0, 1);
        levels[i] = ofLerp(levels[i], level, rate);
    }
}
void AudioMapper::draw(){
    int x = 0;
    int y = 0;
    for (unsigned int i = 0; i < levels.size(); i++){
        
        // set colour
        if (isFadeOn) {
            ofSetColor(colour * levels[i]);
        }
        else {
            ofSetColor(colour);
        }
        
        // Top to bottom, or bottom to up only
        if (layout == UP_DOWN) {
            ofRect(x, height, thick, -levels[i]*height);
            x += thick + gap;
        }
        else if (layout == DOWN_UP){
            ofRect(x, 0, thick, levels[i]*height);
            x += thick + gap;
        }
        
        // left to right or right to left only
        else if (layout == LEFT_RIGHT){
            ofRect(0, y, levels[i]*width, thick);
            y += thick + gap;
        }
        else if (layout == RIGHT_LEFT){
            ofRect(width, y, -levels[i]*width, thick);
            y += thick + gap;
        }
        
        // MIRROR_SIDE_V, MIRROR_SIDE_H, MIRROR_CENTRE_V, MIRROR_CENTRE_H, SOLID_V, SOLID_H
        // Side mirrors, like teeth
        else if (layout == MIRROR_SIDE_V) {
            ofRect(x, height, thick, -levels[i]*height);
            ofRect(x, 0, thick, levels[i]*height);
            x += thick + gap;
            
        }
        else if (layout == MIRROR_SIDE_H) {
            ofRect(0, y, levels[i]*width, thick);
            ofRect(width, y, -levels[i]*width, thick);
            y += thick + gap;
            
        }
        
        // centre mirrors, like ?
        else if (layout == MIRROR_CENTRE_V) {
            ofRect(x, height/2, thick, -levels[i]*height/2);
            ofRect(x, height/2, thick, levels[i]*height/2);
            x += thick + gap;
            
        }
        else if (layout == MIRROR_CENTRE_H) {
            ofRect(width/2, y, levels[i]*width/2, thick);
            ofRect(width/2, y, -levels[i]*width/2, thick);
            y += thick + gap;
            
        }
        
        // solid
        else if (layout == SOLID_V){
            ofRect(x, 0, thick, height);
            x += thick + gap;
        }
        else if (layout == SOLID_H){
            ofRect(0, y, width, thick);
            y += thick + gap;
        }
        
    }
    ofSetColor(255);
}

void AudioMapper::resetLevels(){
    width = ofGetWidth();
    height = ofGetHeight();
    int n;
    if (useLevelCount) {
        int max = height;
        if (getIsLayoutVertical()) max = width;
        n = levelCount;
        float thickDouble = (float)max/(float)n;
        ofLogNotice() << "Use levels " << levelCount << " thick: " << thick;
        thick = thickDouble * 0.5;
        gap = thickDouble * 0.5;
        if (levelCount == 1) thick = thickDouble;
    }
    else {
        if (layout == UP_DOWN || layout == DOWN_UP) {
            n = width / int(thick + gap);
        }
        else {
            n = height / int(thick + gap);
        }
    }
    levels.clear();
    levels.assign(n, 0.0);
}

bool AudioMapper::getIsLayoutVertical(){
    if (layout==SOLID_V || layout==UP_DOWN || layout==DOWN_UP || layout==MIRROR_CENTRE_V || layout==MIRROR_SIDE_V) {
        return true;
    }
    else {
        return false;
    }
}

void AudioMapper::clientDidSomething(RemoteUIServerCallBackArg &arg){
	switch (arg.action) {
		case CLIENT_UPDATED_PARAM:
            ofLogVerbose() << "CLIENT_UPDATED_PARAM: " << arg.paramName << " - " << arg.param.getValueAsString();
            if (arg.paramName == "layout" || arg.paramName == "thick" || arg.paramName == "gap" || arg.paramName=="useLevelCount" || arg.paramName=="levelCount")
                resetLevels();
			break;
		default:
			break;
	}
}