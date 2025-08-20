#pragma once
#include <array>

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

struct ParameterSpaceVertex
{
    float x,y,z;
};

struct Face
{
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<Texture> textures;
};

struct Point
{
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;
};

struct Line
{
    std::vector<Vertex> vertices;
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

struct Curve
{
    std::string type;
    int degree = 3;
    int vertexCount;
    std::vector<Vertex> controlPoints;
    std::vector<float> parameters;
    std::array<float, 2> globalParameterRange;
    bool hasParameters = false;
    // std::string interpMethod;
    // bool hasInterpMethod = false;
};


struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<std::shared_ptr<Face>> faces;
    std::vector<Normal> normals;
    std::vector<Texture> textures;
    std::vector<ParameterSpaceVertex> psvs;
    std::vector<std::shared_ptr<Point>> points;
    std::vector<std::shared_ptr<Line>> lines;
    std::vector<std::shared_ptr<Curve>> curves;
    std::vector<Group> groups;
    std::vector<Object> objects;
    std::vector<Smoothing> smooths;
    bool c_interp = false;
    bool d_interp = false;
};