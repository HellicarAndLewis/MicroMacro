//
//  ParticleSystem.h
//  slitscan
//
//  Created by Chris Mullany on 01/04/2015
//

#pragma once

#include "Tree.h"
#include "Particle.h"
#include "ParticlePool.h"
#include "ParticleEffector.h"

class ParticleSystem {
protected:
	float timeStep;
    ParticlePool pool;
    list<ParticleEffector*> updateEffectors;

public:
	ParticleSystem();
	void setup(int max = 3000, float timeStep = 1);
	void setTimeStep(float timeStep);
    
	Particle* birth();
	Particle* birth(ofVec3f pos, ofVec3f vel, float damping = 0.01);
    
	vector<Particle*> getNeighbors(float x, float y, float radius) const;
    
	int size();
	Particle* operator[](unsigned i);

	void resetForces();
    
    // effectors
    virtual void applyEffectors();
    void addEffector(ParticleEffector& effector);
	void removeEffector(ParticleEffector& effector);
	void clearEffectors();
    
	void update();
	void updatePool();
	void draw();
    
};
