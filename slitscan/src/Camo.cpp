//
//  Camo.cpp
//  Camo
//
//  Created by Chris Mullany on 24/03/2015.
//
//

#include "Camo.h"

void Camo::setup(Cam* cam, int w, int h){
    
    this->cam = cam;
    this->width = w;
    this->height = h;
    
    // defaults
    isCamoEnabled = true;
    isDrawFlowDebugOn = false;
    isCamoSquare = true;
    isCamoImage = true;
    camoMinFlow = 9;
    camoMaxFlow = 15;
    camoFlowRes = 2;
    camoBlockMin = 4;
    camoBlockMax = 15;
    camoAlphaMin = 150;
    camoFade = 20;
    
    // camo images for when isCamoImage is true
    camoShader.load("shaders/camo.vert", "shaders/camo.frag");
    ofDirectory camoImageDir;
    int nFiles = camoImageDir.listDir("camo");
    if(nFiles) {
        for(int i=0; i<camoImageDir.numFiles(); i++) {
            // add the image to the vector
            string filePath = camoImageDir.getPath(i);
            camoImages.push_back(ofImage());
            camoImages.back().loadImage(filePath);
            
        }
    }
    else ofLogError("Couldn't find camo images folder");
    camoImageIndex = 0;
    
    camoMask.setup(width, height);
    camoPatternFbo.allocate(width, height);
    
    // Using ofxRemoteUI https://github.com/armadillu/ofxRemoteUI/
    ofAddListener(RUI_GET_OF_EVENT(), this, &Camo::clientDidSomething);
    RUI_SHARE_PARAM(isCamoEnabled);
    RUI_SHARE_PARAM(isCamoSquare);
    RUI_SHARE_PARAM(isCamoImage);
    RUI_SHARE_PARAM(camoMinFlow, 0.00, 20.00);
    RUI_SHARE_PARAM(camoMaxFlow, 0.00, 20.00);
    RUI_SHARE_PARAM(camoFlowRes, 1, 10);
    RUI_SHARE_PARAM(camoBlockMin, 1, 10);
    RUI_SHARE_PARAM(camoBlockMax, 2, 40);
    RUI_SHARE_PARAM(camoAlphaMin, 0, 255);
    RUI_SHARE_PARAM(camoFade, 2, 255);
    RUI_SHARE_PARAM(camoImageIndex, 0, nFiles-1);
    RUI_SHARE_PARAM(isDrawFlowDebugOn);
    
}


void Camo::update(){
    if (isCamoSquare) {
        // Draw custom shapes based on flow at point
        // optical flow from cam
        float xratio = cam->camWidth / cam->cvWidth;
        float yratio = cam->camHeight / cam->cvHeight;
        ofVec2f pos;
        // draw into FBO
        camoPatternFbo.begin();
        // FBO fade trick
        ofSetColor(0, 0, 0, camoFade);
        ofRect(0,0,camoPatternFbo.getWidth(),camoPatternFbo.getHeight());
        int x, y;
        for ( y = 0; y < cam->cvHeight; y+=camoFlowRes ){
            for ( x = 0; x < cam->cvWidth; x+=camoFlowRes ){
                ofVec2f vec = cam->opticalFlowLk.flowAtPoint(x, y);
                if (vec.length() > camoMinFlow) {
                    // movement detected, draw a square here
                    float grey = ofMap(vec.length(), camoMinFlow, camoMaxFlow, camoAlphaMin, 255, true);
                    ofSetColor(grey);
                    // offset the z to force smaller squares to the front
                    ofPoint p = ofPoint(x*xratio, y*yratio);
                    p.z = ofMap(vec.length(), camoMinFlow, camoMaxFlow, 0, -20, true);
                    float size = ofMap(vec.length(), camoMinFlow, camoMaxFlow, camoBlockMax, camoBlockMin, true);
                    ofRect(p, size, size);
                }
            }
        }
        ofSetColor(255);
        camoPatternFbo.end();
    }
    else {
        // just draw the optical flow pattern for a more organic/warped shape
        camoPatternFbo.begin();
        ofClear(0);
        cam->drawFlow();
        camoPatternFbo.end();
    }
}


void Camo::draw(){
    if (isCamoImage) {
        // This draws actual camo colours
        // by mask revealing an image
        // mask is contents of camoPatternFbo which can be optical flow image or custom shapes
        camoMask.beginMask();
        camoPatternFbo.draw(0,0);
        camoMask.endMask();
        // masking off camoImage
        camoMask.begin();
        camoImages[camoImageIndex].draw(0, 0);
        camoMask.end();
        camoMask.draw();
    }
}

void Camo::drawFlow(){
    camoPatternFbo.draw(0, 0);
}

void Camo::begin(){
    if (!isCamoImage) {
        // Not using image masking means use shader
        // this needs to be wrapped in begin and end so that SlitScan can draw into the shader
        // converts greyscale texture into camo colours
        camoShader.begin();
        camoShader.setUniformTexture("camoText", camoPatternFbo.getTextureReference(), 1);
        // now slit scan should draw before calling end()
    }
}
void Camo::end(){
    if (!isCamoImage) {
        camoShader.end();
    }
}

void Camo::clientDidSomething(RemoteUIServerCallBackArg &arg){
    switch (arg.action) {
        case CLIENT_UPDATED_PARAM:
            ofLogVerbose() << "CLIENT_UPDATED_PARAM: " << arg.paramName << " - " << arg.param.getValueAsString();
            break;
        default:
            break;
    }
}

void Camo::keyPressed(int key){
    switch (key) {
        case 'c':
            camoImageIndex++;
            if (camoImageIndex > camoImages.size()-1) {
                camoImageIndex = 0;
            }
            break;
            
        default:
            break;
    }
}

void Camo::keyReleased(int key){
}


void Camo::mouseMoved(int x, int y ){
    
}


void Camo::mouseDragged(int x, int y, int button){
    
}


void Camo::mousePressed(int x, int y, int button){
    
}


void Camo::mouseReleased(int x, int y, int button){
    
}


void Camo::windowResized(int w, int h){
    
}


void Camo::gotMessage(ofMessage msg){
    
}


void Camo::dragEvent(ofDragInfo dragInfo){
    
}
