//
//  Slicer.h
//  slitscan
//
//  Created by Chris on 09/03/2015.
//
//

#pragma once
#include "ofMain.h"

// Slicer chops up an image/texture into vertical strips and draw the output into an FBO
// TODO: hook up to GUI controls, add horizontal
class Slicer {
public:
    
    void setup(int w=0, int h=0);
    void update();
    void draw(int x=0, int y=0);
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
	
    ofImage     srcImg;
    ofFbo       maskFbo;
    ofFbo       fbo;
    ofShader    shader;
    
};