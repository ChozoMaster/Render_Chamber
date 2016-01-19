#include "Particle.h"
#include "parameters.h"

Particle::Particle(float mass /*= 1*/, Vector3f position /*= Vector3f(0, 0, 0)*/)
{
	m_mass = mass;
	m_invMass = 1.0 / mass;
	m_position = position;
	resetSumOfForces();
	m_velocity = Vector3f(0, 0, 0);
	simObjType = SimulationObjectType::PARTICLE;
}

void Particle::addForce(const Vector3f& force)
{
	m_sumOfForces += force;
}

void Particle::resetSumOfForces()
{
	m_sumOfForces = m_mass * GRAVITY;
}

Eigen::Vector3f Particle::getAcceleration() const
{
	return m_invMass * m_sumOfForces;
}

Eigen::Matrix4f Particle::translationnMatrix() const
{
	Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
	mat.col(3) = Eigen::Vector4f(m_position.x(), m_position.y(), m_position.z(), 1.0);
	return mat;
}

