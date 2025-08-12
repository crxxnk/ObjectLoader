#include "ObjectLoader.h"
#include "ObjectLoader.cpp"
#include <memory>

int main()
{
    std::unique_ptr<ModelLoader> loader;
    loader = std::make_unique<ObjLoader>();
    loader->load("cube.obj");
    for (auto &v : loader->getVertices())
        std::cout << v.x << " " << v.y << " " << v.z << std::endl;

    for (auto &v : loader->getTextures())
        std::cout << v.u << " " << v.v << std::endl;

    for (auto &v : loader->getNormals())
        std::cout << v.x << " " << v.y << " " << v.z << std::endl;

    return 0;
}