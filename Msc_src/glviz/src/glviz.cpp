// This file is part of GLviz.
//
// Copyright (C) 2014 by Sebastian Lipponer.
// 
// GLviz is free software: you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// GLviz is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GLviz. If not, see <http://www.gnu.org/licenses/>.

#include "glviz.hpp"

#include "camera.hpp"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <AntTweakBar.h>

#include <iostream>
#include <cstdlib>
#include <functional>

namespace GLviz
{

namespace
{

int m_opengl_major_version(4), m_opengl_minor_version(3);
//int m_screen_width(1280), m_screen_height(720);
int m_screen_width(1920), m_screen_height(1080);
int m_half_screen_width(m_screen_width / 2), m_half_screen_height(m_screen_height / 2);
bool m_lmb_down(false), m_mmb_down(false), m_rmb_down(false);

std::function<void(unsigned char glutKey, int mouseX, int mouseY)>    m_keyboard_callback;
std::function<void(int button, int dir, int x, int y)>    m_mouseWheel_callback;
std::function<void()>    m_display_callback;
std::function<void (int)> m_timer_callback;

TwBar*   m_bar;
Camera*  m_camera;
void TW_CALL BreakOnError(const char *errorMessage)
{
	assert(errorMessage == NULL); // show an assert message if an error occurred
}

void
displayFunc()
{
	m_display_callback();
	GLenum er = glGetError();
	if (er != GL_NO_ERROR){
		std::cout << "OPENGL ERROR \n";
	}
   
	const char* TwEr = TwGetLastError();
	if (TwEr != 0)
		std::cout << TwEr;

	TwDraw();
    glutSwapBuffers();
    glutPostRedisplay();
}

void
reshapeFunc(int width, int height)
{
    m_screen_width  = width;
    m_screen_height = height;
	m_half_screen_width = width / 2;
	m_half_screen_height = height / 2;

    const float aspect = static_cast<float>(m_screen_width) /
        static_cast<float>(m_screen_height);

//	glViewport(0, 0, 1000, 1000);
	glViewport(0, 0, m_screen_width, m_screen_height);
//	glViewport(0, 0, 500, 500);
    //m_camera->set_perspective(60.0f, aspect, -1.0f, 1.0f);
	m_camera->set_perspective(60.0f, aspect, 0.005f, 15.0f);
    TwWindowSize(m_screen_width, m_screen_height);

}


void
mouseFunc(int button, int state, int x, int y)
{
	if (!TwEventMouseButtonGLUT(button, state, x, y))
	{
		float xf = static_cast<float>(x) / static_cast<float>(m_screen_width);
		float yf = static_cast<float>(y) / static_cast<float>(m_screen_height);

		if (button == GLUT_LEFT_BUTTON)
		{
			m_lmb_down = (state == GLUT_DOWN) ? true : false;
			m_camera->trackball_begin_motion(xf, yf);
		}

		if (button == GLUT_RIGHT_BUTTON)
		{
			m_rmb_down = (state == GLUT_DOWN) ? true : false;
			m_camera->trackball_begin_motion(xf, yf);
		}

		if (button == GLUT_MIDDLE_BUTTON)
		{
			m_mmb_down = (state == GLUT_DOWN) ? true : false;
			m_camera->trackball_begin_motion(xf, yf);
		}
	}
}

void
motionFunc(int x, int y)
{
	if (!TwEventMouseMotionGLUT(x, y))
	{
		float xf = static_cast<float>(x) / static_cast<float>(m_screen_width);
		float yf = static_cast<float>(y) / static_cast<float>(m_screen_height);

		if (m_lmb_down)
		{
			m_camera->trackball_end_motion_rotate(xf, yf);
		}
		else if (m_rmb_down)
		{
			m_camera->trackball_end_motion_zoom(xf, yf);
		}
		else if (m_mmb_down)
		{
			m_camera->trackball_end_motion_translate(xf, yf);
		}
	}
}

void
timerFunc(int delta_t_msec)
{
	m_timer_callback(delta_t_msec);
    glutTimerFunc(delta_t_msec, timerFunc, delta_t_msec);
}

void
keyboardFunc(unsigned char glutKey, int mouseX, int mouseY)
{
	m_keyboard_callback(glutKey, mouseX, mouseY);
	TwEventKeyboardGLUT(glutKey, mouseX, mouseY);
}

void mouseWheel(int button, int dir, int x, int y)
{
	m_mouseWheel_callback(button, dir, x, y);
}
void
terminateFunc()
{
    TwTerminate();
}

}

int
screen_width()
{
    return m_screen_width;
}

int
screen_height()
{
    return m_screen_height;
}

int
half_screen_width()
{
	return m_half_screen_width;
}

int
half_screen_height()
{
	return m_half_screen_height;
}


Camera*
camera()
{
    return m_camera;
}

void
set_camera(Camera& camera)
{
    m_camera = &camera;
}

TwBar*
twbar()
{
    return m_bar;
}

void
display_callback(void (*display_callback)())
{
	m_display_callback = display_callback;
}

void keyboard_callback(void(*keyboard_callback)(unsigned char glutKey, int mouseX, int mouseY))
{
	m_keyboard_callback = keyboard_callback;
}

void
timer_callback(void (*timer_callback)(int), int delta_t_msec)
{
	m_timer_callback = timer_callback;
	glutTimerFunc(delta_t_msec, timerFunc, delta_t_msec);
}
void mouseWheel_callback(void(*mouseWheel_callback)(int button, int dir, int x, int y))
{
	m_mouseWheel_callback = mouseWheel_callback;
}

void
cout_opengl_version()
{
    GLint context_major_version, context_minor_version, context_profile;

    glGetIntegerv(GL_MAJOR_VERSION, &context_major_version);
    glGetIntegerv(GL_MINOR_VERSION, &context_minor_version);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &context_profile);

