#include "Spring.h"
#include <iostream>
#include "GL\glew.h"

float m_color[4] = {1.0, 0.0, 0.0, 1.0};


Spring::Spring(RigidBody* rbStart, RigidBody* rbEnd, Vector3f contactPointStart, Vector3f contactPointEnd, float stiffness, float damping, float lengthRelaxed, float maxLength)
{
	initializeSpring(rbStart, rbEnd, contactPointStart, contactPointEnd, stiffness, damping, lengthRelaxed, maxLength);
}

Spring::Spring(RigidBody* rbStart, Particle* pEnd, Vector3f contactPointStart, float stiffness, float damping, float lengthRelaxed, float maxLength)
{
	initializeSpring(rbStart, pEnd, contactPointStart, Vector3f(0, 0, 0), stiffness, damping, lengthRelaxed, maxLength);
}

Spring::Spring(Particle* pStart, RigidBody* rbEnd, Vector3f contactPointEnd, float stiffness, float damping, float lengthRelaxed, float maxLength)
{
	initializeSpring(pStart, rbEnd, Vector3f(0, 0, 0), contactPointEnd, stiffness, damping, lengthRelaxed, maxLength);
}

Spring::Spring(Particle* pStart, Particle* pEnd, float stiffness, float damping, float lengthRelaxed, float maxLength)
{
	initializeSpring(pStart, pEnd, Vector3f(0, 0, 0), Vector3f(0, 0, 0), stiffness, damping, lengthRelaxed, maxLength);
}

void Spring::initializeSpring(Particle* objStart, Particle* objEnd, Vector3f contactStart, Vector3f contactEnd, float stiffness, float damping, float lengthRelaxed, float maxLength)
{
	m_A = objStart;
	m_B = objEnd;
	m_ContactPoint_A = contactStart;
	m_ContactPoint_B = contactEnd;
	m_stiffness = stiffness;
	m_damping = damping;
	m_lengthRelaxed = lengthRelaxed;
	m_maxLength = maxLength;
	updateStartAndEndPosition();
}

void Spring::updateCurrentLength()
{
	m_currentLength = (m_Position_A - m_Position_B).norm();
}


void Spring::updateStartAndEndPosition()
{
	switch (m_A->simObjType)
	{

	case Particle::SimulationObjectType::PARTICLE:
	{
		 Particle *p = static_cast<Particle *>(m_A);
		 if (p)
		 {
			 m_Position_A = p->m_position;
		 }
		 break;
	}
	case Particle::SimulationObjectType::RIGIDBODY:
	{
		  RigidBody *r = static_cast<RigidBody *>(m_A);
		  if (r)
		  {
			  m_Position_A = r->m_position + (r->getRotationMatrix() * m_ContactPoint_A);
		  }
		  break;
	}
	default:
		std::cout << "Unknown SimulationObjectType: " << m_A->simObjType << std::endl;
	}


	switch (m_B->simObjType)
	{
	case Particle::SimulationObjectType::PARTICLE:
	{
		 Particle *p = static_cast<Particle *>(m_B);
		 if (p)
		 {
			 m_Position_B = p->m_position;
		 }
		 break;
	}
	case Particle::SimulationObjectType::RIGIDBODY:
	{
		  RigidBody *r = static_cast<RigidBody *>(m_B);
		  if (r)
		  {
			  m_Position_B = r->m_position + (r->getRotationMatrix() * m_ContactPoint_B);
		  }
		  break;
	}
	default:
		std::cout << "Unknown SimulationObjectType: " << m_B->simObjType << std::endl;
	}
}

void Spring::clampLength()
{
	// negative maximum length disables clamping
	if (m_maxLength < 0)
	{
		return;
	}
	updateStartAndEndPosition();
	updateCurrentLength();
	if (m_currentLength < m_maxLength)
	{
		return;
	}

	Particle* oSta = dynamic_cast<Particle*>(m_A);
	Particle* oEnd = dynamic_cast<Particle*>(m_B);

	// direction from start to end
	Vector3f dir = (m_Position_B - m_Position_A);
	dir.normalize();
	float lDiff = (m_currentLength - m_maxLength);
	// if start object has mass 0 it is static
	if (oSta->m_mass == 0)
	{
		// if end object also has mass 0 it is static as well, do nothing
		if (oEnd->m_mass == 0)
		{
			return;
		}
		Vector3f newEPos = oEnd->m_position - lDiff * dir;
		oEnd->m_position = (newEPos);
	}
	// if end object has mass 0 it is static
	else if (oEnd->m_mass == 0)
	{
		// no need to check if start object has mass 0, since it was checked in the previous if-statement

		Vector3f newSPos = oSta->m_position + lDiff * dir;
		oSta->m_position = (newSPos);
	}
	// if none of the objects has mass 0, they both will be moved by half the distance
	else
	{
		Vector3f shift = 0.5 * lDiff * dir;
		Vector3f newSPos = oSta->m_position + shift;
		oSta->m_position = (newSPos);
		Vector3f newEPos = oEnd->m_position - shift;
		oEnd->m_position = (newEPos);
	}
}

float Spring::calculateForce()
{
	float springForce = m_stiffness*(m_lengthRelaxed - m_currentLength);
	return springForce;
}

void Spring::applyForces()
{
	updateStartAndEndPosition();
	updateCurrentLength();

	float forceValue = calculateForce();
	Vector3f normalizedSpringVector = (m_Position_B - m_Position_A);
	normalizedSpringVector.normalized();

	Particle *p1 = m_A;
	Particle *p2 = m_B;

	float relativeVelocity = (p2->m_velocity - p1->m_velocity).dot(normalizedSpringVector);
	float dampingValue = m_damping*relativeVelocity;

	Vector3f springForceOn2ndObj = (forceValue - dampingValue)*normalizedSpringVector;
	Vector3f springForceOn1stObj = -springForceOn2ndObj;

	switch (m_A->simObjType)
	{
	case Particle::SimulationObjectType::PARTICLE:
	{
		 Particle *p = static_cast<Particle *>(m_A);
		 if (p)
		 {
			 p->addForce(springForceOn1stObj);
		 }
		 break;
	}
	case Particle::SimulationObjectType::RIGIDBODY:
	{
		RigidBody *r = static_cast<RigidBody *>(m_A);
		if (r)
		{
		  r->addForce(springForceOn1stObj);
		  r->addTorque((springForceOn1stObj).cross(r->getRotationMatrix() * m_ContactPoint_A));
		}
		break;
	}
	default:
		std::cout << "Unknown SimulationObjectType: " << m_A->simObjType << std::endl;
	}

	switch (m_B->simObjType)
	{
	case Particle::SimulationObjectType::PARTICLE:
	{
		 Particle *p = static_cast<Particle *>(m_B);
		 if (p)
		 {
			 p->addForce(springForceOn2ndObj);
		 }
		 break;
	}
	case Particle::SimulationObjectType::RIGIDBODY:
	{
		 RigidBody *r = static_cast<RigidBody *>(m_B);
		 if (r)
		 {
			 r->addForce(springForceOn2ndObj);
			 r->addTorque((springForceOn2ndObj).cross(r->getRotationMatrix() * m_ContactPoint_B));
		 }
		 break;
	}
	default:
		std::cout << "Unknown SimulationObjectType: " << m_B->simObjType << std::endl;
	}
}
