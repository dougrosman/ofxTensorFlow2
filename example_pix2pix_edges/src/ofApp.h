
#pragma once
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"

#include <iostream>

class ofApp : public ofBaseApp{

	public:

		void setup(){

            std::cout << "start setup" << std::endl;
            std::cout.flush();

            camWidth = 640;
            camHeight = 480;

            colorImg.allocate(camWidth, camHeight);
            grayImage.allocate(camWidth, camHeight);
            grayBg.allocate(camWidth, camHeight);
            grayDiff.allocate(camWidth, camHeight);

            bLearnBackground = true;

            thresholdValue = 50;

            videoGrabber.setDeviceID(0);
            videoGrabber.setDesiredFrameRate(30);
            videoGrabber.initGrabber(camWidth, camHeight);

            gui.setup();
            gui.add(threshold.setup("threshold", 30, 10, 300));
            gui.add(lineWidth.setup("lineWidth", 0, 1, 10));
            gui.add(blobs.setup("blobs", 0, 5, 50));

            ofSetVerticalSync(true);

            std::cout << "done setup" << std::endl;
            std::cout.flush();
        }

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

                polylines.clear();
                smoothed.clear();
                resampled.clear();

                for(unsigned int i = 0; i < contourFinder.blobs.size(); i++) {
                    ofPolyline cur;
                    // add all the current vertices to cur polyline
                    cur.addVertices(contourFinder.blobs[i].pts);
                    cur.setClosed(true);

                    // add the cur polyline to all these vector<ofPolyline>
                    polylines.push_back(cur);
                    smoothed.push_back(cur.getSmoothed(8));
                    resampled.push_back(cur.getResampledByCount(100).getSmoothed(8));

                    }
                }
        }

        void draw(){

            ofBackground(100,100,100);

            ofSetHexColor(0xffffff);
            // colorImg.draw(20,20);    // The incoming color image
            grayImage.draw(camWidth, camHeight);  // A gray version of the incoming video
            grayBg.draw(camWidth, 0);     // The stored background image
            grayDiff.draw(0, camHeight);  // The thresholded difference image

            ofNoFill();
            ofDrawRectangle(0,0, camWidth,camHeight);

            for(unsigned int i = 0; i < polylines.size(); i++) {
                ofNoFill();
                ofSetLineWidth(lineWidth);
                ofSetColor(0, 0, 0);
                smoothed[i].draw();
            }

            ofSetLineWidth(1);
            gui.draw();
        }


        void keyPressed(int key){
            bLearnBackground = true;
        }
    
        ofxCvColorImage			colorImg;
        ofxCvGrayscaleImage 	grayImage;
        ofxCvGrayscaleImage 	grayBg;
        ofxCvGrayscaleImage 	grayDiff;
        ofxCvContourFinder		contourFinder;

        int						thresholdValue;
        bool					bLearnBackground;

		vector<ofPolyline> polylines, smoothed, resampled;

        ofPixels frameProcessed;
        ofVideoGrabber videoGrabber;
        ofTexture inputTexture;
        ofTexture outputTexture;

        int camWidth;
        int camHeight;

        ofxFloatSlider threshold;
        ofxFloatSlider lineWidth;
        ofxFloatSlider blobs;
        ofxPanel gui;
};
