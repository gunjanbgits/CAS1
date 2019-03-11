#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

// HELPER FUNCTIONS

float ease(float p) {
    return 3*p*p - 2*p*p*p;
}

float ease(float p, float g) {
    if (p < 0.5)
        return 0.5 * pow(2*p, g);
    else
        return 1 - 0.5 * pow(2*(1 - p), g);
}


//--------------------------------------------------------------
void ofApp::setup(){
    
    counterA = 0;
    counterB = 0;
    
    juju = ofColor(255,226,170);
    
    modeSelector = "2";

    // Setup post-processing chain
    post.init(ofGetWidth(), ofGetHeight());
    post.createPass<BloomPass>()->setEnabled(true);
    
    myFont.load("MajorMonoDisplay-Regular.ttf", 48, true, true);
    myFontSmall.load("SpaceMono-Regular.ttf", 10, true, true);
    
    //ofSetVerticalSync(true);
    ofBackground(0);
    ofSetFrameRate(60);
    
    width = ofGetWidth();
    height = ofGetHeight();
    
//    //GUI SETUP
//    gui.setup();
//    gui.add(camOffsetX.set("CAM offsetX", 1, 1, 640));
//    gui.add(camOffsetY.set("CAM offsetY", 1, 1, 384));
//    gui.add(minArea.set("Min area", 1, 1, 100));
//    gui.add(maxArea.set("Max area", 200, 1, 500));
//    gui.add(blurAmount.set("Blur", 5, 1, 100));
//    gui.add(thresholdVal.set("Threshold", 168, 0, 255));
//    gui.add(holes.set("Holes", false));
//    gui.add(fieldOfGlow.set("RangeOfInfluence", 10, 1, 40));
//    gui.add(proximity.set("Proximity", 1, 5, 40));
//    
//    
//    // Particle GUI
//    gui.add(timeMult.set("timeMult", .2, .005, .5));
//    gui.add(scaleMult.set("ScaleMult", .02, .005, .5));
//    gui.add(radius.set("Radius", .2, .1, 5));
//    gui.add(numFrames.set("numFrames", 75, 60, 600));
//    //gui.add(gridScale.set("gridScale", 50, 20, 80));
//    gui.add(vScale.set("vScale", 0.04, 0.01, .5));
    
    //Load Movie
    
        movie.load("juju1.mov");
        movie.play();
    
    //CAM FEED
    
//    movie.setDeviceID(1);
//    movie.setup(1280, 768);
    
    contourFinder.setMinAreaRadius(1);
    contourFinder.setMaxAreaRadius(100);
    contourFinder.setThreshold(15);
    
    // wait for half a second before forgetting something
    contourFinder.getTracker().setPersistence(15);
    
    // an object can move up to 32 pixels per frame
    contourFinder.getTracker().setMaximumDistance(32);
    showLabels = true;
    
    //Particle Setup
    cols = 64;//round(width / gridScale);
    rows = 42;//round(height / gridScale);
    
    null.set(0,0);
    contPoints.push_back(null);
    
    flowField.resize(rows*cols);
    
    for(int i = 0; i< flowField.size(); i++){
        flowField[i] = null;
    }
    
    int num = 1000;
    p.assign(num, Particle());
    resetParticles();
    
}

// Reset Particles System !!
//--------------------------------------------------------------
void ofApp::resetParticles(){

    attractPoints.clear();
    attractPointsWithMovement.clear();
    
    for(unsigned int i = 0; i < p.size(); i++){
        p[i].setAttractPoints(&attractPointsWithMovement);
        p[i].reset();
    }
}

//--------------------------------------------------------------
// Create Flow Field

