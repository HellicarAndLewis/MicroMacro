#include "ofMain.h"
#include "ofApp.h"
#include "ofxArgs.h"


int main(int argc, char *argv[]){
    // read in arguments
    ofxArgs* args = new ofxArgs(argc, argv);
    string address = args->getString("-address", "");
    
    // setup window
    ofSetupOpenGL(640, 480, OF_WINDOW);
    
    // run app
    ofApp* app = new ofApp();
    app->address = address;
    ofRunApp(app);
    delete args;
}
