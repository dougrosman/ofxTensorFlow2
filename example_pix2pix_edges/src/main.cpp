#include "ofMain.h"
#include "ofApp.h"

//========================================================================

int main(int argc, char * argv[]){

    ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

    // name the model located relative to bin/data/
    ofRunApp(new ofApp("model"));
}
