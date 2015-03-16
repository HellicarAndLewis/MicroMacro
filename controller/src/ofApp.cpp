#include "ofApp.h"


void ofApp::setup(){
    ofBackground(30,30,30);
    // open an outgoing connection to address:PORT
    // see if an address has been specified in the app arguments
    if (address == "") address = DEFAULT_ADDRESS;
    sender.setup(address, PORT);
    // setup clients
    clients.assign(4, Client());
    for( int i = 0; i < clients.size(); i++){
        clients[i].id = i+1;
    }
}


void ofApp::update(){
}


void ofApp::draw(){
    
    int x = 10;
    int y = 150;
    int size = 60;
    int pad = 10;
    for( int i = 0; i < clients.size(); i++){
        if (clients[i].isSelected) ofSetColor(200, 200, 200);
        else ofSetColor(100, 100, 100);
        ofRect(x, y, size, size);
        ofSetColor(0);
        ofDrawBitmapString(ofToString(clients[i].id), x + (size/2), y + (size/2));
        x += size + pad;
    }
    
    ofSetColor(255);
    
    // display instructions
    string buf;
    buf = "Broadcasting on " + string(address) + ":" + ofToString(PORT);
    ofDrawBitmapString(buf, 10, 20);
    ofDrawBitmapString("Use numeric keys to toggle selected clients (1-9)", 10, 50);
    ofDrawBitmapString("press m to broadcast mode change", 10, 80);
    
}

string ofApp::getSelectedClientsString() {
    string s = "";
    for( int i = 0; i < clients.size(); i++){
        if (clients[i].isSelected) {
            s += ofToString(clients[i].id) + "|";
        }
    }
    return s;
}

void ofApp::keyPressed(int key){
    ofLogNotice() << key;
    if(key == 'm'){
        ofxOscMessage m;
        m.setAddress("/mode");
        // send target client IDs as | delimited list
        m.addStringArg(getSelectedClientsString());
        m.addIntArg(1);
        sender.sendMessage(m);
    }
    // 48 - 57 is 1 to 0
    else if (key > 48 && key < 59) {
        int i = key - 49;
        clients[i].isSelected = !clients[i].isSelected;
    }
}


void ofApp::keyReleased(int key){
    
}


void ofApp::mouseMoved(int x, int y){
    ofxOscMessage m;
    m.setAddress("/mouse/position");
    m.addIntArg(x);
    m.addIntArg(y);
    sender.sendMessage(m);
}


void ofApp::mouseDragged(int x, int y, int button){
    
}


void ofApp::mousePressed(int x, int y, int button){
    ofxOscMessage m;
    m.setAddress("/mouse/button");
    m.addStringArg("down");
    sender.sendMessage(m);
}


void ofApp::mouseReleased(int x, int y, int button){
    ofxOscMessage m;
    m.setAddress("/mouse/button");
    m.addStringArg("up");
    sender.sendMessage(m);
    
}


void ofApp::windowResized(int w, int h){
    
}


void ofApp::gotMessage(ofMessage msg){
    
}


void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

