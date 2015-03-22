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
    useCamo = false;
    useOpticCamo = true;
    isDrawFlowDebugOn = false;
    
    cam.setup(isCapture720, 0.3);
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
    
    // camo
    camoShader.load("shaders/camo.vert", "shaders/camo.frag");
    camoImage.loadImage("camo/camo.jpg");
    camoMask.setup(width, height);
    camoPatternFbo.allocate(width, height);
    camoMinFlow = 9;
    camoMaxFlow = 15;
    isCamoSquare = false;
    isCamoImage = true;
    
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
    // share controls
	ofAddListener(RUI_GET_OF_EVENT(), this, &SlitScan::clientDidSomething);
    
    RUI_NEW_GROUP("Slit Scan");
    string modeLabels[] = {"CAM", "SLIT_SCAN", "SLICE_SINGLE", "SLICE_DOUBLE", "CAMO"};
	RUI_SHARE_ENUM_PARAM(mode, CAM, CAMO, modeLabels);
    RUI_SHARE_PARAM(currentSampleMapIndex, 0, sampleMapStrings.size()-1);
	RUI_SHARE_PARAM(slitScanTimeWidth, 0, 120);
	RUI_SHARE_PARAM(slitScanTimeDelay, 0, 120);
    
	RUI_NEW_GROUP("Slicer (slice modes)");
	RUI_SHARE_PARAM(sliceThickness, 0, 30 );
	RUI_SHARE_PARAM(sliceOffset, 0, 30 );
    RUI_SHARE_PARAM(sliceVertical);
    RUI_SHARE_PARAM(sliceWeave);
    
    RUI_NEW_GROUP("Aberration");
    RUI_SHARE_PARAM(aberrationROffset.x, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationROffset.y, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationGOffset.x, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationGOffset.y, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationBOffset.x, 0.0, 50.0);
    RUI_SHARE_PARAM(aberrationBOffset.y, 0.0, 50.0);
    
    RUI_NEW_GROUP("Flow");
    RUI_SHARE_PARAM(useCamo);
    RUI_SHARE_PARAM(useOpticCamo);
    RUI_SHARE_PARAM(isCamoSquare);
    RUI_SHARE_PARAM(isCamoImage);
    RUI_SHARE_PARAM(cam.flowSize, 0, 10);
    RUI_SHARE_PARAM(cam.blur, 0, 9);
    RUI_SHARE_PARAM(cam.opticalFlowSensitivity, 0.00, 1.00);
    RUI_SHARE_PARAM(cam.opticalFlowSmoothing, 0.00, 1.00);
    RUI_SHARE_PARAM(camoMinFlow, 0.00, 20.00);
    RUI_SHARE_PARAM(camoMaxFlow, 0.00, 20.00);
    RUI_SHARE_PARAM(isDrawFlowDebugOn);
    
}


void SlitScan::update(){
    cam.update();
    if (mode > 0) {
        // all modes above 0 use slitscan
        if (cam.isFrameNew) {
            cam.doFlow = useCamo;
            if (useCamo) {
                updateCamoPattern();
                slitScan.setDelayMap(cam.delayMap);
            }
            slitScan.addImage(cam.getImage());
        }
    }
    if (mode > 1) {
        // all modes above 1 use slicer
        slicer[0].begin();
        drawSlitScan();
        slicer[0].end();
        slicer[1].begin();
        drawSlitScan();
        slicer[1].end();
    }
    
    aberrationFbo.begin();
    ofClear(0, 0, 0, 0);
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
        default:
            break;
    }
    ofPopMatrix();
    aberrationFbo.end();
}

