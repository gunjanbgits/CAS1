//
//  particle.cpp
//  flowFieldBase
//
//  Created by Gunjan Bhutani on 3/2/19.
//


#include "particle.h"



//------------------------------------------------------------------
Particle::Particle(){
    attractPoints = NULL;
}

//------------------------------------------------------------------
void Particle::setAttractPoints( vector <ofPoint> * attract ){
    attractPoints = attract;
}


//------------------------------------------------------------------
void Particle::reset(){
    //the unique val allows us to set properties slightly differently for each particle
    uniqueVal = ofRandom(-10000, 10000);
    
    pos.x = ofRandom(0,1280);
    pos.y = ofRandom(0,800);
    
//    vel.x = ofRandom(-1,1);
//    vel.y = ofRandom(-1,1);

    vel.x = 0;
    vel.y = 0;
    
    
    acc   = ofPoint(0,0);
    
    scale = ofRandom(0.5, 1.0);

    drag  = ofRandom(0.95, 0.998);

}

void Particle::follow(vector<ofPoint> _flowField, int _gridScale, int _cols){
    int x = floor(pos.x / _gridScale);
    int y = floor(pos.y / _gridScale);
    int index = x + y * _cols;
    
    //testVal = _flowField[index].x;
    ofPoint flowForce;
    flowForce.set(_flowField[index]);
    //testVal = flowForce.x;
    applyForce(flowForce);
    
}

void Particle::applyForce(ofPoint force){
    acc += force;
    //acc.normalize();
}

void Particle::update(){

//    ofPoint attractPt(ofGetMouseX(), ofGetMouseY());
//    frc = attractPt-pos; // we get the attraction force/vector by looking at the mouse pos relative to our pos
//    frc.normalize(); //by normalizing we disregard how close the particle is to the attraction point
//
//    vel += frc * 0.01; //apply force
    
    if( attractPoints ){
        
        //1 - find closest attractPoint
        ofPoint closestPt;
        int closest = -1;
        float closestDist = 9999999;
        
        for(unsigned int i = 0; i < attractPoints->size(); i++){
            float lenSq = ( attractPoints->at(i)-pos ).lengthSquared();
            if( lenSq < closestDist ){
                closestDist = lenSq;
                closest = i;
            }
        }
        
        //2 - if we have a closest point - lets calcuate the force towards it
        if( closest != -1 ){
            closestPt = attractPoints->at(closest);
            float dist = sqrt(closestDist);
            
            //in this case we don't normalize as we want to have the force proportional to distance
            frc = closestPt - pos;
            
            vel *= drag;
            
            //lets also limit our attraction to a certain distance and don't apply if 'f' key is pressed
            if( dist < 100 && dist > 40 && !ofGetKeyPressed('f') ){
                vel -= frc * 0.003;
            }else{
                //if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy.
                frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
                frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
                vel -= frc * 0.025;
            }
            
        }
        
    }
    
    
    
    
    
    
    
    vel *= drag; //apply drag
    vel += acc*0.029; //apply force

    //2 - UPDATE OUR POSITION
    
    pos += vel;
    
    acc*=.91;
    
    vel.limit(0.5);
    
}

void Particle::edges(){
    
    //3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN
    //we could also pass in bounds to check - or alternatively do this at the ofApp level
    if( pos.x > 1270 ){
        pos.x = 10;
    }else if( pos.x < 10 ){
        pos.x = 1270;
    }
    if( pos.y > 790 ){
        pos.y = 10;
    }
    else if( pos.y < 10 ){
        pos.y = 790;
    }
    
}

//------------------------------------------------------------------
void Particle::draw(){
        
    ofSetColor(255);
    ofFill();
    //ofDrawBitmapString(ofToString(testVal), pos.x, pos.y);
    ofDrawRectangle(pos.x, pos.y, scale * 5.0 , scale * 5.0);
}
