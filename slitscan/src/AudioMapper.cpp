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
    layout2 = NONE;
    thick = 20;
    gap = 20;
    levelCount = 20;
    useLevelCount = false;
    isScaleOn = false;
    usePerlin = false;
    isNebulaOn = false;
    mapMin = 0.001;
    mapMax = 0.1;
    peakToLengthRatio = 1.0;
    audioMirror = false;
    audioThreshold = 0.5;
    audioPeakDecay = 0.96;
    audioMaxDecay = 0.995;
    bgImage = NULL;
    bg = CAM;
    mic.setup();
    
    textureImg.loadImage("textures/nebula1.jpg");
    
    ofSetCircleResolution(200);
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
    // share controls
    string modeLabels[] = {"NONE", "LEFT_RIGHT", "RIGHT_LEFT", "UP_DOWN", "DOWN_UP", "MIRROR_SIDE_V", "MIRROR_SIDE_H", "MIRROR_CENTRE_V", "MIRROR_CENTRE_H", "SOLID_V", "SOLID_H"};
    RUI_NEW_GROUP("Audio Mapper");
    ofAddListener(RUI_GET_OF_EVENT(), this, &AudioMapper::clientDidSomething);
    // appearance
    RUI_SHARE_ENUM_PARAM(layout, NONE, SOLID_H, modeLabels);
    RUI_SHARE_ENUM_PARAM(layout2, NONE, SOLID_H, modeLabels);
    RUI_SHARE_PARAM(isFadeOn);
    RUI_SHARE_PARAM(isScaleOn);
    // BG drawing mode
    string bgLabels[] = {"GREYSCALE", "GREYSCALE_NOISE", "CAM", "CAM_SLICE_V", "CAM_SLICE_H"};
    RUI_SHARE_ENUM_PARAM(bg, GREYSCALE, CAM_SLICE_H, bgLabels);
    RUI_SHARE_PARAM(isNebulaOn);
    RUI_SHARE_PARAM(usePerlin);
    RUI_SHARE_PARAM(useLevelCount);
    RUI_SHARE_PARAM(levelCount, 1, 99);
    RUI_SHARE_PARAM(thick, 1, 100);
    RUI_SHARE_PARAM(gap, 0, 100);
    RUI_SHARE_PARAM(peakToLengthRatio, 0.1, 1);
    RUI_SHARE_COLOR_PARAM(colour);
    // sampling
    RUI_SHARE_PARAM(audioThreshold, 0.0, 1.0);
    RUI_SHARE_PARAM(audioPeakDecay, 0.9, 1.0);
    RUI_SHARE_PARAM(audioMaxDecay, 0.9, 1.0);
    RUI_SHARE_PARAM(audioMirror);
}
void AudioMapper::update(){
    
    resetLevels();
    
    mic.fftFile.setThreshold(audioThreshold);
    mic.fftFile.setPeakDecay(audioPeakDecay);
    mic.fftFile.setMaxDecay(audioMaxDecay);
    mic.fftFile.setMirrorData(audioMirror);
    mic.fftLive.setThreshold(audioThreshold);
    mic.fftLive.setPeakDecay(audioPeakDecay);
    mic.fftLive.setMaxDecay(audioMaxDecay);
    mic.fftLive.setMirrorData(audioMirror);
    
    mic.update();
    
    
    int n  = levels.size();
    if (!audioMirror) n *= 2;
    float * audioData = new float[n];
    //mic.fftFile.getFftPeakData(audioData, n);
    mic.fftLive.getFftPeakData(audioData, n);
    // populate levels for drawing
    for(int i=0; i<levels.size(); i++) {
        float audioValue = audioData[i];
        levels[i] = audioValue;
    }
    delete[] audioData;
    
}

void AudioMapper::draw(){
    ofSetColor(255);
    if (bg >= CAM && bgImage != NULL) {
        // This is a cam/slitscan texture mode, the bars act as a mask
        bgFbo.begin();
        int rnd = sin(ofGetElapsedTimef());
        // Draw stretched slices of the camera texture or draw the whole thing
        if (bg == CAM_SLICE_V)
            bgImage->drawSubsection(0, 0, width, height, (bgImage->width/2)+(rnd*100), 0, 1, bgImage->height);
        else if (bg == CAM_SLICE_H)
            bgImage->drawSubsection(0, 0, width, height, 0, (bgImage->height/2)+(rnd*100), bgImage->width, 1);
        else
            bgImage->draw(0, 0, width, height);
        bgFbo.end();
        
        // draw bars into mask fbo
        alphaMask.beginMask();
        drawBars(layout);
        if (layout2 != NONE) drawBars(layout2);
        ofSetColor(255);
        alphaMask.endMask();
        // draw camera into contents fbo
        alphaMask.begin();
        bgFbo.draw(0,0);
        alphaMask.end();
        alphaMask.draw();
    }
    else {
        ofEnableDepthTest();
        drawBars(layout);
        if (layout2 != NONE) drawBars(layout2);
        ofSetColor(255);
        ofDisableDepthTest();
    }
    
    //mic.draw();
}

