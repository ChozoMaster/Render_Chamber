#pragma once
#include "rply.h"

#include <vector>
#include <Eigen/Core>
#include <iostream>
#include <iosfwd>
#include <array>
#include <fstream>

using namespace Eigen;
class FileLoader
{


public:
	
	static void load_triangle_mesh_raw(std::string const& filename, std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals);
	static void load_raw(std::string const& filename, std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals);
	static int vertex_cb(p_ply_argument argument);
	static int face_cb(p_ply_argument argument);
	static bool load_ply(std::string const& filename, std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals);
	static Eigen::Vector3f calculateMassCenter(std::vector<Eigen::Vector3f>& positions);
	static Eigen::Matrix3f calculateInertiaTensor(std::vector<Eigen::Vector3f>& positions, Eigen::Vector3f center, float pointMass = 1.0);
};