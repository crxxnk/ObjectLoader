#include "ObjectLoader.h"
#include "ObjectLoader.cpp"
#include <memory>

int main()
{
    std::unique_ptr<ModelLoader> loader;
    loader = std::make_unique<ObjLoader>();
    try {
        loader->load("cube.obj");
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    // for (auto &v : loader->getVertices())
    //     std::cout << v.x << " " << v.y << " " << v.z << std::endl;

    // for (auto &v : loader->getTextures())
    //     std::cout << v.u << " " << v.v << std::endl;

    // for (auto &v : loader->getNormals())
    //     std::cout << v.x << " " << v.y << " " << v.z << std::endl;

    for (const auto& f : loader->getFaces()) {
        std::cout << "Face:" << std::endl;
        for (const auto& v : f.vertices)
            std::cout << "  Vertex: " << v.x << " " << v.y << " " << v.z << std::endl;
        for (const auto& t : f.textures)
            std::cout << "  Texture: " << t.u << " " << t.v << std::endl;
        for (const auto& n : f.normals)
            std::cout << "  Normal: " << n.x << " " << n.y << " " << n.z << std::endl;
    }

    return 0;
}