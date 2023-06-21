#pragma once

struct Vertex2D
{
	float x, y;
	float u, v;
};

struct Vertex3D
{
	float x, y, z;
	float u, v;
	float light;
};

struct Vertex3DT : public Vertex3D
{
	float transparency;
};