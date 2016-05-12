#version 440


#define NEIGHBOR_NOT_FOUND 0xFFFFFFFF
#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_MASK_TAG_STATIC (0x00000003 << 30)
//#define NODE_NOT_FOUND 0xFFFFFFFF

const ivec3 brickOffsets[27] = {
  ivec3(0, 0, 0), ivec3(1, 0, 0), ivec3(2, 0, 0), ivec3(0, 1, 0), ivec3(1, 1, 0), ivec3(2, 1, 0),  ivec3(0, 2, 0),  ivec3(1, 2, 0),  ivec3(2, 2, 0),
  ivec3(0, 0, 1), ivec3(1, 0, 1), ivec3(2, 0, 1), ivec3(0, 1, 1), ivec3(1, 1, 1), ivec3(2, 1, 1),  ivec3(0, 2, 1),  ivec3(1, 2, 1),  ivec3(2, 2, 1),
  ivec3(0, 0, 2), ivec3(1, 0, 2), ivec3(2, 0, 2), ivec3(0, 1, 2), ivec3(1, 1, 2), ivec3(2, 1, 2),  ivec3(0, 2, 2),  ivec3(1, 2, 2),  ivec3(2, 2, 2),
  };

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1 ) in;

//Octree Buffer
layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;
//layout(binding = 5, r32ui) uniform uimageBuffer nodePool_attribute;
//TODO: Has to be float not int
layout(binding = 7, r32f) uniform  image3D brickpool_occlusion;

//Neighbor Buffer
//Mirrors the addresses of the nodepoolChildren times 6 and saves the neighbor address
//A NeighborTile consists of 6 Neighbors
layout(binding = 3, r32ui) uniform uimageBuffer nodePool_Neighbors;

uniform int numberOfNodes; 
uniform int leafOffset; 
uniform int brickDimension;

const float weights[4] = {1.0, 0.5, 0.25, 0.125};
//const float weights[4] = {1.0/27.0, 1.0/27.0, 1.0/27.0, 1.0/27.0};



//Calculate the 3D index out of a linear index with a given cube size
ivec3 unravelIndex(in int idx, in int size){
	return ivec3(idx % size, (idx/size) % size, (idx /(size*size)));
}

