//
//  SlitScan.cpp
//  slitscan
//
//  Created by Chris on 11/03/2015.
//
//

#include "SlitScan.h"

void SlitScan::setup(){
    
    // defaults
    sliceThickness = 20;
    sliceOffset = 10;
    slitScanTimeWidth = 30;
    slitScanTimeDelay = 0;
    sliceVertical = true;
    sliceWeave = false;
    
    doAudioMap = true;
    audioMap = NULL;
    
    cam.setup(isCapture1080, 0.3);
    width = cam.camWidth;
    height = cam.camHeight;
    
    // slit scan setup
    // setup slitscan, specify max buffer size
    slitScan.setup(width, height, 120);
    // blending means the edges between the scans are feathered
    slitScan.setBlending(true);
    // "time delay is the deepest in history the delay can go
    // and width is the number of frames the distortion will encompass
    // note that the delay cannot be more than the total capacity"
    slitScan.setTimeDelayAndWidth(slitScanTimeDelay, slitScanTimeWidth);
    // delay maps
    string dir = (width == 640) ? "maps" : "mapsHD";
    if (width == 1280) dir = "maps720";
	sampleMapStrings.push_back(dir+"/down_to_up.png");
    sampleMapStrings.push_back(dir+"/left_to_right.png");
	sampleMapStrings.push_back(dir+"/soft_noise.png");
	sampleMapStrings.push_back(dir+"/random_grid.png");
	sampleMapStrings.push_back(dir+"/up_to_down.png");
	sampleMapStrings.push_back(dir+"/right_to_left.png");
	for(int i = 0; i < sampleMapStrings.size(); i++){
		ofImage* map = new ofImage();
		map->allocate(width, height, OF_IMAGE_GRAYSCALE);
		map->loadImage(sampleMapStrings[i]);
		sampleMaps.push_back( map );
	}
    slitScan.setDelayMap(*(sampleMaps[currentSampleMapIndex = 0]));
    
    // give slicer a width and height to work with
    slicer[0].setup(width, height);
    slicer[1].setup(width, height);
    
    // aberration
    aberrationShader.load("shaders/aberration.vert", "shaders/aberration.frag");
    aberrationFbo.allocate(width, height);
    aberrationFbo.begin();
    ofClear(0, 0, 0);
    aberrationFbo.end();
    aberrationROffset.set(0);
    aberrationGOffset.set(0);
    aberrationBOffset.set(0);
    aberrationROffsetOut.set(0);;
    aberrationGOffsetOut.set(0);;
    aberrationBOffsetOut.set(0);;
    
    levels.setup(width, height);
    
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
	ofAddListener(RUI_GET_OF_EVENT(), this, &SlitScan::clientDidSomething);
    
    
    RUI_NEW_GROUP("Levels");
    RUI_SHARE_PARAM(levelsEnabled);
    RUI_SHARE_PARAM(levels.brightness, 0, 2);
    RUI_SHARE_PARAM(levels.contrast, 0, 2);
    RUI_SHARE_PARAM(levels.saturation, 0, 2);
    RUI_SHARE_PARAM(levels.gamma, 0, 2);
    RUI_SHARE_PARAM(levels.minInput, 0, 1);
    RUI_SHARE_PARAM(levels.maxInput, 0, 1);
    RUI_SHARE_PARAM(levels.minOutput, 0, 1);
    RUI_SHARE_PARAM(levels.maxOutput, 0, 1);
    
    RUI_NEW_GROUP("Slit Scan");
    string modeLabels[] = {"CAM", "SLIT_SCAN", "SLICE_SINGLE", "SLICE_DOUBLE", "SLICE_ONLY"};
	RUI_SHARE_ENUM_PARAM(mode, CAM, SLICE_ONLY, modeLabels);
    RUI_SHARE_PARAM(currentSampleMapIndex, 0, sampleMapStrings.size()-1);
	RUI_SHARE_PARAM(slitScanTimeWidth, 0, 120);
	RUI_SHARE_PARAM(slitScanTimeDelay, 0, 120);
    
	RUI_NEW_GROUP("Slicer (slice modes)");
	RUI_SHARE_PARAM(sliceThickness, 0, 30 );
	RUI_SHARE_PARAM(sliceOffset, 0, 30 );
    RUI_SHARE_PARAM(sliceVertical);
    RUI_SHARE_PARAM(sliceWeave);
    RUI_SHARE_PARAM(doAudioMap);
    
    RUI_NEW_GROUP("Aberration");
    RUI_SHARE_PARAM(aberrationROffset.x, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationROffset.y, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationGOffset.x, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationGOffset.y, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationBOffset.x, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationBOffset.y, 0.0, 50.0);
    
    RUI_NEW_GROUP("Camo");
    RUI_SHARE_PARAM(cam.flowSize, 0, 10);
    RUI_SHARE_PARAM(cam.blur, 0, 9);
    
    // camo
    camo.setup(&cam, width, height);
    
}


