//
//  ParticlePool.h
//  slitscan
//
//  Created by Chris Mullany on 01/04/2015
//

#pragma once
#include "Particle.h"
#include <list>
#include <set>
#include <vector>

class ParticlePool {
public:
	
	ParticlePool(){
		particleCount = 0;
	}
	
	~ParticlePool(){
	}
	
	virtual void setup(int particleCount){
		if(particleCount != this->particleCount){
			this->particleCount = particleCount;
			setup();
		}
	}
	
	virtual void setup(){
		particles.clear();
		alive.clear();
		dead.clear();
        
        particles.assign(particleCount, Particle());
		particles.reserve(particleCount);
		alive.reserve(particleCount);
		dead.reserve(particleCount);
        
        update();
	}
	
	/**
	 separate all particles into alive and dead particle lists
	 */
	virtual void update(){
		
		alive.clear();
		dead.clear();
		
		Particle* particle;
        for(int i=0; i<particles.size(); i++){
			particle = &particles[i];
			if (particle->isAlive()) {
				alive.push_back(particle);
			}
            else {
				dead.push_back(particle);
			}
		}
		
	}
	
	virtual int getAliveCount(){
		return alive.size();
	}
	
	virtual int getDeadCount(){
		return dead.size();
	}
	
	virtual int getTotalCount(){
		return particles.size();
	}
	
	virtual vector<Particle>& getParticles(){
		return particles;
	}
	
	virtual vector<Particle*>& getAlive(){
		return alive;
	}
	
	virtual vector<Particle*>& getDead(){
		return dead;
	}
	
	Particle* popDeadParticle(){
		if(dead.size() == 0) return NULL;
		Particle* particle = dead.back();
		dead.pop_back();
		return particle;
	}
	
protected:
    
	vector<Particle> particles;
	vector<Particle*> alive;
	vector<Particle*> dead;
	
	int particleCount;
    
};