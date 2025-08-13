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

    for (const auto& g : loader->getGroups()) {
        std::cout << "Group " << g.name << ": "<< std::endl;
        for(size_t index : g.faceIndices) {
            const Face& f = loader->getFaces()[index];
            for (const auto& v : f.vertices)
                std::cout << "  Vertex: " << v.x << " " << v.y << " " << v.z << std::endl;
            for (const auto& t : f.textures)
                std::cout << "  Texture: " << t.u << " " << t.v << std::endl;
            for (const auto& n : f.normals)
                std::cout << "  Normal: " << n.x << " " << n.y << " " << n.z << std::endl;
        }
    }

    return 0;
}