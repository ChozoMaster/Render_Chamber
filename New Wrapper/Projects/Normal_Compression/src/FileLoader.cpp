#include "FileLoader.h"
#include "Utility.cpp"
#include "config.hpp"

void FileLoader::load_triangle_mesh_raw(std::string const& filename, v3f_list& vertices, v3i_list& faces, v3f_list& normals)
{
	std::cout << "\nRead " << filename << "." << std::endl;
	std::ifstream input(filename);

	if (input.good())
	{
		input.close();
		load_raw(filename, vertices, faces, normals);
	}
	else
	{
		input.close();

		std::ostringstream fqfn;
		fqfn << path_resources;
		fqfn << filename;
		load_raw(fqfn.str(), vertices, faces, normals);
	}
	std::cout << "  #vertices " << vertices.size() << std::endl;
	std::cout << "  #faces    " << faces.size() << std::endl;
	Utility::calculate_vertex_normals_from_triangle_mesh(vertices, faces, normals);
}


void FileLoader::load_raw(std::string const& filename, v3f_list& vertices, v3i_list& faces, v3f_list& normals)
{

	std::ifstream input(filename, std::ios::in | std::ios::binary);

	if (input.fail())
	{
		std::ostringstream error_message;
		error_message << "Error: Can not open "
			<< filename << "." << std::endl;

		throw std::exception(error_message.str().c_str());
	}

	unsigned int nv;
	input.read(reinterpret_cast<char*>(&nv), sizeof(unsigned int));
	vertices.resize(nv);
	for (unsigned int i(0); i < nv; ++i)
	{
		input.read(reinterpret_cast<char*>(vertices[i].data()),
			3 * sizeof(float));
	}

	unsigned int nf;
	input.read(reinterpret_cast<char*>(&nf), sizeof(unsigned int));
	faces.resize(nf);

	for (unsigned int i(0); i < nf; ++i)
	{
		input.read(reinterpret_cast<char*>(faces[i].data()),
			3 * sizeof(unsigned int));
	}

	input.close();

}


Eigen::Vector3f FileLoader::calculateMassCenter(v3f_list& positions){
	Eigen::Vector3f center(0.0, 0.0, 0.0);
	for (int i = 0; i < positions.size(); i++){
		center += positions.at(i);
	}
	center /= positions.size();
	return center;
}

Eigen::Matrix3f FileLoader::calculateInertiaTensor(v3f_list& positions, Eigen::Vector3f center, float pointMass){
	Eigen::Matrix3f inertiaTensor = Eigen::Matrix3f::Zero();
	Eigen::Vector3f pos;
	float x,y,z,x_sqr, y_sqr, z_sqr;
	for (int i = 0; i < positions.size(); i++){
		pos = positions.at(i);
		pos -= center;
		x = pos.x();
		y = pos.y();
		z = pos.z();
		x_sqr = std::pow(x, 2);
		y_sqr = std::pow(y, 2);
		z_sqr = std::pow(z, 2);
		Eigen::Matrix3f temp;
		temp << y_sqr + z_sqr,  -x*y, -x*z, -y*x, x_sqr + z_sqr, -y*z, -z*x, -z*y, x_sqr + y_sqr;
		inertiaTensor += temp;
	}
	return (pointMass * inertiaTensor) / positions.size();
}

int FileLoader::vertex_cb(p_ply_argument argument) {
	long value_index;
	void *pdata;
	v3f_list * vertexArray;
	ply_get_argument_user_data(argument, &pdata, &value_index);
	vertexArray = static_cast <v3f_list  *>(pdata);
	float value = ply_get_argument_value(argument);
	switch (value_index) {
	case 0:
		vertexArray->push_back(Eigen::Vector3f(value, 0.0, 0.0));
		break;
	case 1:
		vertexArray->at(vertexArray->size() - 1)[1] = value;
		break;
	case 2:
		vertexArray->at(vertexArray->size() - 1)[2] = value;
		break;
	default:
		break;
	}
	return 1;
}


int FileLoader::face_cb(p_ply_argument argument) {
	long length, value_index;
	void *pdata;
	v3i_list* faceArray;
	ply_get_argument_user_data(argument, &pdata, NULL);
	faceArray = static_cast <v3i_list *>(pdata);
	ply_get_argument_property(argument, NULL, &length, &value_index);
	int value = ply_get_argument_value(argument);
	switch (value_index) {
	case 0:
		faceArray->push_back(std::array<unsigned int, 3>{{ value, 0, 0 }});
		break;
	case 1:
		faceArray->at(faceArray->size() - 1)[1] = value;
		break;
	case 2:
		faceArray->at(faceArray->size() - 1)[2] = value;
		break;
	default:
		break;
	}
	return 1;
}

bool FileLoader::load_ply(std::string const& filename, v3f_list& vertices, v3i_list& faces, v3f_list& normals)
{
	std::ostringstream fqfn;
	fqfn << path_resources;
	fqfn << filename;
	p_ply ply = ply_open(fqfn.str().c_str(), NULL, 0, NULL);
	if (!ply){
		return false;
	}
	if (!ply_read_header(ply)){
		return false;
	}
	long nvertices, ntriangles;
	nvertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb, (void*)&vertices, 0);
	ply_set_read_cb(ply, "vertex", "y", vertex_cb, (void*)&vertices, 1);
	ply_set_read_cb(ply, "vertex", "z", vertex_cb, (void*)&vertices, 2);
	ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, (void*)&faces, 0);
	printf("%ld\n%ld\n", nvertices, ntriangles);
	if (!ply_read(ply)) return 1;
	ply_close(ply);
	Utility::calculate_vertex_normals_from_triangle_mesh(vertices, faces, normals);
	return 0;
}
