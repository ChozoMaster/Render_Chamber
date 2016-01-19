#include "AntTweakBar.h"
#include "parameters.h"
#include <memory>
#include "MyViz.h"
#include "FileLoader.h"
#include "Particle.h"
#include "Spring.h"
#include "Rigidbody.h"
#include "Integrator.h"
#include "Cuboid.h"

std::unique_ptr<MyViz> viz;
std::vector<Particle*> particles;
std::vector<Spring*> springs;
std::vector<RigidBody*> rigidbodies;
Eigen::Matrix3f inertiaTensor = Eigen::Matrix3f::Zero();
Eigen::Vector3f contactPoint(0.0, 0.0, 0.0);
int pipelineStag = 0;
int frameskip(0);


float g_time(0.0f);
bool g_stop_simulation(true);

std::vector<Eigen::Vector3f> currentVertexArray_reference;
std::vector<Eigen::Vector3f> currentNormalsArray_reference;
std::vector<Eigen::Vector3f> currentVertexArray;
std::vector<Eigen::Vector3f> currentNormalsArray;
std::vector<std::array<unsigned int, 3> > currentFaceArray;

void TW_CALL reset_simulation(void*)
{
	g_time = 0.0f;
	Parameter::getInstance()->time = 0.0f;
	particles.clear();
	rigidbodies.clear();
	springs.clear();
	Particle* p1 = new Particle(0.0, Eigen::Vector3f(0.0, 1.0, 0.0));
	Particle* p2 = new Particle(1.0, Eigen::Vector3f(0.0, 1.0, 0.0));
	//Cuboid* r1 = new Cuboid(inertiaTensor, 1.0, Eigen::Vector3f(0.0, 1.0, 0.0));
	Cuboid* r1 = new Cuboid(Eigen::Vector3f(0.5, 0.5, 0.5), 1.0, Eigen::Vector3f(0.0, 1.0, 0.0));
	Spring* s1 = new Spring(r1, p1, contactPoint, 100.0, 10.0, 0.5);
	particles.push_back(p1);
	//particles.push_back(p2);
	rigidbodies.push_back(r1);
	springs.push_back(s1);
}

static void TW_CALL setSmoothCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->shading_method = *(int *)value;
	viz->updateAmbientOcclusionDefines();
	viz->updateMeshRenderingDefines();

}

static void TW_CALL getSmoothCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->shading_method;

}

static void TW_CALL setTextureParameterCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->textureSetup = *(int *)value;

}

static void TW_CALL getTextureParameterCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->textureSetup;
	viz->updateVoxelDefines();
}


static void TW_CALL setOctreeDepthCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->OctreeLevel = *(int *)value;
	viz->compute_Octree(static_cast<GLsizei>(Parameter::getInstance()->vertexCount), 1);

}

static void TW_CALL getOctreeDepthCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->OctreeLevel;
}

static void TW_CALL setVoxelTypeCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->voxelizationType = *(int *)value;

	switch (Parameter::getInstance()->voxelizationType)
	{
	case 0:
		viz->voxelization_shader_program = viz->program_fragmentParallel_voxelization;
		break;
	case 1:
		viz->voxelization_shader_program = viz->program_TriangleParallel_voxelization;
		break;
	default:
		break;
	}

}

static void TW_CALL setVolumeShadingCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->useVolumeRenderingShading = *(bool *)value;
	viz->updateVolumeRenderingDefines();
}

static void TW_CALL getVolumeShadingCallback(void * value, void * clientData)
{
	*(bool *)value = Parameter::getInstance()->useVolumeRenderingShading;

}

static void TW_CALL setVolumeSampleCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->volumeRenderingSampleCount = *(int *)value;
	viz->updateVolumeRenderingDefines();
}

static void TW_CALL getVolumeSampleCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->volumeRenderingSampleCount;

}

static void TW_CALL getVoxelTypeCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->voxelizationType;

}


static void TW_CALL setConservativeCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->use_conservative = *(bool *)value;
	viz->updateVoxelDefines();
}

static void TW_CALL getConservativeCallback(void * value, void * clientData)
{
	*(bool *)value = Parameter::getInstance()->use_conservative;

}

