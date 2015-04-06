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
    isAlphaOn = false;
    isFadeOn = true;
    audioMirror = false;
    particleMode = false;
    mapMin = 0.001;
    mapMax = 0.1;
    perlinMulti = 1.0;
    peakToLengthRatio = 1.0;
    audioThreshold = 0.5;
    audioPeakDecay = 0.96;
    audioMaxDecay = 0.995;
    bg = CAM;
    mic.setup();
    particleVel = 20;
    particleLength = 20;
    particleThreshold = 0.4;
    particleDecayFrames = 100;
    particleDamping = 0.01;
    ofEnableAlphaBlending();
    
    textureImg.loadImage("textures/nebula1.jpg");
    
    ofDirectory dir;
    int nFiles = dir.listDir("audioBGs");
    if(nFiles) {
        for(int i=0; i<dir.numFiles(); i++) {
            // add the image to the vector
            string filePath = dir.getPath(i);
            bgImages.push_back(ofImage());
            bgImages.back().loadImage(filePath);
            
        }
    }
    else ofLogError("Couldn't find audio mapper images folder");
    bgImagesIndex = 0;
    
    particleSystem.setup(3000);
    ofSetCircleResolution(200);
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
    // share controls
    string modeLabels[] = {"NONE", "LEFT_RIGHT", "RIGHT_LEFT", "UP_DOWN", "DOWN_UP", "MIRROR_SIDE_V", "MIRROR_SIDE_H", "MIRROR_CENTRE_V", "MIRROR_CENTRE_H", "SOLID_V", "SOLID_H"};
    RUI_NEW_GROUP("Audio Mapper");
    ofAddListener(RUI_GET_OF_EVENT(), this, &AudioMapper::clientDidSomething);
    // appearance
    RUI_SHARE_ENUM_PARAM(layout, NONE, SOLID_H, modeLabels);
    RUI_SHARE_ENUM_PARAM(layout2, NONE, SOLID_H, modeLabels);
    RUI_SHARE_PARAM(isAlphaOn);
    RUI_SHARE_PARAM(isScaleOn);
    
    // BG drawing mode
    string bgLabels[] = {"GREYSCALE", "GREYSCALE_NOISE", "CAM", "CAM_SLICE_V", "CAM_SLICE_H", "IMG", "IMG_SLICE_V", "IMG_SLICE_H"};
    RUI_SHARE_ENUM_PARAM(bg, GREYSCALE, IMG_SLICE_H, bgLabels);
    RUI_SHARE_PARAM(bgImagesIndex, 0, nFiles-1);
    RUI_SHARE_PARAM(isNebulaOn);
    RUI_SHARE_PARAM(usePerlin);
    RUI_SHARE_PARAM(perlinMulti, 0.001, 2.0);
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
    
    RUI_NEW_GROUP("Audio Mapper Particles");
    RUI_SHARE_PARAM(particleMode);
    RUI_SHARE_PARAM(particleVel, 1, 40);
    RUI_SHARE_PARAM(particleLength, 1, 400);
    RUI_SHARE_PARAM(particleThreshold, 0, 1);
    RUI_SHARE_PARAM(particleDecayFrames, 1, 300);
    RUI_SHARE_PARAM(particleDecayFrames, 1, 300);
    RUI_SHARE_PARAM(particleDamping, 0.01, 0.08);
    
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
        previousLevels[i] = levels[i];
        float audioValue = audioData[i];
        levels[i] = audioValue;
    }
    delete[] audioData;
    
    if (particleMode) {
        updateParticles();
    }
}

