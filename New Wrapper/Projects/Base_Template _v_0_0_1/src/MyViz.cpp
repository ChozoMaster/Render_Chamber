#include "MyViz.h"
#include "GL\freeglut_std.h"

MyViz::MyViz():
program_meshDraw(mesh3_vs_glsl, mesh3_gs_glsl, mesh3_fs_glsl)
	{
		timer.setMaxTicks(15);
		try
		{
			organizeBlockBindings();
		}
		catch (uniform_not_found_error const& e)
		{
			std::cerr << "Warning: Failed to set a uniform variable." << std::endl
				<< e.what() << std::endl;
		}
		frambufferobject.Init(GLviz::screen_width(), GLviz::screen_height());
		glGenFramebuffers(1, &frameBufferID);

		//atomicCounterBuffer.initialize();
	
		// Setup vertex array v.
		vertex_array_v.bind();

		vertex_array_buffer.bind();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

		color_array_buffer.bind();

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE,
			3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

		vertex_array_v.unbind();
		vertex_array_buffer.unbind();

		// Setup vertex array vf.
		vertex_array_vf.bind();

		vertex_array_buffer.bind();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

		index_array_buffer.bind();
		vertex_array_buffer.unbind();

		vertex_array_vf.unbind();

		// Setup vertex array vnf.
		vertex_array_vnf.bind();

		vertex_array_buffer.bind();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

		vertexNormal_array_buffer.bind();
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

		index_array_buffer.bind();
		vertexNormal_array_buffer.unbind();
		vertex_array_buffer.unbind();


		vertex_array_vnf.unbind();

		vertex_array_color.bind();
		color_array_buffer.bind();

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE,
			3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));
		vertex_array_color.unbind();
		color_array_buffer.unbind();

		// Bind uniforms to their binding points.
		uniform_camera.bind_buffer_base(0);
		uniform_material.bind_buffer_base(1);
		uniform_wireframe.bind_buffer_base(2);
		uniform_sphere.bind_buffer_base(3);

		camera.translate(Eigen::Vector3f(0.0f, 0.0f, -4.0f));
	}

	void MyViz::organizeBlockBindings(){
		program_meshDraw.set_uniform_block_binding("Camera", 0);
		program_meshDraw.set_uniform_block_binding("Material", 1);
		program_meshDraw.set_uniform_block_binding("Wireframe", 2);
	}
	//-------------------------------------------------------------------------
	//  Draw FPS
	//-------------------------------------------------------------------------
	void MyViz::drawFPS()
	{
		//  Load the identity matrix so that FPS string being drawn
		//  won't get animates
		glLoadIdentity();

		//  Print the FPS to the window
		//printw(-0.9, -0.9, 0, "FPS: %4.2f", fps);
	}

	//-------------------------------------------------------------------------
	// Calculates the frames per second
	//-------------------------------------------------------------------------
	void MyViz::calculateFPS()
	{
		//  Increase frame count
		Parameter::getInstance()->frameCount++;

		//  Get the number of milliseconds since glutInit called 
		//  (or first call to glutGet(GLUT ELAPSED TIME)).
		Parameter::getInstance()->currentTime = glutGet(GLUT_ELAPSED_TIME);

		//  Calculate time passed
		int timeInterval = Parameter::getInstance()->currentTime - Parameter::getInstance()->previousTime;

		if (timeInterval > 1000)
		{
			//  calculate the number of frames per second
			Parameter::getInstance()->fps = Parameter::getInstance()->frameCount / (timeInterval / 1000.0f);

			//  Set time
			Parameter::getInstance()->previousTime = Parameter::getInstance()->currentTime;

			//  Reset frame count
			Parameter::getInstance()->frameCount = 0;
		}
	}


	void MyViz::draw_mesh(GLsizei nf)
	{
		glDisable(GL_CULL_FACE);
	//	GLviz::resetViewPort();
		program_meshDraw.use();
		/*vertex_array_v.bind();
		glDrawArrays(GL_TRIANGLES, 0, nf);
		vertex_array_v.unbind();*/
		//program_occusionDrawer_3DTexture.setParameter(shaderAttrib::fv3, (void*)&Parameter::getInstance()->lightPosition, "lightPosition");


	/*	glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glGenRenderbuffers(1, &render_buf);
		glBindRenderbuffer(GL_RENDERBUFFER, render_buf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, 2000, 2000);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf);*/

		if (Parameter::getInstance()->shading_method == 0)
		{
				// Flat.
				vertex_array_vf.bind();
				glDrawElements(GL_TRIANGLES, nf, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
				vertex_array_vf.unbind();
		}
		else
		{
				// Smooth.
				vertex_array_vnf.bind();

				glDrawElements(GL_TRIANGLES, nf,
					GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
				vertex_array_vnf.unbind();


		}
		program_meshDraw.unuse();
	}

	void MyViz::drawLightSource()
	{
		
		if (Parameter::getInstance()->draw_shadows){

			glPushMatrix();
			glLoadMatrixf(camera.get_projection_matrix().data());
			glMultMatrixf(camera.get_modelview_matrix().data());

			glColor3f(1.0,0.0,0);
			glPointSize(20.0);

			glBegin(GL_POINTS);
			glVertex3f(Parameter::getInstance()->lightPosition.x(), Parameter::getInstance()->lightPosition.y(), Parameter::getInstance()->lightPosition.z());
			glEnd();
			glPopMatrix();
		}

	}

	
	void MyViz::updateMeshRenderingDefines()
	{
		std::set<std::string> defines;
		if (Parameter::getInstance()->shading_method)
			defines.insert("SMOOTH");
		program_meshDraw.initialize_program_obj(defines);
	}

	bool MyViz::loadDataFromCache(const std::string& name){
		v3f_list_catalog_itr vertexItr, faceNormalsItr, vertexNormalsItr;
		v3i_list_catalog_itr faceItr;
		if ((vertexItr = m_vertex_cache.find(name)) == m_vertex_cache.end())
			return false;
		if ((faceNormalsItr = m_faceNormals_cache.find(name)) == m_faceNormals_cache.end())
			return false;
		if ((vertexNormalsItr = m_vertexNormals_cache.find(name)) == m_vertexNormals_cache.end())
			return false;
		if ((faceItr = m_face_cache.find(name)) == m_face_cache.end())
			return false;

		m_currentFaces = faceItr->second;
		m_currentVertices = vertexItr->second;
		m_current_VertexNormals = vertexNormalsItr->second;
		m_current_FaceNormals = faceNormalsItr->second;
		return true;
	}

	bool MyViz::storeDataInCache(const std::string& name){
		m_vertex_cache[name] = m_currentVertices;
		m_face_cache[name] = m_currentFaces;
		m_faceNormals_cache[name] = m_current_FaceNormals;
		m_vertexNormals_cache[name] = m_current_VertexNormals;
		return true;
	}