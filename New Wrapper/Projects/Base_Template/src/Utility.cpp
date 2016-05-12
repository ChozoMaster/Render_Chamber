#include <windows.h>
#include <wingdi.h>
#include <vector>
#include <Eigen/Core>
#include <array>
#include <GLviz>
#include "GlobalDefines.h"

using namespace Eigen;

namespace Utility{
static inline void calculate_vertex_normals_from_triangle_mesh(const v3f_list& vertices, const v3i_list& faces, v3f_list& normals)
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

v3f_list inline calculate_vertex_normals_from_faceNormals(const v3f_list& vertices, const v3i_list& facesIdx, const v3f_list& faceNormals)
{
	std::vector<int> counterVec(vertices.size(), 0);
	v3f_list result(vertices.size(), Eigen::Vector3f::Zero());

	//Sum Normals
//#pragma omp parallel for
	for (int i = 0; i < facesIdx.size(); i++){

		auto vertexIndices = facesIdx[i];
		auto normal = faceNormals[i];
		for (int k = 0; k < 3; k++){
			auto vIdx = vertexIndices[k];
//#pragma omp atomic
			result[vertexIndices[k]] += normal;
//#pragma omp atomic
			counterVec[vertexIndices[k]]++;
		}
	}
	//Take the average
	for (int i = 0; i < vertices.size(); i++){
//#pragma omp atomic
		result[i] /= float(counterVec[i]);
	}
	return result;
}

static inline Eigen::Vector3f calcualteLightPosition(const float& lightSource_radius, const Eigen::Vector2f& lightAngles){
		return Eigen::Vector3f(lightSource_radius * sin(lightAngles.x()) * cos(lightAngles.y()), lightSource_radius * sin(lightAngles.x()) * sin(lightAngles.y()), lightSource_radius * cos(lightAngles.x()));
	}

static inline void bindVertexArrayToBuffer(GLviz::glVertexArray& vArray, GLviz::glArrayBuffer& vBuffer, int index)
	{
		vArray.bind();
		vBuffer.bind();

		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

		vArray.unbind();
		vBuffer.unbind();
	}

static inline void bindVertexAndColorArrayToBuffer(GLviz::glVertexArray& vArray, GLviz::glArrayBuffer& vBuffer, GLviz::glVertexArray& cArray, GLviz::glArrayBuffer& cBuffer, int index)
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

static void bindVertexArrayToBuffer(GLviz::glArrayBuffer& vBuffer, v3f_list vArray)
{
	vBuffer.set_buffer_data(3 * sizeof(GLfloat)
		* vArray.size(), vArray.front().data());
}

static void bindIndexArrayToBuffer(GLviz::glElementArrayBuffer& index_array_buffer, v3i_list indexArray)
{
	index_array_buffer.set_buffer_data(3 * sizeof(GLuint)
		* indexArray.size(), indexArray.front().data());
}

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

static void generateTestMesh1(v3f_list& vertices, std::vector<std::array<unsigned int, 3> >& faces, v3f_list& normals)
{
	//vertices.resize(8);
	////Back z face

	vertices.resize(8);
	vertices[0] = Eigen::Vector3f(-0.05, 0.9, +0.9);
	vertices[1] = Eigen::Vector3f(-0.05, 0.9, -0.9);
	vertices[2] = Eigen::Vector3f(-0.05, -0.9, -0.9);
	vertices[3] = Eigen::Vector3f(-0.05, -0.9, +0.9);


	vertices[4] = Eigen::Vector3f(-0.25, 0.5, +0.5);
	vertices[5] = Eigen::Vector3f(-0.25, 0.5, -0.5);
	vertices[6] = Eigen::Vector3f(-0.25, -0.5, -0.5);
	vertices[7] = Eigen::Vector3f(-0.25, -0.5, +0.5);

	faces.push_back(std::array<unsigned int, 3>{{ 2, 1, 0 }});
	faces.push_back(std::array<unsigned int, 3>{{ 0, 3, 2 }});
	faces.push_back(std::array<unsigned int, 3>{{ 6, 5, 4 }});
	faces.push_back(std::array<unsigned int, 3>{{ 4, 7, 6 }});

	calculate_vertex_normals_from_triangle_mesh(vertices, faces, normals);

}
}