#include <windows.h>
#include <wingdi.h>
#include <vector>
#include <Eigen/Core>
#include <array>
#include <GLviz>
//#include "FreeImage.h"

using namespace Eigen;

namespace Utility{
static void set_vertex_normals_from_triangle_mesh(std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals)
	{
		unsigned int nf(static_cast<unsigned int>(faces.size())),
			nv(static_cast<unsigned int>(vertices.size()));
		normals.clear();
		normals.resize(vertices.size());
		std::fill(normals.begin(), normals.end(), Vector3f::Zero());

		for (unsigned int i(0); i < nf; ++i)
		{
			std::array<unsigned int, 3> const& f_i = faces[i];

			Vector3f const& p0(vertices[f_i[0]]);
			Vector3f const& p1(vertices[f_i[1]]);
			Vector3f const& p2(vertices[f_i[2]]);

			Vector3f n_i = (p0 - p1).cross(p0 - p2);

			normals[f_i[0]] += n_i;
			normals[f_i[1]] += n_i;
			normals[f_i[2]] += n_i;
		}

		for (unsigned int i(0); i < nv; ++i)
		{
			if (!normals[i].isZero())
			{
				normals[i].normalize();
			}
		}
	}


static void add_Floor(std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals)
	{
		float size = 5.0;
		float z = -0.45;
		vertices.push_back(Eigen::Vector3f(-size, z, -size));
		unsigned int index = vertices.size() - 1;

		vertices.push_back(Eigen::Vector3f(+size, z, +size));
		vertices.push_back(Eigen::Vector3f(+size, z, -size));
		vertices.push_back(Eigen::Vector3f(-size, z, +size));

		std::array<unsigned int, 3> triangle1 = { index, index + 1, index + 2 };
		std::array<unsigned int, 3> triangle2 = { index, index + 3, index + 1 };
		faces.push_back(triangle1);
		faces.push_back(triangle2);
		normals.push_back(Eigen::Vector3f(0.0, 1.0, 0.0));
		normals.push_back(Eigen::Vector3f(0.0, 1.0, 0.0));
		normals.push_back(Eigen::Vector3f(0.0, 1.0, 0.0));
		normals.push_back(Eigen::Vector3f(0.0, 1.0, 0.0));

	}

static void add_Room(std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals)
{

	float xNeg = -0.7;
	float xPos = 1.0;

	float yNeg = -0.45;
	float yPos = 1.0;

	float zNeg = -0.45;
	float zPos = 1.0;


	/*float xNeg = -1.0;
	float xPos = 1.0;

	float yNeg = -1.0;
	float yPos = 1.0;

	float zNeg = -0.45;
	float zPos = 1.0;*/


	vertices.push_back(Eigen::Vector3f(xNeg, yNeg, zNeg));
	unsigned int index = vertices.size() - 1;


	vertices.push_back(Eigen::Vector3f(xPos, yNeg, zPos)); //idx + 1
	vertices.push_back(Eigen::Vector3f(xPos, yNeg, zNeg)); // idx + 2
	vertices.push_back(Eigen::Vector3f(xNeg, yNeg, zPos)); //idx + 3
	std::array<unsigned int, 3> triangle1 = { index, index + 1, index + 2 };
	std::array<unsigned int, 3> triangle2 = { index, index + 3, index + 1 };
	faces.push_back(triangle1);
	faces.push_back(triangle2);


	//Add z wall
	vertices.push_back(Eigen::Vector3f(xPos, yPos, zNeg)); //idx + 4
	vertices.push_back(Eigen::Vector3f(xNeg, yPos, zNeg)); //idx + 5
	std::array<unsigned int, 3> triangle3 = { index, index + 2, index +  4};
	std::array<unsigned int, 3> triangle4 = { index, index + 4, index + 5 };
	faces.push_back(triangle3);
	faces.push_back(triangle4);


	//Add x-wall
	vertices.push_back(Eigen::Vector3f(xNeg, yPos, zPos)); //idx + 6
	std::array<unsigned int, 3> triangle5 = { index, index + 6, index + 3 };
	std::array<unsigned int, 3> triangle6 = { index, index + 5, index + 6 };
	faces.push_back(triangle5);
	faces.push_back(triangle6);
	set_vertex_normals_from_triangle_mesh(vertices, faces, normals);


}

static void create_cube(std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals, float size)
{

	float xNeg = -size;
	float xPos = size;

	float yNeg = -size;
	float yPos = size;

	float zNeg = -size;
	float zPos = size;



	vertices.push_back(Eigen::Vector3f(xNeg, yNeg, zNeg));
	unsigned int index = vertices.size() - 1;


	vertices.push_back(Eigen::Vector3f(xPos, yNeg, zPos)); //idx + 1
	vertices.push_back(Eigen::Vector3f(xPos, yNeg, zNeg)); // idx + 2
	vertices.push_back(Eigen::Vector3f(xNeg, yNeg, zPos)); //idx + 3
	std::array<unsigned int, 3> triangle1 = { index, index + 1, index + 2 };
	std::array<unsigned int, 3> triangle2 = { index, index + 3, index + 1 };
	faces.push_back(triangle1);
	faces.push_back(triangle2);


	//Add z wall
	vertices.push_back(Eigen::Vector3f(xPos, yPos, zNeg)); //idx + 4
	vertices.push_back(Eigen::Vector3f(xNeg, yPos, zNeg)); //idx + 5
	std::array<unsigned int, 3> triangle3 = { index, index + 2, index + 4 };
	std::array<unsigned int, 3> triangle4 = { index, index + 4, index + 5 };
	faces.push_back(triangle3);
	faces.push_back(triangle4);


	//Add x-wall
	vertices.push_back(Eigen::Vector3f(xNeg, yPos, zPos)); //idx + 6
	std::array<unsigned int, 3> triangle5 = { index, index + 6, index + 3 };
	std::array<unsigned int, 3> triangle6 = { index, index + 5, index + 6 };
	faces.push_back(triangle5);
	faces.push_back(triangle6);
	set_vertex_normals_from_triangle_mesh(vertices, faces, normals);


}


static void generateTestMesh1(std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals)
	{
		//vertices.resize(8);
		////Back z face

		//vertices[0] = Eigen::Vector3f(0.0, 0.0, 0.0);
		//vertices[1] = Eigen::Vector3f(0.0, 0.0, 1.0);
		//vertices[2] = Eigen::Vector3f(1.0, 0.0, 1.0);
		//vertices[3] = Eigen::Vector3f(1.0, 0.0, 0.0);
		//faces.push_back(std::array<unsigned int, 3>{{ 0, 1, 2 }});
		//faces.push_back(std::array<unsigned int, 3>{{ 0, 2, 3 }});


		//vertices[4] = Eigen::Vector3f(0.0, 1.0, 1.0);
		//vertices[5] = Eigen::Vector3f(0.0, 1.0, 0.0);
		//faces.push_back(std::array<unsigned int, 3>{{ 0, 4, 1 }});
		//faces.push_back(std::array<unsigned int, 3>{{ 0, 5, 4 }});

		//vertices[6] = Eigen::Vector3f(1.0, 1.0, 0.0);
		//vertices[7] = Eigen::Vector3f(0.0, 1.0, 0.0);
		//faces.push_back(std::array<unsigned int, 3>{{ 0, 3, 6 }});
		//faces.push_back(std::array<unsigned int, 3>{{ 0, 6, 7 }});
		//set_vertex_normals_from_triangle_mesh(vertices, faces, normals);

	vertices.resize(8);
	vertices[0] = Eigen::Vector3f(-0.05,	0.9 , +0.9);
	vertices[1] = Eigen::Vector3f(-0.05,	0.9 , -0.9);
	vertices[2] = Eigen::Vector3f(-0.05,	-0.9, -0.9);
	vertices[3] = Eigen::Vector3f(-0.05,	-0.9, +0.9);

	/*vertices[4] = Eigen::Vector3f(0.9, 0.25, +0.9);
	vertices[5] = Eigen::Vector3f(0.9, 0.25, -0.9);
	vertices[6] = Eigen::Vector3f(-0.9, 0.25, -0.9);
	vertices[7] = Eigen::Vector3f(-0.9, 0.25, +0.9);*/

	vertices[4] = Eigen::Vector3f(-0.25, 0.5, +0.5);
	vertices[5] = Eigen::Vector3f(-0.25, 0.5, -0.5);
	vertices[6] = Eigen::Vector3f(-0.25, -0.5, -0.5);
	vertices[7] = Eigen::Vector3f(-0.25, -0.5, +0.5);

	//faces.push_back(std::array<unsigned int, 3>{{ 0, 1, 2 }});
	//faces.push_back(std::array<unsigned int, 3>{{ 2, 3, 0 }});
	//faces.push_back(std::array<unsigned int, 3>{{ 4, 5, 6 }});
	//faces.push_back(std::array<unsigned int, 3>{{ 6, 7, 4 }});

	faces.push_back(std::array<unsigned int, 3>{{ 2, 1, 0 }});
	faces.push_back(std::array<unsigned int, 3>{{ 0, 3, 2 }});
	faces.push_back(std::array<unsigned int, 3>{{ 6, 5, 4 }});
	faces.push_back(std::array<unsigned int, 3>{{ 4, 7, 6 }});

	set_vertex_normals_from_triangle_mesh(vertices, faces, normals);

	}

static void generateTestMesh2(std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals)
	{
		vertices.resize(8);
		vertices[0] = Eigen::Vector3f(-1.0, -1.0, -0.1);
		vertices[1] = Eigen::Vector3f(+1.0, -1.0, -0.1);
		vertices[2] = Eigen::Vector3f(+1.0, +1.0, -0.1);
		vertices[3] = Eigen::Vector3f(-1.0, +1.0, -0.1);
		faces.push_back(std::array<unsigned int, 3>{{ 0, 1, 2 }});
		faces.push_back(std::array<unsigned int, 3>{{ 0, 2, 3 }});

		vertices[4] = Eigen::Vector3f(+0.5, +0.5, 0.3);
		vertices[5] = Eigen::Vector3f(+0.5, -0.5, 0.3);
		vertices[6] = Eigen::Vector3f(-0.5, -0.5, 0.3);
		vertices[7] = Eigen::Vector3f(-0.5, +0.5, 0.3);
		faces.push_back(std::array<unsigned int, 3>{{ 4, 5, 6 }});
		faces.push_back(std::array<unsigned int, 3>{{ 6, 7, 4 }});
		set_vertex_normals_from_triangle_mesh(vertices, faces, normals);
	}

static void generateTestMesh3(std::vector<Eigen::Vector3f>& vertices, std::vector<std::array<unsigned int, 3> >& faces, std::vector<Eigen::Vector3f>& normals)
	{
		vertices.resize(8);
		vertices[0] = Eigen::Vector3f(0.9, -0.05, +0.9);
		vertices[1] = Eigen::Vector3f(0.9, -0.05, -0.9);
		vertices[2] = Eigen::Vector3f(-0.9, -0.05, -0.9);
		vertices[3] = Eigen::Vector3f(-0.9, -0.05, +0.9);

		/*vertices[4] = Eigen::Vector3f(0.9, 0.25, +0.9);
		vertices[5] = Eigen::Vector3f(0.9, 0.25, -0.9);
		vertices[6] = Eigen::Vector3f(-0.9, 0.25, -0.9);
		vertices[7] = Eigen::Vector3f(-0.9, 0.25, +0.9);*/

		/*vertices[4] = Eigen::Vector3f(0.3, 0.25, +0.5);
		vertices[5] = Eigen::Vector3f(0.3, 0.25, -0.5);
		vertices[6] = Eigen::Vector3f(-0.3, 0.25, -0.5);
		vertices[7] = Eigen::Vector3f(-0.3, 0.25, +0.5);*/

		vertices[4] = Eigen::Vector3f(0.3, 0.25, +0.5);
		vertices[5] = Eigen::Vector3f(0.3, 0.25, 0.1);
		vertices[6] = Eigen::Vector3f(0.1, 0.25, 0.1);
		vertices[7] = Eigen::Vector3f(0.1, 0.25, +0.5);

		//faces.push_back(std::array<unsigned int, 3>{{ 0, 1, 2 }});
		//faces.push_back(std::array<unsigned int, 3>{{ 2, 3, 0 }});
		faces.push_back(std::array<unsigned int, 3>{{ 4, 5, 6 }});
		faces.push_back(std::array<unsigned int, 3>{{ 6, 7, 4 }});
		set_vertex_normals_from_triangle_mesh(vertices, faces, normals);
	}



static void generateVoxelVertices(std::vector<Eigen::Vector3f>& vertices, std::vector<Eigen::Vector3f>& colors)
	{
			colors.resize(24);
			vertices.resize(24);
			vertices[0] = Eigen::Vector3f(-1.0, -1.0, -1.0);
			colors[0] = Eigen::Vector3f(0.0, 0.0, 0.0);
			vertices[1] = Eigen::Vector3f(-1.0, 1.0, -1.0);
			colors[1] = Eigen::Vector3f(0.0, 1.0, 0.0);
			vertices[2] = Eigen::Vector3f(1.0, 1.0, -1.0);
			colors[2] = Eigen::Vector3f(1.0, 1.0, 0.0);
			vertices[3] = Eigen::Vector3f(1.0, -1.0, -1.0);
			colors[3] = Eigen::Vector3f(1.0, 0.0, 0.0);

			//Left X Face
			vertices[4] = Eigen::Vector3f(-1.0, -1.0, -1.0);
			colors[4] = Eigen::Vector3f(0.0, 0.0, 0.0);
			vertices[5] = Eigen::Vector3f(-1.0, -1.0, 1.0);
			colors[5] = Eigen::Vector3f(0.0, 0.0, 1.0);
			vertices[6] = Eigen::Vector3f(-1.0, 1.0, 1.0);
			colors[6] = Eigen::Vector3f(0.0, 1.0, 1.0);
			vertices[7] = Eigen::Vector3f(-1.0, 1.0, -1.0);
			colors[7] = Eigen::Vector3f(0.0, 1.0, 0.0);

			//Bottom Y Face
			vertices[8] = Eigen::Vector3f(-1.0, -1.0, -1.0);
			colors[8] = Eigen::Vector3f(0.0, 0.0, 0.0);
			vertices[9] = Eigen::Vector3f(1.0, -1.0, -1.0);
			colors[9] = Eigen::Vector3f(1.0, 0.0, 0.0);
			vertices[10] = Eigen::Vector3f(1.0, -1.0, 1.0);
			colors[10] = Eigen::Vector3f(1.0, 0.0, 1.0);
			vertices[11] = Eigen::Vector3f(-1.0, -1.0, 1.0);
			colors[11] = Eigen::Vector3f(0.0, 0.0, 1.0);

			//Front Z Face
			vertices[12] = Eigen::Vector3f(-1.0, -1.0, 1.0);
			colors[12] = Eigen::Vector3f(0.0, 0.0, 1.0);
			vertices[13] = Eigen::Vector3f(1.0, -1.0, 1.0);
			colors[13] = Eigen::Vector3f(1.0, 0.0, 1.0);
			vertices[14] = Eigen::Vector3f(1.0, 1.0, 1.0);
			colors[14] = Eigen::Vector3f(1.0, 1.0, 1.0);
			vertices[15] = Eigen::Vector3f(-1.0, 1.0, 1.0);
			colors[15] = Eigen::Vector3f(0.0, 1.0, 1.0);

			//Right X Face
			vertices[16] = Eigen::Vector3f(1.0, -1.0, -1.0);
			colors[16] = Eigen::Vector3f(1.0, 0.0, 0.0);
			vertices[17] = Eigen::Vector3f(1.0, 1.0, -1.0);
			colors[17] = Eigen::Vector3f(1.0, 1.0, 0.0);
			vertices[18] = Eigen::Vector3f(1.0, 1.0, 1.0);
			colors[18] = Eigen::Vector3f(1.0, 1.0, 1.0);
			vertices[19] = Eigen::Vector3f(1.0, -1.0, 1.0);
			colors[19] = Eigen::Vector3f(1.0, 0.0, 1.0);

			//Top Y Face
			vertices[20] = Eigen::Vector3f(-1.0, 1.0, -1.0);
			colors[20] = Eigen::Vector3f(0.0, 1.0, 0.0);
			vertices[21] = Eigen::Vector3f(-1.0, 1.0, 1.0);
			colors[21] = Eigen::Vector3f(0.0, 1.0, 1.0);
			vertices[22] = Eigen::Vector3f(1.0, 1.0, 1.0);
			colors[22] = Eigen::Vector3f(1.0, 1.0, 1.0);
			vertices[23] = Eigen::Vector3f(1.0, 1.0, -1.0);
			colors[23] = Eigen::Vector3f(1.0, 1.0, 0.0);
		}

static Eigen::Vector3f calcualteLightPosition(const float& lightSource_radius, const Eigen::Vector2f& lightAngles){
		return Eigen::Vector3f(lightSource_radius * sin(lightAngles.x()) * cos(lightAngles.y()), lightSource_radius * sin(lightAngles.x()) * sin(lightAngles.y()), lightSource_radius * cos(lightAngles.x()));
	}

static void bindVertexArrayToBuffer(GLviz::glVertexArray& vArray, GLviz::glArrayBuffer& vBuffer, int index)
	{
		vArray.bind();
		vBuffer.bind();

		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

		vArray.unbind();
		vBuffer.unbind();
	}

static void bindVertexAndColorArrayToBuffer(GLviz::glVertexArray& vArray, GLviz::glArrayBuffer& vBuffer, GLviz::glVertexArray& cArray, GLviz::glArrayBuffer& cBuffer, int index)
{
	vArray.bind();
	vBuffer.bind();

	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

	cBuffer.bind();

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE,
		3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

	vArray.unbind();
	vBuffer.unbind();
	cBuffer.unbind();

	cArray.bind();
	cBuffer.bind();

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE,
		3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));
	cArray.unbind();
	cBuffer.unbind();
}

