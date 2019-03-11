//
//  GuiApp.h
//  CAS1
//
//  Created by Gunjan Bhutani on 3/9/19.
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class GuiApp: public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    ofParameterGroup parameters;
    ofParameter<float> minArea, maxArea, thresholdVal, fieldOfGlow, proximity;
    ofParameter<int> blurAmount, camOffsetX, camOffsetY, cropHeight, cropWidth;
    ofParameter<bool> holes, invert;
    
    ofParameter<int> numFrames, gridScale, offsetX, offsetY;
    ofParameter<float> timeMult, scaleMult, radius, vScale;
    ofParameter<bool> flagA, flagB, flagC, flagD;

    ofxPanel gui;
};
