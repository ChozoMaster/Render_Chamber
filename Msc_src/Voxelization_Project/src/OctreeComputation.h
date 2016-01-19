#pragma once
#include <GLviz>


class OctreeComputation{
protected:
private:
	
public:
	GLviz::ProgramComputeShader program_CS_Oct_Init, program_CS_Oct_flag, program_CS_Oct_Aloc, program_CS_neighbor_Aloc, program_CS_writeLeafValues, program_CS_borderTransfer,
		program_CS_mipmapCenter, program_CS_mipmapCorners, program_CS_mipmapEdges, program_CS_mipmapFaces;
	GLviz::glAtomicCounter* flaggedNodeCounter;

	int voxelResolution, maxOctreeLevel, maxNodes, dataWidth, dataHeight, groupDimX, groupDimY, numVoxelFrag, brickSize, brickDimension;
	GLviz::glImageBufferUniform* fragmentListBuffer, *nodePoolBuffer, *nodePoolNeighbors;
	GLviz::glImage3DUniform* brickImage;

	int levelOffsets[11];
	int nodesOnLevel[11];

//	OctreeComputation();
	OctreeComputation();
	void createStructure();
	void initializeVariables(GLsizei numberOfVoxelFragments, GLviz::glImageBufferUniform* flb, GLviz::glImageBufferUniform* npb, GLviz::glImageBufferUniform* npn, GLviz::glImage3DUniform* bi, GLviz::glAtomicCounter* flaggedNodeCounter);
	void computeNeighbors();
	void computeLastLevelNeighbors();
	void initializeLeaves();
	void mipMapInteriorNodes();
	void initializeNodePoolBuffer();
	void initializeNeighborBuffer();
	void initializeBrickImageBuffer(int& brickSizeOut, int& brickDimensionOut);
};