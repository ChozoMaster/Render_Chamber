#pragma once
#include <GLviz>
#define GRAVITY Vector3f(0.0, -9.81, 0.0);
	//FPS COUNTER
class Parameter{
protected:
	Parameter() {};
private:
	static Parameter* instance ;
public:
	static Parameter* getInstance();

	//Simulation time
	float time = (0.0f);
	float timeStepSize = 0.0001;
	bool stop_simulation = (true);
	const unsigned int delta_t_msec = 15;
	bool use_dynamicTransform = (false);

	//  Rendering Infos
	int frameCount = 0;
	int vertexCount = 0;
	int currentNumberOfVertices = 0;
	int currentNumberOfFaces = 0;

	//  currentTime - previousTime is the time elapsed
	//  between every call of the Idle function
	int currentTime = 0, previousTime = 0;

	//  Number of frames per second
	float fps = 0;

	//Overall Mode
	//1.Volume Rendering
	//2.Phong/Flat Shading
	//3. Octree Ambient Occlusion
	//4. 3DTexture AO
	int renderMode = (1);


	//Voxelization Parameters
	int voxelizationType = (0);
	float hybridCutoff = 1.0;
	int OctreeLevel = 8; 
	int paintEmptyCells = 0;
	int use_conservative = (0);
	
	//Mesh settings
	int testMeshNumber = (3);
	bool use_floor = false;

	//Ambient Occlusion Rendering parameters
	int draw_occlusion = (1);
	int shading_method = (0);
	int draw_shadows = (0);
	int coneNumber = (0);
	bool use_all_cones = false;
	float distance_weight = (1.0);
	float coneAngle = 1 / sqrt(3);
	int jittering = 1;
	float coneDistance = 1.0;
	float aoIntensity = 1.0;
	int coneConstellation = 2.0;
	bool halfResDS = false;

	//Volume Rendering
	float iso_value = (0.5);
	bool useVolumeRenderingShading = false;
	int volumeRenderingSampleCount = 200;

	//Mipmap debugging
	//TODO DELETE
	bool debug_mipmap = (true);
	bool debug_mipmap_faces = (true);
	bool debug_mipmap_edges = (true);
	bool debug_mipmap_corners = (true);
	bool debug_mipmap_center = (true);
	bool debug_mipmap_borderTransfer = (true);

	//Point Light settings
	float lightSource_radius = 2.0;
	Eigen::Vector2f lightAngles = Eigen::Vector2f(45.0, 45.0);
	Eigen::Vector3f lightPosition = Eigen::Vector3f(lightSource_radius * sin(lightAngles.x()) * cos(lightAngles.y()), lightSource_radius * sin(lightAngles.x()) * sin(lightAngles.y()), lightSource_radius * cos(lightAngles.x()));
	float angle_step = 0.1;

	//Texture Parameter
	struct textureParameter {
		GLint internalFormat = GL_R32F;
		GLenum format = GL_RED;
		GLenum type = GL_FLOAT;
		GLint magFilter = GL_LINEAR;
		GLint minFilter = GL_LINEAR_MIPMAP_LINEAR;
		GLint texturewrap = GL_CLAMP;
	} texparams;

	int texture_minFilter = (2);
	int texture_magFilter = (0);
	int texture_wrap = (0);
	int textureSetup = (0);
};

