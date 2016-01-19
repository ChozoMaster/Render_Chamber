#version 440

#define NODE_MASK_VALUE 0x3FFFFFFF
#define NODE_MASK_TAG (0x00000001 << 31)
#define NODE_MASK_BRICK (0x00000001 << 30)
#define NODE_MASK_TAG_STATIC (0x00000003 << 30)
#define NODE_NOT_FOUND 0xFFFFFFFF
#define EPSILON 0.00001

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
  

const float nodeSizes[] = {1, 0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625, 0.0078125, 0.00390625, 0.001953125, 0.0009765625};

vec3 mapToPixelCenter(in vec3 inputCoordinates ){
 return ((inputCoordinates * 2) - 0.5);
}
//Calculate the 3D index out of a linear index with a given cube size
vec3 unravelIndex(in int idx, in int size){
	return vec3(idx % size, (idx/size) % size, (idx /(size*size)));
}

layout(std140) uniform Material
{
    vec3 color;
	float shininess;
}
material;

layout (std140) uniform Wireframe
{
	vec3 color_wireframe;
	ivec2 viewport;
};


in block
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
In;

smooth in vec3 relativPosition_FS;
smooth in vec3 relativNormal_FS;

const vec3 localConeDirections[5] = {
vec3(0.866, 0.0 ,0.5),
vec3(0.268, 0.824 , 0.5),
vec3(-0.701, 0.510 , 0.5),
vec3(-0.701, -0.510 , 0.5),
vec3(0.268, -0.824, 0.5)
};

layout(binding = 4, r32ui) uniform uimageBuffer nodePool_children;
//layout(binding = 5, r32ui) uniform uimageBuffer nodePool_attribute;
layout(binding=1) uniform sampler3D brick_image;
uniform int maxLevel;
uniform int debug_level;
uniform int debug_offset;
uniform float weight;
uniform int brickDimension;
uniform int brickSize;

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
		tFarOut = min(TFar.x, min(TFar.y, TFar.z));
		
		//take the maximum near plane value
		tNearOut = max(TNear.x, max(TNear.y, TNear.z));
		
		return (tNearOut < tFarOut);
  }

  float raycastInsideBrick(in vec3 brickAddress, in vec3 enterPosition, in vec3 leavePosition, in vec3 rayDirection, in uint level, in float nodeSize){
	float occlusion = 0.0;

	float stepLength = length(leavePosition - enterPosition);
	vec3 brickAddressRelative = (brickAddress + (1.0/3.0))/ float(brickDimension);
	//TODO: IS THIS RIGHT?
	float samplingRate = 25.0;
	float samplingStepSize = (stepLength/samplingRate);
	float occlusionValue = 0.0;
	float eps = 0.00001;
	int sampleCount = int(ceil(stepLength / samplingStepSize));

	for (int i = 0; i < samplingRate; ++i) {
	vec3 samplePosition = enterPosition + rayDirection * samplingStepSize * float(i);
	samplePosition = mapToPixelCenter(samplePosition);
	//ivec3 brickCoordinates = ivec3(samplePosition * 3.0);
	//Calculate Address Offset to load the value;
	
	//occlusionValue += imageLoad(brickpool_occlusion, int(brickAddress + addressOffset)).x;
	occlusionValue += texture(brick_image, brickAddressRelative + (samplePosition / float(brickSize))).x;

	}	

	return occlusionValue / samplingRate;/// sampleCount;
}
  
 int traverseOctree_fromLevel(in vec3 relativePosition, in uint targetLevel, 
							out vec3 nodeBoundaryMin, out vec3 nodeBoundaryMax,
							out int parentsAddress, out vec3 parentsBoundaryMin, out vec3 parentsBoundaryMax);


   //Cone Trace Function
  //rayOriginRelative is normalized between 0 and 1
  //Cone diameter is the implicite 
