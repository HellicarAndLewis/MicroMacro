#include "ofMain.h"
#include "ofApp.h"
#include "ofxArgs.h"


int main(int argc, char *argv[]){
    // read in arguments
    ofxArgs* args = new ofxArgs(argc, argv);
    bool antiAlias = args->getBool("-antiAlias", false);
    bool fullScreen = args->getBool("-fullScreen", false);
    bool logToFile  = args->getInt("-logToFile", false);
    bool appendToLog  = args->getInt("-appendToLog", false);
    int logLevel = args->getInt("-logLevel", 1); //OF_LOG_VERBOSE,OF_LOG_NOTICE,OF_LOG_WARNING,OF_LOG_ERROR,OF_LOG_FATAL_ERROR,OF_LOG_SILENT
    int id = args->getInt("-id", 0);
    
    // setup window
    if (fullScreen) ofSetupOpenGL(640, 480, OF_FULLSCREEN);
    else ofSetupOpenGL(640, 480, OF_WINDOW);
    
    // logging
    ofSetLogLevel((ofLogLevel)logLevel);
    string toFileString = (logToFile) ? "log to file" : "log to cout";
    args->printArgs();
    ofLogNotice() << "Setting log level " << logLevel << ", " << toFileString << ", args count " << args->getCount();
    if (logToFile) {
        string format = "log-%Y-%m";
        string timestamp = ofGetTimestampString(format);
        string filename = "logs/" + timestamp + ".txt";
        ofLogToFile(filename, appendToLog);
    }
    
    // run app
    ofApp* app = new ofApp();
    app->id = id;
    ofRunApp(app);
    delete args;
}
