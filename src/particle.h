//
//  particle.h
//  flowFieldBase
//
//  Created by Gunjan Bhutani on 3/2/19.
//

enum particleMode{
    PARTICLE_MODE_ATTRACT = 0,
    PARTICLE_MODE_REPEL,
    PARTICLE_MODE_NEAREST_POINTS,
    PARTICLE_MODE_NOISE
};

#pragma once
#include "ofMain.h"

class Particle{
    
public:
    Particle();
    
    void setMode(particleMode newMode);
    void setAttractPoints( vector <ofPoint> * attract );
    
    void reset();
    void update();
    void draw();
    void edges();
    void applyForce(ofPoint Force);
    void follow(vector<ofPoint> _flowField, int _gridScale, int _cols);
    
    ofPoint pos;
    ofPoint vel;
    ofPoint acc;
    ofPoint frc;
    
    float maxSpeed;

    float testVal;
    
    float drag;
    float uniqueVal;
    float scale;
    
    particleMode mode;
    
    vector <ofPoint> * attractPoints;
};
