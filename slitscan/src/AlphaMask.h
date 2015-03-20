//
//  AlphaMask.h
//  slitscan
//
//  Created by Chris Mullany on 20/03/2015.
//
//

#pragma once
#include "ofMain.h"
#include "AlphaMask.h"

class AlphaMask {
public:
    
    void setup(int w=0, int h=0);
    void update();
    void draw(int x=0, int y=0);
    void refresh();
    void begin();
    void end();
    void beginMask();
    void endMask();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofFbo       maskFbo;
    ofFbo       fbo;
    ofShader    shader;
    
};