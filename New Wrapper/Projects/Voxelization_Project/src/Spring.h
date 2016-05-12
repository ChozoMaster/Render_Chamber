#pragma once
#include <Eigen/Core>
#include "Particle.h"
#include "Rigidbody.h"
using namespace Eigen;

class Spring{
public:
	Particle* m_A;
	Particle* m_B;
	Vector3f m_ContactPoint_A;
	Vector3f m_ContactPoint_B;
	Vector3f	m_Position_A;
	Vector3f	m_Position_B;

	float m_stiffness;
	float m_damping;
	float m_maxLength;
	float m_lengthRelaxed;
	float m_currentLength;

	Spring(RigidBody* rbStart, RigidBody* rbEnd, Vector3f contactPointStart, Vector3f contactPointEnd, float stiffness, float damping, float lengthRelaxed, float maxLength = -1.0);
	Spring(RigidBody* rbStart, Particle* pEnd, Vector3f contactPointStart, float stiffness, float damping, float lengthRelaxed, float maxLength = -1.0);
	Spring(Particle* pStart, RigidBody* rbEnd, Vector3f contactPointEnd, float stiffness, float damping, float lengthRelaxed, float maxLength = -1.0);
	Spring(Particle* pStart, Particle* pEnd, float stiffness, float damping, float lengthRelaxed, float maxLength = -1.0);
	void updateStartAndEndPosition();
	void initializeSpring(Particle* objStart, Particle* objEnd, Vector3f contactStart, Vector3f contactEnd, float stiffness, float damping, float lengthRelaxed, float maxLength);
	void updateCurrentLength();
	float calculateForce();
	void applyForces();
	void clampLength();
};