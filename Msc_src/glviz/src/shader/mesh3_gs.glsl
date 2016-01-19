#version 420
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

//#define CUTOFF 5

layout(std140, column_major) uniform Camera
{
    mat4 modelview_matrix;
	mat4 projection_matrix;
};

#ifdef WIREFRAME
	layout(std140) uniform Wireframe
	{
		vec3 color_wireframe;
		ivec2 viewport;
	};
#endif

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

//JUST FOR CLASSIFY IMAGE, DELETE LATER
in vec3 voxelSpacePosition[];

in block
{
	#ifdef SMOOTH
	    vec3 normal;
	#endif
	vec3 position;
	vec3 color;
}
In[];

in vec3 relativPosition[];
in vec3 relativNormal[];

out vec3 relativPosition_FS;

out vec3 relativNormal_FS;

out block
{
	#ifdef SMOOTH
		vec3 normal;
	#else
		flat vec3 normal;
	#endif

	vec3 position;
	vec3 color;
	#ifdef WIREFRAME
	    noperspective vec3 distance;
	#endif
}
Out;

//uniform float cutoff;

void swizzle(inout vec3 v0, inout vec3 v1, inout vec3 v2, inout vec3 normal);

bool classifyTriangle(vec3 v0, vec3 v1, vec3 v2, float cutoff)
{
	float triangleArea2D;
	triangleArea2D = abs(v0.x*(v1.y-v2.y) + v1.x*(v2.y-v0.y) + v2.x*(v0.y-v1.y)) * 0.5;
	bool result = (triangleArea2D <= cutoff);
	return result;

}

bool classifyTriangleDepth(vec3 planeOrigin, vec3 normal, vec3 AABBmin, vec3 AABBmax)
{
	float zMinFloor = floor(AABBmin.z);
	float zMaxCeil  =  ceil(AABBmax.z);
	
	int zMin = int(zMinFloor) - int(zMinFloor == AABBmin.z);
	int zMax = int(zMaxCeil ) + int(zMaxCeil == AABBmax.z);
//	zMin = float(AABBmin.z);
	bool zExpand = (zMax - zMin) == 1.0;
	return zExpand;

}

void main()
{
	vec3 v0_voxelSpace = voxelSpacePosition[0];
	vec3 v1_voxelSpace = voxelSpacePosition[1];
	vec3 v2_voxelSpace = voxelSpacePosition[2];
	vec3 voxelSpace_normal = normalize(cross(v1_voxelSpace - v0_voxelSpace, v2_voxelSpace - v1_voxelSpace));
	swizzle(v0_voxelSpace, v1_voxelSpace, v2_voxelSpace, voxelSpace_normal);
	vec3 AABBmin = min(min(v0_voxelSpace, v1_voxelSpace), v2_voxelSpace);
	vec3 AABBmax = max(max(v0_voxelSpace, v1_voxelSpace), v2_voxelSpace);
	vec3 colorOut = vec3(0.5);
	//if(classifyTriangle(v0_voxelSpace, v1_voxelSpace, v2_voxelSpace, cutoff) && classifyTriangleDepth(v0_voxelSpace, voxelSpace_normal, AABBmin, AABBmax))
	//	colorOut = vec3(0.0, 1.0, 0.0);
	//else
	//	colorOut = vec3(1.0, 0.0, 0.0);
	#ifdef WIREFRAME
	    vec2 w0 = (1.0 / gl_in[0].gl_Position.w)
			* gl_in[0].gl_Position.xy * viewport.xy;
		vec2 w1 = (1.0 / gl_in[1].gl_Position.w)
			* gl_in[1].gl_Position.xy * viewport.xy;
		vec2 w2 = (1.0 / gl_in[2].gl_Position.w)
			* gl_in[2].gl_Position.xy * viewport.xy;

		mat3 matA = mat3(vec3(1.0, w0), vec3(1.0, w1), vec3(1.0, w2));
		float area = abs(determinant(matA));
	#endif
	
	#ifndef SMOOTH
		vec3 normal = normalize(cross(In[1].position - In[0].position, In[2].position - In[0].position));
		relativNormal_FS = normalize(cross(relativPosition[1].xyz - relativPosition[0].xyz, relativPosition[2].xyz - relativPosition[0].xyz));
	#endif
   
	gl_Position = gl_in[0].gl_Position;
	Out.color = colorOut;
	#ifdef SMOOTH
		relativNormal_FS = relativNormal[0];
		Out.normal = In[0].normal;
	#else
		Out.normal = normal;
	#endif
	relativPosition_FS = relativPosition[0];
	//relativNormal_FS = relativNormal[0];
	Out.position = In[0].position;
	#ifdef WIREFRAME
	    Out.distance = vec3(area / length(w2 - w1), 0.0, 0.0);
	#endif
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	Out.color = colorOut;
		#ifdef SMOOTH
		relativNormal_FS = relativNormal[1];
		Out.normal = In[1].normal;
	#else
		Out.normal = normal;
	#endif
	Out.position = In[1].position;
	relativPosition_FS = relativPosition[1];
//	relativNormal_FS = relativNormal[1];
	#ifdef WIREFRAME
	    Out.distance = vec3(0.0, area / length(w2 - w0), 0.0);
	#endif
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	Out.color = colorOut;
	#ifdef SMOOTH
		relativNormal_FS = relativNormal[2];
		Out.normal = In[2].normal;
	#else
		Out.normal = normal;
	#endif
	Out.position = In[2].position;
	relativPosition_FS = relativPosition[2];
	//relativNormal_FS = relativNormal[2];
	#ifdef WIREFRAME
	    Out.distance = vec3(0.0, 0.0, area / length(w1 - w0));
	#endif
	EmitVertex();
}

