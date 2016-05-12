#include "MyViz.h"
#include "GL\freeglut_std.h"

MyViz::MyViz()
:
program_OctreeDrawer(octreeDrawer_vs_glsl, octreeDrawer_gs_glsl, octreeDrawer_fs_glsl),
program_voxelization(voxelization_vs_glsl, voxelization_gs_glsl, voxelization_fs_glsl, std::set<std::string>{ "FORMAT_R32F" }),
program_meshDraw(mesh3_vs_glsl, mesh3_gs_glsl, mesh3_fs_glsl),
program_DS_geometry(ds_geometryPass_vs_glsl, ds_geometryPass_gs_glsl, ds_geometryPass_fs_glsl),
program_occusionDrawer(mesh3_vs_glsl, mesh3_gs_glsl, ambientOcclusion_fs_glsl),
program_occusionDeferredShading(ambientOcclusionDeferredShading_vs_glsl, NULL, ambientOcclusionDeferredShading_fs_glsl),
//program_occusionDrawer(default_vs_glsl, NULL, ambientOcclusion_fs_glsl),
program_CubeDraw(default_vs_glsl, NULL, default_fs_glsl),
program_volumeRendering(default_vs_glsl, NULL, volumeRendering_fs_glsl, std::set<std::string>{ "SAMPLECOUNT 200", "ISOVALUE 0.5"}),
program_fragmentParallel_voxelization(voxelization_vs_glsl, voxelization_gs_glsl, voxelization_3DTexture_fs_glsl, std::set<std::string>{ "FORMAT_R32F" }),
program_occusionDrawer_3DTexture(mesh3_vs_glsl, mesh3_gs_glsl, ambientOcclusion_voxel_fs_glsl),
//program_hybrid_voxelization(VoxelizationHybrid_vs_glsl, VoxelizationHybrid_gs_glsl, VoxelizationHybrid_fs_glsl, std::set<std::string>{ "USEVOXELFRAGMENTLIST"}),
program_hybrid_voxelization(VoxelizationHybrid_vs_glsl, VoxelizationHybrid_gs_glsl, VoxelizationHybrid_fs_glsl, std::set<std::string>{ "FORMAT_R32F" }),
program_TriangleParallel_voxelization(voxelization_vs_glsl, voxelization_TriangleParallel_gs_glsl, default_fs_glsl, std::set<std::string>{ "FORMAT_R32F" }),
use_halfRes_DS(false)
	{
		//voxelization_shader_program = program_fragmentParallel_voxelization;
		voxelization_shader_program = program_hybrid_voxelization;
		//program_meshDrawer = GLviz::ProgramDrawShader(mesh3_vs_glsl, mesh3_gs_glsl, mesh3_fs_glsl);
		//program_meshDrawer.use();
		timer.setMaxTicks(15);
		try
		{
			program_CubeDraw.set_uniform_block_binding("Camera", 0);
			program_volumeRendering.set_uniform_block_binding("Camera", 0);
			program_occusionDrawer.set_uniform_block_binding("Camera", 0);
			program_occusionDrawer.set_uniform_block_binding("Material", 1);
			program_occusionDrawer.set_uniform_block_binding("Wireframe", 2);
			program_occusionDrawer_3DTexture.set_uniform_block_binding("Camera", 0);
			program_occusionDrawer_3DTexture.set_uniform_block_binding("Material", 1);
			program_occusionDrawer_3DTexture.set_uniform_block_binding("Wireframe", 2);
		}
		catch (uniform_not_found_error const& e)
		{
			std::cerr << "Warning: Failed to set a uniform variable." << std::endl
				<< e.what() << std::endl;
		}
		frambufferobject.Init(GLviz::screen_width(), GLviz::screen_height());
		m_DS_gbuffer.Init(GLviz::screen_width(), GLviz::screen_height(), false);

		frontDirectionn.initialize(Eigen::Vector2i(1000, 1000));
		backDirection.initialize(Eigen::Vector2i(1000, 1000));
		glGenFramebuffers(1, &frameBufferID);


		atomicCounterBuffer.initialize();
		flaggedNodeCounter.initialize();

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

		normal_array_buffer.bind();
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

		index_array_buffer.bind();
		normal_array_buffer.unbind();
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
		uniform_voxel.bind_buffer_base(4);
		uniform_voxel.bind_buffer_base(5);

		GLuint tex;
		//GLviz::UniformBufferMaterial    uniform_image;
		//	uniform_image.bind_buffer_base(4);

		camera.translate(Eigen::Vector3f(0.0f, 0.0f, -2.0f));
		Utility::bindVertexAndColorArrayToBuffer(quad_vertex_array, volumeQuad_vertex_array_buffer, quad_color_array, volumeQuad_color_array_buffer, 0);
		//int workGroupCount = 0;
		//glGetIntegeri_v(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, 2, &workGroupCount);
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

	void MyViz::draw_volume(GLsizei nf, v3f_list m_colors, v3f_list m_cubeVertices, Eigen::Matrix4f transformMatrix)
	{
		voxelize_3Dtexture(transformMatrix, nf);
		//voxelize_fragmentList(1, nf);
		setup_BackTexture();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		

		program_volumeRendering.use();
		quad_vertex_array.bind();
		//volumeQuad_color_array_buffer.bind();


		GLuint loc_voxelImageTex = glGetUniformLocation(program_volumeRendering.m_program_obj, "voxelImage");
		glUniform1i(loc_voxelImageTex, 1);


		GLuint loc_frontTex = glGetUniformLocation(program_volumeRendering.m_program_obj, "frontTex");
		//frontDirectionn.bind();
		glUniform1i(loc_frontTex, 0);


		glActiveTexture(GL_TEXTURE1);
		voxelImage.bind();
		glActiveTexture(GL_TEXTURE0);
		frontDirectionn.bind();
		//glBindSampler(0, frontDirectionn.tex);

		//
		//glBindSampler(1, voxelImage.tex);


		int resX = GLviz::screen_width();
		int resY = GLviz::screen_height();
		program_volumeRendering.setParameter(shaderAttrib::i1, (void*)&resX, "resX");
		program_volumeRendering.setParameter(shaderAttrib::i1, (void*)&resY, "resY");
		program_volumeRendering.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->iso_value, "isoValue");
		//voxelization_shader_program.setParameter(shaderAttrib::mat4x4, (void*)&transformMatrix, "transform");
		glDrawArrays(GL_QUADS, 0, 24);
		program_volumeRendering.unuse();

		frontDirectionn.unbind();
		quad_vertex_array.unbind();
		//volumeQuad_color_array_buffer.unbind();
	}

	void MyViz::setup_BackTexture()
	{

		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		program_CubeDraw.use();
		quad_vertex_array.bind();

		frontDirectionn.bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frontDirectionn.tex, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_QUADS, 0, 24);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glDisable(GL_CULL_FACE);
		//frontDirectionn.unbind();
		program_CubeDraw.unuse();
		frontDirectionn.unbind();
		quad_vertex_array.unbind();
		glActiveTexture(GL_TEXTURE0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}

	void MyViz::draw_mesh(GLsizei nf, Eigen::Matrix4f transformMatrix)
	{
		glDisable(GL_CULL_FACE);
		GLviz::resetViewPort();
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



		program_meshDraw.setParameter(shaderAttrib::mat4x4, (void*)&transformMatrix, "transform");
		program_meshDraw.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->hybridCutoff, "cutoff");
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
	//	Utility::screenshot("screenshot.bmp");
		program_meshDraw.unuse();

		/*	deferredShading_GeometryPass(nf);
			deferredShading_LightPass();*/
	}


	void MyViz::deferredShading_GeometryPass(GLsizei nf)
	{
	
	//	if (use_halfRes_DS)
	//		glViewport(0, 0, GLviz::screen_width() / 2, GLviz::screen_height() / 2);

		m_DS_gbuffer.BindForWriting();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);
		program_DS_geometry.use();

		if (Parameter::getInstance()->shading_method == 0)
		{
			// Flat.
			vertex_array_vf.bind();
			//timer.startTimer();
			glDrawElements(GL_TRIANGLES, nf, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
			/*timer.stopTimer();
			timer.accumulateTime();
			timer.getAverageElapsedTime();*/
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
		program_DS_geometry.unuse();
		m_DS_gbuffer.unbind();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLviz::resetViewPort();
		//deferredShading_LightPass() ;
	}

	void MyViz::deferredShading_LightPass()
	{
		m_DS_gbuffer.BindForReading();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		GLsizei HalfWidth = (GLsizei)(GLviz::screen_width() / 2.0f);
		GLsizei HalfHeight = (GLsizei)(GLviz::screen_height() / 2.0f);

		m_DS_gbuffer.SetReadBuffer(GLviz::GBuffer::GBUFFER_TEXTURE_TYPE_TANGENT);
		glBlitFramebuffer(0, 0, GLviz::screen_width(), GLviz::screen_height(),
			0, 0, GLviz::screen_width(), GLviz::screen_height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

		/*	m_DS_gbuffer.SetReadBuffer(GLviz::GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
			glBlitFramebuffer(0, 0, GLviz::screen_width(), GLviz::screen_height(),
			0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			m_DS_gbuffer.SetReadBuffer(GLviz::GBuffer::GBUFFER_TEXTURE_TYPE_COLOR);
			glBlitFramebuffer(0, 0, GLviz::screen_width(), GLviz::screen_height(),
			0, HalfHeight, HalfWidth, GLviz::screen_height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

			m_DS_gbuffer.SetReadBuffer(GLviz::GBuffer::GBUFFER_TEXTURE_TYPE_GLOBALNORMAL);
			glBlitFramebuffer(0, 0, GLviz::screen_width(), GLviz::screen_height(),
			HalfWidth, HalfHeight, GLviz::screen_width(), GLviz::screen_height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

			m_DS_gbuffer.SetReadBuffer(GLviz::GBuffer::GBUFFER_TEXTURE_TYPE_GLOBALPOSITION);
			glBlitFramebuffer(0, 0, GLviz::screen_width(), GLviz::screen_height(),
			HalfWidth, 0, GLviz::screen_width(), HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);*/
	}

	void MyViz::drawSprings(std::vector<Spring*> springs)
	{
		glColor3f(1.0, 0.0, 0.0);
		glLineWidth(5.0);
		glPushMatrix();
		//
		glLoadMatrixf(camera.get_projection_matrix().data());
		glMultMatrixf(camera.get_modelview_matrix().data());

		glBegin(GL_LINES);
		for (std::vector<Spring*>::iterator sIt = springs.begin(); sIt != springs.end(); ++sIt)
		{
			Spring* s = *sIt;
	
			glVertex3f(s->m_Position_A[0], s->m_Position_A[1], s->m_Position_A[2]);
			glVertex3f(s->m_Position_B[0], s->m_Position_B[1], s->m_Position_B[2]);
			
		}
		glEnd();
		glPopMatrix();
	}

	void MyViz::drawLightSource()
	{
		//glLoadMatrixf(matrixModelView.getTranspose());
		//
		//glLoadIdentity();
		//gluLookAt(camPos.x, camPos.y, camPos.z,         // Position
		//	camPos.x + camDir.x, camPos.y + camDir.y, camPos.z + camDir.z,    // Lookat
		//	camUp.x, camUp.y, camUp.z);               // Up-direction
		//// apply rotation
		//glRotatef(camAngleY, 1, 0, 0); // window y axis rotates around x 
		//glRotatef(camAngleX, 0, 1, 0); // window x axis rotates around up vector
		//glMatrixMode(GL_MODELVIEW);
		if (Parameter::getInstance()->draw_shadows){
			//	gluPerspective(camera.m_fovy_grad, camera.m_aspect, camera.get_frustum().near_(), camera.get_frustum().far_());
			glPushMatrix();
			//

					glLoadMatrixf(camera.get_projection_matrix().data());
					glMultMatrixf(camera.get_modelview_matrix().data());

			//glLoadMatrixf(camera.get_modelview_matrix().data());
			//glMultMatrixf(camera.get_projection_matrix().data());

			glColor3f(1.0,0.0,0);
			glPointSize(20.0);

			glBegin(GL_POINTS);
			glVertex3f(Parameter::getInstance()->lightPosition.x(), Parameter::getInstance()->lightPosition.y(), Parameter::getInstance()->lightPosition.z());
			glEnd();
			glPopMatrix();
		}

	}

	unsigned int MyViz::voxelize_fragmentList(int voxelResolution, GLsizei nf)
	{

		int voxelResolution_ = pow(2, Parameter::getInstance()->OctreeLevel);
		//	brickImage.initialize(Eigen::Vector3i(100002,4,4), Parameter::getInstance()->OctreeLevel, Parameter::getInstance()->texparams.internalFormat, Parameter::getInstance()->texparams.format, Parameter::getInstance()->texparams.type);
		//voxelImage.initialize(Eigen::Vector3i(voxelResolution_, voxelResolution_, voxelResolution_), Parameter::getInstance()->OctreeLevel, Parameter::getInstance()->texparams.internalFormat, Parameter::getInstance()->texparams.format, Parameter::getInstance()->texparams.type);

		//	voxelImage.initialize(Eigen::Vector3i(voxelResolution, voxelResolution, voxelResolution), 1, texparams.internalFormat, texparams.format, texparams.type);

		//Set OpenGL parameter for the correct Rasterization
		glViewport(0, 0, voxelResolution_, voxelResolution_);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		//--------------------------Start Voxelization------------------------
		voxelization_shader_program.use();

		//Bind Fragment List for Octree generation
		//Allocate the right amount of memory, to save every fragment
		//voxelImage.bind();

		//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		flaggedNodeCounter.bind();
		flaggedNodeCounter.reset();

		//Bind Fragment List for Octree generation
		//Allocate the right amount of memory, to save every fragment
		//TODO: DELETE HARDCODED NUMBER, BUT HOW? (Only with 2 Step voxelization???)
		//timer.startTimer();
		fragmentListBuffer.initialize(1, sizeof(GLuint)* 22000000, GL_R32UI);
		/*timer.stopTimer();
		timer.accumulateTime();
		timer.getAverageElapsedTime();*/

		//fragmentListBuffer.initialize(1, sizeof(GLuint)* 1700000, GL_R32UI);
		fragmentListBuffer.bind();
		//Bind Images for computation
		glBindImageTexture(7, fragmentListBuffer.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI);
		glBindImageTexture(6, fragmentListBuffer.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI);

	/*	glBindImageTexture(7, voxelImage.tex, 0, GL_TRUE, 0, GL_READ_WRITE, Parameter::getInstance()->texparams.internalFormat);
		glBindImageTexture(6, voxelImage.tex, 0, GL_TRUE, 0, GL_READ_WRITE, Parameter::getInstance()->texparams.internalFormat);
*/
		//setup parameter
		voxelization_shader_program.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->hybridCutoff, "cutoff");
		voxelization_shader_program.setParameter(shaderAttrib::i1, (void*)&voxelResolution_, "voxel_Res");
		//voxelization_shader_program.setParameter(shaderAttrib::mat4x4, (void*)&transformMatrix, "transform");
		//Chose between an indexed vertex array or non indexed
		/*if (testMeshNumber >= 3){*/

		//}
		//else{
		//	vertex_array_v.bind();
		//	glDrawArrays(GL_TRIANGLES, 0, nf);
		//	vertex_array_v.unbind();
		//}

		vertex_array_vf.bind();

	//	timer.startTimer();
		glDrawElements(GL_TRIANGLES, nf, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
		/*timer.stopTimer();
		timer.accumulateTime();
		timer.getAverageElapsedTime();*/



		// compute and print the elapsed time in millisec

		vertex_array_vf.unbind();

		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
		////	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
		//glGenerateMipmap(GL_TEXTURE_3D);
		//glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
		////GL_LINEAR
		//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, Parameter::getInstance()->texparams.texturewrap);
		//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, Parameter::getInstance()->texparams.texturewrap);
		//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, Parameter::getInstance()->texparams.magFilter);
		//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, Parameter::getInstance()->texparams.minFilter);
		flaggedNodeCounter.unbind();
		voxelization_shader_program.unuse();

		GLviz::resetViewPort();
		glEnable(GL_DEPTH_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//	//	brickImage.initialize(Eigen::Vector3i(3, 3, 3), Parameter::getInstance()->OctreeLevel, GL_R32F, GL_RED, Parameter::getInstance()->texparams.type);
	//	//Set OpenGL parameter for the correct Rasterization
	//	glViewport(0, 0, voxelResolution, voxelResolution);
	//	glDisable(GL_DEPTH_TEST);
	//	glDisable(GL_CULL_FACE);
	//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);



	//	//--------------------------Start Voxelization------------------------
	//	program_voxelization.use();
	//	flaggedNodeCounter.bind();
	//	flaggedNodeCounter.reset();

	//	//Bind Fragment List for Octree generation
	//	//Allocate the right amount of memory, to save every fragment
	//	//TODO: DELETE HARDCODED NUMBER, BUT HOW? (Only with 2 Step voxelization???)
	//	fragmentListBuffer.initialize(1, sizeof(GLuint)* 1000000, GL_R32UI);
	//	fragmentListBuffer.bind();
	//	//Bind Images for computation
	//	glBindImageTexture(7, fragmentListBuffer.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI);
	//	//setup parameter
	//	program_voxelization.setParameter(shaderAttrib::i1, (void*)&voxelResolution, "voxel_Res");
	//	//Chose between an indexed vertex array or non indexed
	//	vertex_array_vf.bind();
	//	glDrawElements(GL_TRIANGLES, nf, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
	//	vertex_array_vf.unbind();

	//	//Read number of voxel fragments
		
		/*GLuint count[1];
		flaggedNodeCounter.bind();
		glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), count);
		flaggedNodeCounter.unbind();*/
	////	unsigned int numVoxelFrag = count[0];

	//	flaggedNodeCounter.unbind();
	//	program_voxelization.unuse();
	//	std::cout << count[0] << "\n";
		GLuint count = flaggedNodeCounter.getCount();
		flaggedNodeCounter.unbind();
		return count;
	}

	void MyViz::voxelize_3Dtexture(Eigen::Matrix4f transformMatrix, GLsizei nf)
	{
		int voxelResolution = pow(2, Parameter::getInstance()->OctreeLevel);
	//	brickImage.initialize(Eigen::Vector3i(100002,4,4), Parameter::getInstance()->OctreeLevel, Parameter::getInstance()->texparams.internalFormat, Parameter::getInstance()->texparams.format, Parameter::getInstance()->texparams.type);
		voxelImage.initialize(Eigen::Vector3i(voxelResolution, voxelResolution, voxelResolution), Parameter::getInstance()->OctreeLevel, Parameter::getInstance()->texparams.internalFormat, Parameter::getInstance()->texparams.format, Parameter::getInstance()->texparams.type);
		
		//	voxelImage.initialize(Eigen::Vector3i(voxelResolution, voxelResolution, voxelResolution), 1, texparams.internalFormat, texparams.format, texparams.type);

		//Set OpenGL parameter for the correct Rasterization
		glViewport(0, 0, voxelResolution, voxelResolution);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		//--------------------------Start Voxelization------------------------
		voxelization_shader_program.use();

		//Bind Fragment List for Octree generation
		//Allocate the right amount of memory, to save every fragment
		voxelImage.bind();

		//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
		//Bind Images for computation
		glBindImageTexture(7, voxelImage.tex, 0, GL_TRUE, 0, GL_READ_WRITE, Parameter::getInstance()->texparams.internalFormat);
		glBindImageTexture(6, voxelImage.tex, 0, GL_TRUE, 0, GL_READ_WRITE, Parameter::getInstance()->texparams.internalFormat);

		//setup parameter
		voxelization_shader_program.setParameter(shaderAttrib::i1, (void*)&voxelResolution, "voxel_Res");
		//voxelization_shader_program.setParameter(shaderAttrib::mat4x4, (void*)&transformMatrix, "transform");
		voxelization_shader_program.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->hybridCutoff, "cutoff");
		//Chose between an indexed vertex array or non indexed
		/*if (testMeshNumber >= 3){*/

		//}
		//else{
		//	vertex_array_v.bind();
		//	glDrawArrays(GL_TRIANGLES, 0, nf);
		//	vertex_array_v.unbind();
		//}

		vertex_array_vf.bind();

		timer.startTimer();
		glDrawElements(GL_TRIANGLES, nf, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
			timer.stopTimer();
			timer.accumulateTime();
			timer.getAverageElapsedTime();

		

		// compute and print the elapsed time in millisec
		
		vertex_array_vf.unbind();

		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
		//	glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
		glGenerateMipmap(GL_TEXTURE_3D);
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
		//GL_LINEAR
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, Parameter::getInstance()->texparams.texturewrap);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, Parameter::getInstance()->texparams.texturewrap);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, Parameter::getInstance()->texparams.magFilter);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, Parameter::getInstance()->texparams.minFilter);

		voxelization_shader_program.unuse();

		GLviz::resetViewPort();
		glEnable(GL_DEPTH_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		
	}



	void MyViz::ambientOcclusion_voxel(GLsizei nf, Eigen::Matrix4f transformMatrix)
	{
		voxelize_3Dtexture(transformMatrix, nf);

		program_occusionDrawer_3DTexture.use();
		voxelImage.bind();

		GLuint loc_voxelImageTex = glGetUniformLocation(program_occusionDrawer_3DTexture.m_program_obj, "voxelImage");

		glUniform1i(loc_voxelImageTex, 0);

		//setup parameter
		program_occusionDrawer_3DTexture.setParameter(shaderAttrib::fv3, (void*)&Parameter::getInstance()->lightPosition, "lightPosition");
		program_occusionDrawer_3DTexture.setParameter(shaderAttrib::i1, (void*)&Parameter::getInstance()->OctreeLevel, "maxLevel");
		program_occusionDrawer_3DTexture.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->distance_weight, "weight");
		program_occusionDrawer_3DTexture.setParameter(shaderAttrib::mat4x4, (void*)&transformMatrix, "transform");

		draw(nf);
	//	draw_mesh(nf);
		program_occusionDrawer_3DTexture.unuse();
		drawLightSource();
	}

	void MyViz::draw(GLsizei nf)
	{
		if (Parameter::getInstance()->shading_method == 0)
		{
			//flat
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
	}



	

	void MyViz::updateVoxelDefines()
	{
			std::set<std::string> defines;
			if (Parameter::getInstance()->use_dynamicTransform)
				defines.insert("DYNAMIC");

			if (Parameter::getInstance()->use_conservative)
				defines.insert("CONSERVATIVE");

			switch (Parameter::getInstance()->textureSetup)
			{
			case 0:
				defines.insert("FORMAT_R32F");
				break;
			case 1:
				defines.insert("FORMAT_R8");
				break;
			default:
				break;
			}
			voxelization_shader_program.initialize_program_obj(defines);
		}

	void MyViz::updateVolumeRenderingDefines()
	{
		std::set<std::string> defines;
		std::stringstream sst;
		sst << "SAMPLECOUNT " << Parameter::getInstance()->volumeRenderingSampleCount;	
		defines.insert(sst.str());
		sst.str("");
		sst << "ISOVALUE " << Parameter::getInstance()->iso_value;
		defines.insert(sst.str());
		if (Parameter::getInstance()->use_dynamicTransform)
			defines.insert("DYNAMIC");
		if (Parameter::getInstance()->useVolumeRenderingShading){
			defines.insert("USE_SHADING");
		}
		
		program_volumeRendering.initialize_program_obj(defines);
	}

	void MyViz::updateMeshRenderingDefines()
	{
		std::set<std::string> defines;
		if (Parameter::getInstance()->shading_method)
			defines.insert("SMOOTH");
		if (Parameter::getInstance()->use_dynamicTransform)
			defines.insert("DYNAMIC");
		program_meshDraw.initialize_program_obj(defines);
		program_DS_geometry.initialize_program_obj(defines);
	}

	void MyViz::updateAmbientOcclusionDefines()
	{
		std::set<std::string> defines;
		std::stringstream sst;
		sst << "CONENUMBER " << Parameter::getInstance()->coneNumber;
		defines.insert(sst.str());
		if (Parameter::getInstance()->shading_method)
			defines.insert("SMOOTH");

		if (Parameter::getInstance()->draw_shadows)
			defines.insert("DRAWSHADOWS");

		if (Parameter::getInstance()->draw_occlusion)
			defines.insert("DRAWOCCLUSION");

		if (Parameter::getInstance()->use_dynamicTransform)
			defines.insert("DYNAMIC");

		if (Parameter::getInstance()->use_all_cones)
			defines.insert("USE_ALL_CONES");

		program_occusionDrawer_3DTexture.initialize_program_obj(defines);
		program_occusionDeferredShading.initialize_program_obj(defines);
	}

	void MyViz::compute_Octree(GLsizei nf, int stage)
	{	

		numVoxelFrags = voxelize_fragmentList(pow(2, Parameter::getInstance()->OctreeLevel), nf);
		
		
		octreeComp.initializeVariables(numVoxelFrags, &fragmentListBuffer, &nodePoolBuffer, &nodePoolNeighbors, &brickImage, &flaggedNodeCounter);
		octreeComp.initializeNodePoolBuffer(); 	
		octreeComp.createStructure();
		octreeComp.initializeNeighborBuffer();
		octreeComp.computeNeighbors();
		octreeComp.computeLastLevelNeighbors();
		octreeComp.initializeBrickImageBuffer(brickSize, brickDimension);
		octreeComp.initializeLeaves();
		octreeComp.mipMapInteriorNodes();
		



		//switch (pipelineStage)
		//{

		//case 0:{
		//		   
		//		   numVoxelFrags = voxelize_fragmentList(pow(2, Parameter::getInstance()->OctreeLevel), nf);
		//		   if (bufferStage)
		//			   octreeComp.initializeVariables(numVoxelFrags, &fragmentListBuffer, &nodePoolBuffer1, &nodePoolNeighbors, &brickImage1, &flaggedNodeCounter);
		//		   else
		//			   octreeComp.initializeVariables(numVoxelFrags, &fragmentListBuffer, &nodePoolBuffer2, &nodePoolNeighbors, &brickImage2, &flaggedNodeCounter);

		//		   octreeComp.initializeNodePoolBuffer();
		//		  
		//		   pipelineStage++;
		//		   break;
		//		
		//}
		//case 1:{
		//		   octreeComp.createStructure();
		//		  // timer.startTimer();
		//		   octreeComp.initializeNeighborBuffer();
		//		   /* timer.stopTimer();
		//			timer.accumulateTime();
		//			timer.getAverageElapsedTime();*/
		//		  

		//		   pipelineStage++;
		//		   break;
		//}
		//case 2:{
		//		   
		//		   
		//		   octreeComp.computeNeighbors();
		//		   octreeComp.computeLastLevelNeighbors();
		//		   octreeComp.initializeBrickImageBuffer(brickSize2, brickDimension2);
		//		   
		//		   pipelineStage++;
		//		   break;
		//}
		//case 3:{	
		//		  
		//		   octreeComp.initializeLeaves();  
		//		   octreeComp.mipMapInteriorNodes();
		//		  

		//		   if (bufferStage){
		//			   nodePoolBuffer = nodePoolBuffer1;
		//			   brickImage = brickImage1;
		//		   }
		//		   else{
		//			   nodePoolBuffer = nodePoolBuffer2;
		//			   brickImage = brickImage2;
		//		   }
		//		   brickSize = brickSize2;
		//		   brickDimension = brickDimension2;
		//		   pipelineStage = 0;
		//		   bufferStage = !bufferStage;
		//		   break;
		//}
		//default:
		//	break;
		//}
		
	}
	void MyViz::draw_octree(){
		program_OctreeDrawer.use();
		nodePoolBuffer.bind();
		for (int i = 1; i <= Parameter::getInstance()->OctreeLevel; i++){
			int numThreads = pow(8, i);
			program_OctreeDrawer.setParameter(shaderAttrib::i1, (void*)&numThreads, "numberOfThreads");
			program_OctreeDrawer.setParameter(shaderAttrib::i1, (void*)&i, "currentLevel");
			program_OctreeDrawer.setParameter(shaderAttrib::i1, (void*)&Parameter::getInstance()->paintEmptyCells, "paintEmptyCells");
			glBindImageTexture(4, nodePoolBuffer.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
			glDrawArraysInstanced(GL_POINTS, 0, 1, numThreads);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
		vertex_array_voxel.unbind();
		program_OctreeDrawer.unuse();
	}

	void MyViz::ambientOcclusion_Octree(int nf, int brickSize, int brickDimension)
	{
		program_occusionDrawer.use();

		nodePoolBuffer.bind();

		GLuint loc_brickImageTex = glGetUniformLocation(program_occusionDrawer.m_program_obj, "brick_image");

		glUniform1i(loc_brickImageTex, 1);
		glActiveTexture(GL_TEXTURE1);
		brickImage.bind();


		glBindImageTexture(4, nodePoolBuffer.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		//glBindImageTexture(6, brickpool_occlusion.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
		//setup parameter
		program_occusionDrawer.setParameter(shaderAttrib::i1, (void*)&Parameter::getInstance()->OctreeLevel, "maxLevel");
		program_occusionDrawer.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->distance_weight, "weight");
		program_occusionDrawer.setParameter(shaderAttrib::i1, (void*)&brickDimension, "brickDimension");
		program_occusionDrawer.setParameter(shaderAttrib::i1, (void*)&brickSize, "brickSize");

		draw(nf);


		program_occusionDrawer.unuse();
		brickImage.unbind();
		//vertex_array_voxel.unbind();
	}

	void MyViz::ambientOcclusionDeferredShading(int nf)
	{

		
		

		deferredShading_GeometryPass(nf);

		
		// restore default FBO
		//frambufferobject.BindForWriting();

		//glViewport(0, 0, GLviz::screen_width() / 2, GLviz::screen_height() / 2);

		program_occusionDeferredShading.use();


		m_DS_gbuffer.BindForReading();

		glActiveTexture(GL_TEXTURE6);
		brickImage.bind();

		glActiveTexture(GL_TEXTURE5);
		nodePoolBuffer.bind();

		//glBindImageTexture(5, nodePoolBuffer.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		//glBindImageTexture(6, brickpool_occlusion.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
		//setup parameter
		program_occusionDeferredShading.setParameter(shaderAttrib::i1, (void*)&Parameter::getInstance()->OctreeLevel, "maxLevel");
		program_occusionDeferredShading.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->distance_weight, "weight");
		program_occusionDeferredShading.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->coneAngle, "aperture");
		program_occusionDeferredShading.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->coneDistance, "maxConeDistance");
		program_occusionDeferredShading.setParameter(shaderAttrib::f1, (void*)&Parameter::getInstance()->aoIntensity, "intensity");
		program_occusionDeferredShading.setParameter(shaderAttrib::i1, (void*)&Parameter::getInstance()->jittering, "jittering");
		program_occusionDeferredShading.setParameter(shaderAttrib::i1, (void*)&Parameter::getInstance()->coneConstellation, "coneConstellation");
	//	program_occusionDeferredShading.setParameter(shaderAttrib::i1, (void*)&nodePoolLength, "nodePoolLength");
		program_occusionDeferredShading.setParameter(shaderAttrib::i1, (void*)&brickSize, "brickSize");
		program_occusionDeferredShading.setParameter(shaderAttrib::i1, (void*)&brickDimension, "brickDimension");
		quad_vertex_array.bind();

		//timer.startTimer();	
		glDrawArrays(GL_QUADS, 0, 4);
		/*timer.stopTimer();
		timer.accumulateTime();
		timer.getAverageElapsedTime();*/

		quad_vertex_array.unbind();

		program_occusionDeferredShading.unuse();
		brickImage.unbind();
		m_DS_gbuffer.unbind();
		nodePoolBuffer.unbind();
		frambufferobject.unbind();

	/*	frambufferobject.BindForReading();
		glBlitFramebuffer(0, 0, GLviz::screen_width(), GLviz::screen_height(),
			0, 0, GLviz::screen_width(), GLviz::screen_height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
		frambufferobject.unbind();*/
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//GLviz::resetViewPort();

		//vertex_array_voxel.unbind();
	}
