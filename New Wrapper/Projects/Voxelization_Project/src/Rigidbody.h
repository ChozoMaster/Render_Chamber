#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "Particle.h"
using namespace Eigen;
class RigidBody : public Particle{
public:
	RigidBody(float mass = 1, Vector3f position = Vector3f(0, 0, 0), Quaternion<float> rotation = Quaternion<float>(1, 0, 0, 0));
	Quaternion<float> m_rotation;
	Vector3f m_angularVelocity;
	Vector3f	m_sumOfTorques;
	Matrix3f m_inertiaTensor;
	Matrix3f m_invInertiaTensor;

	Vector3f getAngularAcceleration() const;
	void setInertiaTensor(Matrix3f val);
	void addTorque(const Vector3f& torque);
	Matrix3f getGlobalInvInertiaTensor() const;
	Matrix3f getRotationMatrix() const;
	void resetSumOfTorques();
	Eigen::Matrix4f transformationMatrix() const;
};