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

  //TODO IN THIS SHADER
  //SET MAX LEVEL VARIABLE
  //SET LEAFSIZE VARIABLE
  
  
  
  //http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
  // Slab Test implementation
  //intersects a Ray with an origin and a direction with a box, which is parametrized with their diagonal corner coordinates (normalized 0,1)
  //returns true if the ray intersects the Box
  //also returns the near and far relative coordinate --> the distance of the ray travelled in the rayDirection until it hits the near or far plane
  bool rayIntersection(in vec3 rayOrigin, in vec3 rayDirection, in vec3 boxCorner1, in vec3 boxCorner2, out float tNearOut, out float tFarOut){
		vec3 T1 = (boxCorner1 - rayOrigin) / rayDirection;
		vec3 T2 = (boxCorner2 - rayOrigin) / rayDirection;
		
		//swap T1 and T2 if intersection with near and far plane is reverse 
		vec3 TFar = max(T1, T2);
		vec3 TNear = min(T1, T2);
		
		//take the minimum far plane value
		tFar = min(TFar.x, min(TFar.y, TFar.z));
		
		//take the maximum near plane value
		tNear = max(TNear.x, min(TNear.y, TNear.z));
		
		return tNear < tFar;
  }
  
  //Cone Trace Function
  //rayOriginRelative is normalized between 0 and 1
  //Cone diameter is the implicite 
coneTrace(in vec3 rayOriginRelative, in vec3 rayDirection, in float coneDiameter){

//initialize values
float rayEnter = 0.0;
float rayExit = 0.0;

//Check if Ray intersects with the whole volume
if (!rayIntersection(rayOriginRelative, rayDirection, vec3(0.0), vec3(1.0), rayEnter, rayExit)) {
    return;
  }
  //If the origin is in the volume (thats how it should be) the enterValue will be negative, set it to zero in that case
  rayEnter = max(rayEnter, 0.0);
  
  //Initialize Values 
  //Childmin and max will be the normalized bounding box position of the children for interpolation between the levels
  vec3 childBoundaryMin = vec3(0.0);
  vec3 childBoundaryMax = vec3(1.0);
  
  //parentMin and max will be the normalized bounding box position of the parents for interpolation between the levels
  vec3 parentBoundaryMin = vec3(0.0);
  vec3 parentBoundaryMax = vec3(1.0);
  
  //The max distance of the ray
  float end = rayExit;
  
  //The diameter will get larger over the distance, 0.0 at the origin
  float currentDiameter = 0.0
  //The distance of the ray/cone-axis we are tracing. initial value is the ray entrance point + the size of the voxels/smallest octree cells
  float currentDistance = rayEnter + nodeSizes[maxLevel - 1];
  float eps = 0.000001; //precision
  while(f < end){
		vec3 relativePosition =  (rayOriginTex + rayDirTex * f);
		//calculate current diameter by multiplying the distance from the origin with the diameters relative angle
		currentDiameter = coneDiameter * f;
		
		//clamp it between the smallest octree cell size to 1.0
		currentDiameter = clamp(currentDiameter, 1.0/LEAFESIZE,1.0);
		
		//Calculate the octree level according to the current diameter
		//The reciprocal of the diameter calculates the number of nodes on these level (non sparse)
		//Take the binary logarithm to get the octree level
		//clamp it between zero and the max level, counting from zero, meaning substract 1 from the number of levels
		//Actually substract a little over one because the it will be rounded up in the next step
		//Do I need abs??
		float lod = clamp(abs(log2(1.0/currentDiameter)), 0.0, float(maxLevel) - 1.00001);
		//Now we round up, so we have to interpolate between this level and the over it, which is the parent
		uint octreeLevel = uint(ceil(lod));
		//TODO Add Node not Found Mask
		int parentAddress = 0;
		//Now travers Octree to the target Level
		int childAddress = traverseOctree_fromLevel(relativePosition, octreeLevel, 
							childBoundaryMin, childBoundaryMax,
							parentAddress, parentBoundaryMin, parentBoundaryMax,);
		//Intersect Ray with the child to get the next ray original which is the Exit point of this ray at the voxel
		rayIntersection(relativePosition, rayDirection, childBoundaryMin, childBoundaryMax, rayEnter, rayExit);			
		//TODO Calculate Occlusion
	
	   
	   //The Exit point is the new origin + some precision offset to get an offset from the actual boundary
		f += rayExit + eps;
  }

}

//Traversing function for Cone Tracing, it will traverse to the given position in the targetLevel
//the return value is the childs address
//Also the childrens and parents boundary positions and the parentsAddress are written back
int traverseOctree_fromLevel(in vec3 relativePosition, in uint targetLevel, 
							out vec3 nodeBoundaryMin, out  nodeBoundaryMax,
							out int parentsAddress, out vec3 parentsBoundaryMin, out vec3 parentsBoundaryMax,){
	//relative Positions of the OctreeVoxel
	vec3 nodeBoundaryMin = vec3(0.0);
	vec3 nodeBoundaryMax = vec3(1.0);
	
	vec3 parentsBoundaryMin = vec3(0.0);
	vec3 parentsBoundaryMax = vec3(1.0);
	
	//The output Address of the Octree brick
	int nodeAddress = 0; 
	
	foundOnLevel = 0;
	
	//Sidelength on level 0
	float sideLength = 1.0;
	
	for(uint level = 0; level < targetLevel; ++level){
		//Load the childs Address out of the NodePool
		uint nextNode = imageLoad(nodePool_children, nodeAddress).x;
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
		parentsAddress = nodeAddress;
		nodeAddress = int(childAddress + offsetU);

		//Every Iteration the sideLength devided by half
		sideLength = sideLength / 2.0;
		
		parentsBoundaryMin = nodeBoundaryMin;
		parentsBoundaryMax = nodeBoundaryMax:
		
		//calculate Voxel Boundarys on the actual level, in the range of 0-1
		nodeBoundaryMin +=  vec3(childOffsets[offsetU]) * vec3(sideLength);
		nodeBoundaryMax = nodeBoundaryMin + vec3(sideLength);
		
		//The actual relative position of the voxel will be the inverse in the next level,
		//if the relative position is over 0.5, as long as it is under 0.5 it will still 
		//remain in the same Octree Sector
		//Remember offsetVec is already rounded so it is either 0 or 1
		
		relativePosition = 2.0 * relativePosition - vec3(offsetVec);
	}
	
	//The Method will return the Address of the Node who has no children yet
	return nodeAddress;
}



