void AudioMapper::drawBars(Layout layout){
    if (!getIsLayoutVertical()) {
        // flip so base tones are at the bottom
        ofPushMatrix();
        ofScale(1, -1);
        ofTranslate(0, -height, 0 );
    }
    
    int x = 0;
    int y = 0;
    float barWidth;
    float barHeight;
    
    for (unsigned int i = 0; i < levels.size(); i++){
        
        barWidth = levels[i]*width;
        barHeight = levels[i]*height;
        
        if (usePerlin) {
            barWidth *= ofNoise(y, ofGetElapsedTimef()) + 0.1;
            barHeight *= ofNoise(x, ofGetElapsedTimef()) + 0.1;
        }
        
        // Set the rectangle(s) to represent the level bar
        // Top to bottom, or bottom to up only
        ofRectangle bar, bar2;
        if (layout == UP_DOWN) {
            bar.set(x, barHeight, thick, -barHeight*peakToLengthRatio);
            x += thick + gap;
        }
        else if (layout == DOWN_UP){
            bar.set(x, height - barHeight, thick, barHeight*peakToLengthRatio);
            x += thick + gap;
        }
        // left to right or right to left only
        else if (layout == LEFT_RIGHT){
            bar.set(barWidth, y, -barWidth*peakToLengthRatio, thick);
            y += thick + gap;
        }
        else if (layout == RIGHT_LEFT){
            bar.set(width-barWidth, y, barWidth*peakToLengthRatio, thick);
            y += thick + gap;
        }
        // MIRROR_SIDE_V, MIRROR_SIDE_H
        // Side mirrors, like teeth
        else if (layout == MIRROR_SIDE_V) {
            bar.set(x, barHeight, thick, -barHeight*peakToLengthRatio);
            bar2.set(x, height - barHeight, thick, barHeight*peakToLengthRatio);
            x += thick + gap;
        }
        else if (layout == MIRROR_SIDE_H) {
            bar.set(barWidth, y, -barWidth*peakToLengthRatio, thick);
            bar2.set(width-barWidth, y, barWidth*peakToLengthRatio, thick);
            y += thick + gap;
        }
        // MIRROR_CENTRE_V, MIRROR_CENTRE_H
        // centre mirrors, like ?
        else if (layout == MIRROR_CENTRE_V) {
            bar.set(x, (height/2)-(barHeight/2), thick, barHeight*0.5*peakToLengthRatio);
            bar2.set(x, (height/2)+(barHeight/2), thick, -barHeight*0.5*peakToLengthRatio);
            x += thick + gap;
        }
        else if (layout == MIRROR_CENTRE_H) {
            bar.set((width/2)+(barWidth/2), y, -barWidth*0.5*peakToLengthRatio, thick);
            bar2.set((width/2)-(barWidth/2), y, barWidth*0.5*peakToLengthRatio, thick);
            y += thick + gap;
        }
        // SOLID_V, SOLID_H
        // solid bars
        else if (layout == SOLID_V){
            bar.set(x, 0, thick, height);
            x += thick + gap;
        }
        else if (layout == SOLID_H){
            bar.set(0, y, width, thick);
            y += thick + gap;
        }
        
        // set colour / brightness
        float brightness = 1.0;
        if (isFadeOn) {
            brightness = ofMap(levels[i], 0, 1, 0.0, 1.0, true);
        }
        if (bg == GREYSCALE_NOISE) {
            ofSetColor( colour * levels[i] * ofRandomuf() );
        }
        else {
            ofSetColor(colour * brightness);
        }
        
        // Draw the bars
        if (isScaleOn) {
            ofPushMatrix();
            ofTranslate(0, 0, ofMap(levels[i], 0, 1, -100, 0));
        }
        if (isNebulaOn) {
            drawNebula(bar, peakToLengthRatio);
            drawNebula(bar2, peakToLengthRatio);
        }
        else {
            ofRect(bar);
            ofRect(bar2);
        }
        if (isScaleOn) ofPopMatrix();
        
        // plus circles?
        /*
         ofPushMatrix();
         ofTranslate(0, 0, 10);
         ofNoFill();
         float halfHeight = height/2;
         if (getIsLayoutVertical()) {
         ofCircle(x-gap, height*.5, ofMap(levels[i], 0.5, 1, 0, thick*6, true));
         }
         else {
         ofCircle(width/2, y+thick/2, ofMap(levels[i], 0, 1, 0, 200));
         }
         //ofCircle(width/2, height/2, ofMap(levels[i], 0, 1, 0, 200));
         ofFill();
         ofPopMatrix();
         */
        
    }
    if (!getIsLayoutVertical()) {
        ofPopMatrix();
    }
    ofSetColor(255);
}


void AudioMapper::drawNebula(ofRectangle rect, float heightPercent){
    if (getIsLayoutVertical()) {
        textureImg.drawSubsection(rect.x, rect.y, rect.width, rect.height,
                                  ofRandom(textureImg.getWidth()-thick), 0, thick, textureImg.getHeight()*heightPercent);
    }
    else {
        textureImg.drawSubsection(rect.x, rect.y, rect.width, rect.height,
                                  0, ofRandom(textureImg.getHeight()-thick), textureImg.getWidth()*heightPercent, thick);
    }
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
        //ofLogNotice() << "Use levels " << levelCount << " thick: " << thick;
        thick = thickDouble * 0.5;
        gap = thickDouble * 0.5;
        if (levelCount == 1) thick = thickDouble;
    }
    else {
        if (getIsLayoutVertical()) {
            n = (width / int(thick + gap)) + 1;
        }
        else {
            n = (height / int(thick + gap)) + 1;
        }
    }
    levels.clear();
    levels.assign(n, 0.0);
}

void AudioMapper::allocateScenes() {
    alphaMask.setup(ofGetWidth(), ofGetHeight());
    bgFbo.allocate(ofGetWidth(), ofGetHeight());
    resetLevels();
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
                //resetLevels();
                break;
        case CLIENT_DID_SET_PRESET:
            break;
        default:
            break;
    }
}

void AudioMapper::keyPressed(int key){
    switch (key) {
        case 'a':
            break;
            
        default:
            break;
    }
}