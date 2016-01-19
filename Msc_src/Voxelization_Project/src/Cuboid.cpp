#include "Cuboid.h"

Cuboid::Cuboid(Vector3f size, float mass, Vector3f position, Eigen::Quaternion<float> rotation) :
RigidBody(mass, position, rotation)
{
	m_size = size;
	float j11 = (m_size[1] * m_size[1] + m_size[2] * m_size[2]) / float(12);
	float j22 = (m_size[0] * m_size[0] + m_size[2] * m_size[2]) / float(12);
	float j33 = (m_size[0] * m_size[0] + m_size[1] * m_size[1]) / float(12);
	Eigen::Matrix3f tempmat = Eigen::Matrix3f::Identity();;
	tempmat.col(0) = Eigen::Vector3f(j11, 0, 0);
	tempmat.col(1) = Eigen::Vector3f(0, j22, 0);
	tempmat.col(2) = Eigen::Vector3f(0, 0, j33);
	setInertiaTensor(tempmat);
}

Cuboid::Cuboid(Eigen::Matrix3f inertia, float mass, Vector3f position, Eigen::Quaternion<float> rotation) :
RigidBody(mass, position, rotation)
{

	setInertiaTensor(inertia);
}

