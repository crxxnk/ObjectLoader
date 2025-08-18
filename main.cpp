#include "ObjectLoader.h"
#include "ObjectLoader.cpp"

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
        for(auto f : g.faces) {
            for (const auto& v : f->vertices)
                std::cout << "Vertex: " << v.x << " " << v.y << " " << v.z << std::endl;
            for (const auto& t : f->textures)
                std::cout << "Texture: " << t.u << " " << t.v << std::endl;
            for (const auto& n : f->normals)
                std::cout << "Normal: " << n.x << " " << n.y << " " << n.z << std::endl;
        }
    }

    // for (const auto& p : loader->getPsvs()) {
    //     std::cout << "Psv: " << p.x << " " << p.y << " " << p.z << std::endl;
    // }

    std::cout << "Points:" << std::endl;
    for (const auto& p : loader->getPoints()) {
        for (const auto &v : p->vertices)
            std::cout << "Vertex: " << v.x << " " << v.y << " " << v.z << std::endl;
        for (const auto& t : p->textures)
            std::cout << "Texture: " << t.u << " " << t.v << std::endl;
    }

    std::cout << "Lines:" << std::endl;
    for (const auto& l : loader->getLines()) {
        for (const auto &v : l->vertices)
            std::cout << "Vertex: " << v.x << " " << v.y << " " << v.z << std::endl;
        for (const auto& t : l->textures)
            std::cout << "Texture: " << t.u << " " << t.v << std::endl;
    }

    std::cout << "Curves:" << std::endl;
    for (const auto& c : loader->getCurves()) {
        for (const auto &v : c->controlPoints)
            std::cout << "Vertex: " << v.x << " " << v.y << " " << v.z << std::endl;
        std::cout << "Global parameter range from " << c->globalParameterRange.at(0) << " to " << c->globalParameterRange.at(1) << std::endl;
    }

    return 0;
}