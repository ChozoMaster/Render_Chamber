#version 420
#define L     0.7071067811865475244008443621048490392848359376884740	//sqrt(2)/2

flat out uint dominantAxisCase;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int voxel_Res;




in vec3 vs_color[];


//Voxel Texture
	#ifdef FORMAT_R32F
	    layout(binding = 7, r32f) uniform  image3D voxelImage;
	#endif

	#ifdef FORMAT_R8
	    layout(binding = 7, r8) uniform  image3D voxelImage;
	#endif
	
void computeBaryCoords(inout vec3 b0, inout vec3 b1, inout vec3 b2, vec2 v0, vec2 v1, vec2 v2)
{
	vec3 vb;
	float invDetA = 1.0 / ((v0.x*v1.y) - (v0.x*v2.y) - (v1.x*v0.y) + (v1.x*v2.y) + (v2.x*v0.y) - (v2.x*v1.y));

	vb.x = ( (b0.x*v1.y) - (b0.x*v2.y) - (v1.x*b0.y) + (v1.x*v2.y) + (v2.x*b0.y) - (v2.x*v1.y) ) * invDetA;
	vb.y = ( (v0.x*b0.y) - (v0.x*v2.y) - (b0.x*v0.y) + (b0.x*v2.y) + (v2.x*v0.y) - (v2.x*b0.y) ) * invDetA;
	vb.z = 1 - vb.x - vb.y;
	b0 = vb;

	vb.x = ( (b1.x*v1.y) - (b1.x*v2.y) - (v1.x*b1.y) + (v1.x*v2.y) + (v2.x*b1.y) - (v2.x*v1.y) ) * invDetA;
	vb.y = ( (v0.x*b1.y) - (v0.x*v2.y) - (b1.x*v0.y) + (b1.x*v2.y) + (v2.x*v0.y) - (v2.x*b1.y) ) * invDetA;
	vb.z = 1 - vb.x - vb.y;
	b1 = vb;

	vb.x = ( (b2.x*v1.y) - (b2.x*v2.y) - (v1.x*b2.y) + (v1.x*v2.y) + (v2.x*b2.y) - (v2.x*v1.y) ) * invDetA;
	vb.y = ( (v0.x*b2.y) - (v0.x*v2.y) - (b2.x*v0.y) + (b2.x*v2.y) + (v2.x*v0.y) - (v2.x*b2.y) ) * invDetA;
	vb.z = 1 - vb.x - vb.y;
	b2 = vb;
}


void swizzle(inout vec3 v0, inout vec3 v1, inout vec3 v2, inout vec3 normal)
{

float maxVal = max(abs(normal.x) , max(abs(normal.y) , abs(normal.z)));
		//Case 1 x is dominant axis
		//z --> x
		//y --> y
		//x --> z

		//find 3 triangle edge plane

		if(maxVal == abs(normal.x)){
		dominantAxisCase = 0;
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
		dominantAxisCase = 1;
		v0 =  v0.zxy; 
		v1 =  v1.zxy;                            
		v2 =  v2.zxy;
		normal = normal.zxy;
		}
		
		else if(maxVal == abs(normal.z))
		{
		//Case 3 z is dominant axis
		dominantAxisCase = 2;
		}

}

