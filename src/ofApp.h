#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "particle.h"

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void resetParticles();
        void createFlowField();
        void drawParticles();

		void keyPressed(int key);
    
        int counterA, counterB;
    
        ofTrueTypeFont myFont;

        //ofVideoPlayer movie;
        ofVideoGrabber movie;
    
        ofImage imgThresh, imgBlur, imgInvert;
    
        ofxCv::ContourFinder contourFinder;
        vector< vector<cv::Point> > quads;
        vector<int> sides;
        bool showLabels, guiFlag, flowFieldFlag, transitionA, transitionB, stateA, stateB;
        vector <glm::vec3> contPoints;
        string modeSelector;
    
    
        ofxPanel gui;
        ofParameter<float> minArea, maxArea, thresholdVal, fieldOfGlow, proximity;
        ofParameter<int> blurAmount;
        ofParameter<bool> holes;

    
        //  Particle Setup
        vector <Particle> p;
        vector <ofPoint> repulsePoints, attractPoints;
        vector <ofPoint> repulsePointsWithMovement, attractPointsWithMovement;
    
        vector <ofPoint> flowField;
    
        ofPoint null;
    
        int width, height, rows, cols;
    
        ofParameter<int> numFrames, gridScale;
        ofParameter<float> timeMult, scaleMult, radius, vScale;

		
};