float coneTrace(in vec3 rayOriginRelative, in vec3 rayDirection, in float coneDiameter, in float maxDistance){

//initialize values
float rayEnter = 0.0;
float rayExit = 0.0;

// vec3 signVector = sign(rayDirection);
 vec3 boxMin = vec3(0.0);
 vec3 boxMax = vec3(1.0);

//Check if Ray intersects with the whole volume
if (!rayIntersection(rayOriginRelative, rayDirection, vec3(0.0), vec3(1.0), rayEnter, rayExit)) {
	//TODO RETURN ERROR VALUE
    return 0.0;
  }
  //If the origin is in the volume (thats how it should be) the enterValue will be negative, set it to zero in that case
  rayEnter = max(rayEnter, 0.0);
   //The max distance of the ray
  float end = clamp(maxDistance, 0.0, rayExit);
 // rayEnter = 0.0;

  //Initialize Values 
  //Childmin and max will be the normalized bounding box position of the children for interpolation between the levels
  vec3 childBoundaryMin = vec3(0.0);
  vec3 childBoundaryMax = vec3(1.0);
  
  //parentMin and max will be the normalized bounding box position of the parents for interpolation between the levels
  vec3 parentBoundaryMin = vec3(0.0);
  vec3 parentBoundaryMax = vec3(1.0);
  
  int parentAddress = 0;
  uint octreeLevel = maxLevel;
  traverseOctree_fromLevel(rayOriginRelative, octreeLevel, 
							childBoundaryMin, childBoundaryMax,
							parentAddress, parentBoundaryMin, parentBoundaryMax);
  rayIntersection(rayOriginRelative, rayDirection, childBoundaryMin, childBoundaryMax, rayEnter, rayExit);
  
 
//   float end = 1.0;
  
  //The diameter will get larger over the distance, 0.0 at the origin
  float currentDiameter = 0.0;
 
 // float currentDistance = 0.5;
  float eps = 0.000001; //precision

    //The distance of the ray/cone-axis we are tracing. initial value is the ray entrance point + the size of the voxels/smallest octree cells
 // float currentDistance = nodeSizes[maxLevel];
 float currentDistance = rayEnter + nodeSizes[maxLevel - 2];
 // float currentDistance = 0.0;
  float start_offset = rayEnter + nodeSizes[maxLevel - 1];
 // float currentDistance = rayExit + eps;

  float occlusion = 0.0;
  int steps = 0;
  while(currentDistance < end){
		if(steps > 50)
		break;
		// atomicCounterIncrement(nextFreeAddress_Acounter);
	//	vec3 relativePosition =  rayOriginRelative + (rayDirection * (start_offset + currentDistance));
		vec3 relativePosition =  rayOriginRelative + (rayDirection * currentDistance);
		//vec3 relativePosition =  (rayOriginRelative + rayDirection * 0.250000);
		//calculate current diameter by multiplying the distance from the origin with the diameters relative angle
		currentDiameter = coneDiameter * currentDistance;
		
		//clamp it between the smallest octree cell size to 1.0
	//	currentDiameter = clamp(currentDiameter, 1.0/float(nodeSizes[maxLevel]),1.0);
		currentDiameter = clamp(currentDiameter, nodeSizes[maxLevel],1.0);

		//Calculate the octree level according to the current diameter
		//The reciprocal of the diameter calculates the number of nodes on these level (non sparse)
		//Take the binary logarithm to get the octree level
		//clamp it between zero and the max level, counting from zero, meaning substract 1 from the number of levels
		//Actually substract a little over one because the it will be rounded up in the next step
		//Do I need abs??
		float lod = clamp(abs(log2(1.0/currentDiameter)), 0.0, float(maxLevel) - 1.00001);
		//Now we round up, so we have to interpolate between this level and the over it, which is the parent
		//octreeLevel = uint(ceil(lod));
		octreeLevel = maxLevel;
		//TODO Add Node not Found Mask
		//int parentAddress = 0;
		//Now travers Octree to the target Level
		int childAddress = traverseOctree_fromLevel(relativePosition, octreeLevel, 
							childBoundaryMin, childBoundaryMax,
							parentAddress, parentBoundaryMin, parentBoundaryMax);
		
		//vec3 cBrickPosition = vec3(((childAddress % brickDimension) * 3) + 1, (((childAddress % (brickDimension * brickDimension)) / brickDimension) * 3) + 1, ((childAddress / (brickDimension * brickDimension)) * 3) + 1);
		vec3 cBrickPosition = unravelIndex(childAddress, brickDimension);
		vec3 pBrickPosition = unravelIndex(parentAddress, brickDimension);
		
		//childAddress * 3;//int(imageLoad(nodePool_attribute, childAddress).x);
	//	vec3 pBrickPosition = vec3(((parentAddress % brickDimension) * 3) + 1, (((parentAddress % (brickDimension * brickDimension)) / brickDimension) * 3) + 1 , ((parentAddress / (brickDimension * brickDimension)) * 3) + 1);
		//int pBrickAddress = parentAddress * 3;

		////Intersect Ray with the child to get the next ray original which is the Exit point of this ray at the voxel
		//if(!rayIntersection(relativePosition, rayDirection, childBoundaryMin, childBoundaryMax, rayEnter, rayExit)){
		//	occlusion = 0.0;
		//	break;
		//}			
		rayIntersection(relativePosition, rayDirection, childBoundaryMin, childBoundaryMax, rayEnter, rayExit);
		
		//Calculate Occlusion
		//uint childAttributeAddress = imageLoad(nodePool_attribute, childAddress).x;
		// if (childAddress != int(NODE_NOT_FOUND)) {
		float sizeOfChild = nodeSizes[octreeLevel];
	
		//calculate Position in the Brick
		//First transform the global the global position to the childs local position (everything normalized between 0 and 1)
		vec3 childEnterPosition = ((relativePosition - childBoundaryMin)/ sizeOfChild);
		vec3 childLeavePosition = ((relativePosition + rayDirection * rayExit) - childBoundaryMin) / sizeOfChild;

		vec3 parentEnterPosition = ((relativePosition - parentBoundaryMin)/ (sizeOfChild * 2.0));
		vec3 parentLeavePosition = ((relativePosition + rayDirection * rayExit) - parentBoundaryMin) / (sizeOfChild * 2.0);
		
		float distanceWeight = 1.0 / (1.0 + (currentDistance * weight));
		//occlusion += imageLoad(brickpool_occlusion, brickAddress + 10).x;// * distanceWeight;
		//Weight the occlusion according to the travelled distance
		float cCol = 0.0;
		float pCol = 0.0;
		if(childAddress != 0){
	//	cCol = texture(brick_image, (cBrickPosition + 0.5)/ float(brickDimension)).x;
		cCol = raycastInsideBrick(cBrickPosition, childEnterPosition, childLeavePosition, rayDirection, octreeLevel, sizeOfChild) * distanceWeight;
	//	pCol = raycastInsideBrick(pBrickPosition, parentEnterPosition, parentLeavePosition, rayDirection, octreeLevel - 1, sizeOfChild * 2.0) * distanceWeight;

	//	cCol = raycastInsideBrick(cBrickPosition, childEnterPosition, childLeavePosition, rayDirection, octreeLevel, sizeOfChild) * distanceWeight;
	//	pCol = raycastInsideBrick(pBrickPosition, parentEnterPosition, parentLeavePosition, rayDirection, octreeLevel - 1, sizeOfChild * 2.0) * distanceWeight;
		}
		float result = cCol;
		//float result = mix(pCol, cCol, fract(lod));
		occlusion += (1.0 - occlusion) * result;
		 if (occlusion> 0.99)
			 break;

	   //The Exit point is the new origin + some precision offset to get an offset from the actual boundary
		currentDistance += rayExit + eps;
		steps++;
  }
  //camp the occlusion between 0.0 (not occluded) and 1.0 (full occluded)
	return clamp(occlusion, 0.0, 1.0);
}
 
  float insideBrick(in vec3 brickPosition, in vec3 enterPosition){
	vec3 samplePosition = vec3(enterPosition.x, enterPosition.y, enterPosition.z);
	//ivec3 brickCoordinates = ivec3(samplePosition * 3.0);
	//Calculate Address Offset to load the value;
	//int addressOffset = debug_offset +   brickCoordinates.y * 3 +  9 *brickCoordinates.z ;
	float occlusionValue = texture(brick_image, (brickPosition + vec3(0.999, 0.5, 0.5)) / float(brickDimension)).x;
//	float occlusionValue = texture(brick_image, vec3(10.5, 10.5, 10.5) / float(brickSize)).x;
	//float occlusionValue = 0.0;
	return occlusionValue;// sampleCount;
}
 
  
    //Cone Trace Function
  //rayOriginRelative is normalized between 0 and 1
  //Cone diameter is the implicite 
