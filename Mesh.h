#pragma once
#include <vector>

struct Vertex
{
    float x,y,z;
};

struct Normal
{
    float x,y,z;
};

struct Texture
{
    float u,v;
};

struct Face
{
    Vertex vertex;
    Texture texture;
    Normal normal;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<Normal> normals;
    std::vector<Texture> textures;
};