//static void bindVertexAndColorArrayToBuffer(GLviz::glArrayBuffer& vBuffer, GLviz::glArrayBuffer& cBuffer, int index)
//{
//	vBuffer.bind();
//
//	glEnableVertexAttribArray(index);
//	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE,
//		3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));
//
//	cBuffer.bind();
//
//	glEnableVertexAttribArray(3);
//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE,
//		3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));
//	vBuffer.unbind();
//	cBuffer.unbind();
//}

static void bindVertexArrayToBuffer(GLviz::glArrayBuffer& vBuffer, std::vector<Eigen::Vector3f> vArray)
{
	vBuffer.set_buffer_data(3 * sizeof(GLfloat)
		* vArray.size(), vArray.front().data());
}

static void bindIndexArrayToBuffer(GLviz::glElementArrayBuffer& index_array_buffer, std::vector<std::array<unsigned int, 3> > indexArray)
{
	index_array_buffer.set_buffer_data(3 * sizeof(GLuint)
		* indexArray.size(), indexArray.front().data());
}

//static void screenshot(std::string path, int width, int height)
//{
//	// Make the BYTE array, factor of 3 because it's RBG.
//	BYTE* pixels = new BYTE[3 * width * height];
//
//	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);
//
//	// Convert to FreeImage format & save to file
//	FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x000000, 0x000000, 0x000000, false);
//	FreeImage_Save(FIF_BMP, image, path.c_str(), 0);
//
//	// Free resources
//	FreeImage_Unload(image);
//	delete[] pixels;
//}

