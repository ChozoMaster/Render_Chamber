#pragma once
#include "rply.h"

#include <vector>
#include <Eigen/Core>
#include <iostream>
#include <iosfwd>
#include <array>
#include <fstream>
#include "GlobalDefines.h"

using namespace Eigen;
class FileLoader
{


public:
	
	static void load_triangle_mesh_raw(std::string const& filename, v3f_list& vertices, v3i_list& faces, v3f_list& normals);
	static void load_raw(std::string const& filename, v3f_list& vertices, v3i_list& faces, v3f_list& normals);
	static int vertex_cb(p_ply_argument argument);
	static int face_cb(p_ply_argument argument);
	static bool load_ply(std::string const& filename, v3f_list& vertices, v3i_list& faces, v3f_list& normals);
	static Eigen::Vector3f calculateMassCenter(v3f_list& positions);
	static Eigen::Matrix3f calculateInertiaTensor(v3f_list& positions, Eigen::Vector3f center, float pointMass = 1.0);
};