    std::cout << "  OpenGL version " << context_major_version << "."
        << context_minor_version << " ";

    switch (context_profile)
    {
    case GL_CONTEXT_CORE_PROFILE_BIT:
        std::cout << "core";
        break;

    case GL_CONTEXT_COMPATIBILITY_PROFILE_BIT:
        std::cout << "compatibility";
        break;
    }

    std::cout << " profile context." << std::endl;
}

void
cout_glew_version()
{
    std::cout << "  GLEW version " << glewGetString(GLEW_VERSION)
        << "." << std::endl;
}

void
init(int argc, char* argv[])
{
    // Initialize GLUT.
    glutInit(&argc, argv);
	glutInitContextVersion(4, 4);
	
   // glutInitContextVersion(m_opengl_major_version, m_opengl_minor_version);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
//	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(m_screen_width, m_screen_height);
	//glutInitWindowSize(1000, 1000);
    glutCreateWindow("GLviz");
    // Print OpenGL version.
    cout_opengl_version();


    // Initialize GLEW.
    {
        glewExperimental = GL_TRUE;
        GLenum error = glewInit();

        if (GLEW_OK != error)
        {
            std::cerr << "Failed to initialize GLEW:" << std::endl;
            std::cerr << "Error: " << glewGetErrorString(error) << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    // Print GLEW version.
    cout_glew_version();
	glClearColor(0.0, 0.0, 0.0, 0.0f);
    // Initialize AntTweakBar.
    {
        int tw_init_ok = TwInit(TW_OPENGL, NULL);

        if (!tw_init_ok)
        {
            std::cerr << TwGetLastError() << std::endl;
            exit(EXIT_FAILURE);
        }

        m_bar = TwNewBar("TweakBar");
        TwWindowSize(m_screen_width, m_screen_height);

        
		(" GLOBAL help='some useful text' ");
        TwDefine(" TweakBar size='275 400' color='96 200 224' text=dark valueswidth=100 refresh=0.01 alpha=255");
    }
	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
    atexit(terminateFunc);
	
    // Initialize GLUT callbacks.
    glutDisplayFunc(displayFunc);
    glutReshapeFunc(reshapeFunc);

    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
    glutPassiveMotionFunc(motionFunc);
	glutMouseWheelFunc(mouseWheel);

	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);

    TwGLUTModifiersFunc(glutGetModifiers);
}

int
exec(Camera& camera)
{
    m_camera = &camera;

    glutMainLoop();
    return EXIT_SUCCESS;
}

void resetViewPort()
{
	glViewport(0, 0, m_screen_width, m_screen_height);
}



}