float getOcclusionValue(in vec3 position){
		vec3 childBoundaryMin      = vec3(0.0);
		vec3 childBoundaryMax      = vec3(1.0);
		vec3 parentBoundaryMin     = vec3(0.0);
		vec3 parentBoundaryMax     = vec3(1.0);
		int parentAddress         = 0;
		float sizeOfChild = nodeSizes[maxLevel];
		int childAddress = traverseOctree_fromLevel(position, maxLevel, 
							childBoundaryMin, childBoundaryMax,
							parentAddress, parentBoundaryMin, parentBoundaryMax);
	//	int brickAddress = childAddress * 27;//int(imageLoad(nodePool_attribute, childAddress).x);
		
		vec3 childEnterPosition = ((position - childBoundaryMin)/ sizeOfChild);
	//	vec3 brickPosition = vec3(1.5 + ((float(childAddress % brickDimension)) * 3.0), 1.5 + ((float(childAddress % (brickDimension * brickDimension))) / float(brickDimension) * 3.0), 1.5 + ((float(childAddress) / (float(brickDimension * brickDimension))) * 3.0));
	vec3 brickPosition = 0.0 + (unravelIndex(childAddress, brickDimension));
	//	childEnterPosition.z = 0.0;
	//	if(normal.z < 0.0){
	////	childEnterPosition = ((position - childBoundaryMin)/ sizeOfChild);
	//	childEnterPosition.z = 1.0;
	//	}
		//vec3 childLeavePosition = childEnterPosition;
		//childLeavePosition.z = 1.0;
	//	vec3 childLeavePosition = ((1.0 - boxMax - relativePosition) / sizeOfChild);
	//	vec3 childLeavePosition = ((position + normal * rayExit) - boxMin) / sizeOfChild;
	float occlusion = 0;
		occlusion = insideBrick(brickPosition, childEnterPosition);
		//float occlusion = imageLoad(brickpool_occlusion, brickAddress + offSet).x;
	//	if(childAddress == 0)
	//	occlusion = 0.0;
	return occlusion;
}



  //Traversing function for Cone Tracing, it will traverse to the given position in the targetLevel