void SlitScan::updateCamoPattern() {
    // Draw custom shapes based on flow at point?
    if (isCamoSquare) {
        // optical flow from cam
        float xratio = cam.camWidth / cam.cvWidth;
        float yratio = cam.camHeight / cam.cvHeight;
        ofVec2f pos;
        int gridSize = 3;
        // draw into FBO
        camoPatternFbo.begin();
        ofSetColor(0, 0, 0, 120);
        ofRect(0,0,camoPatternFbo.getWidth(),camoPatternFbo.getHeight());
        // shader range = 0.6 - 1.0
        int x, y;
        for ( y = 0; y < cam.cvHeight; y+=gridSize ){
            for ( x = 0; x < cam.cvWidth; x+=gridSize ){
                ofVec2f vec = cam.opticalFlowLk.flowAtPoint(x, y);
                if (vec.length() > camoMinFlow) {
                    float grey = ofMap(vec.length(), camoMinFlow, camoMaxFlow, 160, 255, true);
                    // do something
                    ofPoint p = ofPoint(x*xratio, y*yratio);
                    ofSetColor(grey);
                    float size = ofMap(vec.length(), camoMinFlow, camoMaxFlow, 15, 4, true);
                    //ofTriangle(p + ofPoint(-size, -size), p + ofPoint(size, -size), p + ofPoint(0, size*0.5));
                    ofRect(p, size, size);
                    //ofSetLineWidth(6);
                    //ofLine(x*xratio,y*yratio,((x*xratio)+vec.x),((y*yratio)+vec.y));
                }
            }
        }
        ofSetLineWidth(1);
        ofSetColor(255);
        camoPatternFbo.end();
    }
    else {
        camoPatternFbo.begin();
        ofClear(0);
        cam.drawFlow();
        camoPatternFbo.end();
    }
}


void SlitScan::draw(int w, int h){
    // Use shader to draw whole scene with aberration effect
    aberrationShader.begin();
    aberrationShader.setUniform2f("rOffset", aberrationROffset.x, aberrationROffset.y);
    aberrationShader.setUniform2f("gOffset", aberrationGOffset.x, aberrationGOffset.y);
    aberrationShader.setUniform2f("bOffset", aberrationBOffset.x, aberrationBOffset.y);
    aberrationFbo.draw(0, 0, w, h);
    aberrationShader.end();
    // draw debug over the top if its on
    if (isDrawFlowDebugOn) {
        cam.drawFlowDebug(cam.camWidth, cam.camHeight);
        camoPatternFbo.draw(0,0, w, h);
    }
    
}

void SlitScan::drawSlitScan(){
    if (useCamo && !useOpticCamo) {
        // This draws actual camo colours
        // either by mask revealing an image
        // or by using a shader
        if (isCamoImage) {
            // draw output direcly on the bottom
            slitScan.getOutputImage().draw(0, 0);
            // mask is contents of camoPatternFbo which can be optical flow image or custom shapes
            camoMask.beginMask();
            camoPatternFbo.draw(0,0);
            camoMask.endMask();
            // masking off camoImage
            camoMask.begin();
            camoImage.draw(0, 0);
            camoMask.end();
            camoMask.draw();
        }
        else {
            // or use camo shader
            // converts greyscale texture into camo colours
            camoShader.begin();
            camoShader.setUniformTexture("camoText", camoPatternFbo.getTextureReference(), 1);
            slitScan.getOutputImage().draw(0, 0);
            camoShader.end();
        }
    }
    else {
        // If useCamo is on AND useOpticCamo is on, this creates a warped camo effect
        // by using the optical flow image as the slit scan distort map
        // otherwise this just draws slit scan with the selected distort map
        slitScan.getOutputImage().draw(0, 0);
    }
}

void SlitScan::drawQuad(int w, int h){
    glBegin(GL_QUADS);
    glMultiTexCoord2f(GL_TEXTURE0, 0.0f, cam.camHeight);
    glVertex3f(0, h, 0);
    glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glMultiTexCoord2f(GL_TEXTURE0, cam.camWidth, 0.0f);
    glVertex3f(w, 0, 0);
    glMultiTexCoord2f(GL_TEXTURE0, cam.camWidth, cam.camHeight);
    glVertex3f(w, h, 0);
    glEnd();
}


void SlitScan::keyPressed(int key){
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
            else if (arg.paramName == "currentSampleMapIndex") slitScan.setDelayMap(*(sampleMaps[currentSampleMapIndex]));
            else if (arg.paramName == "useCamo" && !useCamo) slitScan.setDelayMap(*(sampleMaps[currentSampleMapIndex]));
			break;
		default:
			break;
	}
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