static void TW_CALL setShadowCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->draw_shadows = *(bool *)value;
	viz->updateAmbientOcclusionDefines();
}

static void TW_CALL getShadowCallback(void * value, void * clientData)
{
	*(bool *)value = Parameter::getInstance()->draw_shadows;

}

static void TW_CALL setOcclusionCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->draw_occlusion = *(bool *)value;
	viz->updateAmbientOcclusionDefines();
}

static void TW_CALL getOcclusionCallback(void * value, void * clientData)
{
	*(bool *)value = Parameter::getInstance()->draw_occlusion;

}

static void TW_CALL setAllConesCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->use_all_cones = *(bool *)value;
	viz->updateAmbientOcclusionDefines();
}

static void TW_CALL getAllConesCallback(void * value, void * clientData)
{
	*(bool *)value = Parameter::getInstance()->use_all_cones;

}

static void TW_CALL setConeNumberCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->coneNumber = *(int *)value;
	viz->updateAmbientOcclusionDefines();
}

static void TW_CALL getConeNumberCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->coneNumber;

}

static void TW_CALL setDynamicCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->use_dynamicTransform = *(bool *)value;
	viz->updateAmbientOcclusionDefines();
	viz->updateVolumeRenderingDefines();
	viz->updateVoxelDefines();
	viz->updateMeshRenderingDefines();
}

static void TW_CALL getDynamicCallback(void * value, void * clientData)
{
	*(bool *)value = Parameter::getInstance()->use_dynamicTransform;

}

static void TW_CALL setHalfResDSCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->halfResDS = *(bool *)value;
	viz->m_DS_gbuffer.Init(GLviz::half_screen_width(), GLviz::half_screen_height(), Parameter::getInstance()->halfResDS);
	viz->use_halfRes_DS = Parameter::getInstance()->halfResDS;

}

static void TW_CALL getHalfResDSCallback(void * value, void * clientData)
{
	*(bool *)value = Parameter::getInstance()->halfResDS;

}

static void TW_CALL setMinFilterCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->texture_minFilter = *(int *)value;
	switch (Parameter::getInstance()->texture_minFilter)
	{
	case 0:
		Parameter::getInstance()->texparams.minFilter = GL_LINEAR;
		break;
	case 1:
		Parameter::getInstance()->texparams.minFilter = GL_NEAREST;
		break;
	case 2:
		Parameter::getInstance()->texparams.minFilter = GL_LINEAR_MIPMAP_LINEAR;
		break;
	}

}

static void TW_CALL getMinFilterCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->texture_minFilter;
}

static void TW_CALL setMagFilterCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->texture_magFilter = *(int *)value;
	switch (Parameter::getInstance()->texture_magFilter)
	{
	case 0:
		Parameter::getInstance()->texparams.magFilter = GL_LINEAR;
		break;
	case 1:
		Parameter::getInstance()->texparams.magFilter = GL_NEAREST;
		break;
	}

}

static void TW_CALL getMagFilterCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->texture_magFilter;
}

static void TW_CALL setTextureWrapCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->texture_wrap = *(int *)value;
	switch (Parameter::getInstance()->texture_wrap)
	{
	case 0:
		Parameter::getInstance()->texparams.texturewrap = GL_CLAMP;
		break;
	case 1:
		Parameter::getInstance()->texparams.texturewrap = GL_REPEAT;
		break;
	}

}

static void TW_CALL getTextureWrapCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->texture_wrap;
}


void TW_CALL getMeshCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->testMeshNumber;
}

