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

struct Group
{
    std::string name;
    std::vector<size_t> faceIndices;
};

struct Object
{
    std::string name;
    std::vector<size_t> faceIndices;
    //std::vector<Face*> faces; //! Maybe will add pointers instead of index later
    std::vector<Group> groups;
};

struct Smoothing
{
    int smoothness;
    std::vector<size_t> faceIndices;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<Normal> normals;
    std::vector<Texture> textures;
    std::vector<Group> groups;
    std::vector<Object> objects;
    std::vector<Smoothing> smooths;
};