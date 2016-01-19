#pragma once
#include <GLviz>
#include <vector>
#include <exception>
#include <iostream>
#include "parameters.h"
#include "Utility.cpp"
#include "Spring.h"
#include "GPUTimer.h"
#include "OctreeComputation.h"

extern unsigned char const voxelization_3DTexture_fs_glsl[];
extern unsigned char const ambientOcclusion_voxel_fs_glsl[];

extern unsigned char const default_fs_glsl[];
extern unsigned char const default_vs_glsl[];
extern unsigned char const default2_vs_glsl[];

extern unsigned char const volumeRendering_fs_glsl[];

extern unsigned char const ambientOcclusion_fs_glsl[];
extern unsigned char const ambientOcclusionDeferredShading_fs_glsl[];
extern unsigned char const ambientOcclusionDeferredShading_vs_glsl[];


extern unsigned char const mesh3_vs_glsl[];
extern unsigned char const mesh3_gs_glsl[];
extern unsigned char const mesh3_fs_glsl[];

extern unsigned char const ds_geometryPass_vs_glsl[];
extern unsigned char const ds_geometryPass_gs_glsl[];
extern unsigned char const ds_geometryPass_fs_glsl[];



extern unsigned char const octreeDrawer_vs_glsl[];
extern unsigned char const octreeDrawer_gs_glsl[];
extern unsigned char const octreeDrawer_fs_glsl[];

extern unsigned char const voxelization_vs_glsl[];
extern unsigned char const voxelization_gs_glsl[];
extern unsigned char const voxelization_fs_glsl[];

extern unsigned char const VoxelizationHybrid_vs_glsl[];
extern unsigned char const VoxelizationHybrid_gs_glsl[];
extern unsigned char const VoxelizationHybrid_fs_glsl[];

extern unsigned char const voxelization_TriangleParallel_gs_glsl[];

class MyViz
{


public:
	GLviz::Camera camera;
	OctreeComputation octreeComp;
	GLviz::glVertexArray	vertex_array_v, vertex_array_vf, vertex_array_vnf, vertex_array_color, vertex_array_voxel, vertex_array_Octree, vertex_array_DummyTriangle_1, vertex_array_DummyTriangle_2, vertex_array_DummyTriangle_3, quad_vertex_array, quad_color_array;
	GLviz::glArrayBuffer	vertex_array_buffer, normal_array_buffer, color_array_buffer, volumeQuad_vertex_array_buffer, volumeQuad_color_array_buffer;
	GLviz::GBuffer			m_DS_gbuffer;
	GLviz::glElementArrayBuffer  index_array_buffer;
	GLviz::glAtomicCounter atomicCounterBuffer, flaggedNodeCounter;
	GLviz::glImageBufferUniform fragmentListBuffer, nodePoolBuffer, nodePoolBuffer1, nodePoolBuffer2, nodePoolNeighbors, nodePool_attribute, brickpool_occlusion;
	GLviz::glImage3DUniform voxel_color, brickImage, brickImage1, brickImage2, voxelImage;
	GLviz::glImage2DUniform frontDirectionn, backDirection;
	GLviz::glFramebufferObject frambufferobject;
	GLuint frameBufferID;

	GLviz::UniformBufferCamera      uniform_camera;
	GLviz::UniformBufferMaterial    uniform_material;
	GLviz::UniformBufferWireframe   uniform_wireframe;
	GLviz::UniformBufferSphere      uniform_sphere;
	GLviz::UniformVoxelConfiguration   uniform_voxel;

	GLviz::ProgramDrawShader  program_OctreeDrawer, program_voxelDrawer, program_voxelization, program_fragmentParallel_voxelization, program_occusionDrawer, program_occusionDeferredShading,
		program_occusionDrawer_3DTexture, program_CubeDraw, program_volumeRendering, program_meshDraw, program_TriangleParallel_voxelization, voxelization_shader_program, 
		program_hybrid_voxelization, program_DS_geometry;

	std::vector<Eigen::Vector3f>               m_dragon_vertices, m_bunny_vertices, m_xyzDragon_vertices, m_HappyStatue_vertices, m_ThaiStatue_vertices, m_cornellBox_vertices;
	std::vector<Eigen::Vector3f>               m_dragon_normals, m_bunny_normals, m_xyzDragon_normals, m_HappyStatue_normals, m_ThaiStatue_normals, m_cornellBox_normals;
	std::vector<Eigen::Vector3f>               m_colors;
	std::vector<std::array<unsigned int, 3> >  m_dragon_faces, m_bunny_faces, m_xyzDragon_faces, m_HappyStatue_faces, m_ThaiStatue_faces, m_cornellBox_faces;

	std::vector<Eigen::Vector3f>               m_voxel_vertices, m_TestMesh1_vertices, m_TestMesh2_vertices, m_TestMesh3_vertices, m_cubeVertices;
	std::vector<Eigen::Vector3f>               m_TestMesh1_normals, m_TestMesh2_normals, m_TestMesh3_normals;
	std::vector<std::array<unsigned int, 3> >  m_TestMesh1_faces, m_TestMesh2_faces, m_TestMesh3_faces;
	GPUTimer timer;
	int brickSize, brickDimension, brickSize2, brickDimension2, numVoxelFrags;
	bool use_halfRes_DS;
	int nodePoolLength;
	int pipelineStage = 0;
	bool bufferStage = true;
	//GLuint fbo, render_buf;

	//FUNCTIONS
	MyViz();
	void drawFPS();
	void calculateFPS();
	//void draw_volume(GLsizei nf, std::vector<Eigen::Vector3f> m_colors, std::vector<Eigen::Vector3f> m_cubeVertices);
	void draw_volume(GLsizei nf, std::vector<Eigen::Vector3f> m_colors, std::vector<Eigen::Vector3f> m_cubeVertices, Eigen::Matrix4f transformMatrix);
	void setup_BackTexture();
	void draw_mesh(GLsizei nf, Eigen::Matrix4f transformMatrix = Eigen::Matrix4f::Zero());
	void deferredShading_GeometryPass(GLsizei nf);
	void deferredShading_LightPass();
	void draw_octree();
	void drawLightSource();
	//void ambientOcclusion_voxel(GLsizei nf);
	void ambientOcclusion_voxel(GLsizei nf, Eigen::Matrix4f transformMatrix);
	void ambientOcclusion_Octree(int nf, int brickSize, int brickDimension);
	

	void draw(GLsizei nf);
	void compute_Octree(GLsizei nf, int stage);

	unsigned int voxelize_fragmentList(int voxelResolution, GLsizei nf);

	void updateVoxelDefines();
	void updateAmbientOcclusionDefines();
	void updateVolumeRenderingDefines();
	void updateMeshRenderingDefines();
	void drawSprings(std::vector<Spring*> springs);
	void voxelize_3Dtexture(Eigen::Matrix4f transformMatrix, GLsizei nf);
	void ambientOcclusionDeferredShading(int nf);

};