void TW_CALL setMeshCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->testMeshNumber = *(int *)value;

	
	

	switch (Parameter::getInstance()->testMeshNumber)
	{
	case 0:
		currentVertexArray = viz->m_TestMesh1_vertices;
		currentNormalsArray = viz->m_TestMesh1_normals;
		currentFaceArray = viz->m_TestMesh1_faces;
		break;
	case 1:
		currentVertexArray = viz->m_TestMesh2_vertices;
		currentNormalsArray = viz->m_TestMesh2_normals;
		currentFaceArray = viz->m_TestMesh2_faces;
		break;
	case 2:
		currentVertexArray = viz->m_TestMesh3_vertices;
		currentNormalsArray = viz->m_TestMesh3_normals;
		currentFaceArray = viz->m_TestMesh3_faces;
		break;
	case 3:{
		if (viz->m_dragon_vertices.size() == 0){
			FileLoader::load_triangle_mesh_raw("stanford_dragon_v40k_f80k.raw", viz->m_dragon_vertices, viz->m_dragon_faces, viz->m_dragon_normals);
			//Utility::add_Room();
		}
		currentVertexArray = viz->m_dragon_vertices;
		currentNormalsArray = viz->m_dragon_normals;
		currentFaceArray = viz->m_dragon_faces;

		Eigen::Vector3f massCenter = FileLoader::calculateMassCenter(currentVertexArray);
		inertiaTensor = FileLoader::calculateInertiaTensor(currentVertexArray, massCenter, 1.0);
		contactPoint = currentVertexArray.at(10000);
		break;
		}
	case 4:
		if (viz->m_bunny_vertices.size() == 0){
			FileLoader::load_ply("bun_zipper.ply", viz->m_bunny_vertices, viz->m_bunny_faces, viz->m_bunny_normals);
		}
		currentVertexArray = viz->m_bunny_vertices;
		currentNormalsArray = viz->m_bunny_normals;
		currentFaceArray = viz->m_bunny_faces;
		break;
	case 5:
		if (viz->m_xyzDragon_vertices.size() == 0){
			FileLoader::load_ply("xyzrgb_dragon_.ply", viz->m_xyzDragon_vertices, viz->m_xyzDragon_faces, viz->m_xyzDragon_normals);
		}
		currentVertexArray = viz->m_xyzDragon_vertices;
		currentNormalsArray = viz->m_xyzDragon_normals;
		currentFaceArray = viz->m_xyzDragon_faces;
		break;

	case 6:
		if (viz->m_HappyStatue_vertices.size() == 0){
			FileLoader::load_ply("happy_vrip_.ply", viz->m_HappyStatue_vertices, viz->m_HappyStatue_faces, viz->m_HappyStatue_normals);
		}
		currentVertexArray = viz->m_HappyStatue_vertices;
		currentNormalsArray = viz->m_HappyStatue_normals;
		currentFaceArray = viz->m_HappyStatue_faces;
		break;

	case 7:
		if (viz->m_ThaiStatue_vertices.size() == 0){
			//cornellBox_xyzrgbDragon
			//cornellBox_HappyBuddah
			//cornell_xyzblend_buddah
		//	FileLoader::load_ply("cornell_xyzblend_buddah.ply", viz->m_ThaiStatue_vertices, viz->m_ThaiStatue_faces, viz->m_ThaiStatue_normals);
		//	FileLoader::load_ply("cornellBox_HappyBuddah.ply", viz->m_ThaiStatue_vertices, viz->m_ThaiStatue_faces, viz->m_ThaiStatue_normals); 
			
			FileLoader::load_ply("CombinedScene.ply", viz->m_ThaiStatue_vertices, viz->m_ThaiStatue_faces, viz->m_ThaiStatue_normals);
			//FileLoader::load_ply("Sponza__extra_small_stanford.ply", viz->m_ThaiStatue_vertices, viz->m_ThaiStatue_faces, viz->m_ThaiStatue_normals);
		//	FileLoader::load_ply("xyzrgb_statuette.ply", viz->m_ThaiStatue_vertices, viz->m_ThaiStatue_faces, viz->m_ThaiStatue_normals);
		}
		currentVertexArray = viz->m_ThaiStatue_vertices;
		currentNormalsArray = viz->m_ThaiStatue_normals;
		currentFaceArray = viz->m_ThaiStatue_faces;
		break;
	
	case 8:
		if (viz->m_cornellBox_vertices.size() == 0){
			FileLoader::load_ply("cornellBox.ply", viz->m_cornellBox_vertices, viz->m_cornellBox_faces, viz->m_cornellBox_normals);
		}
		currentVertexArray = viz->m_cornellBox_vertices;
		currentNormalsArray = viz->m_cornellBox_normals;
		currentFaceArray = viz->m_cornellBox_faces;
		break;
	}
	//if (Parameter::getInstance()->use_floor){
		Utility::add_Room(currentVertexArray, currentFaceArray, currentNormalsArray);