void SlitScan::update(){
    cam.update();
    
    float amount = 0.1;
    aberrationROffsetOut.x = ofLerp(aberrationROffsetOut.x, aberrationROffset.x, 0.1);
    aberrationROffsetOut.y = ofLerp(aberrationROffsetOut.y, aberrationROffset.y, 0.1);
    aberrationGOffsetOut.x = ofLerp(aberrationGOffsetOut.x, aberrationGOffset.x, 0.1);
    aberrationGOffsetOut.y = ofLerp(aberrationGOffsetOut.y, aberrationGOffset.y, 0.1);
    aberrationBOffsetOut.x = ofLerp(aberrationBOffsetOut.x, aberrationBOffset.x, 0.1);
    aberrationBOffsetOut.y = ofLerp(aberrationBOffsetOut.y, aberrationBOffset.y, 0.1);
    
    if (mode > CAM) {
        // all modes above 0 use slitscan
        if (cam.isFrameNew) {
            // if camo is enabled, force the camera to do flow calculations
            // and force slitscan delay map to use the flow image
            cam.doFlow = camo.isCamoEnabled;
            if (camo.isCamoEnabled) {
                camo.update();
                slitScan.setDelayMap(cam.delayMap);
            }
            slitScan.addImage(cam.getPixels());
        }
    }
    if (mode > SLIT_SCAN) {
        // all modes above 1 use slicer
        slicer[0].begin();
        drawSlitScan();
        slicer[0].end();
        slicer[1].begin();
        drawSlitScan();
        slicer[1].end();
    }
    
    if (levelsEnabled) {
        levels.begin();
        drawCurrentMode();
        levels.end();
        aberrationFbo.begin();
        ofClear(0, 0, 0, 0);
        levels.draw();
        aberrationFbo.end();
    }
    else {
        aberrationFbo.begin();
        ofClear(0, 0, 0, 0);
        drawCurrentMode();
        aberrationFbo.end();
    }
    
}

void SlitScan::draw(int w, int h){
    // Use shader to draw whole scene with aberration effect
    aberrationShader.begin();
    aberrationShader.setUniform2f("rOffset", aberrationROffsetOut.x, aberrationROffsetOut.y);
    aberrationShader.setUniform2f("gOffset", aberrationGOffsetOut.x, aberrationGOffsetOut.y);
    aberrationShader.setUniform2f("bOffset", aberrationBOffsetOut.x, aberrationBOffsetOut.y);
    aberrationFbo.draw(0, 0, w, h);
    aberrationShader.end();
    // draw debug over the top if its on
    if (camo.isDrawFlowDebugOn) {
        cam.drawFlowDebug(cam.camWidth, cam.camHeight);
        camo.drawFlow();
    }
    
}

