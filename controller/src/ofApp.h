#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

// Use network broadcast address
#define DEFAULT_ADDRESS "192.168.1.255"
#define PORT 12345

struct Client {
    int id;
    bool isSelected = false;
};

class ofApp : public ofBaseApp {
    
public:
    
    void setup();
    void update();
    void draw();
    
    string getSelectedClientsString();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    vector<Client> clients;
    ofxOscSender sender;
    string address;
    int port;
};