//the return value is the childs address
//Also the childrens and parents boundary positions and the parentsAddress are written back
int traverseOctree_fromLevel(in vec3 relativePosition, in uint targetLevel, 
							out vec3 nodeBoundaryMin, out vec3 nodeBoundaryMax,
							out int parentsAddress, out vec3 parentsBoundaryMin, out vec3 parentsBoundaryMax){
	//relative Positions of the OctreeVoxel
	nodeBoundaryMin = vec3(0.0);
	nodeBoundaryMax = vec3(1.0);
	
	parentsBoundaryMin = vec3(0.0);
	parentsBoundaryMax = vec3(1.0);
	
	//The output Address of the Octree brick
	int nodeAddress = 0; 
	
	//foundOnLevel = 0;
	
	//Sidelength on level 0
	float sideLength = 1.0;
	
	for(uint level = 0; level < targetLevel; ++level){
		//Load the childs Address out of the NodePool
		uint nextNode = imageLoad(nodePool_children, nodeAddress).x;
		//Extract the Address using the defined Mask
		uint childAddress = nextNode & NODE_MASK_VALUE;
		//If the child address is zero, it means we found have to further split the node and we are finished in this step
		if(childAddress == 0U){
			nodeAddress = 0;
			return nodeAddress;
			//foundOnLevel = level;
		//	break;
		}
		//If not we go one level deeper
		//Remember an Octree node consists of 2x2x2 Children, 
		//which in our implementation lie in 8 consecutive addresses.
		//The following equations address these children according to the relative position of the Fragment	
		uvec3 offsetVec = uvec3(2.0 * (relativePosition ));


		uint offsetU = offsetVec.x + offsetVec.y * 2U + offsetVec.z * 4U;
		
		//restart recursion with the the childs address
		//old Address + child Offset 
		parentsAddress = nodeAddress;
		nodeAddress = int(childAddress + offsetU);

		//Every Iteration the sideLength devided by half
		sideLength = sideLength / 2.0;
		
		parentsBoundaryMin = nodeBoundaryMin;
		parentsBoundaryMax = nodeBoundaryMax;
		
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
  
mat3 calculateRotationMatrix(in vec3 origin, in vec3 normal)
{
	//calculate first orthogonal vector
	vec3 yAxis = cross(normal, vec3(1.0, 0.0, 0.0));
		//If vectors are parallel
		if(length(yAxis) < EPSILON)
			yAxis = cross(normal, vec3(0.0, 0.0, 1.0));
		
	yAxis = normalize(yAxis);	
	vec3 xAxis = cross(yAxis, normal);	
	xAxis = normalize(xAxis);
	mat3 transform = mat3(xAxis, yAxis, normal);

	return transform;
	
}

vec3[6] calculateConeDirections(in mat3 transformation)
{
vec3 coneDirections[6];
//Cone in normal direction
coneDirections[0] = transformation[2].xyz;
coneDirections[1] = (transformation * localConeDirections[0]);
coneDirections[2] = (transformation * localConeDirections[1]);
coneDirections[3] = (transformation * localConeDirections[2]);
coneDirections[4] = (transformation * localConeDirections[3]);
coneDirections[5] = (transformation * localConeDirections[4]);

coneDirections[1]  = normalize(coneDirections[1] );
coneDirections[2]  = normalize(coneDirections[2] );
coneDirections[3]  = normalize(coneDirections[3] );
coneDirections[4]  = normalize(coneDirections[4] );
coneDirections[5]  = normalize(coneDirections[5] );
return coneDirections;
}

#define FRAG_COLOR 0
layout(location = FRAG_COLOR, index = 0) out vec4 frag_color;


void main()
{


	#ifdef SMOOTH
		vec3 normal_eye = normalize(In.normal);
	#else
		vec3 normal_eye = In.normal;
	#endif
	vec3 normalizedNormal = normalize(relativNormal_FS);
//	vec3 normalizedNormal = vec3(1.0, 0.0, 0.0);

	mat3 transformationMatrix = calculateRotationMatrix(relativPosition_FS.xyz, normalizedNormal);
	vec3 coneDirections[] = calculateConeDirections(transformationMatrix);
	//60 degrees coneangle
	float radius = 1.0/sqrt(3);
	
//	vec3 relativPosition_FSTemp = vec3(relativPosition_FS.xy, relativPosition_FS.z/relativPosition_FS.w);
//	float occlusion1 = getOcclusionValue(vec3(x,y,z), maxLevel);
	//vec3 relativPosition_FSTemp = relativPosition_FS;
	//relativPosition_FSTemp.z = 0.45;
	float occlusion1 = 1.0 - getOcclusionValue(relativPosition_FS.xyz);
	//#ifdef CONENUMBER
	//	float occlusion1 = 1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[CONENUMBER], radius, 2.5);
	//#else
	//	float occlusion1 = 1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[0], radius, 2.5);
	//#endif
	//occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[1], radius, 0.5));
	//occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[2], radius, 0.5));
	//occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[3], radius, 0.5));
	//occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[4], radius, 0.5));
	//occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[5], radius, 0.5));


	
	float final_occlusion = occlusion1 / 1.0;
	
	//take the inverse, because 1.0 means fully occluded and 0.0 means no occlusion
	//final_occlusion = 1.0 - final_occlusion;
	if (!gl_FrontFacing)
	{
	    normal_eye = -normal_eye;
	}
	vec3 testColor = vec3(1.0, 1.0, 1.0) * final_occlusion;
	
	const vec3 light_eye = vec3(0.0, 0.0, 1.0);
	//
    float dif = max(dot(light_eye, normal_eye), 0.0);
    vec3 view_eye = normalize(In.position);
    vec3 refl_eye = reflect(light_eye, normal_eye);
    //
	float spe = pow(clamp(dot(refl_eye, view_eye), 0.0, 1.0),
        material.shininess);
    float rim = pow(1.0 + dot(normal_eye, view_eye), 3.0);
    //
    //vec3 color = 0.15 * In.color;
	vec3 color = 0.15 * testColor;
    color += 0.6 * dif * testColor;
	//color += 0.1 * spe * vec3(1.0);
    color += 0.1 * rim * vec3(1.0);

	#ifdef WIREFRAME
	    float d = min(In.distance.x, min(In.distance.y, In.distance.z));
		float i = exp2(-0.75 * d * d);
	    color = mix(color, color_wireframe, i);
	#endif

    // Gamma correction. Assuming gamma of 2.0 rather than 2.2.
//	frag_color = vec4(1.0 * final_occlusion, 0.0, 0.0, 1.0);
//frag_color = vec4( 0.0, 0.0, final_occlusion, 1.0);
   frag_color = vec4(testColor, 1.0);
  // frag_color = vec4( material.color, 1.0);
}
