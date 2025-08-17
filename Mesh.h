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
    std::vector<std::shared_ptr<Face>> faces;
};

struct Object
{
    std::string name;
    std::vector<std::shared_ptr<Face>> faces;
    std::vector<Group> groups;
};

struct Smoothing
{
    int smoothness;
    std::vector<std::shared_ptr<Face>> faces;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<std::shared_ptr<Face>> faces;
    std::vector<Normal> normals;
    std::vector<Texture> textures;
    std::vector<Group> groups;
    std::vector<Object> objects;
    std::vector<Smoothing> smooths;
};