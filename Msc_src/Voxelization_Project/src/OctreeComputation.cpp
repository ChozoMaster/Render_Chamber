#include "OctreeComputation.h"
#include <math.h> 
#include "parameters.h"

extern unsigned char const octreeAlocator_cs_glsl[];
extern unsigned char const octreeClearer_cs_glsl[];
extern unsigned char const octreeFlagger_cs_glsl[];

extern unsigned char const neighborAllocator_cs_glsl[];
extern unsigned char const writeLifeValues_cs_glsl[];
extern unsigned char const borderTransfer_cs_glsl[];

extern unsigned char const mipmapCenter_cs_glsl[];
extern unsigned char const mipmapEdges_cs_glsl[];
extern unsigned char const mipmapFaces_cs_glsl[];
extern unsigned char const mipmapCorners_cs_glsl[];

OctreeComputation::OctreeComputation()
:program_CS_Oct_flag(octreeFlagger_cs_glsl),
program_CS_Oct_Aloc(octreeAlocator_cs_glsl),
program_CS_Oct_Init(octreeClearer_cs_glsl),
program_CS_neighbor_Aloc(neighborAllocator_cs_glsl),
program_CS_writeLeafValues(writeLifeValues_cs_glsl),
program_CS_borderTransfer(borderTransfer_cs_glsl),
program_CS_mipmapCenter(mipmapCenter_cs_glsl),
program_CS_mipmapFaces(mipmapFaces_cs_glsl),
program_CS_mipmapCorners(mipmapCorners_cs_glsl),
program_CS_mipmapEdges(mipmapEdges_cs_glsl)
{
}
void OctreeComputation::initializeVariables(GLsizei numVoxelFrag, GLviz::glImageBufferUniform* flb, GLviz::glImageBufferUniform* npb, GLviz::glImageBufferUniform* npn, GLviz::glImage3DUniform* bi, GLviz::glAtomicCounter* counter){
	this->numVoxelFrag = numVoxelFrag;
	this->fragmentListBuffer = flb;
	this->nodePoolBuffer = npb;
	this->nodePoolNeighbors = npn;
	this->brickImage = bi;
	this->flaggedNodeCounter = counter;
	voxelResolution = pow(2, Parameter::getInstance()->OctreeLevel);
	//int numVoxelFrag = voxelize_fragmentList(voxelResolution, nf);
	//--------------------------Start Octree Computation------------------------
	maxOctreeLevel = Parameter::getInstance()->OctreeLevel;
	//Calculate the worst case memory requirements for the Octree (All cells are filled)
	//According to wolframalpha x^1+x^2+x^3+...+x^n + 1 is this
	maxNodes = (1.0 / 7.0) * (8 - pow(8, -(maxOctreeLevel))) *numVoxelFrag;
	maxNodes *= 8;
	//	nodePoolLength = maxNodes;
	//Calculate the group Dimensions for the compute dispatch
	dataWidth = 1024;
	dataHeight = (numVoxelFrag + 1023) / dataWidth;
	//Local Worksize is 8
	groupDimX = dataWidth / 8;
	groupDimY = (dataHeight + 7) / 8;
	//Initialization values
	//Allocate 1 (root node)
	
}

void OctreeComputation::initializeNodePoolBuffer(){

	//And intitialize the Buffer
	nodePoolBuffer->initialize(3, sizeof(GLuint)* maxNodes, GL_R32UI);
}

void OctreeComputation::initializeNeighborBuffer(){

	nodePoolNeighbors->initialize(3, sizeof(GLuint)*  brickSize  * 3, GL_R32UI);
}