static void screenshot(const char *fileName)
{
	FILE *file;
	GLviz::resetViewPort();
	unsigned long imageSize;
	GLbyte *data = NULL;
	GLint viewPort[4];
	GLenum lastBuffer;
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	bmfh.bfType = 'MB';
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = 54;
	glGetIntegerv(GL_VIEWPORT, viewPort);
	//viewPort[0] = 0;
	//viewPort[1] = 0;
	//viewPort[2] = 2000;
	//viewPort[3] = 2000;
	imageSize = ((viewPort[2] + ((4 - (viewPort[2] % 4)) % 4))*viewPort[3] * 3) + 2;
	bmfh.bfSize = imageSize + sizeof(bmfh)+sizeof(bmih);
	data = (GLbyte*)malloc(imageSize);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_PACK_SWAP_BYTES, 1);
	glGetIntegerv(GL_READ_BUFFER, (GLint*)&lastBuffer);
	glReadBuffer(GL_FRAMEBUFFER);
	glReadPixels(0, 0, viewPort[2], viewPort[3], GL_BGR, GL_UNSIGNED_BYTE, data);
	data[imageSize - 1] = 0;
	data[imageSize - 2] = 0;
	glReadBuffer(lastBuffer);
	file = fopen(fileName, "wb");
	bmih.biSize = 40;
	bmih.biWidth = viewPort[2];
	bmih.biHeight = viewPort[3];
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = 0;
	bmih.biSizeImage = imageSize;
	bmih.biXPelsPerMeter = 45089;
	bmih.biYPelsPerMeter = 45089;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;
	fwrite(&bmfh, sizeof(bmfh), 1, file);
	fwrite(&bmih, sizeof(bmih), 1, file);
	fwrite(data, imageSize, 1, file);
	free(data);
	fclose(file);
}
}