void SlitScan::drawCurrentMode() {
    // Flip the whole scene to mirror camera input
    ofPushMatrix();
    ofScale(-1, 1);
    ofTranslate(-aberrationFbo.getWidth(), 0, 0 );
    switch (mode) {
        case CAM:
            cam.draw(0, 0);
            break;
        case SLIT_SCAN:
            drawSlitScan();
            break;
        case SLICE_SINGLE:
            cam.draw(0, 0);
            slicer[0].draw(0);
            break;
        case SLICE_DOUBLE:
            cam.draw(0, 0);
            if (sliceWeave) {
                slicer[0].draw(sliceOffset, 0);
                slicer[1].draw(0, sliceOffset);
            }
            else if (sliceVertical) {
                slicer[0].draw(-sliceOffset);
                slicer[0].draw(sliceOffset);
            }
            else {
                slicer[0].draw(0, -sliceOffset);
                slicer[0].draw(0, sliceOffset);
            }
            break;
        case SLICE_ONLY:
            slicer[0].draw(0);
            break;
        default:
            break;
    }
    ofPopMatrix();
}

void SlitScan::drawSlitScan(){
    if (camo.isCamoEnabled) {
        // depending on camo settings, it's either drawn on top of slit scan or blended with it
        // hence the use of a begin() and end() before drawing
        camo.begin();
        slitScan.getOutputImage().draw(0, 0);
        camo.end();
        camo.draw();
    }
    else {
        slitScan.getOutputImage().draw(0, 0);
    }
}

void SlitScan::resetDelayMap(){
    slitScan.setDelayMap(*(sampleMaps[currentSampleMapIndex]));
}

void SlitScan::clientDidSomething(RemoteUIServerCallBackArg &arg){
    switch (arg.action) {
		case CLIENT_UPDATED_PARAM:
            ofLogVerbose() << "CLIENT_UPDATED_PARAM: " << arg.paramName << " - " << arg.param.getValueAsString();
            if (arg.paramName == "slitScanTimeWidth") slitScan.setTimeWidth(slitScanTimeWidth);
            else if (arg.paramName == "slitScanTimeDelay") slitScan.setTimeDelay(slitScanTimeDelay);
            else if (arg.paramName == "sliceThickness") {
                slicer[0].setThickness(sliceThickness);
                slicer[1].setThickness(sliceThickness);
            }
            else if (arg.paramName == "sliceVertical") {
                slicer[0].setVertical(sliceVertical);
                slicer[1].setVertical(sliceVertical);
            }
            else if (arg.paramName == "sliceWeave" && sliceWeave) {
                slicer[0].setVertical(true);
                slicer[1].setVertical(false);
            }
            else if (arg.paramName == "currentSampleMapIndex") resetDelayMap();
            // Slight hack, listen to the isCamoEnabled param from Camo
            else if (arg.paramName == "isCamoEnabled" && !camo.isCamoEnabled) resetDelayMap();
            else if (arg.paramName == "doAudioMap" && !doAudioMap) resetDelayMap();
            break;
        case CLIENT_DID_SET_PRESET:
            if (!camo.isCamoEnabled || !doAudioMap) resetDelayMap();
            slicer[0].setThickness(sliceThickness);
            slicer[1].setThickness(sliceThickness);
            slicer[0].setVertical(sliceVertical);
            slicer[1].setVertical(sliceVertical);
            slitScan.setTimeWidth(slitScanTimeWidth);
            slitScan.setTimeDelay(slitScanTimeDelay);
            break;
        default:
			break;
	}
}

void SlitScan::keyPressed(int key){
    camo.keyPressed(key);
}

void SlitScan::keyReleased(int key){
}


void SlitScan::mouseMoved(int x, int y ){
    
}


void SlitScan::mouseDragged(int x, int y, int button){
    
}


void SlitScan::mousePressed(int x, int y, int button){
    
}


void SlitScan::mouseReleased(int x, int y, int button){
    
}


void SlitScan::windowResized(int w, int h){
    
}


void SlitScan::gotMessage(ofMessage msg){
    
}


void SlitScan::dragEvent(ofDragInfo dragInfo){
    
}
