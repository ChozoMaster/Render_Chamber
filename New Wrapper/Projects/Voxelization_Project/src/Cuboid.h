#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "RigidBody.h"
using namespace Eigen;
class Cuboid : public RigidBody{
public:
	Vector3f m_size;

	Cuboid(Vector3f size, float mass, Vector3f position = Vector3f(0, 0, 0), Quaternion<float> rotation = Quaternion<float>(1, 0, 0, 0));
	Cuboid(Eigen::Matrix3f inertia, float mass, Vector3f position = Vector3f(0, 0, 0), Eigen::Quaternion<float> rotation = Quaternion<float>(1, 0, 0, 0));
};