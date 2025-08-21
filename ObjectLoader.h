#pragma once
#include "ModelLoader.h"
#include "MaterialLoader.cpp"
#include "Obj_Prefix.h"

struct Vertex;
struct Face;
struct Normal;
struct Texture;

class ObjLoader : public ModelLoader
{
public:
    void load(const std::string &path) override;
    // void loadMaterial(const std::string &path) override;
    // void parseVertex(const std::string &line);
    // void parseNormal(const std::string &line);
    // void parseTexture(const std::string &line);
    // void parseGroup(const std::string &line);
    // void parseObject(const std::string &line);
    // void parseSmoothing(const std::string &line);
    // std::shared_ptr<Face> parseFace(const std::string &line);
    template<typename T>
    const std::optional<T> parseElement(const std::string &line);
    template<typename T>
    void storeElement(const std::optional<T> &element);
};