#version 440


//Nodes encoded after Crassin et al.:
//One Node conists of 64Bits (Luminance + Alpha Channel)

//Luminance Channel (32Bits):
//30 Bits encode child adress for inner nodes, zero means there is no child
//1 Bit: Max subdivision flag, indicates whether the node is refined to a maximum
//1 Bit : Node Type flag, indicates whether content is a RGBA8 value or described by a brick

//Alpha Channel (32Bits):
//Either constant color 3x10Bits
//Or Brick Adress
//Masked to read and save the values
#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_MASK_TAG_STATIC (0x00000003 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF



//Calculate the 3D index out of a linear index with a given cube size
ivec3 unravelIndex(in int idx, in int size){
	return ivec3(idx % size, (idx/size) % size, (idx /(size*size)));
}


//Octree Buffer
layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;
//TODO: Has to be float not int

layout(binding = 8, r32f) uniform  image3D brickpool_occlusion;
uniform int numberOfVoxels; 
uniform int leafOffset;
uniform int brickDimension;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;
//Write Occlusion in leafs
void main()
{   
		int idx = int(gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);
		if(idx  >= numberOfVoxels)
			return;
			
		
		int nodeAddress = leafOffset + idx;
		ivec3 brickPosition = 3 * unravelIndex(nodeAddress, brickDimension);
		uint nextAddress = imageLoad(nodePool_children, nodeAddress).x;
		nextAddress = (nextAddress & NODE_MASK_TAG);

		//Brick Address Overview 
		//
		// Z-Dim 0
		// 6--7--8
		// |  |  |
		// 3--4--5
		// |  |  |
		// 0--1--2
		
		// Z-Dim 1
		// 15--16--17
		// |   |   |
		// 12--13--14
		// |   |   |
		// 9--10--11
		
		// Z-Dim 2
		// 24--25--26
		// |   |   |
		// 21--22--23
		// |   |   |
		// 18--19--20
		// Store Occlusion
		
		//center
		if(nextAddress != 0U){
		//Filled Cell
		//imageStore(brickpool_occlusion, int(brickAdress + 13U), vec4(1.0, 0.0, 0.0, 0.0));
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 1, brickPosition.z + 1), vec4(1.0, 0.0, 0.0, 0.0));
		//imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 0, brickPosition.z + 0), vec4(1.0, 0.0, 0.0, 0.0));
	//	imageStore(brickpool_occlusion, ivec3(10.0, 10.0, 10.0), vec4(1.0, 0.0, 0.0, 0.0));
		//Border pre weighted for linear interpolation
		//Corners - 3 Neighbors +  Self = 1/4
	vec4 value = vec4(0.125,0.0, 0.0, 0.0);
//	vec4 value = vec4(0.0, 0.0, 0.0, 0.0);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 0, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 0, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 2, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 2, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 0, brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 0, brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 2, brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 2, brickPosition.z + 2), value);
		
		//Faces - 1 Neighbor + Self
	//	value = vec4(0.0,0.0, 0.0, 0.0);
		value = vec4(0.5,0.0, 0.0, 0.0);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 1, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 0, brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 1, brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 1, brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 1, brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 2, brickPosition.z + 1), value);
		
		//Edges - 2 Neighbors + self
		value = vec4(0.25,0.0, 0.0, 0.0);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1,  brickPosition.y + 0,  brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0,  brickPosition.y + 1,  brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2,  brickPosition.y + 1,  brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1,  brickPosition.y + 2,  brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0,  brickPosition.y + 0,  brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2,  brickPosition.y + 0,  brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2,  brickPosition.y + 2,  brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0,  brickPosition.y + 2,  brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1,  brickPosition.y + 0,  brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0,  brickPosition.y + 1,  brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2,  brickPosition.y + 1,  brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1,  brickPosition.y + 2,  brickPosition.z + 2), value);
		return;
		
		}
		
		// 3x3x3 empty Cell
		//Corners - 3 Neighbors +  Self = 1/4
		vec4 value = vec4(0.0,0.0, 0.0, 0.0);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 1, brickPosition.z + 1), value);

		value = vec4(0.0,0.0, 0.0, 0.0);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 0, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 0, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 2, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 2, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 0, brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 0, brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 2, brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 2, brickPosition.z + 2), value);
		
		//Faces - 1 Neighbor + Self
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 1, brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 0, brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0, brickPosition.y + 1, brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2, brickPosition.y + 1, brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 1, brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1, brickPosition.y + 2, brickPosition.z + 1), value);
		
		//Edges - 2 Neighbors + self
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1,  brickPosition.y + 0,  brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0,  brickPosition.y + 1,  brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2,  brickPosition.y + 1,  brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1,  brickPosition.y + 2,  brickPosition.z + 0), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0,  brickPosition.y + 0,  brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2,  brickPosition.y + 0,  brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2,  brickPosition.y + 2,  brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0,  brickPosition.y + 2,  brickPosition.z + 1), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1,  brickPosition.y + 0,  brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 0,  brickPosition.y + 1,  brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 2,  brickPosition.y + 1,  brickPosition.z + 2), value);
		imageStore(brickpool_occlusion, ivec3(brickPosition.x + 1,  brickPosition.y + 2,  brickPosition.z + 2), value);
}































