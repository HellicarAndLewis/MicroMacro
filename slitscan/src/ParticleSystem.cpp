#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() :
	timeStep(1){
}

void ParticleSystem::setup(int max, float timeStep) {
	pool.setup(max);
    setTimeStep(timeStep);
}

void ParticleSystem::setTimeStep(float timeStep) {
	this->timeStep = timeStep;
}

Particle* ParticleSystem::birth() {
	Particle* p = pool.popDeadParticle();
    if (p != NULL) p->setState(Particle::ALIVE);
    return p;
}


Particle* ParticleSystem::birth(ofVec3f pos, ofVec3f vel, float damping){
    Particle* p = birth();
    if (p != NULL) p->setup(pos, vel, damping);
    return p;
}

int ParticleSystem::size(){
	return pool.getAliveCount();
}

Particle* ParticleSystem::operator[](unsigned i) {
    Particle* p = pool.getAlive().at(i);
	return p;
}

void ParticleSystem::resetForces() {
    vector<Particle*>& particles = pool.getAlive();
	int n = pool.getAliveCount();
	for(int i = 0; i < n; i++)
		particles[i]->resetForce();
}

void ParticleSystem::applyEffectors(){
    for (list<ParticleEffector*>::iterator it = updateEffectors.begin(); it != updateEffectors.end(); ++it) {
        ParticleEffector& effector = **it;
        if(effector.enabled){
            effector.update();
            effector.apply(pool);
        }
    }
}

void ParticleSystem::addEffector(ParticleEffector& effector){
	updateEffectors.push_back(&effector);
}

void ParticleSystem::removeEffector(ParticleEffector& effector){
	updateEffectors.remove(&effector);
}

void ParticleSystem::clearEffectors(){
	updateEffectors.clear();
}

void ParticleSystem::update() {
    pool.update();
    applyEffectors();
    vector<Particle*>& particles = pool.getAlive();
	for(int i = 0; i < particles.size(); i++)
        particles[i]->updatePosition(timeStep);
    //ofLogVerbose() << "particles: " << pool.getTotalCount() << " alive: " << pool.getAliveCount();
}

void ParticleSystem::updatePool() {
     pool.update();
}

void ParticleSystem::draw() {
	vector<Particle*>& particles = pool.getAlive();
	int n = particles.size();
	for(int i = 0; i < n; i++) {
        particles[i]->draw();
    }
}
