#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>
#include <optional>
#include <concepts>
#include "Mesh.h"

class ModelLoader
{
public:
    Mesh mesh;
    virtual ~ModelLoader() = default;
    virtual void load(const std::string &path) = 0;
    const std::vector<Vertex> &getVertices() const { return this->mesh.vertices; }
    const std::vector<Normal> &getNormals() const { return this->mesh.normals; }
    const std::vector<Texture> &getTextures() const { return this->mesh.textures; }
    const std::vector<ParameterSpaceVertex> &getPsvs() const { return this->mesh.psvs; }
    const std::vector<std::shared_ptr<Point>> &getPoints() const { return this->mesh.points; }
    const std::vector<std::shared_ptr<Line>> &getLines() const { return this->mesh.lines; }
    const std::vector<std::shared_ptr<Face>> &getFaces() const { return this->mesh.faces; }
    const std::vector<std::shared_ptr<Curve>> &getCurves() const { return this->mesh.curves; }
    const std::vector<Group> &getGroups() const { return this->mesh.groups; }
    const std::vector<Object> &getObjects() const { return this->mesh.objects; }
};