//Iterate through all nodes/tiles and allocate a Brick for each
void main()
{   
			int idx = int(gl_GlobalInvocationID.y * 1024U + gl_GlobalInvocationID.x);
			
		if(idx  >= numberOfNodes)
			return;
		
		int nodeAddress = leafOffset + idx;
		int childAddress = int(imageLoad(nodePool_children,  nodeAddress).x);
		ivec3 brickPosition = 3 * unravelIndex(nodeAddress, brickDimension);
		
		//If no child exists no mipmapping possible
		if ((NODE_MASK_VALUE & childAddress) == 0) { 
			return;  
		}
		childAddress = NODE_MASK_VALUE & childAddress;
		
		//Octant brick overview
		// X-Marks the interpolated bricks
		//Above the brick Offsets are the Octant Offsets
		// Octree Z-Dim 0 , Overall Z-Dim 1
		
		//   Off 3 	    Off 4
		// 15--16--17 15--16--17
		// |   |   |  |   |   |
		// 12--XX--XX 12--XX--14
		// |   |   |  |   |   |
		// 9 --10--11 9 --XX--11
		
		//   Off 1 	    Off 2
		// 15--XX--XX 15--16--17
		// |   |   |  |   |   |
		// 12--XX--XX 12--XX--14
		// |   |   |  |   |   |
		// 9 --10--11 9--10--11
		
		//CORE
		// Octree Z-Dim 0 , Overall Z-Dim 2
		//   Off 1 	    Off 2
		// 24--25--26 24--25--26
		// |   |   |  |   |   |
		// 21--XX--XX 21--XX--23
		// |   |   |  |   |   |
		// 18--19--20 18--XX--20
		
		//   Off 1 	    Off 2
		// 24--XX--XX 24--25--26
		// |   |   |  |   |   |
	    // 21--XX--XX 21--XX--23
        // |   |   |  |   |   |
        // 18--19--20 18--19--20
		
		//   Off 7 	    Off 8
		// 15--16--17 15--16--17
		// |   |   |  |   |   |
		// 12--XX--XX 12--XX--14
		// |   |   |  |   |   |
		// 9 --10--11 9 --XX--11
		
		//   Off 5 	    Off 6
		// 15--XX--XX 15--16--17
		// |   |   |  |   |   |
		// 12--XX--XX 12--XX--14
		// |   |   |  |   |   |
		// 9 --10--11 9--10--11
		
		//Repeats for OCtree Z-Dim 1
		
		ivec3 brick_1_Position = 3 * unravelIndex((childAddress + 0), brickDimension);
		ivec3 brick_2_Position = 3 * unravelIndex((childAddress + 1), brickDimension);
		ivec3 brick_3_Position = 3 * unravelIndex((childAddress + 2), brickDimension);
		ivec3 brick_4_Position = 3 * unravelIndex((childAddress + 3), brickDimension);
		ivec3 brick_5_Position = 3 * unravelIndex((childAddress + 4), brickDimension);
		ivec3 brick_6_Position = 3 * unravelIndex((childAddress + 5), brickDimension);
		ivec3 brick_7_Position = 3 * unravelIndex((childAddress + 6), brickDimension);
		ivec3 brick_8_Position = 3 * unravelIndex((childAddress + 7), brickDimension);
		
		
				//   Off 3 	    Off 4
		// 15--16--17 15--16--17
		// |   |   |  |   |   |
		// 12--XX--XX 12--XX--14
		// |   |   |  |   |   |
		// 9 --10--11 9 --XX--11
		
		//   Off 1 	    Off 2
		// 15--XX--XX 15--16--17
		// |   |   |  |   |   |
		// 12--XX--XX 12--XX--14
		// |   |   |  |   |   |
		// 9 --10--11 9--10--11
		
		//CORE
		// Octree Z-Dim 0 , Overall Z-Dim 2
		//   Off 1 	    Off 2
		// 24--25--26 24--25--26
		// |   |   |  |   |   |
		// 21--XX--XX 21--XX--23
		// |   |   |  |   |   |
		// 18--19--20 18--XX--20
		
		//   Off 1 	    Off 2
		// 24--XX--XX 24--25--26
		// |   |   |  |   |   |
	    // 21--XX--XX 21--XX--23
        // |   |   |  |   |   |
        // 18--19--20 18--19--20
		
		//   Off 7 	    Off 8
		// 15--16--17 15--16--17
		// |   |   |  |   |   |
		// 12--XX--XX 12--XX--14
		// |   |   |  |   |   |
		// 9 --10--11 9 --XX--11
		
		//   Off 5 	    Off 6
		// 15--XX--XX 15--16--17
		// |   |   |  |   |   |
		// 12--XX--XX 12--XX--14
		// |   |   |  |   |   |
		// 9 --10--11 9--10--11
			float value = 1.0/ 27.0 *(
			imageLoad(brickpool_occlusion, brick_1_Position + brickOffsets[13] ).x * weights[0] +
			imageLoad(brickpool_occlusion, brick_1_Position + brickOffsets[14] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_1_Position + brickOffsets[16] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_1_Position + brickOffsets[17] ).x * weights[2] +

			imageLoad(brickpool_occlusion, brick_2_Position + brickOffsets[12] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_2_Position + brickOffsets[13] ).x * weights[0] +
			imageLoad(brickpool_occlusion, brick_2_Position + brickOffsets[15] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_2_Position + brickOffsets[16] ).x * weights[1] +
																			    
			imageLoad(brickpool_occlusion, brick_3_Position + brickOffsets[10] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_3_Position + brickOffsets[11] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_3_Position + brickOffsets[13] ).x * weights[0] +
			imageLoad(brickpool_occlusion, brick_3_Position + brickOffsets[14] ).x * weights[1] +
																			    
			imageLoad(brickpool_occlusion, brick_4_Position + brickOffsets[9]  ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_4_Position + brickOffsets[10] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_4_Position + brickOffsets[12] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_4_Position + brickOffsets[13] ).x * weights[0] +
																			    
																			    
			imageLoad(brickpool_occlusion, brick_1_Position + brickOffsets[22] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_1_Position + brickOffsets[23] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_1_Position + brickOffsets[25] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_1_Position + brickOffsets[26] ).x * weights[3] +
																			    
			imageLoad(brickpool_occlusion, brick_2_Position + brickOffsets[21] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_2_Position + brickOffsets[22] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_2_Position + brickOffsets[24] ).x * weights[3] +
			imageLoad(brickpool_occlusion, brick_2_Position + brickOffsets[25] ).x * weights[2] +
																			    
			imageLoad(brickpool_occlusion, brick_3_Position + brickOffsets[19] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_3_Position + brickOffsets[20] ).x * weights[3] +
			imageLoad(brickpool_occlusion, brick_3_Position + brickOffsets[22] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_3_Position + brickOffsets[23] ).x * weights[2] +
																			    
			imageLoad(brickpool_occlusion, brick_4_Position + brickOffsets[18] ).x * weights[3] +
			imageLoad(brickpool_occlusion, brick_4_Position + brickOffsets[19] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_4_Position + brickOffsets[21] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_4_Position + brickOffsets[22] ).x * weights[1] +
																			    
																			    
			imageLoad(brickpool_occlusion, brick_5_Position + brickOffsets[4]  ).x * weights[0] +
			imageLoad(brickpool_occlusion, brick_5_Position + brickOffsets[5]  ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_5_Position + brickOffsets[7]  ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_5_Position + brickOffsets[8]  ).x * weights[2] +
																			    
			imageLoad(brickpool_occlusion, brick_6_Position + brickOffsets[3]  ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_6_Position + brickOffsets[4]  ).x * weights[0] +
			imageLoad(brickpool_occlusion, brick_6_Position + brickOffsets[6]  ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_6_Position + brickOffsets[7]  ).x * weights[1] +
																			   
			imageLoad(brickpool_occlusion, brick_7_Position + brickOffsets[1]  ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_7_Position + brickOffsets[2]  ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_7_Position + brickOffsets[4]  ).x * weights[0] +
			imageLoad(brickpool_occlusion, brick_7_Position + brickOffsets[5]  ).x * weights[1] +
																			    
			imageLoad(brickpool_occlusion, brick_8_Position + brickOffsets[0]  ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_8_Position + brickOffsets[1]  ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_8_Position + brickOffsets[3]  ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_8_Position + brickOffsets[4]  ).x * weights[0] +
																			    
																			   
			imageLoad(brickpool_occlusion, brick_5_Position + brickOffsets[13] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_5_Position + brickOffsets[14] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_5_Position + brickOffsets[16] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_5_Position + brickOffsets[17] ).x * weights[3] +
																			   
			imageLoad(brickpool_occlusion, brick_6_Position + brickOffsets[12] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_6_Position + brickOffsets[13] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_6_Position + brickOffsets[15] ).x * weights[3] +
			imageLoad(brickpool_occlusion, brick_6_Position + brickOffsets[16] ).x * weights[2] +
																			   
			imageLoad(brickpool_occlusion, brick_7_Position + brickOffsets[10] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_7_Position + brickOffsets[11] ).x * weights[3] +
			imageLoad(brickpool_occlusion, brick_7_Position + brickOffsets[13] ).x * weights[1] +
			imageLoad(brickpool_occlusion, brick_7_Position + brickOffsets[14] ).x * weights[2] +
																			   
			imageLoad(brickpool_occlusion, brick_8_Position + brickOffsets[9]  ).x * weights[3] +
			imageLoad(brickpool_occlusion, brick_8_Position + brickOffsets[10] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_8_Position + brickOffsets[12] ).x * weights[2] +
			imageLoad(brickpool_occlusion, brick_8_Position + brickOffsets[13] ).x * weights[1]
		);	
			



		//if(value > 0.0)
		//atomicCounterIncrement(nextFreeAddress_Acounter);
		//for(int i = 0; i < 27; i++){
		// imageStore(brickpool_occlusion, brickAddress + i, vec4(imageLoad(brickpool_occlusion, brick_5_Adress + i).x, 0.0, 0.0, 0.0));
		// }

		imageStore(brickpool_occlusion, brickPosition + brickOffsets[13], vec4(value, 0.0, 0.0, 0.0));
	//	imageStore(brickpool_occlusion, int(brickAddress + 13), vec4(0.7, 0.0, 0.0, 0.0));
}






