void OctreeComputation::initializeBrickImageBuffer(int& brickSizeOut, int& brickDimensionOut){
	brickSize = std::ceil(std::pow(((float)brickSize), 1.0 / 3.0)) * 3;
	brickSizeOut = brickSize;
	//brickImage.initialize(Eigen::Vector3i(brickSize, 3, 3), 1, Parameter::getInstance()->texparams.internalFormat, Parameter::getInstance()->texparams.format, Parameter::getInstance()->texparams.type);
	brickImage->initialize(Eigen::Vector3i(brickSize, brickSize, brickSize), 1, Parameter::getInstance()->texparams.internalFormat, Parameter::getInstance()->texparams.format, Parameter::getInstance()->texparams.type);
	//
	brickDimension = brickSize / 3.0;
	brickDimensionOut = brickDimension;
}
void OctreeComputation::createStructure(){
	int numberOfNodesToAllocate = 1;
	//Adressoffset 0, first node to save
	int addressOffset = 0;
	//The offset of the next free addresses to save children
	int freeSpaceOffset = 1;

	int maxNumberOfNodes = 1;

	int initGroupDimX, initGroupDimY, initGroupDimZ, dataDepth;

	for (int i = 0; i < maxOctreeLevel; i++){
		levelOffsets[i] = addressOffset;
		nodesOnLevel[i] = numberOfNodesToAllocate;
		//std::cout << "numberOfNodesToAllocate " << numberOfNodesToAllocate << "\n";
		//----------------------------Step 1, Cell Flag------------------------------ 
		program_CS_Oct_flag.use();
		nodePoolBuffer->bind();
		//set Parameter
		glActiveTexture(GL_TEXTURE5);
		nodePoolBuffer->bind();

		program_CS_Oct_flag.setParameter(shaderAttrib::i1, (void*)&voxelResolution, "voxelGridResolution");
		program_CS_Oct_flag.setParameter(shaderAttrib::i1, (void*)&i, "currentLevel");
		program_CS_Oct_flag.setParameter(shaderAttrib::i1, (void*)&numVoxelFrag, "numberOfVoxelFragments");
		//DELETE LATER
		program_CS_Oct_flag.setParameter(shaderAttrib::i1, (void*)&addressOffset, "startAddress");
		glBindImageTexture(2, fragmentListBuffer->tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI);
		glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		//start computationParameter::getInstance()->

		glDispatchCompute(groupDimX, groupDimY, 1);

		//sync
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		program_CS_Oct_flag.unuse();

		//----------------------------Step 2, Cell Allocation------------------------------
		//Because local worksize of 64
		dataHeight = (numberOfNodesToAllocate + 1023) / dataWidth;
		initGroupDimX = dataWidth / 8;
		initGroupDimY = (dataHeight + 7) / 8;

		//	int workGroupDimension = numberOfNodesToAllocate + 63 / 64;
		program_CS_Oct_Aloc.use();
		flaggedNodeCounter->bind();
		//set Parameter, reset Counter
		flaggedNodeCounter->reset();
		GLuint count1 = flaggedNodeCounter->getCount();
		program_CS_Oct_Aloc.setParameter(shaderAttrib::i1, (void*)&numberOfNodesToAllocate, "numberOFThreads");
		program_CS_Oct_Aloc.setParameter(shaderAttrib::i1, (void*)&addressOffset, "nodeAddress");
		program_CS_Oct_Aloc.setParameter(shaderAttrib::i1, (void*)&freeSpaceOffset, "freeSpaceStartAddress");
		glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		//Start Computation

		glDispatchCompute(initGroupDimX, initGroupDimY, 1);

		//	glDispatchCompute(workGroupDimension, 1, 1);

		//sync
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		program_CS_Oct_Aloc.unuse();
		//Read number of voxel allocated voxel fragments to calculate the number of nodes to initialize
		GLuint count = flaggedNodeCounter->getCount();
		//Nodes to initialize = Allocated node x8, because OCtree
		unsigned int nodesToInitialize = count * 8;

		//----------------------------Step 3, Cell Initialization------------------------------
		program_CS_Oct_Init.use();
		//Set Parameter
		program_CS_Oct_Init.setParameter(shaderAttrib::i1, (void*)&nodesToInitialize, "numberOFThreads");
		program_CS_Oct_Init.setParameter(shaderAttrib::i1, (void*)&freeSpaceOffset, "freeSpaceStartAddress");
		glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		//calculate Work sizes, local worksize 8
		dataWidth = 1024;
		dataHeight = (nodesToInitialize + 1023) / dataWidth;
		initGroupDimX = dataWidth / 8;
		initGroupDimY = (dataHeight + 7) / 8;
		//Start computation
		//timer.startTimer();
		glDispatchCompute(initGroupDimX, initGroupDimY, 1);
		//timer.stopTimer();
		//sync
	//	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		program_CS_Oct_Init.unuse();

		//Set Parameter for next iteration
		//All initialized children will be traversed in the next steps and tested if they are filled or not
		numberOfNodesToAllocate = nodesToInitialize;
		//The next address offset is the previously free space offset, because we just allocated the new children there
		addressOffset = freeSpaceOffset;
		//The free space offset simply moves by the number of allocations in the last step
		freeSpaceOffset += numberOfNodesToAllocate;
		maxNumberOfNodes *= 8;
		flaggedNodeCounter->unbind();
	}


	levelOffsets[maxOctreeLevel] = addressOffset;
	nodesOnLevel[maxOctreeLevel] = numberOfNodesToAllocate;
	int nodeSums = addressOffset + numberOfNodesToAllocate;
	brickSize = nodeSums;
	float sparsity = ((float)numberOfNodesToAllocate) / ((float)maxNumberOfNodes);
	//----------------------------Tag BRICKS------------------------------
	program_CS_Oct_flag.use();
	nodePoolBuffer->bind();
	//set Parameter
	glActiveTexture(GL_TEXTURE5);
	nodePoolBuffer->bind();

	program_CS_Oct_flag.setParameter(shaderAttrib::i1, (void*)&voxelResolution, "voxelGridResolution");
	program_CS_Oct_flag.setParameter(shaderAttrib::i1, (void*)&maxOctreeLevel, "currentLevel");
	program_CS_Oct_flag.setParameter(shaderAttrib::i1, (void*)&numVoxelFrag, "numberOfVoxelFragments");
	//DELETE LATER
	program_CS_Oct_flag.setParameter(shaderAttrib::i1, (void*)&addressOffset, "startAddress");
	glBindImageTexture(2, fragmentListBuffer->tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGB10_A2UI);
	glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	//start computation

	glDispatchCompute(groupDimX, groupDimY, 1);
	//sync
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	program_CS_Oct_flag.unuse();
}

