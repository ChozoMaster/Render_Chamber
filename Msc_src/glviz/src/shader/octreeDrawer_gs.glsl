#version 440

#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_MASK_TAG_STATIC (0x00000003 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF

const uvec3 childOffsets[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(0, 1, 0),
  uvec3(1, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(0, 1, 1), 
  uvec3(1, 1, 1)
  };

out uint dominantAxisCase;
layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;
layout(points) in;
layout(line_strip, max_vertices = 24) out;


uniform int currentLevel;
uniform int numberOfThreads;
uniform int paintEmptyCells;
layout(std140, column_major) uniform Camera
{
    mat4 modelview_matrix;
	mat4 projection_matrix;
};

smooth out vec4 color_out;

in int instanceID[];

vec4 projectPosition( vec4 position)
{
vec4 position_eye = modelview_matrix * position;
return projection_matrix * position_eye;
}

void createFrame( vec3 position, float size)
{

//	 v8-----------v7		
//  / |			 / |
//v4-----------v3  |
//|	  |			|  |
//|	  |			|  |
//|	 v5---------|--v6
//|	/			| /
//v1-----------v2
	//size /= 2;
	vec4 v1 =  projectPosition(vec4(vec3(position) + vec3(-size, -size, size)	, 1.0));
	vec4 v2 =  projectPosition(vec4(vec3(position) + vec3(size, -size, size)	, 1.0));
	vec4 v3 =  projectPosition(vec4(vec3(position) + vec3(size, size, size)		, 1.0));
	vec4 v4 =  projectPosition(vec4(vec3(position) + vec3(-size, size, size)	, 1.0));
	vec4 v5 =  projectPosition(vec4(vec3(position) + vec3(-size, -size, -size)	, 1.0));
	vec4 v6 =  projectPosition(vec4(vec3(position) + vec3(size, -size, -size)	, 1.0));
	vec4 v7 =  projectPosition(vec4(vec3(position) + vec3(size, size, -size)	, 1.0));
	vec4 v8 =  projectPosition(vec4(vec3(position) + vec3(-size, size, -size)	, 1.0));

	
//	color_out = vec3(1.0, 0.0, 0.0);
		gl_Position = v1;
		EmitVertex();
		gl_Position = v2;
		EmitVertex();
		EndPrimitive();
	
		gl_Position = v2;
		EmitVertex();
		gl_Position = v3;
		EmitVertex();
		EndPrimitive();
	
		gl_Position = v3;
		EmitVertex();
		gl_Position = v4;
		EmitVertex();
		EndPrimitive();
	
		gl_Position = v4;
		EmitVertex();
		gl_Position = v1;
		EmitVertex();
		EndPrimitive();
	
		gl_Position = v1;
		EmitVertex();
		gl_Position = v5;
		EmitVertex();
		EndPrimitive();

		gl_Position = v2;
		EmitVertex();
		gl_Position = v6;
		EmitVertex();
		EndPrimitive();

		gl_Position = v3;
		EmitVertex();
		gl_Position = v7;
		EmitVertex();
		EndPrimitive();

		gl_Position = v4;
		EmitVertex();
		gl_Position = v8;
		EmitVertex();
		EndPrimitive();

		gl_Position = v5;
		EmitVertex();
		gl_Position = v6;
		EmitVertex();
		EndPrimitive();
	
		gl_Position = v6;
		EmitVertex();
		gl_Position = v7;
		EmitVertex();
		EndPrimitive();
	
		gl_Position = v7;
		EmitVertex();
		gl_Position = v8;
		EmitVertex();
		EndPrimitive();
	
		gl_Position = v8;
		EmitVertex();
		gl_Position = v5;
		EmitVertex();
		EndPrimitive();
}

void main()
{
	color_out = vec4(0.0,1.0,0.0, 0.0);
	//Tile Position
	int idx_NodeOffset = 0;
	//Node 1 to 8
	int idx_InnerOffset = 0;
	uint address = imageLoad(nodePool_children, idx_NodeOffset).x;
	vec3 drawPosition = vec3(0.0, 0.0, 0.0);
	float drawSize = 1.0;
	address = address & NODE_MASK_VALUE;
	float scale = 1.0;
	float offset = 0.0;

	//At root node
	address = imageLoad(nodePool_children, idx_NodeOffset + idx_InnerOffset).x;
	address = address & NODE_MASK_VALUE;
	for(int i = currentLevel; i > 0; i--){
		idx_InnerOffset = int(mod((instanceID[0] / pow(8,i-1)), 8)) ;
	//	idx_InnerOffset = instanceID[0];
		
		
		drawPosition += (childOffsets[idx_InnerOffset] / vec3(scale));
		scale *= 2;
		offset += 1/scale;
	//	drawPosition += (childOffsets[idx_InnerOffset]) - 0.5;
		address = imageLoad(nodePool_children, int(address) + idx_InnerOffset).x;
		address = address & NODE_MASK_VALUE;
			if(address == 0U){
				if((paintEmptyCells == 1) && (i == 1)){
					color_out = vec4(1.0,0.0,0.0, 1.0);
					createFrame(drawPosition - vec3(offset), 1.0/scale);
					break;
					}	else{
						return;
						}
					//	createFrame(drawPosition - vec3(offset), 1.0/scale);
			}
		
	}
	createFrame(drawPosition - vec3(offset), 1.0/scale);
}
