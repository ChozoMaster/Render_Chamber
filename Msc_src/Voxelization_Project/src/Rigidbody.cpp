#include "Rigidbody.h"




RigidBody::RigidBody(float mass /*= 1*/, Vector3f position /*= Vector3f(0, 0, 0)*/, Quaternion<float> rotation /*= Quaternion<float>(1, 0, 0, 0)*/)
:Particle(mass, position)
{
	m_rotation = rotation;
	m_angularVelocity = Vector3f(0, 0, 0);
	m_inertiaTensor = Matrix3f();
	m_invInertiaTensor = Matrix3f();
	resetSumOfTorques();
	simObjType = SimulationObjectType::RIGIDBODY;
}

Vector3f RigidBody::getAngularAcceleration() const
{
	Matrix3f rotMat = getRotationMatrix();
	Matrix3f rotMatT = rotMat.transpose();
	Matrix3f itGlobal = rotMatT * m_inertiaTensor * rotMat;   // TODO check anordnung
	Matrix3f itInvGlobal = rotMatT * m_invInertiaTensor * rotMat;
	// matrices are transposed, multiplication is "backwards"
	return (itInvGlobal * (m_sumOfTorques - (m_angularVelocity.cross(itGlobal * m_angularVelocity))));
}

void RigidBody::setInertiaTensor(Matrix3f val)
{
	m_inertiaTensor = val;
	m_invInertiaTensor = val.inverse();
}

void RigidBody::addTorque(const Vector3f& torque)
{
	m_sumOfTorques += torque;
}

Matrix3f RigidBody::getGlobalInvInertiaTensor() const
{
	Matrix3f rotMat = getRotationMatrix();
	return (rotMat * m_invInertiaTensor * rotMat.transpose());
}

Eigen::Matrix3f RigidBody::getRotationMatrix() const
{
	Matrix3f r = m_rotation.matrix();
	return r.transpose();
}

void RigidBody::resetSumOfTorques()
{
	m_sumOfTorques = Vector3f(0, 0, 0);
}

Eigen::Matrix4f RigidBody::transformationMatrix() const
{
	Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
	mat.block(0, 0, 3, 3) = getRotationMatrix();
	mat.col(3) = Eigen::Vector4f(m_position.x(), m_position.y(), m_position.z(), 1.0);
	return mat;
}