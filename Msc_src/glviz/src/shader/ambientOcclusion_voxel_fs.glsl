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
uniform vec3 lightPosition;

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

uniform int maxLevel;
uniform int octreeResolution;
uniform int debug_level;
uniform int debug_offset;
uniform float weight;

//Voxel Texture
//layout(binding = 8, r32f) uniform  image3D voxelImage;
uniform sampler3D voxelImage;

 //http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
  // Slab Test implementation
  //intersects a Ray with an origin and a direction with a box, which is parametrized with their diagonal corner coordinates (normalized 0,1)
  //returns true if the ray intersects the Box
  //also returns the near and far relative coordinate --> the distance of the ray travelled in the rayDirection until it hits the near or far plane
  bool rayIntersection(in vec3 rayOrigin, in vec3 rayDirection, in vec3 boxCorner1, in vec3 boxCorner2, out float tNearOut, out float tFarOut){
  
		vec3 T1 = (boxCorner1 - rayOrigin) / rayDirection;
		vec3 T2 = (boxCorner2 - rayOrigin) / rayDirection;

		//if(rayDirection.x == 0.0){
		//T1.x = boxCorner1.x - rayOrigin.x;
		//T2.x = boxCorner2.x - rayOrigin.x;
		//}
		//if(rayDirection.y == 0.0){
		//T1.y = boxCorner1.y - rayOrigin.y;
		//T2.y = boxCorner2.y - rayOrigin.y;
		//}
		//if(rayDirection.z == 0.0){
		//T1.z = boxCorner1.z - rayOrigin.z;
		//T2.z = boxCorner2.z - rayOrigin.z;
		//}
		//swap T1 and T2 if intersection with near and far plane is reverse 
		vec3 TFar = max(T1, T2);
		vec3 TNear = min(T1, T2);
		
		//take the minimum far plane value
		tFarOut = min(TFar.x, min(TFar.y, TFar.z));
		
		//take the maximum near plane value
		tNearOut = max(TNear.x, max(TNear.y, TNear.z));
		
		return (tNearOut < tFarOut);
  }

  
  
 
 
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
  
