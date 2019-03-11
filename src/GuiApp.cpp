//
//  GuiApp.cpp
//  CAS1
//
//  Created by Gunjan Bhutani on 3/9/19.
//

#include "GuiApp.h"

void GuiApp::setup(){
    //GUI SETUP
    parameters.add(camOffsetX.set("CAM offsetX", 310, 1, 640));
    parameters.add(camOffsetY.set("CAM offsetY", 200, 1, 384));
    parameters.add(cropHeight.set("Crop Height", 340, 1, 1280));
    parameters.add(cropWidth.set("Crop Width", 580, 1, 800));
    parameters.add(minArea.set("Min area", 5, 1, 100));
    parameters.add(maxArea.set("Max area", 200, 1, 500));
    parameters.add(blurAmount.set("Blur", 1, 1, 100));
    parameters.add(thresholdVal.set("Threshold", 140, 0, 255));
    parameters.add(holes.set("Holes", true));
    parameters.add(invert.set("invert", false));
    parameters.add(fieldOfGlow.set("RangeOfInfluence", 10, 1, 40));
    parameters.add(proximity.set("Proximity", 1, 5, 40));
    
    
    // Particle GUI
    parameters.add(timeMult.set("timeMult", .2, .005, .5));
    parameters.add(scaleMult.set("ScaleMult", .02, .005, .5));
    parameters.add(radius.set("Radius", .2, .1, 5));
    parameters.add(numFrames.set("numFrames", 75, 60, 600));
    //gui.add(gridScale.set("gridScale", 50, 20, 80));
    parameters.add(vScale.set("vScale", 0.04, 0.01, .5));
    
    parameters.add(flagA.set("flagA", true));
    parameters.add(flagB.set("flagB", false));
    parameters.add(flagC.set("flagC", false));
    parameters.add(flagD.set("flagD", false));
    parameters.add(offsetX.set("offsetX", 0, -100, 100));
    parameters.add(offsetY.set("offsetY", 0, -100, 100));
    parameters.add(endTime.set("timerEnd", 1000.0, 0.0, 6000.0));
    
    gui.setup(parameters);
    ofBackground(0);
    ofSetVerticalSync(false);
}

void GuiApp::update(){
    
}

void GuiApp::draw(){
    gui.draw();
}
