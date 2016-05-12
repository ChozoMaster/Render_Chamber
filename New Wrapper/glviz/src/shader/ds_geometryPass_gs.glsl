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

#define EPSILON 0.00001

layout(std140, column_major) uniform Camera
{
    mat4 modelview_matrix;
	mat4 projection_matrix;
};


layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in block
{
	#ifdef SMOOTH
		vec3 relativNormal;
	    vec3 normal;
	#endif

	vec3 tangent;
	vec3 position;
	vec3 color;
	vec3 relativPosition;
	
}
In[];







out block
{
	#ifdef SMOOTH
		vec3 relativNormal;
		vec3 normal;
		vec3 tangent;
	#else
		flat vec3 normal;
		flat vec3 relativNormal;
		flat vec3 tangent;
	#endif

	vec3 position;
	vec3 color;
	vec3 relativPosition;	
}
Out;


vec3 calculateTangent(in vec3 normal)
{
	//calculate first orthogonal vector
	vec3 tangent = cross(normal, vec3(1.0, 0.0, 0.0));
		//If vectors are parallel
		if(length(tangent) < EPSILON)
			tangent = cross(normal, vec3(0.0, 0.0, 1.0));
		
	tangent = normalize(tangent);
	return tangent;
	
}


void main()
{
	
	
	
	#ifndef SMOOTH
		Out.normal = normalize(cross(In[1].position - In[0].position, In[2].position - In[0].position));
		Out.relativNormal = normalize(cross(In[1].relativPosition.xyz - In[0].relativPosition.xyz, In[2].relativPosition.xyz - In[0].relativPosition.xyz));
		Out.tangent = calculateTangent(Out.relativNormal);
	#endif
   


	gl_Position = gl_in[0].gl_Position;
	Out.color = In[0].color;
	#ifdef SMOOTH
		Out.relativNormal = In[0].relativNormal;
		Out.normal = In[0].normal;
		Out.tangent = calculateTangent(Out.relativNormal);
	#endif
	Out.relativPosition = In[0].relativPosition;
	Out.position = In[0].position;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	Out.color = In[1].color;
		#ifdef SMOOTH
		Out.relativNormal = In[1].relativNormal;
		Out.normal = In[1].normal;
		Out.tangent = calculateTangent(Out.relativNormal);
	#endif

	Out.position = In[1].position;
	Out.relativPosition = In[1].relativPosition;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	Out.color = In[2].color;
	#ifdef SMOOTH
		Out.relativNormal = In[2].relativNormal;
		Out.normal = In[2].normal;
		Out.tangent = calculateTangent(Out.relativNormal);
	#endif

	Out.position = In[2].position;
	Out.relativPosition = In[2].relativPosition;
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