//	}
	currentNormalsArray_reference = currentNormalsArray;
	currentVertexArray_reference = currentVertexArray;
	Utility::bindVertexArrayToBuffer(viz->vertex_array_buffer, currentVertexArray);
	Utility::bindVertexArrayToBuffer(viz->normal_array_buffer, currentNormalsArray);
	Utility::bindIndexArrayToBuffer(viz->index_array_buffer, currentFaceArray);

	Parameter::getInstance()->currentNumberOfVertices = currentVertexArray.size();
	Parameter::getInstance()->currentNumberOfFaces = currentFaceArray.size();
	Parameter::getInstance()->vertexCount = currentFaceArray.size() * 3;
	viz->compute_Octree(static_cast<GLsizei>(Parameter::getInstance()->vertexCount), 1);
}

static void TW_CALL setFloorCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->use_floor = *(bool *)value;
	setMeshCallback(&Parameter::getInstance()->testMeshNumber, NULL);

}

static void TW_CALL getFloorCallback(void * value, void * clientData)
{
	*(bool *)value = Parameter::getInstance()->use_floor;

}

void timerFunc(int delta_t_msec)
{
	float delta_t_sec = static_cast<float>(delta_t_msec) / 1000.0f;
			if (frameskip == 5){
				
				frameskip = 0;
				if (!g_stop_simulation)
				{
					
					g_time += delta_t_sec;

					const float k = 40.0f;
					const float a = 0.3f;
					const float v = 5.0f;

					#pragma omp parallel for
					for (int i = 0; i < currentVertexArray.size(); ++i)
					{
						const float x = currentVertexArray_reference[i].x() + currentVertexArray_reference[i].y()
							+ currentVertexArray_reference[i].z();

						const float u = 5.0f * (x - 0.75f * std::sin(2.5f * g_time));
						const float w = (a / 2.0f) * (1.0f
							+ std::sin(k * x + v * g_time));

						currentVertexArray[i] = currentVertexArray_reference[i] + (std::exp(-u * u) * w)
							* currentVertexArray_reference[i];
					}

					Utility::set_vertex_normals_from_triangle_mesh(currentVertexArray, currentFaceArray, currentNormalsArray);
					return;
				}
			}
			frameskip++;
			
}



	//float delta_t_sec = static_cast<float>(delta_t_msec) / 1000.0f;

	//if (!Parameter::getInstance()->stop_simulation)
	//{
	//	Parameter::getInstance()->time += delta_t_sec;

	//	AngleAxisf rotation(0.015f, Vector3f::UnitY());

	//	for (unsigned int i(0); i < viz->m_dragon_vertices.size(); ++i)
	//	{
	//		viz->m_dragon_vertices[i] = rotation * viz->m_dragon_vertices[i];
	//	}
	//}

void mykeyboardFunc(unsigned char glutKey, int mouseX, int mouseY)
{

	switch (glutKey)
	{
	case 'w':
	{
				Parameter::getInstance()->lightAngles.x() += Parameter::getInstance()->angle_step;
				break;
	}
	case 'a':
	{
				Parameter::getInstance()->lightAngles.y() -= Parameter::getInstance()->angle_step;
				break;
	}
	case 's':
	{
				Parameter::getInstance()->lightAngles.x() -= Parameter::getInstance()->angle_step;
				break;
	}
	case 'd':
	{
				Parameter::getInstance()->lightAngles.y() += Parameter::getInstance()->angle_step;
				break;
	}
	case 'p':
	{		//	Utility::screenshot("screenshot.bmp", GLviz::screen_width(), GLviz::screen_height());
				Utility::screenshot("screenshot.bmp");
				break;
	}
	}
	Parameter::getInstance()->lightPosition = Utility::calcualteLightPosition(Parameter::getInstance()->lightSource_radius, Parameter::getInstance()->lightAngles);
}

void mousewheelFunc(int button, int dir, int x, int y)
{

	if (dir > 0)
	{
		Parameter::getInstance()->lightSource_radius -= 0.1;
	}
	else
	{
		Parameter::getInstance()->lightSource_radius += 0.1;
	}
	Parameter::getInstance()->lightPosition = Utility::calcualteLightPosition(Parameter::getInstance()->lightSource_radius, Parameter::getInstance()->lightAngles);
}