void main()
{
		//Transform world coordinates in voxel coordinates
		vec3 v0 = ((gl_in[0].gl_Position.xyz + 1.0)/2.0) * voxel_Res;
		vec3 v1 = ((gl_in[1].gl_Position.xyz + 1.0)/2.0) * voxel_Res;
		vec3 v2 = ((gl_in[2].gl_Position.xyz + 1.0)/2.0) * voxel_Res;

		vec3 normal = normalize(cross(v1 - v0, v2 - v1));
		swizzle(v0, v1, v2, normal);
		
	vec3 AABBmin = min(min(v0, v1), v2);
	vec3 AABBmax = max(max(v0, v1), v2);
	
	ivec3 minVoxIndex = ivec3(clamp(floor(AABBmin), ivec3(0), ivec3(voxel_Res)));
	ivec3 maxVoxIndex = ivec3(clamp( ceil(AABBmax), ivec3(0), ivec3(voxel_Res)));
	//Edges for swizzled vertices;
	vec3 e0 = v1 - v0;	//figure 17/18 line 2
	vec3 e1 = v2 - v1;	//figure 17/18 line 2
	vec3 e2 = v0 - v2;	//figure 17/18 line 2

	vec2 n_e0_xy,n_e1_xy, n_e2_xy;
	vec2 n_e0_yz, n_e1_yz, n_e2_yz ;
	vec2 n_e0_zx, n_e1_zx, n_e2_zx ;
	
	
	if(normal.z >= 0){
	//INward Facing edge normals XY
	n_e0_xy = normalize(vec2(-e0.y, e0.x)) ; 
	n_e1_xy = normalize(vec2(-e1.y, e1.x)) ; 
	n_e2_xy = normalize(vec2(-e2.y, e2.x)) ; 
	}
	else{
	n_e0_xy = normalize(vec2(e0.y, -e0.x)) ; 
	n_e1_xy = normalize(vec2(e1.y, -e1.x)) ; 
	n_e2_xy = normalize(vec2(e2.y, -e2.x)) ; 
	}
	if(normal.x >= 0){
	//INward Facing edge normals YZ
	n_e0_yz = normalize(vec2(-e0.z, e0.y))  ;
	n_e1_yz = normalize(vec2(-e1.z, e1.y))  ;
	n_e2_yz = normalize(vec2(-e2.z, e2.y))  ;
	}
	else{
	n_e0_yz = normalize(vec2(e0.z, -e0.y)) ;
	n_e1_yz = normalize(vec2(e1.z, -e1.y)) ;
	n_e2_yz = normalize(vec2(e2.z, -e2.y)) ;
	}
	if(normal.y >= 0){
	//INward Facing edge normals ZX
	n_e0_zx = normalize(vec2(-e0.x, e0.z)) ;
	n_e1_zx = normalize(vec2(-e1.x, e1.z)) ;
	n_e2_zx = normalize(vec2(-e2.x, e2.z)) ;
	}
	else{
	n_e0_zx = normalize(vec2(e0.x, -e0.z));
	n_e1_zx = normalize(vec2(e1.x, -e1.z));
	n_e2_zx = normalize(vec2(e2.x, -e2.z));
	}
	
	
	float d_e0_xy = -dot(n_e0_xy, v0.xy) + max(0.0f, n_e0_xy.x) + max(0.0f, n_e0_xy.y);	//figure 17 line 7
	float d_e1_xy = -dot(n_e1_xy, v1.xy) + max(0.0f, n_e1_xy.x) + max(0.0f, n_e1_xy.y);	//figure 17 line 7
	float d_e2_xy = -dot(n_e2_xy, v2.xy) + max(0.0f, n_e2_xy.x) + max(0.0f, n_e2_xy.y);	//figure 17 line 7
	      
	float d_e0_yz = -dot(n_e0_yz, v0.yz) + max(0.0f, n_e0_yz.x) + max(0.0f, n_e0_yz.y);	//figure 17 line 8
	float d_e1_yz = -dot(n_e1_yz, v1.yz) + max(0.0f, n_e1_yz.x) + max(0.0f, n_e1_yz.y);	//figure 17 line 8
	float d_e2_yz = -dot(n_e2_yz, v2.yz) + max(0.0f, n_e2_yz.x) + max(0.0f, n_e2_yz.y);	//figure 17 line 8
	      
	float d_e0_zx = -dot(n_e0_zx, v0.zx) + max(0.0f, n_e0_zx.x) + max(0.0f, n_e0_zx.y);	//figure 18 line 9
	float d_e1_zx = -dot(n_e1_zx, v1.zx) + max(0.0f, n_e1_zx.x) + max(0.0f, n_e1_zx.y);	//figure 18 line 9
	float d_e2_zx = -dot(n_e2_zx, v2.zx) + max(0.0f, n_e2_zx.x) + max(0.0f, n_e2_zx.y);	//figure 18 line 9


	vec3 nProj = (normal.z < 0.0) ? -normal : normal;	//figure 17/18 line 10

	float dTri = dot(nProj, v0);

	float dTriFatMin = dTri - max(nProj.x, 0) - max(nProj.y, 0);	//figure 17 line 11
	float dTriFatMax = dTri - min(nProj.x, 0) - min(nProj.y, 0);	//figure 17 line 12

	float nzInv = 1.0 / nProj.z;

	ivec3 p;					//voxel coordinate
	int   zMin,      zMax;		//voxel Z-range
	float zMinInt,   zMaxInt;	//voxel Z-intersection min/max
	float zMinFloor, zMaxCeil;	//voxel Z-intersection floor/ceil
	
	for(p.x = minVoxIndex.x; p.x < maxVoxIndex.x; p.x++)	//figure 17 line 13, figure 18 line 12
	{
		for(p.y = minVoxIndex.y; p.y < maxVoxIndex.y; p.y++)	//figure 17 line 14, figure 18 line 13
		{
			float dd_e0_xy = d_e0_xy + dot(n_e0_xy, p.xy);
			float dd_e1_xy = d_e1_xy + dot(n_e1_xy, p.xy);
			float dd_e2_xy = d_e2_xy + dot(n_e2_xy, p.xy);
		
			bool xy_overlap = (dd_e0_xy >= 0) && (dd_e1_xy >= 0) && (dd_e2_xy >= 0);

			if(xy_overlap)	//figure 17 line 15, figure 18 line 14
			{
				float dot_n_p = dot(nProj.xy, p.xy);

				zMinInt = (-dot_n_p + dTriFatMin) * nzInv;
				zMaxInt = (-dot_n_p + dTriFatMax) * nzInv;

				zMinFloor = floor(zMinInt);
				zMaxCeil  =  ceil(zMaxInt);

				zMin = int(zMinFloor) - int(zMinFloor == zMinInt);
				zMax = int(zMaxCeil ) + int(zMaxCeil  == zMaxInt);

				zMin = max(minVoxIndex.z, zMin);	//clamp to bounding box max Z
				zMax = min(maxVoxIndex.z, zMax);	//clamp to bounding box min Z

				for(p.z = zMin; p.z < zMax; p.z++)	//figure 17/18 line 18
				{
					float dd_e0_yz = d_e0_yz + dot(n_e0_yz, p.yz);
					float dd_e1_yz = d_e1_yz + dot(n_e1_yz, p.yz);
					float dd_e2_yz = d_e2_yz + dot(n_e2_yz, p.yz);

					float dd_e0_zx = d_e0_zx + dot(n_e0_zx, p.zx);
					float dd_e1_zx = d_e1_zx + dot(n_e1_zx, p.zx);
					float dd_e2_zx = d_e2_zx + dot(n_e2_zx, p.zx);

					bool yz_overlap = (dd_e0_yz >= 0) && (dd_e1_yz >= 0) && (dd_e2_yz >= 0);
					bool zx_overlap = (dd_e0_zx >= 0) && (dd_e1_zx >= 0) && (dd_e2_zx >= 0);

					if(yz_overlap && zx_overlap)	//figure 17/18 line 19
					{
						ivec3 origCoord = (dominantAxisCase == 0) ? p.zxy : (dominantAxisCase == 1) ? p.yzx : p.xyz;
						imageStore(voxelImage, origCoord, vec4(1,0,0,0));	//figure 17/18 line 20
					}
				}
				//z-loop
			}
			//xy-overlap test
		}
		//y-loop

	}
	
	
	
	

}
