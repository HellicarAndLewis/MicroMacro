//
//  Particle.h
//  slitscan
//
//  Created by Chris Mullany on 01/04/2015
//

#pragma once

#include "ofGraphics.h"

class Particle {
protected:
public:
	ofVec3f velocity;
	ofVec3f force;
    ofVec3f position;
    ofColor color;
    ofRectangle bounds;
    float damping;
    int life;
    
    enum State {
        BIRTHING, ALIVE, DYING, DEAD
    };
    State state;
    
    Particle(){
        velocity.set(0,0);
        bounds.set(0, 0, 0, 0);
        damping = 0;
        setState(DEAD);
        life = 0;
    }
    
	Particle(ofVec3f pos, ofVec3f velocity, float damping = 0.01) :
		position(pos.x, pos.y, pos.z),
		velocity(velocity),
        damping(damping),
        state(DEAD), life(0) {
	}
    
    virtual void setup(ofVec3f pos, ofVec3f velocity, float damping = 0.01) {
        position.set(pos.x, pos.y, pos.z);
        this->velocity = velocity;
        this->damping = damping;
        setState(ALIVE);
        life = 0;
	}
    
	virtual void updatePosition(float timeStep) {
		velocity += force;
        position.x += velocity.x * timeStep;
		position.y += velocity.y * timeStep;
		position.z += velocity.z * timeStep;
        life++;
	}
	virtual void resetForce() {
		force.set(0,0,0);
	}
    virtual void addForce(ofVec3f f){
        force += f;
    }
	virtual void addDampingForce(float damping = .01) {
		force = force - (velocity * this->damping);
	}
	virtual void draw() {
        // default draw
        ofCircle(position, 10);
        //ofRect(position, 10, 10);
	}
    
    
    // state
    void setState(State state){
        //ofLogVerbose() << state;
        if(this->state != state){
            this->state = state;
            if(this->state == ALIVE)
                life = 0;
        }
    }
    State getState(){
        return state;
    }
    bool isDead(){
        return state != DEAD;
    }
    bool isAlive(){
        return state == ALIVE;
    }
};
