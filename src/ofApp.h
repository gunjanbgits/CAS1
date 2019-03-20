#pragma once

#include "ofMain.h"
#include "GuiApp.h"
#include "ofxCv.h"
#include "ofxGui.h"
//#include "SecondApp.h"
#include "particle.h"
#include "ofxPostProcessing.h"

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        shared_ptr<GuiApp> gui;
        //shared_ptr < SecondApp > second;
    
        void resetParticles();
        void createFlowField();
        void drawParticles();

		void keyPressed(int key);
    
        int counterA, counterB;
        ofxPostProcessing post;
    
        ofTrueTypeFont myFont, myFontSmall, myFontSemi;

        //ofVideoPlayer movie;
        ofVideoGrabber movie;
    
        ofColor juju;
    
        ofImage imgThresh, imgBlur, imgInvert, cropSample;
    
        ofxCv::ContourFinder contourFinder;
        vector< vector<cv::Point> > quads;
        vector<int> sides;
        bool showLabels, guiFlag, flowFieldFlag, transitionA, transitionB, stateA, stateB;
        vector <glm::vec3> contPoints;
        string modeSelector;

    
        //  Particle Setup
        vector <Particle> p;
        vector <ofPoint> repulsePoints, attractPoints;
        vector <ofPoint> repulsePointsWithMovement, attractPointsWithMovement;
    
        vector <ofPoint> flowField;
    
        float score;
    
        ofPoint null;
    
        int width, height, rows, cols;
    
        ofSoundPlayer gong;
        bool playedOnce, jujuStart, jujuOne, jujuTwo;
    

		
};
