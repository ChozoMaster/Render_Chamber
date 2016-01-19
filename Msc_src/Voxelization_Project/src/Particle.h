#pragma once
#include <Eigen/Core>
#include <vector>
using namespace Eigen;
class Particle{
public:
	enum SimulationObjectType
	{
		PARTICLE,
		RIGIDBODY
	};
	std::vector<Vector3f> vertices;
	std::vector<Vector3f> faces;
	Particle(float mass = 1, Vector3f position = Vector3f(0, 0, 0));
	SimulationObjectType simObjType;
	float m_mass;
	float m_invMass;
	Vector3f m_velocity;
	Vector3f m_position;
	Vector3f m_sumOfForces;

	void addForce(const Vector3f& force);
	void resetSumOfForces();
	Vector3f getAcceleration() const;
	Eigen::Matrix4f translationnMatrix() const;
};