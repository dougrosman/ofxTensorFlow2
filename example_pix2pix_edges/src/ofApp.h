
#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

#include "cppflow/cppflow.h"

#include <iostream>

class ofApp : public ofBaseApp{

	public:

    // neural network
    cppflow::model model;
    int nnWidth;
    int nnHeight;

    // camera
    ofPixels frameProcessed;
    ofVideoGrabber videoGrabber;
    ofTexture outputTexture;
    ofFbo fbo;
    int camWidth;
    int camHeight;

    // openCv
    ofxCvColorImage			colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvContourFinder		contourFinder;
    bool					bLearnBackground;

    // gui
    ofxFloatSlider threshold;
    ofxFloatSlider lineWidth;
    ofxFloatSlider blobs;
    ofxPanel gui;

    //--------------------------------------------------------------
    ofApp(std::string modelPath)
    : model(ofToDataPath(modelPath)){}

    //--------------------------------------------------------------
    void setup(){

        std::cout << "start setup" << std::endl;
        std::cout.flush();

        camWidth = 640;
        camHeight = 480; 

        nnWidth = 256;
        nnHeight = 256;

        fbo.allocate(camWidth, camHeight, GL_RGB); // with alpha, 8 bits red, 8 bits green, 8 bits 
        colorImg.allocate(camWidth, camHeight);
        grayImage.allocate(camWidth, camHeight);
        grayBg.allocate(camWidth, camHeight);
        grayDiff.allocate(camWidth, camHeight);

        bLearnBackground = true;

        videoGrabber.setDeviceID(0);
        videoGrabber.setDesiredFrameRate(30);
        videoGrabber.initGrabber(camWidth, camHeight);

        frameProcessed.allocate(nnWidth, nnHeight, OF_PIXELS_RGB);
        outputTexture.allocate(frameProcessed);

        gui.setup();
        gui.add(threshold.setup("threshold", 30, 10, 300));
        gui.add(lineWidth.setup("lineWidth", 0, 1, 10));
        gui.add(blobs.setup("blobs", 0, 5, 50));

        ofSetVerticalSync(true);

        std::cout << "done setup" << std::endl;
        std::cout.flush();
    }

    //--------------------------------------------------------------
    void update(){

        // Ask the video player to update itself.
        videoGrabber.update();

        if (videoGrabber.isFrameNew()){ // If there is fresh data...
            
            // Copy the data from the video player into an ofxCvColorImage
            colorImg.setFromPixels(videoGrabber.getPixels());
            
            // Make a grayscale version of colorImg in grayImage
            grayImage = colorImg;
            
            // If it's time to learn the background;
            // copy the data from grayImage into grayBg
            if (bLearnBackground == true){
                grayBg = grayImage; // Note: this is 'operator overloading'
                bLearnBackground = false; // Latch: only learn it once.
            }

            // Take the absolute value of the difference 
            // between the background and incoming images.
            grayDiff.absDiff(grayBg, grayImage);
            
            // Perform an in-place thresholding of the difference image.
            grayDiff.threshold(threshold);

            // Find contours whose areas are betweeen 20 and 25000 pixels.
            // "Find holes" is true, so we'll also get interior contours.
            contourFinder.findContours(grayDiff, 20, 25000, blobs, true);

            fbo.begin();
                ofClear(255,255,255, 0);
                for(unsigned int i = 0; i < contourFinder.blobs.size(); i++) {
                    ofPolyline cur;
                    // add all the current vertices to cur polyline
                    cur.addVertices(contourFinder.blobs[i].pts);
                    cur.setClosed(true);

                    ofSetLineWidth(lineWidth);
                    ofNoFill();
                    ofSetColor(0, 0, 0);
                    cur.getSmoothed(80).draw();
                    }
            fbo.end();

            // get the frame
            ofPixels resizedPixels;
            fbo.getTexture().readToPixels(resizedPixels);

            // resize pixels
            resizedPixels.resize(nnWidth, nnHeight);

            // // get the frame
            // ofPixels & pixels = videoGrabber.getPixels();

            // // resize pixels
            // ofPixels resizedPixels(pixels);
            // resizedPixels.resize(nnWidth, nnHeight);

            // copy to tensor
            cppflow::tensor input(
                    std::vector<float>(resizedPixels.begin(),
                                        resizedPixels.end()),
                                        {nnWidth, nnHeight, 3});

            // cast data type and expand to batch size of 1
            input = cppflow::cast(input, TF_INT32, TF_FLOAT);
            input = cppflow::expand_dims(input, 0);
            input = cppflow::div(input, cppflow::tensor({127.5f}));
            input = cppflow::add(input, cppflow::tensor({-1.0f}));

            // auto & input_vector = input.get_data<float>();
            // for(int i=0; i<input_vector.size(); i++) input_vector[i] = input_vector[i] / 255;

            // start neural network and time measurement
            auto start = std::chrono::system_clock::now();
            auto output = this->model(input);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> diff = end-start;

            // std::cout << output << std::endl;
            std::cout << "Time: " << diff.count() << std::endl;

            // copy to output frame and postprocessing
            auto outputVector = output.get_data<float>();
            // for(int i=0; i<outputVector.size(); i++) frameProcessed[i] = (outputVector[i] + 1) * 127.5;
            for(int i=0; i<outputVector.size(); i++) frameProcessed[i] = ((outputVector[i] * 0.5 )+ 0.5) * 255;

            outputTexture.loadData(frameProcessed);

        }
    }

    //--------------------------------------------------------------
    void draw(){

        ofBackground(100,100,100);

        ofSetHexColor(0xffffff);
        colorImg.draw(20,20);    // The incoming color image
        grayImage.draw(camWidth, camHeight);  // A gray version of the incoming video
        // grayBg.draw(camWidth, 0);     // The stored background image
        grayDiff.draw(0, camHeight);  // The thresholded difference image

        outputTexture.draw(camWidth, 0);
        // ofSetHexColor(0xffffff);
        // videoGrabber.draw(20, 20);
        // outputTexture.draw(20 + camWidth, 20, nnWidth, nnHeight);
        
        fbo.draw(0,0);


        ofSetLineWidth(1);
        gui.draw();
        
    }


    //--------------------------------------------------------------
    void keyPressed(int key){
        bLearnBackground = true;
    }


    void keyReleased(int key){}
    void mouseMoved(int x, int y ){}
    void mouseDragged(int x, int y, int button){}
    void mousePressed(int x, int y, int button){}
    void mouseReleased(int x, int y, int button){}
    void mouseEntered(int x, int y){}
    void mouseExited(int x, int y){}
    void windowResized(int w, int h){}
    void dragEvent(ofDragInfo dragInfo){}
    void gotMessage(ofMessage msg){}

    
};
