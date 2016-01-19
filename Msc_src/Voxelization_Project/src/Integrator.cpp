#include "Integrator.h"
#include "parameters.h"


void Integrator::Euler(std::vector<Particle*>& particles, std::vector<RigidBody*>& rigidBodies, std::vector<Spring*>& springs)
{
	float h = Parameter::getInstance()->timeStepSize;
	prepareForNextTimestep(particles, rigidBodies, springs);
	{
		float mass;
		Particle* p;
		for (std::vector<Particle*>::iterator pIt = particles.begin(); pIt != particles.end(); ++pIt)
		{
			p = *pIt;
			mass = p->m_mass;
			if (mass == 0.0) continue;

			p->m_position = p->m_position + h * p->m_velocity;
			p->m_velocity = p->m_velocity + h * p->getAcceleration();
		}
	}

	{
		float mass;
		RigidBody* r;
		for (std::vector<RigidBody*>::iterator rIt = rigidBodies.begin(); rIt != rigidBodies.end(); ++rIt)
		{
			r = *rIt;
			mass = r->m_mass;
			if (mass == 0.0) continue;	// objects without mass are treated as static
			r->m_position = r->m_position + h * r->m_velocity;
			r->m_velocity = r->m_velocity + h * r->getAcceleration();
			Eigen::Quaternion<float> rot = r->m_rotation;
			Vector3f av = r->m_angularVelocity;
			Eigen::Quaternion<float> partialRot = Eigen::Quaternion<float>(float(0), av[0], av[1], av[2]) * rot;
			partialRot.coeffs() *= h * 0.5;
			rot.coeffs() += partialRot.coeffs();
			rot.normalize();

			r->m_angularVelocity = (r->m_angularVelocity + h * r->getAngularAcceleration());
			r->m_rotation = rot;
		}
	}
}

void Integrator::prepareForNextTimestep(std::vector<Particle*>& particles, std::vector<RigidBody*>& rigidBodies, std::vector<Spring*>& springs)
{
	// reset forces of particles
	for (std::vector<Particle*>::iterator pIt = particles.begin(); pIt != particles.end(); ++pIt)
	{
		(*pIt)->resetSumOfForces();
	}

	for (std::vector<RigidBody*>::iterator rIt = rigidBodies.begin(); rIt != rigidBodies.end(); ++rIt)
	{
		(*rIt)->resetSumOfForces();
		(*rIt)->resetSumOfTorques();
	}

	// sum up all forces of the springs
	for (std::vector<Spring*>::iterator sIt = springs.begin(); sIt != springs.end(); ++sIt)
	{
		(*sIt)->applyForces();
	}
}