//void main()
//{
//	v0_voxelSpace = 
//	#ifdef WIREFRAME
//	    vec2 w0 = (1.0 / gl_in[0].gl_Position.w)
//			* gl_in[0].gl_Position.xy * viewport.xy;
//		vec2 w1 = (1.0 / gl_in[1].gl_Position.w)
//			* gl_in[1].gl_Position.xy * viewport.xy;
//		vec2 w2 = (1.0 / gl_in[2].gl_Position.w)
//			* gl_in[2].gl_Position.xy * viewport.xy;

//		mat3 matA = mat3(vec3(1.0, w0), vec3(1.0, w1), vec3(1.0, w2));
//		float area = abs(determinant(matA));
//	#endif
	
//	#ifndef SMOOTH
//		vec3 normal = normalize(cross(In[1].position - In[0].position, In[2].position - In[0].position));
//		relativNormal_FS = normalize(cross(relativPosition[1].xyz - relativPosition[0].xyz, relativPosition[2].xyz - relativPosition[0].xyz));
//	#endif
   
//	gl_Position = gl_in[0].gl_Position;
//	Out.color = In[0].color;
//	#ifdef SMOOTH
//		relativNormal_FS = relativNormal[0];
//		Out.normal = In[0].normal;
//	#else
//		Out.normal = normal;
//	#endif
//	relativPosition_FS = relativPosition[0];
//	//relativNormal_FS = relativNormal[0];
//	Out.position = In[0].position;
//	#ifdef WIREFRAME
//	    Out.distance = vec3(area / length(w2 - w1), 0.0, 0.0);
//	#endif
//	EmitVertex();

//	gl_Position = gl_in[1].gl_Position;
//	Out.color = In[1].color;
//		#ifdef SMOOTH
//		relativNormal_FS = relativNormal[1];
//		Out.normal = In[1].normal;
//	#else
//		Out.normal = normal;
//	#endif
//	Out.position = In[1].position;
//	relativPosition_FS = relativPosition[1];
////	relativNormal_FS = relativNormal[1];
//	#ifdef WIREFRAME
//	    Out.distance = vec3(0.0, area / length(w2 - w0), 0.0);
//	#endif
//	EmitVertex();

//	gl_Position = gl_in[2].gl_Position;
//	Out.color = In[2].color;
//	#ifdef SMOOTH
//		relativNormal_FS = relativNormal[2];
//		Out.normal = In[2].normal;
//	#else
//		Out.normal = normal;
//	#endif
//	Out.position = In[2].position;
//	relativPosition_FS = relativPosition[2];
//	//relativNormal_FS = relativNormal[2];
//	#ifdef WIREFRAME
//	    Out.distance = vec3(0.0, 0.0, area / length(w1 - w0));
//	#endif
//	EmitVertex();
//}

void swizzle(inout vec3 v0, inout vec3 v1, inout vec3 v2, inout vec3 normal)
{

float maxVal = max(abs(normal.x) , max(abs(normal.y) , abs(normal.z)));
		//Case 1 x is dominant axis
		//z --> x
		//y --> y
		//x --> z

		//find 3 triangle edge plane

		if(maxVal == abs(normal.x)){
		v0 = v0.yzx;                        
		v1 = v1.yzx;                        
		v2 = v2.yzx;
		normal = normal.yzx;
		}
		
		//Case 2 y is dominant axis
		//x --> x
		//-z --> y
		//-y --> z
		else if(maxVal == abs(normal.y))
		{
		v0 =  v0.zxy; 
		v1 =  v1.zxy;                            
		v2 =  v2.zxy;
		normal = normal.zxy;
		}
		
		else if(maxVal == abs(normal.z))
		{
		//Case 3 z is dominant axis
		}

}