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
    easeIn = 0.9;
    easeOut = 0.1;
    resetLevels();
    mic.setup();
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
    // share controls
    string modeLabels[] = {"LEFT_RIGHT", "RIGHT_LEFT", "UP_DOWN", "DOWN_UP"};
	RUI_NEW_GROUP("Audio Mapper");
	RUI_SHARE_ENUM_PARAM(layout, LEFT_RIGHT, DOWN_UP, modeLabels);
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
        levels[i] = ofLerp(levels[i], mic.left[index], rate);
    }
}
void AudioMapper::draw(){
    int x = 0;
    int y = 0;
    ofSetColor(colour);
    for (unsigned int i = 0; i < levels.size(); i++){
        if (layout == UP_DOWN) {
            ofRect(x, height, thick, -levels[i]*height);
            x += thick + gap;
        }
        else if (layout == DOWN_UP){
            ofRect(x, 0, thick, levels[i]*height);
            x += thick + gap;
        }
        else if (layout == LEFT_RIGHT){
            ofRect(0, y, levels[i]*width, thick);
            y += thick + gap;
        }
        else if (layout == RIGHT_LEFT){
            ofRect(width, y, -levels[i]*width, thick);
            y += thick + gap;
        }
        
    }
    ofSetColor(255);
}

void AudioMapper::resetLevels(){
    width = ofGetWidth();
    height = ofGetHeight();
    int n;
    if (layout == UP_DOWN || layout == DOWN_UP) {
        n = width / int(thick + gap);
    }
    else {
        n = height / int(thick + gap);
    }
    levels.clear();
    levels.assign(n, 0.0);
}

void AudioMapper::clientDidSomething(RemoteUIServerCallBackArg &arg){
	switch (arg.action) {
		case CLIENT_UPDATED_PARAM:
            ofLogVerbose() << "CLIENT_UPDATED_PARAM: " << arg.paramName << " - " << arg.param.getValueAsString();
            if (arg.paramName == "layout" || arg.paramName == "thick" || arg.paramName == "gap")
                resetLevels();
			break;
		default:
			break;
	}
}