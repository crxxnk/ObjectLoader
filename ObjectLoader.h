#pragma once
#include "ModelLoader.h"

struct Vertex;
struct Face;
struct Normal;
struct Texture;

class ObjLoader : public ModelLoader
{
public:
    bool load(const std::string &path) override;
    void parseVertex(const std::string &line);
    void parseFace(const std::string &line);
    void parseNormal(const std::string &line);
    void parseTexture(const std::string &line);
};