//   float end = 1.0;
  
  //The diameter will get larger over the distance, 0.0 at the origin
  float currentDiameter = 0.0;

 // float currentDistance = 0.5;
  float eps = 0.000001; //precision

    //The distance of the ray/cone-axis we are tracing. initial value is the ray entrance point + the size of the voxels/smallest octree cells
  float currentDistance = 0.0;
  float start_offset = 0.0 + nodeSizes[maxLevel - 2];
  float occlusion = 0.0;
  int steps = 0;
  int maxsteps = 100;
  float stepSize = (end - rayEnter) / maxsteps;
 // float value = texture(voxelImage, rayOriginRelative ).x;
 // return clamp(value, 0.0, 1.0);
 //while(currentDistance < end){
  while(steps < maxsteps){
	//	if(steps > 50)
	//	break;
		// atomicCounterIncrement(nextFreeAddress_Acounter);
		vec3 relativePosition =  (rayOriginRelative + rayDirection * (start_offset + currentDistance));
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
		//octreeLevel = uint(ceil(lod));;
		//float sizeOfChild = nodeSizes[octreeLevel];
		//vec3 absBoundary = relativePosition / sizeOfChild;
		//childBoundaryMin = floor(absBoundary) * sizeOfChild;
		//childBoundaryMax = ceil(absBoundary) * sizeOfChild;
		
		//rayIntersection(relativePosition, rayDirection, childBoundaryMin, childBoundaryMax, rayEnter, rayExit);
		
		float distanceWeight = 1.0 / (1.0 + (currentDistance * weight));

		//occlusion += imageLoad(brickpool_occlusion, brickAddress + 10).x;// * distanceWeight;
		//Weight the occlusion according to the travelled distance
		float value = textureLod(voxelImage, relativePosition, lod).x * distanceWeight;
	//	float value = texture3D(voxelImage, relativePosition).x;
	//	float value = 1.0;
		occlusion += (1.0 - occlusion) * value;
		 if (occlusion> 0.99)
			 break;

	   //The Exit point is the new origin + some precision offset to get an offset from the actual boundary
		//currentDistance += rayExit + eps;
		currentDistance += stepSize;
		steps++;
  }
  //camp the occlusion between 0.0 (not occluded) and 1.0 (full occluded)
	return clamp(occlusion, 0.0, 1.0);
}
 
 
 float rayTrace_shadow(in vec3 rayOriginRelative, in vec3 rayDirection, in float maxDistance){

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
  

 // float currentDistance = 0.5;
  float eps = 0.000001; //precision

    //The distance of the ray/cone-axis we are tracing. initial value is the ray entrance point + the size of the voxels/smallest octree cells
  float currentDistance = rayEnter + nodeSizes[maxLevel - 1];

  float brightness = 0.0;
  int steps = 0;

 // float value = texture(voxelImage, rayOriginRelative ).x;
 // return clamp(value, 0.0, 1.0);
 while(currentDistance < end){
		if(steps > 100)
		break;
		// atomicCounterIncrement(nextFreeAddress_Acounter);
		vec3 relativePosition =  (rayOriginRelative + rayDirection * currentDistance);
		if(any(greaterThan(relativePosition, vec3(1.0))) || any(lessThan(relativePosition, vec3(0.0)))){
		return brightness;
		}
		//vec3 relativePosition =  (rayOriginRelative + rayDirection * 0.250000);
		//calculate current diameter by multiplying the distance from the origin with the diameters relative angle

	
		float sizeOfChild = nodeSizes[octreeLevel];
		vec3 absBoundary = relativePosition / sizeOfChild;
		childBoundaryMin = floor(absBoundary) * sizeOfChild;
		childBoundaryMax = ceil(absBoundary) * sizeOfChild;
		
		//rayIntersection(relativePosition, rayDirection, childBoundaryMin, childBoundaryMax, rayEnter, rayExit);

		//occlusion += imageLoad(brickpool_occlusion, brickAddress + 10).x;// * distanceWeight;
		//Weight the occlusion according to the travelled distance
		float value = texture(voxelImage, relativePosition).x;
		//if(value > 0.0)
		//	return value;
		//
		//
		brightness += value;
		
	//	brightness += (1.0 - brightness) * value;
		if(brightness > 0.99){
		break;
		}
	
	//	occlusion += (1.0 - occlusion) * value;
	//	 if (occlusion> 0.99)
	//		 break;
    //
	   //The Exit point is the new origin + some precision offset to get an offset from the actual boundary
		//currentDistance += rayExit + eps;
		currentDistance += sizeOfChild + eps;
		steps++;
  }
  //camp the occlusion between 0.0 (not occluded) and 1.0 (full occluded)
	return clamp(brightness, 0.0, 1.0);
}
 
 
  
  
mat3 calculateTransformationMatrix(in vec3 normal)
{
	//calculate first orthogonal vector
	vec3 yAxis = normalize(cross(normal, vec3(1.0, 0.0, 0.0)));
		//If vectors are parallel
		if(length(yAxis) < EPSILON)
			yAxis = normalize(cross(normal, vec3(0.0, 0.0, 1.0)));
			
	vec3 xAxis = normalize(cross(yAxis, normal));	
	//return mat3(vec3(xAxis.x, yAxis.x, normal.x),
	//			vec3(xAxis.y, yAxis.y, normal.y),
	//			vec3(xAxis.z, yAxis.z, normal.z));
		return mat3(xAxis,
				yAxis,
				normal
	);

	////calculate first orthogonal vector
	//vec3 yAxis = cross(vec3(1.0, 0.0, 0.0), normal);
	//	//If vectors are parallel
	//	if(length(yAxis) < EPSILON)
	//		yAxis = cross(vec3(1.0, 0.0, 0.0), normal);
			
	//vec3 xAxis = cross(yAxis, normal);	
	//	return mat4(vec4(xAxis, 0.0),
	//			vec4(yAxis, 0.0),
	//			vec4(normal, 0.0),
	//			vec4(origin, 1.0)



	//);
	
}

