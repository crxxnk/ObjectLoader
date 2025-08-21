#include "MaterialLoader.h"

void MtlLoader::load(const std::string &path)
{
    std::string line;

    if(!path.ends_with(".mtl"))
        throw std::invalid_argument("File '" + path + "' is not a Material Template Library file.");

    std::ifstream file(path);
    if(!file)
        throw std::runtime_error("Could not open Material Template Library file.");
    logger.log("Loading file: " + path);

    std::optional<Material> material;

    while(std::getline(file, line)) 
    {
        if (line.rfind(NEW_MATERIAL, 0) == 0) {
            try {
                material = parseElement<Material>(line);
                logger.log(material.value().name, logger.DEBUG);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
    }
}

template<typename T>
[[nodiscard]] const std::optional<T> MtlLoader::parseElement(const std::string &line)
{
    if constexpr (std::is_same_v<T, Material>)
    {
        Material material;
        std::stringstream ss(line);
        std::string prefix; // skip 'newmtl'
        ss >> prefix;
        if(!(ss >> material.name))
            throw std::runtime_error("Expected material name after 'newmtl'");
        logger.log("Parsing material...");
        return material;
    }
}