void AudioMapper::draw(){
    ofSetColor(255);
    if (bg >= CAM) {
        ofTexture * img = &bgImage;
        if (bg >= IMG) {
            img = &bgImages[bgImagesIndex].getTextureReference();
        }
        // This is a cam/slitscan texture mode, the bars act as a mask
        bgFbo.begin();
        ofSetColor(colour);
        int rnd = sin(ofGetElapsedTimef());
        // Draw stretched slices of the camera texture or draw the whole thing
        if (bg == CAM_SLICE_V || bg == IMG_SLICE_V)
            img->drawSubsection(0, 0, width, height, (img->getWidth()/2)+(rnd*500), 0, 1, img->getHeight());
        else if (bg == CAM_SLICE_H || bg == IMG_SLICE_H)
            img->drawSubsection(0, 0, width, height, 0, (img->getHeight()/2)+(rnd*300), img->getWidth(), 1);
        else
            img->draw(0, 0, width, height);
        ofSetColor(255);
        bgFbo.end();
        
        // draw bars into mask fbo
        alphaMask.beginMask();
        if (particleMode) {
            drawParticles(layout);
        }
        else {
            drawBars(layout);
            if (layout2 != NONE) drawBars(layout2);
        }
        ofSetColor(255);
        alphaMask.endMask();
        // draw camera into contents fbo
        alphaMask.begin();
        bgFbo.draw(0,0);
        alphaMask.end();
        alphaMask.draw();
    }
    else {
        if (particleMode) {
            drawParticles(layout);
        }
        else {
            drawBars(layout);
            if (layout2 != NONE) drawBars(layout2);
        }
        ofSetColor(255);
    }
    
}