vec3[6] calculateConeDirections(in mat3 transformation)
{
vec3 coneDirections[6];

//Cone in normal direction
mat3 inverseTransform = inverse(transformation);
coneDirections[0] = transformation[2].xyz;
//coneDirections[1] = (transformation * localConeDirections[0]).xyz;
//coneDirections[2] = (transformation * localConeDirections[1]).xyz;
//coneDirections[3] = (transformation * localConeDirections[2]).xyz;
//coneDirections[4] = (transformation * localConeDirections[3]).xyz;
//coneDirections[5] = (transformation * localConeDirections[4]).xyz;

coneDirections[1] = (inverseTransform * localConeDirections[0]).xyz;
coneDirections[2] = (inverseTransform * localConeDirections[1]).xyz;
coneDirections[3] = (inverseTransform * localConeDirections[2]).xyz;
coneDirections[4] = (inverseTransform * localConeDirections[3]).xyz;
coneDirections[5] = (inverseTransform * localConeDirections[4]).xyz;

//coneDirections[1] = (transformation * transpose(localConeDirections[0])).xyz;
//coneDirections[2] = (transformation * transpose(localConeDirections[1])).xyz;
//coneDirections[3] = (transformation * transpose(localConeDirections[2])).xyz;
//coneDirections[4] = (transformation * transpose(localConeDirections[3])).xyz;
//coneDirections[5] = (transformation * transpose(localConeDirections[4])).xyz;

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

	mat3 transformationMatrix = calculateTransformationMatrix(normalizedNormal);
	vec3 coneDirections[] = calculateConeDirections(transformationMatrix);
	//60 degrees coneangle
	float radius = 1.0/sqrt(3);
	
//	vec3 relativPosition_FSTemp = vec3(relativPosition_FS.xy, relativPosition_FS.z/relativPosition_FS.w);
//	float occlusion1 = getOcclusionValue(vec3(x,y,z), maxLevel);
	//vec3 relativPosition_FSTemp = relativPosition_FS;
	//relativPosition_FSTemp.z = 0.45;
//	float occlusion1 = 1.0 - getOcclusionValue(relativPosition_FSTemp.xyz);
	float occlusion1 = 1.0;
	float brightness = 1.0;
	vec3 lightDirection = lightPosition - relativPosition_FS;

	float lightDistance = length(lightDirection);
	lightDirection = normalize(lightDirection);
	float final_occlusion = 1.0;

	#ifdef DRAWOCCLUSION
		#ifdef USE_ALL_CONES
			occlusion1 = 1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[0], radius, 15.5);
			occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[1], radius, 15.0));
			occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[2], radius, 15.0));
			occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[3], radius, 15.0));
			occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[4], radius, 15.0));
			occlusion1 += (1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[5], radius, 15.0));
			final_occlusion = occlusion1 / 6.0;
		#else
			occlusion1 = 1.0 - coneTrace(relativPosition_FS.xyz, coneDirections[CONENUMBER], radius, 15.5);
			final_occlusion = occlusion1;
		#endif
	#endif

	
	//take the inverse, because 1.0 means fully occluded and 0.0 means no occlusion
	//final_occlusion = 1.0 - final_occlusion;
	if(!gl_FrontFacing)
	{
	    normal_eye = -normal_eye;
	}
	vec3 ambientColor = vec3(1.0, 1.0, 1.0);
	vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
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
	//vec3 color = 0.15 * testColor;
		vec3 color;
		//color +=  diffuseColor * brightness;
		color += ambientColor * final_occlusion;
	#ifdef DRAWSHADOWS
		brightness = 1.0 - rayTrace_shadow(relativPosition_FS, lightDirection, lightDistance);
		color *= brightness;
	#endif
	//color += 0.1 * spe * vec3(1.0);
 //   color += 0.1 * rim * vec3(1.0);

	#ifdef WIREFRAME
	    float d = min(In.distance.x, min(In.distance.y, In.distance.z));
		float i = exp2(-0.75 * d * d);
	    color = mix(color, color_wireframe, i);
	#endif

    // Gamma correction. Assuming gamma of 2.0 rather than 2.2.
//	frag_color = vec4(1.0 * final_occlusion, 0.0, 0.0, 1.0);
//frag_color = vec4( 0.0, 0.0, final_occlusion, 1.0);
   frag_color = vec4(color, 1.0);
  // frag_color = vec4( material.color, 1.0);
}