void OctreeComputation::computeNeighbors()
{
	int initGroupDimX, initGroupDimY;
	int maxNumberOfNodes = 1;

	for (int i = 0; i < maxOctreeLevel; i++){
		program_CS_neighbor_Aloc.use();
		program_CS_neighbor_Aloc.setParameter(shaderAttrib::i1, (void*)&i, "currentLevel");
		program_CS_neighbor_Aloc.setParameter(shaderAttrib::i1, (void*)&maxNumberOfNodes, "numberOfNodes");
		//glBindImageTexture(4, nodePoolBuffer.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glBindImageTexture(3, nodePoolNeighbors->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
		glActiveTexture(GL_TEXTURE5);
		nodePoolBuffer->bind();

		dataHeight = (maxNumberOfNodes + 1023) / dataWidth;
		initGroupDimX = dataWidth / 8;
		initGroupDimY = (dataHeight + 7) / 8;
		glDispatchCompute(initGroupDimX, initGroupDimY, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//	glDispatchCompute(initGroupDimX, initGroupDimY, initGroupDimZ);
		program_CS_neighbor_Aloc.unuse();
		maxNumberOfNodes *= 8;
	}
}

void OctreeComputation::computeLastLevelNeighbors()
{
	int maxNumberOfNodes = pow(8, maxOctreeLevel);
	program_CS_neighbor_Aloc.use();

	program_CS_neighbor_Aloc.setParameter(shaderAttrib::i1, (void*)&maxOctreeLevel, "currentLevel");
	program_CS_neighbor_Aloc.setParameter(shaderAttrib::i1, (void*)&maxNumberOfNodes, "numberOfNodes");
	//glBindImageTexture(4, nodePoolBuffer.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(3, nodePoolNeighbors->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glActiveTexture(GL_TEXTURE5);
	nodePoolBuffer->bind();

	dataHeight = (maxNumberOfNodes + 1023) / dataWidth;
	int initGroupDimX = dataWidth / 8;
	int initGroupDimY = (dataHeight + 7) / 8;
	glDispatchCompute(initGroupDimX, initGroupDimY, 1);
	program_CS_neighbor_Aloc.unuse();
}

void OctreeComputation::initializeLeaves()
{
	program_CS_writeLeafValues.use();
	brickImage->bind();
	//Bind Images for computation
	program_CS_writeLeafValues.setParameter(shaderAttrib::i1, (void*)&nodesOnLevel[maxOctreeLevel], "numberOfVoxels");
	program_CS_writeLeafValues.setParameter(shaderAttrib::i1, (void*)&levelOffsets[maxOctreeLevel], "leafOffset");
	program_CS_writeLeafValues.setParameter(shaderAttrib::i1, (void*)&brickDimension, "brickDimension");
	glBindImageTexture(8, brickImage->tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	//Calculate workitem Sizes
	dataHeight = (nodesOnLevel[maxOctreeLevel] + 1023) / dataWidth;
	int initGroupDimX = dataWidth / 8;
	int initGroupDimY = (dataHeight + 7) / 8;

	glDispatchCompute(initGroupDimX, initGroupDimY, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	program_CS_writeLeafValues.unuse();

	//----------------------------initial border transfer------------------------------
	program_CS_borderTransfer.use();
	unsigned int axis = 0U;

	program_CS_borderTransfer.setParameter(shaderAttrib::i1, (void*)&nodesOnLevel[maxOctreeLevel], "numberOfNodes");
	program_CS_borderTransfer.setParameter(shaderAttrib::i1, (void*)&levelOffsets[maxOctreeLevel], "leafOffset");
	program_CS_borderTransfer.setParameter(shaderAttrib::ui1, (void*)&axis, "axis");
	program_CS_borderTransfer.setParameter(shaderAttrib::i1, (void*)&brickDimension, "brickDimension");
	glBindImageTexture(3, nodePoolNeighbors->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(8, brickImage->tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);

	//Calculate workitem Sizes
	dataHeight = (nodesOnLevel[maxOctreeLevel] + 1023) / dataWidth;
	initGroupDimX = dataWidth / 8;
	initGroupDimY = (dataHeight + 7) / 8;

	glDispatchCompute(initGroupDimX, initGroupDimY, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	axis = 1U;
	program_CS_borderTransfer.setParameter(shaderAttrib::ui1, (void*)&axis, "axis");
	glDispatchCompute(initGroupDimX, initGroupDimY, 1);
	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

	axis = 2U;
	program_CS_borderTransfer.setParameter(shaderAttrib::ui1, (void*)&axis, "axis");
	glDispatchCompute(initGroupDimX, initGroupDimY, 1);
	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

	program_CS_borderTransfer.unuse();
}


void OctreeComputation::mipMapInteriorNodes(){
for (int level = maxOctreeLevel - 1; level >= 0; --level){
	//Calculate workitem Sizes
	dataHeight = (nodesOnLevel[level] + 1023) / dataWidth;
	int initGroupDimX = dataWidth / 8;
	int initGroupDimY = (dataHeight + 7) / 8;

	//----------------------------MIPMAP CENTER------------------------------
	program_CS_mipmapCenter.use();

	program_CS_mipmapCenter.setParameter(shaderAttrib::i1, (void*)&nodesOnLevel[level], "numberOfNodes");
	program_CS_mipmapCenter.setParameter(shaderAttrib::i1, (void*)&levelOffsets[level], "leafOffset");
	program_CS_mipmapCenter.setParameter(shaderAttrib::i1, (void*)&brickDimension, "brickDimension");
	glBindImageTexture(3, nodePoolNeighbors->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	//glBindImageTexture(5, nodePool_attribute.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(7, brickImage->tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
	//	glBindImageTexture(6, brickpool_occlusion.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

	glDispatchCompute(initGroupDimX, initGroupDimY, 1);

	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

	program_CS_mipmapCenter.unuse();


	//----------------------------MIPMAP CORNERS------------------------------
	program_CS_mipmapCorners.use();
	program_CS_mipmapCorners.setParameter(shaderAttrib::i1, (void*)&nodesOnLevel[level], "numberOfNodes");
	program_CS_mipmapCorners.setParameter(shaderAttrib::i1, (void*)&levelOffsets[level], "leafOffset");
	program_CS_mipmapCorners.setParameter(shaderAttrib::i1, (void*)&brickDimension, "brickDimension");
	glBindImageTexture(3, nodePoolNeighbors->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	//glBindImageTexture(5, nodePool_attribute.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(7, brickImage->tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
	//glBindImageTexture(6, brickpool_occlusion.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glDispatchCompute(initGroupDimX, initGroupDimY, 1);

	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
	program_CS_mipmapCorners.unuse();

	//----------------------------MIPMAP EDGES------------------------------
	program_CS_mipmapEdges.use();
	program_CS_mipmapEdges.setParameter(shaderAttrib::i1, (void*)&nodesOnLevel[level], "numberOfNodes");
	program_CS_mipmapEdges.setParameter(shaderAttrib::i1, (void*)&levelOffsets[level], "leafOffset");
	program_CS_mipmapEdges.setParameter(shaderAttrib::i1, (void*)&brickDimension, "brickDimension");
	glBindImageTexture(3, nodePoolNeighbors->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	//glBindImageTexture(5, nodePool_attribute.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(7, brickImage->tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
	//glBindImageTexture(6, brickpool_occlusion.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glDispatchCompute(initGroupDimX, initGroupDimY, 1);


	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
	program_CS_mipmapEdges.unuse();

	////----------------------------MIPMAP FACES------------------------------

	program_CS_mipmapFaces.use();
	program_CS_mipmapFaces.setParameter(shaderAttrib::i1, (void*)&nodesOnLevel[level], "numberOfNodes");
	program_CS_mipmapFaces.setParameter(shaderAttrib::i1, (void*)&levelOffsets[level], "leafOffset");
	program_CS_mipmapFaces.setParameter(shaderAttrib::i1, (void*)&brickDimension, "brickDimension");
	glBindImageTexture(3, nodePoolNeighbors->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(7, brickImage->tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
	//glBindImageTexture(5, nodePool_attribute.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	//glBindImageTexture(6, brickpool_occlusion.tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glDispatchCompute(initGroupDimX, initGroupDimY, 1);

	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
	program_CS_mipmapFaces.unuse();

	//----------------------------border transfer------------------------------

	program_CS_borderTransfer.use();

	unsigned int axis = 0U;
	program_CS_borderTransfer.setParameter(shaderAttrib::i1, (void*)&nodesOnLevel[level], "numberOfNodes");
	program_CS_borderTransfer.setParameter(shaderAttrib::i1, (void*)&levelOffsets[level], "leafOffset");
	program_CS_borderTransfer.setParameter(shaderAttrib::ui1, (void*)&axis, "axis");
	program_CS_borderTransfer.setParameter(shaderAttrib::i1, (void*)&brickDimension, "brickDimension");
	glBindImageTexture(3, nodePoolNeighbors->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(4, nodePoolBuffer->tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindImageTexture(8, brickImage->tex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
	//	timer.startTimer();
	glDispatchCompute(initGroupDimX, initGroupDimY, 1);
	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
	axis = 1U;
	program_CS_borderTransfer.setParameter(shaderAttrib::ui1, (void*)&axis, "axis");
	glDispatchCompute(initGroupDimX, initGroupDimY, 1);
	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

	axis = 2U;
	program_CS_borderTransfer.setParameter(shaderAttrib::ui1, (void*)&axis, "axis");
	glDispatchCompute(initGroupDimX, initGroupDimY, 1);
	////	timer.stopTimer();
	//	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
	program_CS_borderTransfer.unuse();
}
//	timer.accumulateTime();
//	timer.getAverageElapsedTime();

glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

/*GLuint tmp[1];
glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), tmp);
std::cout << ": " << tmp[0] << "\n";*/



/*	GLviz::resetViewPort();
glEnable(GL_DEPTH_TEST);
glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);*/



//	nodePoolBuffer.unbind();
flaggedNodeCounter->unbind();
glDisable(GL_CULL_FACE);

}
