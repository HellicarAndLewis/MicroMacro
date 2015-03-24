//
//  Camo.h
//  slitscan
//
//  Created by Chris Mullany on 24/03/2015.
//
//

#pragma once

#include "ofMain.h"
#include "Cam.h"
#include "AlphaMask.h"
#include "ofxRemoteUIServer.h"

class Camo {
    
public:
    
    Cam* cam;
    int width;
    int height;
    
    ofFbo camoPatternFbo;
    ofShader camoShader;
    AlphaMask camoMask;
    
    // camo textures loaded from data/camo
    vector <ofImage> camoImages;
    int camoImageIndex;
    
    // appearance
    int camoFlowRes;
    int camoBlockMin;
    int camoBlockMax;
    int camoFade;
    bool isCamoEnabled;
    bool isDrawFlowDebugOn;
    bool isCamoSquare;
    bool isCamoImage;
    
    // sensitivity and range
    float camoMinFlow;
    float camoMaxFlow;
    
    void setup(Cam* cam, int w, int h);
    void update();
    void draw();
    void drawFlow();
    void begin();
    void end();
    
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void clientDidSomething(RemoteUIServerCallBackArg & arg);
};