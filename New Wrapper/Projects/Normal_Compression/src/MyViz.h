#pragma once
#include <GLviz>
#include <vector>
#include <exception>
#include <iostream>
#include "parameters.h"
#include "Utility.cpp"
#include "GPUTimer.h"

#include"GlobalDefines.h"

extern unsigned char const default_fs_glsl[];
extern unsigned char const default_vs_glsl[];
extern unsigned char const default2_vs_glsl[];


extern unsigned char const mesh3_vs_glsl[];
extern unsigned char const mesh3_gs_glsl[];
extern unsigned char const mesh3_fs_glsl[];


class MyViz
{


public:
	GLviz::Camera camera;
	GLviz::glVertexArray	vertex_array_v, vertex_array_vf, vertex_array_vnf, vertex_array_color;
	GLviz::glArrayBuffer	vertex_array_buffer, faceNormal_array_buffer, color_array_buffer, vertexNormal_array_buffer;
	GLviz::GBuffer			m_DS_gbuffer;
	GLviz::glElementArrayBuffer  index_array_buffer;
	GLviz::glAtomicCounter atomicCounterBuffer;
	GLviz::glImage2DUniform frontDirectionn, backDirection;
	GLviz::glFramebufferObject frambufferobject;
	GLuint frameBufferID;

	GLviz::UniformBufferCamera      uniform_camera;
	GLviz::UniformBufferMaterial    uniform_material;
	GLviz::UniformBufferWireframe   uniform_wireframe;
	GLviz::UniformBufferSphere      uniform_sphere;

	GLviz::ProgramDrawShader  program_meshDraw;
	std::map<std::string, v3f_list>	m_vertex_cache, m_faceNormals_cache, m_vertexNormals_cache, m_color_cache;
	std::map<std::string, v3i_list> m_face_cache;
	v3f_list				m_currentVertices, m_current_VertexNormals, m_current_FaceNormals, m_currentColors;
	v3i_list				m_currentFaces;
	GPUTimer timer;
	

	//FUNCTIONS
	MyViz();
	void					drawFPS();
	void					calculateFPS();
	void					draw_mesh(GLsizei nf);
	void					drawLightSource();
	void					organizeBlockBindings();
	//void					draw(GLsizei nf);

	void					updateMeshRenderingDefines();
	//Checks corresponding data was already cached
	bool					loadDataFromCache(const std::string&);
	//Stores current data in cache
	//TODO Check data with the same name is already in chache
	bool					storeDataInCache(const std::string&);
};