void displayFunc()
{

	//viz->frontDirectionn.initialize(Eigen::Vector2i(500, 500));
	//viz->backDirection.initialize(Eigen::Vector2i(500, 500));
	//glGenFramebuffers(1, &viz->frameBufferID);

	//Utility::bindVertexArrayToBuffer(viz->vertex_array_buffer, currentVertexArray);
	//Utility::bindVertexArrayToBuffer(viz->normal_array_buffer, currentNormalsArray);
	//Utility::bindIndexArrayToBuffer(viz->index_array_buffer, currentFaceArray);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!Parameter::getInstance()->stop_simulation){
		viz->drawSprings(springs);
		Integrator::Euler(particles, rigidbodies, springs);
	}

	
	viz->calculateFPS();

//	viz->uniform_material.set_buffer_data(mesh_material);
	int screen[2] = { GLviz::screen_width(), GLviz::screen_height() };
	viz->uniform_wireframe.set_buffer_data(false, screen);
	viz->uniform_camera.set_buffer_data(viz->camera);
	Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
	switch (Parameter::getInstance()->renderMode)
	{
	case 0:
		viz->draw_volume(static_cast<GLsizei>(Parameter::getInstance()->vertexCount), viz->m_colors, viz->m_cubeVertices, rigidbodies.at(0)->transformationMatrix());
		break;
	case 1:
		viz->draw_mesh(static_cast<GLsizei>(Parameter::getInstance()->vertexCount), rigidbodies.at(0)->transformationMatrix());
	//	viz->deferredShading_GeometryPass(static_cast<GLsizei>(Parameter::getInstance()->vertexCount));
		break;
	case 2:
		viz->compute_Octree(Parameter::getInstance()->vertexCount, pipelineStag);
		viz->ambientOcclusionDeferredShading(static_cast<GLsizei>(Parameter::getInstance()->vertexCount));
		//viz->draw_mesh(static_cast<GLsizei>(Parameter::getInstance()->vertexCount), rigidbodies.at(0)->transformationMatrix());
		break;
	case 3:

		viz->ambientOcclusion_voxel(static_cast<GLsizei>(Parameter::getInstance()->vertexCount), rigidbodies.at(0)->transformationMatrix());
		break;
	}

	if (pipelineStag == 0)
		pipelineStag = 1;
	else{
		pipelineStag = 0;
	}
}


