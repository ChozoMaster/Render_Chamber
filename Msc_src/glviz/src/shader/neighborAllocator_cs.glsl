#version 440


#define NEIGHBOR_NOT_FOUND 0xEFFFFFFF
#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_MASK_TAG_STATIC (0x00000003 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF

#define SQUARED 1048576
//Predefined positions depending on the offset, these are the actual position (inside a Tile)
//according to the address offsets in the linear Buffer
const vec3 childOffsets[8] = {
 // uvec3(0, 0, 0),
 // uvec3(1, 0, 0),
 // uvec3(0, 1, 0),
 // uvec3(1, 1, 0),
 // uvec3(0, 0, 1),
 // uvec3(1, 0, 1),
 // uvec3(0, 1, 1), 
 // uvec3(1, 1, 1)
  
  vec3(-1.0, -1.0, -1.0),
  vec3(1.0, -1.0, -1.0),
  vec3(-1.0, 1.0, -1.0),
  vec3(1.0, 1.0, -1.0),
  vec3(-1.0, -1.0, 1.0),
  vec3(1.0, -1.0, 1.0),
  vec3(-1.0, 1.0, 1.0), 
  vec3(1.0, 1.0, 1.0)
  };

  const vec3 childOffsetsTraverse[8] = {
  uvec3(0, 0, 0),
  uvec3(1, 0, 0),
  uvec3(0, 1, 0),
  uvec3(1, 1, 0),
  uvec3(0, 0, 1),
  uvec3(1, 0, 1),
  uvec3(0, 1, 1), 
  uvec3(1, 1, 1)
  };

//Octree Buffer
//layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;

//Neighbor Buffer
//Mirrors the addresses of the nodepoolChildren times 6 and saves the neighbor address
//A NeighborTile consists of 6 Neighbors
//Offset 0 : Right Neighbor
//Offset 1 : Top Neighbor
//Offset 2 : Front Neighbor
layout(binding = 3, r32ui) uniform uimageBuffer nodePool_Neighbors;
layout(binding=5) uniform usamplerBuffer nodePool;

//uniform float voxelGridResolution; 

uniform int currentLevel; 
uniform int numberOfNodes;