void AudioMapper::updateParticles(){
    int x = 0;
    int y = 0;
    float barWidth;
    float barHeight;
    float time = ofGetElapsedTimef();
    ofVec2f direction, direction2;
    ofVec2f pos, pos2;
    
    for (unsigned int i = 0; i < levels.size(); i++){
        
        barWidth = levels[i]*width*.4;
        barHeight = levels[i]*height*.4;
        
        // Set the rectangle(s) to represent the level bar
        // Top to bottom, or bottom to up only
        ofRectangle bar, bar2;
        bar2.setHeight(0);
        if (layout == UP_DOWN) {
            direction.set(0, 1);
            bar.set(x, 0, thick, barHeight);
            bar.translateY(-particleLength);
            pos.set(x, 0);
            x += thick + gap;
        }
        else if (layout == DOWN_UP){
            direction.set(0, -1);
            bar.set(x, height-barHeight, thick, barHeight);
            bar.translateY(particleLength);
            pos.set(x, height-particleLength);
            x += thick + gap;
        }
        // left to right or right to left only
        else if (layout == LEFT_RIGHT){
            direction.set(1, 0);
            bar.set(0, y, barWidth-particleLength, thick);
            bar.translateX(-particleLength);
            pos.set(0, y);
            y += thick + gap;
        }
        else if (layout == RIGHT_LEFT){
            direction.set(-1, 0);
            bar.set(width-barWidth, y, barWidth, thick);
            bar.translateX(particleLength);
            pos.set(width-particleLength, y);
            y += thick + gap;
        }
        // MIRROR_SIDE_V, MIRROR_SIDE_H
        // Side mirrors, like teeth
        else if (layout == MIRROR_SIDE_V) {
            direction.set(0, 1);
            bar.set(x, 0, thick, barHeight);
            bar.translateY(-particleLength);
            pos.set(x, 0);
            direction2.set(0, -1);
            bar2.set(x, height/2, thick, barHeight);
            bar2.translateY(particleLength);
            pos2.set(x, height-particleLength);
            x += thick + gap;
        }
        else if (layout == MIRROR_SIDE_H) {
            direction.set(1, 0);
            bar.set(0, y, barWidth-particleLength, thick);
            bar.translateX(-particleLength);
            pos.set(0, y);
            direction2.set(-1, 0);
            bar2.set(width-barWidth, y, barWidth, thick);
            bar2.translateX(particleLength);
            pos2.set(width-particleLength, y);
            y += thick + gap;
        }
        // MIRROR_CENTRE_V, MIRROR_CENTRE_H
        // centre mirrors, like ?
        else if (layout == MIRROR_CENTRE_V || layout == SOLID_V) {
            direction.set(0, -1);
            bar.set(x, 0, thick, barHeight/2);
            bar.translateY(particleLength);
            pos.set(x, (height/2)-particleLength);
            
            direction2.set(0, 1);
            bar2.set(x, height/2, thick, barHeight/2);
            bar2.translateY(-particleLength);
            pos2.set(x, height/2);
            
            x += thick + gap;
        }
        else if (layout == MIRROR_CENTRE_H || layout == SOLID_H) {
            direction.set(1, 0);
            bar.set(width/2, y, barWidth/2, thick);
            bar.translateX(-particleLength);
            pos.set(width/2, y);
            
            direction2.set(-1, 0);
            bar2.set((width/2)-(barWidth/2), y, barWidth/2, thick);
            bar2.translateX(particleLength);
            pos2.set((width/2)-particleLength, y);
            
            y += thick + gap;
        }
        
        // Birth particles
        bar.scaleFromCenter(1.2);
        bar2.scaleFromCenter(1.2);
        if (levels[i] > (previousLevels[i] + particleThreshold)) {
            Particle* p = particleSystem.birth(pos, direction * particleVel);
            if (p != NULL) {
                p->bounds.set(bar.x, bar.y, bar.getWidth(), bar.getHeight());
                if (getIsLayoutVertical())
                    p->shape.set(0, 0, thick, particleLength);
                else
                    p->shape.set(0, 0, particleLength, thick);
            }
            if (bar2.height != 0) {
                p = particleSystem.birth(pos2, direction2 * particleVel);
                if (p != NULL) {
                    p->bounds.set(bar2.x, bar2.y, bar2.getWidth(), bar2.getHeight());
                    if (getIsLayoutVertical())
                        p->shape.set(0, 0, thick, particleLength);
                    else
                        p->shape.set(0, 0, particleLength, thick);
                }
            }
        }
        
    }
    
    // reset forces on particles
    particleSystem.resetForces();
    particleSystem.updatePool();
    for(int i = 0; i < particleSystem.size(); i++) {
        Particle* p = particleSystem[i];
        p->maxDying = particleDecayFrames;
        p->damping = particleDamping;
        p->addDampingForce();
        if (!p->bounds.inside(p->position) || p->velocity.length() < 0.001){
            p->setDying();
        }
    }
    particleSystem.update();
    if (!getIsLayoutVertical()) {
        ofPopMatrix();
    }
    
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
    float time = ofGetElapsedTimef();
    
    for (unsigned int i = 0; i < levels.size(); i++){
        
        barWidth = levels[i]*width;
        barHeight = levels[i]*height;
        
        if (usePerlin) {
            barWidth *= ofNoise(y, time*perlinMulti) + 0.1;
            barHeight *= ofNoise(x, time*perlinMulti) + 0.1;
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
        float alpha = 1.0;
        ofColor tempColour = (bg < CAM) ? colour : ofColor(255);
        if (isFadeOn) brightness = ofMap(levels[i], 0, 1, 0.0, 1.0, true);
        if (isAlphaOn) alpha = levels[i];
        if (bg == GREYSCALE_NOISE) {
            ofSetColor(tempColour * levels[i] * ofRandomuf(), alpha*255 );
        }
        else {
            ofSetColor(tempColour*brightness, alpha*255);
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
            if (!particleMode) {
                ofRect(bar);
                ofRect(bar2);
            }
        }
        if (isScaleOn) ofPopMatrix();
        
    }
    if (!getIsLayoutVertical()) {
        ofPopMatrix();
    }
    
    ofSetColor(255);
}

void AudioMapper::drawParticles(Layout layout) {
    if (!getIsLayoutVertical()) {
        // flip so base tones are at the bottom
        ofPushMatrix();
        ofScale(1, -1);
        ofTranslate(0, -height, 0 );
    }
    for (int i = 0; i < particleSystem.size(); i++) {
        Particle* p = particleSystem[i];
        p->maxDying = particleDecayFrames;
        float rate = ofMap(p->dying, 0, p->maxDying, 1, 0, true);
        if (p->state != Particle::DEAD) {
            //ofNoFill();
            //ofSetColor(255, 0, 0);
            //ofRect(p->bounds);
            //ofFill();
            ofSetColor(255, 255 * rate);
            p->draw();
        }
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
    if (n != levels.size()) {
        levels.clear();
        levels.assign(n, 0.0);
        previousLevels.clear();
        previousLevels.assign(n, 0.0);
    }
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