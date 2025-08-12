#include "ObjectLoader.h"

constexpr char VERTEX_PREFIX = 'v';
constexpr char NORMAL_PREFIX = 'n';
constexpr char TEXTURE_PREFIX = 't';
constexpr char FACE_PREFIX = 'f';
constexpr char GROUP_PREFIX = 'g';
constexpr char OBJECT_PREFIX = 'o';
constexpr char SMOOTHING_PREFIX = 's';
constexpr char MATERIAL_LIB_PREFIX[7] = "mtllib";
constexpr char MATERIAL_USE_PREFIX[7] = "usemtl";

void ObjLoader::parseVertex(const std::string &line)
{
    Vertex vertex;
    std::stringstream ss(line);
    std::string temp;
    ss >> temp >> vertex.x >> vertex.y >> vertex.z;
    mesh.vertices.push_back(vertex);
    std::clog << "[INFO] Parsing vertex..." << std::endl;
}

void ObjLoader::parseNormal(const std::string &line)
{
    Normal normal;
    std::stringstream ss(line);
    std::string temp;
    ss >> temp >> normal.x >> normal.y >> normal.z;
    mesh.normals.push_back(normal);
    std::clog << "[INFO] Parsing normal..." << std::endl;
}

void ObjLoader::parseTexture(const std::string &line)
{
    Texture texture;
    std::stringstream ss(line);
    std::string temp;
    ss >> temp >> texture.u >> texture.v;
    mesh.textures.push_back(texture);
    std::clog << "[INFO] Parsing texture..." << std::endl;
}

bool ObjLoader::load(const std::string &path)
{
    std::ifstream f(path);
    if(!f) {
        std::cerr << "[ERROR] Cannot open file: " << path << std::endl;
        return false;
    }
    std::clog << "[INFO] Loading file: " << path << std::endl;
    std::string line;
    while(std::getline(f, line) && line[0]) 
    {
        if(line.empty() || line[0] == '#') continue;
        if(line[0] == VERTEX_PREFIX && line[1] != NORMAL_PREFIX && line[1] != TEXTURE_PREFIX)
            parseVertex(line);
        else if(line[0] == VERTEX_PREFIX && line[1] == NORMAL_PREFIX)
            parseNormal(line);
        else if(line[0] == VERTEX_PREFIX && line[1] == TEXTURE_PREFIX)
            parseTexture(line);
        // TODO Add parsing for faces
    }
    std::clog << "[INFO] Finished Loading." << std::endl;
    return true;
}