#version 430

#ifdef COMPUTE_SHADER

uniform int val;

struct DrawElementsIndirectCommand {
    uint  count;
    uint  instanceCount;
    uint  firstIndex;
    uint  baseVertex;
    uint  baseInstance;
};

struct AABB 
{
	vec3 pmin;
	vec3 pmax;
};

// readonly, writeonly, ou rien pour les deux
layout(std430, binding = 0) writeonly buffer InDrawCommandsBlock
{
	DrawElementsIndirectCommand a[];
};

layout(std430, binding = 1) readonly buffer OutDrawCommandsBlock
{
	DrawElementsIndirectCommand b[];
};

layout( std430, binding= 2) buffer countData
{
	float count;
};

layout(std430, binding = 3) readonly buffer Regions
{
	AABB c[];
};

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
void main()
{
	const uint ID = gl_GlobalInvocationID.x;
	if(ID < a.length())
	{
		count = c[ID].pmin.x;
	}

}

#endif