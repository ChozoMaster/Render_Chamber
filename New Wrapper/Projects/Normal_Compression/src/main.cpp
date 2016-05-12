#include "AntTweakBar.h"
#include "parameters.h"
#include <memory>
#include "MyViz.h"
#include "FileLoader.h"

std::unique_ptr<MyViz> viz;

float g_time(0.0f);
bool g_stop_simulation(true);

float mesh_material[4] = {
	0.0f, 0.25f, 1.0f, 8.0f
};

void timerFunc(int delta_t_msec)
{

}

void compressNormals(){

}
static void TW_CALL setSmoothCallback(const void * value, void * clientData)
{
	Parameter::getInstance()->shading_method = *(int *)value;
	viz->updateMeshRenderingDefines();
	viz->organizeBlockBindings();

}

static void TW_CALL getSmoothCallback(void * value, void * clientData)
{
	*(int *)value = Parameter::getInstance()->shading_method;

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
	case 0:{
		//TODO this makes no sense because of the confusion of vertex and face normals
			if (!viz->loadDataFromCache("stanford_dragon_v40k_f80k.raw"))
				FileLoader::load_triangle_mesh_raw("stanford_dragon_v40k_f80k.raw", viz->m_currentVertices, viz->m_currentFaces, viz->m_current_VertexNormals);
		//viz->m_current_VertexNormals = Utility::calculate_vertex_normals_from_faceNormals(viz->m_currentVertices, viz->m_currentFaces, viz->m_current_FaceNormals);
		break;
			}
	}
	//TODO can be wrapped
	Utility::bindVertexArrayToBuffer(viz->vertex_array_buffer, viz->m_currentVertices);
	//Utility::bindVertexArrayToBuffer(viz->faceNormal_array_buffer, viz->m_current_FaceNormals);
	Utility::bindVertexArrayToBuffer(viz->vertexNormal_array_buffer, viz->m_current_VertexNormals);
	Utility::bindIndexArrayToBuffer(viz->index_array_buffer, viz->m_currentFaces);

	Parameter::getInstance()->currentNumberOfVertices = viz->m_current_VertexNormals.size();
	Parameter::getInstance()->currentNumberOfFaces = viz->m_currentFaces.size();
//This 
	Parameter::getInstance()->vertexCount = Parameter::getInstance()->currentNumberOfFaces * 3;
}

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
	{	
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
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viz->calculateFPS();

	viz->uniform_material.set_buffer_data(mesh_material);
	int screen[2] = { GLviz::screen_width(), GLviz::screen_height() };
	viz->uniform_wireframe.set_buffer_data(false, screen);
	viz->uniform_camera.set_buffer_data(viz->camera);
	//std::cout << viz->camera.get_modelview_matrix() << "\n";

	//viz->draw(static_cast<GLsizei>(Parameter::getInstance()->currentNumberOfVertices));
	//viz->drawFPS();
	viz->draw_mesh(static_cast<GLsizei>(Parameter::getInstance()->vertexCount));
}


int
main(int argc, char* argv[])
{
	GLviz::init(argc, argv);

	viz = std::unique_ptr<MyViz>(new MyViz());
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

		TwAddVarRO(bar, "vertices", TW_TYPE_INT32,
			&Parameter::getInstance()->currentNumberOfVertices, " precision=3 label='Number of Mesh Vertices' group='Triangle Mesh' ");

		TwAddVarRO(bar, "faces", TW_TYPE_INT32,
			&Parameter::getInstance()->currentNumberOfFaces, " precision=3 label='Number of Mesh Faces' group='Triangle Mesh' ");

		TwType testMeshNumber_type = TwDefineEnumFromString("testMeshNumber_type", "Triangle 1,Triangle 2,Triangle 3,Stanford_Dragon, Stanford_bunny, xyz_dragon, Happy Buddah, Thai Statue, Cornell Box");
		TwAddVarCB(bar, "TestMesh", testMeshNumber_type, setMeshCallback, getMeshCallback, 0, " group='Triangle Mesh' ");

		//TwAddVarCB(bar, positionName.c_str(), TW_TYPE_DIR3D, Emitter::setPositionCallback, Emitter::getPositionCallback, e, (groupName + " label='Position' ").c_str());
		TwType shading_type = TwDefineEnumFromString("shading_type", "Flat,Phong");
		TwAddVarCB(bar, "Shading", shading_type, setSmoothCallback, getSmoothCallback, 0, " key=5 group='Triangle Mesh' ");

	}

	GLviz::display_callback(displayFunc);
	GLviz::keyboard_callback(mykeyboardFunc);
	GLviz::mouseWheel_callback(mousewheelFunc);
	GLviz::timer_callback(timerFunc, 15);
	setMeshCallback(&Parameter::getInstance()->testMeshNumber, NULL);

	return GLviz::exec(viz->camera);
}