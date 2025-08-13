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
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<Texture> textures;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<Normal> normals;
    std::vector<Texture> textures;
};
