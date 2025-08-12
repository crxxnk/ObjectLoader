#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "Mesh.h"

class ModelLoader
{
public:
    Mesh mesh;
    virtual ~ModelLoader() = default;
    virtual bool load(const std::string &path) = 0;
    const std::vector<Vertex> &getVertices() const { return this->mesh.vertices; }
    const std::vector<Normal> &getNormals() const { return this->mesh.normals; }
    const std::vector<Texture> &getTextures() const { return this->mesh.textures; }
    const std::vector<Face> &getFaces() const { return this->mesh.faces; }
};