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

//Predefined positions depending on the offset, these are the actual position (inside a Tile)
//according to the address offsets in the linear Buffer
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

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;
//Contains the voxel position for every fragment
layout(binding = 2, rgb10_a2ui) uniform uimageBuffer voxelFragmentList;

layout(binding = 0) uniform  atomic_uint nextFreeAddress_Acounter;
//Octree Buffer
layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;


layout(binding=5) uniform usamplerBuffer nodePool;


uniform int voxelGridResolution; 
uniform int numberOfVoxelFragments;

uniform int currentLevel;

//DELTE LATER
uniform int startAddress;

//Mark the Node to allocate the children in the allocation step
void flagNode(in uint nodeNext, in int address) {

	//modify childs address to set the Tag to 1
  nodeNext = NODE_MASK_TAG | nodeNext;
  imageStore(nodePool_children, address, uvec4(nodeNext,0, 0, 0));
  memoryBarrier();
}

int traverseOctree(in vec3 relativePosition, out uint foundOnLevel);

//Conversion from 32Bit values to 4 8Bit Values
vec4 convRGBA8ToVec4(uint val) {
    return vec4( float((val & 0x000000FF)), 
                 float((val & 0x0000FF00) >> 8U), 
                 float((val & 0x00FF0000) >> 16U), 
                 float((val & 0xFF000000) >> 24U));
}

//vice versa
uint convVec4ToRGBA8(vec4 val) {
    return (uint(val.w) & 0x000000FF)   << 24U
            |(uint(val.z) & 0x000000FF) << 16U
            |(uint(val.y) & 0x000000FF) << 8U 
            |(uint(val.x) & 0x000000FF);
}

//conversion from 3x10 bit values to 1x32Bit value (LSB) to describe voxel position with a max resoluton of 1024
uint vec3ToUintXYZ10(uvec3 val) {
    return (uint(val.z) & 0x000003FF)   << 20U
            |(uint(val.y) & 0x000003FF) << 10U 
            |(uint(val.x) & 0x000003FF);
}

//Vice versa
uvec3 uintXYZ10ToVec3(uint val) {
    return uvec3(uint((val & 0x000003FF)),
                 uint((val & 0x000FFC00) >> 10U), 
                 uint((val & 0x3FF00000) >> 20U));
}


void main()
{   //Get linear ID
	int idx = int(gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);
		if( idx >= numberOfVoxelFragments )
	    return;

	//Get Voxel position from the voxelFragmentList of the Voxelization Step, 
	//The built in parameter gl_VertexID acts as the workItem ID, similar to CUDA or OpenCL 
	//Take 32Bit int out of FragmentList and get vec3 out of the 3x10 Bits
	uvec3 vec_voxelPos = imageLoad(voxelFragmentList, idx).xyz;

	//Calculate relativ VoxelPosition, to know in which direction
	vec3 relativPosition = vec3(vec_voxelPos) / vec3(voxelGridResolution);

	uint foundOnLevel = 0;

	//Traverse the Octree
	int nodeAddress = traverseOctree(relativPosition, foundOnLevel);
	
	//Only set the address if the child is not on the max Level
	if(foundOnLevel == currentLevel){
		//TODO: DO I NEED THIS??
		//uint nextNodeAddress = imageLoad(nodePool_children, nodeAddress).x;
		flagNode(0,nodeAddress);
	}
}

//The Method will return the Address of the Node who has no children yet
int traverseOctree(in vec3 relativePosition, out uint foundOnLevel){
	//relative Positions of the OctreeVoxel
	vec3 voxelPositionStart = vec3(0.0);
	vec3 voxelPositionEnd = vec3(1.0);
	
	//The output Address of the Octree brick
	int nodeAddress = 0; 
	
	foundOnLevel = 0;
	
	//Sidelength on level 0
	float sideLength = 1.0;
	
	for(uint level = 0; level < currentLevel; ++level){
		//Load the childs Address out of the NodePool
	//	uint nextNode = imageLoad(nodePool_children, nodeAddress).x;
		uint nextNode = texelFetch(nodePool, nodeAddress).x;
		
		//Extract the Address using the defined Mask
		uint childAddress = nextNode & NODE_MASK_VALUE;
		//If the child address is zero, it means we found have to further split the node and we are finished in this step
		
		if(childAddress == 0U){
			foundOnLevel = level;
			break;
		}
		//If not we go one level deeper
		//Remember an Octree node consists of 2x2x2 Children, 
		//which in our implementation lie in 8 consecutive addresses.
		//The following equations address these children according to the relative position of the Fragment	
		uvec3 offsetVec = uvec3(2.0 * relativePosition);


		uint offsetU = offsetVec.x + offsetVec.y * 2U + offsetVec.z * 4U;
		
		//restart recursion with the the childs address
		//old Address + child Offset 
		nodeAddress = int(childAddress + offsetU);

		//Every Iteration the sideLength devided by half
		sideLength = sideLength / 2.0;
		
		//calculate Voxel Boundarys on the actual level, in the range of 0-1
		voxelPositionStart +=  vec3(childOffsets[offsetU]) * vec3(sideLength);
		voxelPositionEnd = voxelPositionStart + vec3(sideLength);
		
		//The actual relative position of the voxel will be the inverse in the next level,
		//if the relative position is over 0.5, as long as it is under 0.5 it will still 
		//remain in the same Octree Sector
		//Remember offsetVec is already rounded so it is either 0 or 1
		
		relativePosition = 2.0 * relativePosition - vec3(offsetVec);
		foundOnLevel = level + 1;
	}
	//The Method will return the Address of the Node who has no children yet
	return nodeAddress;
}





