int traverseOctree(in vec3 relativePosition, in int targetLevel);

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main()
{   
		
		//int idx = int(gl_GlobalInvocationID.z * 1048576 + gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);
		int idx = int(gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);
		if( idx >= numberOfNodes)
			return;
		
		//uint address = imageLoad(nodePool_children, 0).x;
		uint address = 1;
		uint childAddress = address & NODE_MASK_VALUE;
		//if(address  == 0U)
		//atomicCounterIncrement(nextFreeAddress_counter);

		vec3 relativePosition = vec3(0.5);
		//Cell offset
		float cellSize = 0.5;
		int idx_InnerOffset = 0;
		uint nextchildAddress = childAddress;
		for(uint level = 1; level <= currentLevel; ++level){
			//No further child
			if(nextchildAddress == 0U){
				imageStore(nodePool_Neighbors, (int(childAddress) * 3) + 2, uvec4(0,0, 0, 0));
				imageStore(nodePool_Neighbors, (int(childAddress) * 3) + 1, uvec4(0,0, 0, 0));
				imageStore(nodePool_Neighbors, (int(childAddress) * 3) + 0, uvec4(0,0, 0, 0));
				return;
			}
			childAddress = nextchildAddress;
			//To move the cell center the cellsize has to be halfed
			cellSize = cellSize / 2.0;
			idx_InnerOffset = int(mod((idx / pow(8,level-1)), 8));
			childAddress += idx_InnerOffset;
			nextchildAddress = texelFetch(nodePool, int(childAddress)).x;
			relativePosition += (cellSize * childOffsets[idx_InnerOffset]);
	
		}
		vec3 relativePositionNeighbor;
		int neighborAddress;
		uint neighborsChildAdress;
		//3 times the child address because of the needed 3 neighbors
		int storeAddress = int(childAddress);

		//Calculate the actual Cell size from the half cell size
		cellSize *= 2.0;
		//right Neighbour
		relativePositionNeighbor = relativePosition + vec3(cellSize, 0.0, 0.0);
		if(relativePositionNeighbor.x < 1.0){
			neighborAddress = traverseOctree(relativePositionNeighbor, currentLevel);
			neighborAddress = NODE_MASK_VALUE & neighborAddress;

			//neighborsChildAdress = imageLoad(nodePool_children, int(neighborAddress)).x;
			//neighborsChildAdress = (neighborsChildAdress & NODE_MASK_TAG);
			//if(neighborsChildAdress != 0U)
				imageStore(nodePool_Neighbors, (storeAddress * 3) + 0, uvec4(neighborAddress,0, 0, 0));
			//else
			//	imageStore(nodePool_Neighbors, (storeAddress * 3) + 0, uvec4(0 ,0, 0, 0));
		}

		//Top Neighbour
		relativePositionNeighbor = relativePosition + vec3(0.0, cellSize, 0.0);
		if(relativePositionNeighbor.y < 1.0){
			neighborAddress = traverseOctree(relativePositionNeighbor, currentLevel);
			neighborAddress = NODE_MASK_VALUE & neighborAddress;

		//	neighborsChildAdress = imageLoad(nodePool_children, int(neighborAddress)).x;
		//	if(neighborsChildAdress != 0U)
				imageStore(nodePool_Neighbors, (storeAddress * 3) + 1, uvec4(neighborAddress,0, 0, 0));
		//	else
		//		imageStore(nodePool_Neighbors, (storeAddress * 3) + 1, uvec4(0 ,0, 0, 0));
		}
	
		//Front Neighbour
		relativePositionNeighbor = relativePosition + vec3(0.0, 0.0, cellSize);
		if(relativePositionNeighbor.z < 1.0){
			neighborAddress = traverseOctree(relativePositionNeighbor, currentLevel);
			neighborAddress = NODE_MASK_VALUE & neighborAddress;

		//	neighborsChildAdress = imageLoad(nodePool_children, int(neighborAddress)).x;
		//	if(neighborsChildAdress != 0U)
				imageStore(nodePool_Neighbors, (storeAddress * 3) + 2, uvec4(neighborAddress,0, 0, 0));
		//	else
		//		imageStore(nodePool_Neighbors, (storeAddress * 3) + 2, uvec4(0 ,0, 0, 0));
		}
		 
		
}

//The Method will return the Address of the Node who has no children yet
int traverseOctree(in vec3 relativePosition, in int targetLevel){
	//relative Positions of the OctreeVoxel
	vec3 voxelPositionStart = vec3(0.0);
	vec3 voxelPositionEnd = vec3(1.0);
	
	//The output Address of the Octree brick
	int nodeAddress = 0; 
	
	//Sidelength on level 0
	float sideLength = 1.0;
	
	for(uint level = 0; level < targetLevel; ++level){
		//Load the childs Address out of the NodePool
		uint nextNode = texelFetch(nodePool, nodeAddress).x;
		//if(nextNode  == 0U)
		//atomicCounterIncrement(nextFreeAddress_counter);
		//Extract the Address using the defined Mask
		uint childAddress = nextNode & NODE_MASK_VALUE;
		//If the child address is zero, there are no further children
		if(childAddress == 0U){
			//nodeAddress = 0;
			nodeAddress = 0;
			return nodeAddress;
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
		voxelPositionStart +=  vec3(childOffsetsTraverse[offsetU]) * vec3(sideLength);
		voxelPositionEnd = voxelPositionStart + vec3(sideLength);
		
		//The actual relative position of the voxel will be the inverse in the next level,
		//if the relative position is over 0.5, as long as it is under 0.5 it will still 
		//remain in the same Octree Sector
		//Remember offsetVec is already rounded so it is either 0 or 1
		
		relativePosition = 2.0 * relativePosition - vec3(offsetVec);
	}
	
	//The Method will return the Address of the Node who has no children yet
	return nodeAddress;
}





























