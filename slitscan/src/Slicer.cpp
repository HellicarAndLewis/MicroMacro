//
//  Slicer.cpp
//  slitscan
//
//  Created by Chris on 09/03/2015.
//
//

#include "Slicer.h"


void Slicer::setup(int w, int h){
    
    mask.setup(w, h);
    // defaults
    thickness = 20;
    isVertical = true;
    refresh();
}


void Slicer::update(){}


void Slicer::begin() {
    mask.begin();
}

void Slicer::end() {
    mask.end();
}


void Slicer::draw(int x, int y){
    mask.draw(x, y);
}


void Slicer::setThickness(int thickness){
    this->thickness = thickness;
    refresh();
}

void Slicer::setVertical(bool isVertical){
    this->isVertical = isVertical;
    refresh();
}

void Slicer::refresh(){
    // sets thickness of slits by drawing white into the mask FBO
    // mask FBO is used to mask a given texture using being() and end()
    int width = 0;
    int height = 0;
    
    mask.beginMask();
    ofClear(0,0,0,255);
    if (isVertical) {
        while (width < mask.maskFbo.getWidth()) {
            ofRect(width, 0, thickness, mask.maskFbo.getHeight());
            width += thickness * 2;
        }
    }
    else {
        while (height < mask.maskFbo.getHeight()) {
            ofRect(0, height, mask.maskFbo.getWidth(), thickness);
            height += thickness * 2;
        }
    }
    mask.endMask();
}

void Slicer::keyPressed(int key){
}


void Slicer::keyReleased(int key){
    
}


void Slicer::mouseMoved(int x, int y ){
    
}


void Slicer::mouseDragged(int x, int y, int button){
    
}


void Slicer::mousePressed(int x, int y, int button){
}


void Slicer::mouseReleased(int x, int y, int button){
}


void Slicer::windowResized(int w, int h){
    
}


void Slicer::gotMessage(ofMessage msg){
    
}


void Slicer::dragEvent(ofDragInfo dragInfo){
    
}
