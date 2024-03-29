//
//  Slicer.h
//  slitscan
//
//  Created by Chris on 09/03/2015.
//
//

#pragma once
#include "ofMain.h"
#include "AlphaMask.h"

// Slicer chops up an image/texture into vertical strips and draw the output into an FBO
// Basic usage: call begin() draw any texture and call end() and then draw()
class Slicer {
public:
    
    void setup(int w=0, int h=0);
    void update();
    void draw(int x=0, int y=0);
    void setThickness(int thickness);
    void setVertical(bool isVertical);
    void refresh();
    void begin();
    void end();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
	
    AlphaMask mask;
    int thickness;
    bool isVertical;
    
};