void ofApp::createFlowField(){
    
    // Flow Field Setup Area !!
    float t = gui->timeMult*ofGetFrameNum()/gui->numFrames;
    //int t = 1;
    for (int y = 0; y < rows; y++){
        for (int x = 0; x < cols; x++){
            int index = x + y * cols;

            float nsFactor2 = ofMap(ofNoise(gui->scaleMult*x,gui->scaleMult*y,gui->radius*cos(TWO_PI*t),gui->radius*sin(TWO_PI*t)),0,1,-25,25);
            float nsFactor2a = ofMap(ofNoise(200+gui->scaleMult*x,gui->scaleMult*y,gui->radius*cos(TWO_PI*t),gui->radius*sin(TWO_PI*t)),0,1,0,1);
            float l = glm::sqrt(glm::pow(20,2)+glm::pow(20,2));
            float vx = l*cos(nsFactor2);
            float vy = l*sin(nsFactor2);
            float intensity = 0.1+1.5 * ease(nsFactor2a,3.5);
            vx*=intensity;
            vy*=intensity;
            ofPoint flow;
            flow.x = vx;
            flow.y = vy;
            flow.limit(gui->vScale);
            //flow.normalize();
            flowField[index] = flow;
            ofPushMatrix();
            ofTranslate(x+x*20, y+y*20);
            ofSetColor(juju,85);
            ofDrawLine(0,0,vx,vy);
            ofPopMatrix();
        }
    }
}


//--------------------------------------------------------------
// Draw Particles