int
main(int argc, char* argv[])
{
	GLviz::init(argc, argv);

	viz = std::unique_ptr<MyViz>(new MyViz());

	try
	{



	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
	Utility::generateVoxelVertices(viz->m_cubeVertices, viz->m_colors);
	Utility::bindVertexArrayToBuffer(viz->volumeQuad_vertex_array_buffer, viz->m_cubeVertices);
	Utility::bindVertexArrayToBuffer(viz->volumeQuad_color_array_buffer, viz->m_colors);
	Utility::generateTestMesh1(viz->m_TestMesh1_vertices, viz->m_TestMesh1_faces, viz->m_TestMesh1_normals);
	Utility::generateTestMesh2(viz->m_TestMesh2_vertices, viz->m_TestMesh2_faces, viz->m_TestMesh2_normals);
	Utility::generateTestMesh3(viz->m_TestMesh3_vertices, viz->m_TestMesh3_faces, viz->m_TestMesh3_normals);
	// Setup AntTweakBar.
	{
		TwBar* bar = GLviz::twbar();
		TwAddVarRO(bar, "FPS:", TW_TYPE_FLOAT,
			&Parameter::getInstance()->fps, " precision=3 label='FPS:' group='Simulation'");

		TwAddVarRO(bar, "time", TW_TYPE_FLOAT,
			&Parameter::getInstance()->time, " precision=3 label='t in sec' group='Simulation' ");

		TwAddVarRW(bar, "TimestepSize", TW_TYPE_FLOAT,
			&Parameter::getInstance()->timeStepSize,
			" min=0.0 max=1.0 step=0.0005 precision=6 help='Change TimestepSize' group='Simulation' ");

		TwAddButton(bar, "Reset",
			reset_simulation, NULL,
			" key=r help='Reset simulation' group='Simulation' ");

		TwAddVarRW(bar, "Stop", TW_TYPE_BOOLCPP,
			&g_stop_simulation,
			" key=SPACE help='Stop simulation' group='Simulation' ");

		TwAddVarRO(bar, "vertices", TW_TYPE_INT32,
			&Parameter::getInstance()->currentNumberOfVertices, " precision=3 label='Number of Mesh Vertices' group='Triangle Mesh' ");

		TwAddVarRO(bar, "faces", TW_TYPE_INT32,
			&Parameter::getInstance()->currentNumberOfFaces, " precision=3 label='Number of Mesh Faces' group='Triangle Mesh' ");

		TwType testMeshNumber_type = TwDefineEnumFromString("testMeshNumber_type", "Triangle 1,Triangle 2,Triangle 3,Stanford_Dragon, Stanford_bunny, xyz_dragon, Happy Buddah, Thai Statue, Cornell Box");
		TwAddVarCB(bar, "TestMesh", testMeshNumber_type, setMeshCallback, getMeshCallback, 0, " group='Triangle Mesh' ");

		TwAddVarCB(bar, "Add Floor", TW_TYPE_BOOLCPP,
			setFloorCallback, getFloorCallback, 0, "group='Triangle Mesh' ");

		//TwAddVarCB(bar, positionName.c_str(), TW_TYPE_DIR3D, Emitter::setPositionCallback, Emitter::getPositionCallback, e, (groupName + " label='Position' ").c_str());
		TwType shading_type = TwDefineEnumFromString("shading_type", "Flat,Phong");
		TwAddVarCB(bar, "Shading", shading_type, setSmoothCallback, getSmoothCallback, 0, " key=5 group='Triangle Mesh' ");

		TwAddVarCB(bar, "Use dynamic transform", TW_TYPE_BOOLCPP,
		setDynamicCallback, getDynamicCallback, 0, "help='Use dynamic transform' group='Voxelization' ");

		TwType textureFormat = TwDefineEnumFromString("textureFormat", "R32F, R8");
		TwAddVarCB(bar, "TextureFormat", textureFormat, setTextureParameterCallback, getTextureParameterCallback, 0, "group='Voxelization' ");

		TwAddSeparator(bar, NULL, " group='Voxelization' ");

		TwType renderMode_type = TwDefineEnumFromString("Rendermode", "Volume,Mesh,Occlusion Octree, Occlusion_Voxel");
		TwAddVarRW(bar, "RenderMode", renderMode_type, &Parameter::getInstance()->renderMode, " group='Voxelization' ");

		TwType voxelization_type = TwDefineEnumFromString("Voxelization type", "Fragment parallel, Triangle parallel");
		TwAddVarCB(bar, "Voxelization type", voxelization_type, setVoxelTypeCallback, getVoxelTypeCallback, 0, "group='Voxelization' ");


		TwAddVarRW(bar, "Hybrid cutoff", TW_TYPE_FLOAT,
			&Parameter::getInstance()->hybridCutoff,
			" min=0.0 max=100.0 step=0.1 help='Hybrid cutoff' group='Voxelization' ");

		TwAddVarCB(bar, "Octree Depth", TW_TYPE_INT32,
			setOctreeDepthCallback, getOctreeDepthCallback, 0,
			" min=1 max=10 help='Maximum Level' group='Voxelization' ");

	

	

		TwAddSeparator(bar, NULL, " group='Voxelization' ");

		TwType texture_magFilter = TwDefineEnumFromString("magFilter type", "GL_LINEAR, GL_NEAREST");
		TwAddVarCB(bar, "Texture mag filter", texture_magFilter, setMagFilterCallback, getMagFilterCallback, 0, "group='Voxelization' ");

		TwType texture_minFilter = TwDefineEnumFromString("minFilter type", "GL_LINEAR, GL_NEAREST, GL_MIPMAP_LINEAR");
		TwAddVarCB(bar, "Texture min filter", texture_minFilter, setMinFilterCallback, getMinFilterCallback, 0, "group='Voxelization' ");

		TwType texture_wrap = TwDefineEnumFromString("textureWrap type", "GL_CLAMP, GL_REPEAT");
		TwAddVarCB(bar, "Texture wrap", texture_wrap, setTextureWrapCallback, getTextureWrapCallback, 0, "group='Voxelization' ");


		TwAddVarCB(bar, "Ambient Occlusion", TW_TYPE_BOOLCPP,
			setOcclusionCallback, getOcclusionCallback, 0, "help='Draw Ambient Occlusion' group='Ambient Occlusion' ");

		TwAddVarCB(bar, "Shadows", TW_TYPE_BOOLCPP,
			setShadowCallback, getShadowCallback, 0, "help='Draw Shadow' group='Ambient Occlusion' ");

		TwAddVarRW(bar, "Distance weight", TW_TYPE_FLOAT,
			&Parameter::getInstance()->distance_weight,
			" min=-1.0 max=100.0 step=1.0 help='Change Distance weight' group='Ambient Occlusion' ");

		TwAddVarCB(bar, "Use all Cones", TW_TYPE_BOOLCPP,
			setAllConesCallback, getAllConesCallback, 0, " group='Ambient Occlusion' ");
		
		TwAddVarCB(bar, "Cone Number", TW_TYPE_INT32,
			setConeNumberCallback, getConeNumberCallback, 0, "min=0 max=5 step=1 help='Which cone should be traced' group='Ambient Occlusion' ");

		TwAddVarRW(bar, "Cone Distance", TW_TYPE_FLOAT,
			&Parameter::getInstance()->coneDistance,
			" min=0.00001 max=100.0 step=0.01 help='Max Cone Tracing Distance' group='Ambient Occlusion' ");

		TwAddVarRW(bar, "Cone Angle", TW_TYPE_FLOAT,
			&Parameter::getInstance()->coneAngle,
			" min=0.0 max=1.0 step=0.01 help='Cone Aperture Angle' group='Ambient Occlusion' ");

		TwAddVarRW(bar, "Ao Intensity", TW_TYPE_FLOAT,
			&Parameter::getInstance()->aoIntensity,
			" min=0.0 max=100.0 step=0.01 help='Cone Aperture Angle' group='Ambient Occlusion' ");

		TwAddVarRW(bar, "Cone Jittering", TW_TYPE_INT32,
			&Parameter::getInstance()->jittering,
			" min=0 max=1 step=1 help='Cone Aperture Angle' group='Ambient Occlusion' ");

		TwAddVarRW(bar, "Cone Constellation", TW_TYPE_INT32,
			&Parameter::getInstance()->coneConstellation,
			" min=0 max=4 step=1 help='Cone Constellation: 0-3 Cones, 1-5 Cones, 2-9 Cones ' group='Ambient Occlusion' ");

		TwAddVarCB(bar, "Deferred Shading half Resolution", TW_TYPE_BOOLCPP,
			setHalfResDSCallback, getHalfResDSCallback, 0, "help='Use half Resolution for deferred shading' group='Voxelization' ");

		TwAddVarCB(bar, "Use Shading", TW_TYPE_BOOLCPP,
			setVolumeShadingCallback, getVolumeShadingCallback, 0, "help='Use Shader in Volume Rendering' group='Volume Rendering' ");

		TwAddVarCB(bar, "#Sample", TW_TYPE_INT32,
			setVolumeSampleCallback, getVolumeSampleCallback, 0, "min=1.0 max=5000 step=5 help='How many Samples per Ray' group='Volume Rendering' ");

		TwAddVarRW(bar, "Isovalue", TW_TYPE_FLOAT,
			&Parameter::getInstance()->iso_value,
			" min=0.0 max=1.0 step=0.05 help='Change Isovalue' group='Volume Rendering' ");
	}

	GLviz::display_callback(displayFunc);
	GLviz::keyboard_callback(mykeyboardFunc);
	GLviz::timer_callback(timerFunc, 15);
	GLviz::mouseWheel_callback(mousewheelFunc);
	viz->updateAmbientOcclusionDefines();
	viz->updateVoxelDefines();
	setMeshCallback(&Parameter::getInstance()->testMeshNumber, NULL);
	
	reset_simulation(NULL);
	return GLviz::exec(viz->camera);
}