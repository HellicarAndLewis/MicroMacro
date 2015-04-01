//
//  ParticleEffector.h
//  slitscan
//
//  Created by Chris Mullany on 01/04/2015
//

#pragma once
#include "Particle.h"
#include "ParticlePool.h"
#include <set>

using namespace std;

class ParticleEffector {
public:
	ParticleEffector() {
		enabled = true;
	}
	
	virtual void apply(ParticlePool& pool){
		if (enabled) apply(pool.getAlive());
	}
	
	virtual void apply(std::set<Particle*>& particles){
		std::set<Particle*>::iterator it;
		for(it = particles.begin(); it != particles.end(); ++it){
			Particle* particle = *it;
			applyTo(particle);
		}			
	}
	
	virtual void apply(vector<Particle*>& particles){
		vector<Particle*>::iterator it;
		for(it = particles.begin(); it != particles.end(); ++it){
			Particle* particle = *it;
			applyTo(particle);
		}			
	}
	
    virtual void update(){
        // override me
    }
    
	virtual void applyTo(Particle* particle){
        // override me
	}
    
    bool enabled;
    
protected:
};