void ofApp::drawParticles(){
    
    ofSetColor(juju);
    
    for(unsigned int i = 0; i < p.size(); i++){
        
        if(gui->flagA){
            p[i].follow(flowField, 40, cols);
        }
        p[i].update();
        p[i].draw();
        p[i].edges();
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    contPoints.clear();
    
    // Contour FInder Setup Area !!
    
    movie.update();
    if(movie.isFrameNew()) {
        blur(movie, gui->blurAmount);
        
        cropSample = movie.getPixels();
        cropSample.crop(gui->camOffsetX, gui->camOffsetY, gui->cropWidth, gui->cropHeight);
        
        if (gui->invert){
        invert(movie);
        }
        contourFinder.setMinAreaRadius(gui->minArea);
        contourFinder.setMaxAreaRadius(gui->maxArea);
        contourFinder.setThreshold(gui->thresholdVal);
        contourFinder.findContours(cropSample);
        contourFinder.setSortBySize(true);
        contourFinder.setSimplify(true);
        contourFinder.setFindHoles(gui->holes);
        
        if(modeSelector == "0"){
            ofxCv::copy(movie, imgBlur);
            blur(imgBlur, gui->blurAmount);
            imgBlur.update();
            
            convertColor(imgBlur, imgThresh, CV_RGB2GRAY);
            threshold(imgThresh, gui->thresholdVal);
            imgThresh.update();
            
            ofxCv::copy(imgThresh, imgInvert);
            invert(imgInvert);
            imgInvert.update();
        }
        
        int n = contourFinder.size();
        quads.clear();
        quads.resize(n);
        for(int i = 0; i < n; i++) {
            quads[i] = contourFinder.getFitQuad(i);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){


    //ofSetBackgroundAuto(showLabels);
    ofSetLineWidth(2);
    RectTracker& tracker = contourFinder.getTracker();
    
    float t1 = gui->timeMult*10*ofGetFrameNum()/gui->numFrames;
    float t2 = gui->timeMult*10*ofGetFrameNum()/gui->numFrames;
    float animColor = ofMap(sin(t2), -1, 1, 127, 255);
    float animSize = ofMap(sin(t2), -1, 1, 0, 100);
    
    if(modeSelector == "0"){
        ofBackground(0);
        cropSample.draw(0,0,640,400);
        imgBlur.draw(640,0,640,400);
        imgThresh.draw(0,400,640,400);
        imgInvert.draw(640,400,640,400);
        
//        ofBackground(0);
//        ofSetColor(juju);
//        createFlowField();
//        drawParticles();
        
    }
    
    else if(modeSelector == "2"){
        ofBackground(0);
        cropSample.draw((width - gui->cropWidth)/2,(height - gui->cropHeight)/2);
        //movie.getTexture().drawSubsection(width/2-320, height/2-180, 640, 360, camOffsetX, camOffsetY, 640, 360);
    }
    
    else if(modeSelector == "1"){
        ofBackground(0);
        post.begin();
        ofSetColor(juju);
        createFlowField();
        drawParticles();
        if (contourFinder.size()<1){
            ofSetColor(juju, 200);
            ofNoFill();
            ofDrawRectangle(width/2 - (gui->cropWidth+2)/2, height/2-(gui->cropHeight+2)/2 , gui->cropWidth+2, gui->cropHeight+2);
            ofSetColor(0,200);
            ofFill();
            ofDrawRectangle(width/2 - gui->cropWidth/2, height/2-gui->cropHeight/2 , gui->cropWidth, gui->cropHeight);
            string msg = "juju";
            ofSetColor(juju);
            myFont.drawString(msg,width/2-myFont.stringWidth(msg)/2, height/2-myFont.stringHeight(msg)/2+40);
            string msg2 = "PLACE JUJU CARD HERE";
            ofSetColor(juju, animColor);
            myFontSmall.setLetterSpacing(1.8);
            myFontSmall.drawString(msg2,width/2-myFontSmall.stringWidth(msg2)/2, height/2-myFontSmall.stringHeight(msg2)/2+60);
        }
        else if(contourFinder.size()>1){
                ofSetColor(juju, 200);
                ofNoFill();
                ofDrawRectangle(width/2 - (gui->cropWidth+2)/2, height/2-(gui->cropHeight+2)/2 , gui->cropWidth+2, gui->cropHeight+2);
                ofSetColor(0,200);
                ofFill();
                ofDrawRectangle(width/2 - gui->cropWidth/2, height/2-gui->cropHeight/2 , gui->cropWidth, gui->cropHeight);
                ofPushMatrix();
                ofTranslate((width - gui->cropWidth)/2,(height - gui->cropHeight)/2);
                ofSetColor(118,255,208);
                contourFinder.draw();
                for(int i = 0; i < contourFinder.size(); i++) {
                    ofPoint center = toOf(contourFinder.getCenter(i));  // Get Centers of the Shape
                    contPoints.push_back(center);
                    
                    ofPushMatrix();
                    ofSetColor(juju);
                    ofTranslate(center.x, center.y);
                    int label = contourFinder.getLabel(i);
                    string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label)) + ":" + ofToString(i);
                    ofDrawBitmapString(msg, 0, -20);
                    ofVec2f velocity = toOf(contourFinder.getVelocity(i));
                    ofScale(5, 5);
                    ofDrawLine(0, 0, velocity.x, velocity.y);
                    ofPopMatrix();
                    
                    
                    // Proximity Check
                    
                    float circleRadius;
                    ofVec2f circleCenter = toOf(contourFinder.getMinEnclosingCircle(i, circleRadius));
                    for(int j = 0; j<contourFinder.size(); j++){
                        ofPoint centerProximity = toOf(contourFinder.getCenter(j));
                        
                        float circleProximityRadius;
                        ofVec2f circleProximityCenter = toOf(contourFinder.getMinEnclosingCircle(j, circleProximityRadius));
                        float distance = center.distance(circleProximityCenter);
                        
                        if(distance < circleRadius + circleProximityRadius + gui->proximity && i != j){
                            float midX = (center.x+circleProximityCenter.x)/2;
                            float midY = (center.y+circleProximityCenter.y)/2;
                            ofSetColor(255,118,119);
                            ofNoFill();
                            ofDrawCircle(midX, midY, distance);
                            ofSetColor(juju);
                        }
                    }
                    //Proximity Check End
                    ofSetColor(juju);
                    //polyline drawing;
                    ofPolyline shape(contPoints);
                    ofSetLineWidth(2);
                    shape.draw();
                    }
                ofPopMatrix();
                }
        post.end();
        

        
        
    
    attractPointsWithMovement.clear();
    
    for(int i=0; i< contPoints.size(); i++){
        attractPointsWithMovement.push_back(contPoints[i]);
    }
    
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
    //ofDrawBitmapString(ofToString(second->lol), 20, 20);
    
    
        
    }
    
//    // Gui Draw
//    if(guiFlag){
//        gui.draw();
//    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
    if(key == ' ') {
        showLabels = !showLabels;
        resetParticles();
        modeSelector = "0";
    }
    
    if(key == '1') {
        modeSelector = "1";
    }
    
    if(key == '2') {
        modeSelector = "2";
    }
    
    if(key == '3') {
        modeSelector = "3";
    }
    
    if(key == 'p') {
        guiFlag = !guiFlag;
    }
    
    if(key == 'l') {
        flowFieldFlag = !flowFieldFlag;
    }
    
    
}
