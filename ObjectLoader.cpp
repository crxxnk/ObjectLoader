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

//TODO Handle faces with missing texture/normal indices

void ObjLoader::parseVertex(const std::string &line)
{
    Vertex vertex;
    std::stringstream ss(line);
    std::string prefix; // skip 'v'
    ss >> prefix >> vertex.x >> vertex.y >> vertex.z;
    mesh.vertices.push_back(vertex);
    std::clog << "[INFO] Parsing vertex..." << std::endl;
}

int ObjLoader::parseFace(const std::string &line)
{
    //! Face index starts at 1
    Face face;
    std::stringstream ss(line);
    std::string prefix; // skip 'f'
    std::string data;
    char slash;
    int vertexIndex, textureIndex, normalIndex;

    ss >> prefix;
    std::string vertexData;

    while(ss >> vertexData)
    {
        int vIndex = 0, tIndex = 0, nIndex = 0;
        std::stringstream vss(vertexData);

        vss >> vIndex >> slash >> tIndex >> slash >> nIndex;

        try{
            face.vertices.push_back(mesh.vertices.at(vIndex - 1));
            face.textures.push_back(mesh.textures.at(tIndex - 1));
            face.normals.push_back(mesh.normals.at(nIndex - 1));
        } catch(const std::out_of_range& e) {
            std::cerr << "[ERROR] Face Index out of bounds " << e.what() << std::endl;
        }
    }
    mesh.faces.push_back(face);
    size_t faceIndex = mesh.faces.size() - 1;

    std::clog << "[INFO] Parsing face..." << std::endl;
    return faceIndex;
}

void ObjLoader::parseNormal(const std::string &line)
{
    Normal normal;
    std::stringstream ss(line);
    std::string prefix; // skip 'vn'
    ss >> prefix >> normal.x >> normal.y >> normal.z;
    mesh.normals.push_back(normal);
    std::clog << "[INFO] Parsing normal..." << std::endl;
}

void ObjLoader::parseTexture(const std::string &line)
{
    Texture texture;
    std::stringstream ss(line);
    std::string prefix; // skip 'vt'
    ss >> prefix >> texture.u >> texture.v;
    mesh.textures.push_back(texture);
    std::clog << "[INFO] Parsing texture..." << std::endl;
}

void ObjLoader::parseGroup(const std::string &line)
{
    Group group;
    std::stringstream ss(line);
    std::string prefix; // skip 'g'
    ss >> prefix >> group.name;
    mesh.groups.push_back(group);
    std::clog << "[INFO] Parsing group..." << std::endl;
}

void ObjLoader::parseObject(const std::string &line)
{
    Object object;
    std::stringstream ss(line);
    std::string prefix; // skip 'o'
    ss >> prefix >> object.name;
    mesh.objects.push_back(object);
    std::clog << "[INFO] Parsing object..." << std::endl;
}

void ObjLoader::parseSmoothing(const std::string &line)
{
    Smoothing smooth;
    std::stringstream ss(line);
    std::string prefix; // skip 's'
    std::string smoothness;

    ss >> prefix >> smoothness;

    if(smoothness == "off") {
        smooth.smoothness = 0;
    } else if (!smoothness.empty() && std::all_of(smoothness.begin(), smoothness.end(), ::isdigit)) {
        smooth.smoothness = std::stoi(smoothness);
    } else {
        std::cerr << "[WARNING] Smoothness level not specified! Set to 0." << std::endl;
    }

    mesh.smooths.push_back(smooth);
    std::clog << "[INFO] Parsing smoothness..." << std::endl;
}

void ObjLoader::load(const std::string &path)
{
    std::ifstream file(path);
    if(!file)
        throw std::runtime_error("[EXCEPTION] Cannot open file.");

    std::clog << "[INFO] Loading file: " << path << std::endl;
    std::string line;

    Group* currentGroup = nullptr;
    Object* currentObject = nullptr;
    Smoothing* currentSmoothing = nullptr;

    while(std::getline(file, line)) 
    {
        if(line.empty() || line[0] == '#') continue;
        if(line[0] == VERTEX_PREFIX && line[1] != NORMAL_PREFIX && line[1] != TEXTURE_PREFIX)
            parseVertex(line);
        else if(line[0] == VERTEX_PREFIX && line[1] == NORMAL_PREFIX)
            parseNormal(line);
        else if(line[0] == VERTEX_PREFIX && line[1] == TEXTURE_PREFIX)
            parseTexture(line);
        else if(line[0] == FACE_PREFIX) {
            size_t faceIndex = parseFace(line);

            if(!currentGroup) {
                auto it = std::find_if(mesh.groups.begin(), mesh.groups.end(),
                [](const Group& g){ return g.name == "Default"; });
                if (it == mesh.groups.end()) {
                    mesh.groups.push_back(Group{"Default"});
                    currentGroup = &mesh.groups.back();
                } else
                    currentGroup = &(*it);
            }

            if(!currentObject) {
                auto it = std::find_if(mesh.objects.begin(), mesh.objects.end(),
                [](const Object& o){ return o.name == "Default"; });
                if (it == mesh.objects.end()) {
                    mesh.objects.push_back(Object{"Default"});
                    currentObject = &mesh.objects.back();
                } else
                    currentObject = &(*it);
            }

            if(!currentSmoothing) {
                auto it = std::find_if(mesh.smooths.begin(), mesh.smooths.end(),
                [](const Smoothing& s){ return s.smoothness == 0; });
                if (it == mesh.smooths.end()) {
                    mesh.smooths.push_back(Smoothing{0});
                    currentSmoothing = &mesh.smooths.back();
                } else
                    currentSmoothing = &(*it);
            }

            currentGroup->faceIndices.push_back(faceIndex);
            currentSmoothing->faceIndices.push_back(faceIndex);

            auto it = std::find_if(currentObject->groups.begin(), currentObject->groups.end(),
                [&](const Group& g){ return g.name == currentGroup->name; });
            if(it == currentObject->groups.end())
                currentObject->groups.push_back(*currentGroup);

            currentObject->faceIndices.push_back(faceIndex);
        }
        else if(line[0] == GROUP_PREFIX) {
            parseGroup(line);
            currentGroup = &mesh.groups.back();
        }
        else if(line[0] == OBJECT_PREFIX) {
            parseObject(line);
            currentObject = &mesh.objects.back();
        }
        else if(line[0] == SMOOTHING_PREFIX) {
            parseSmoothing(line);
            currentSmoothing = &mesh.smooths.back();
        }
    }
    std::clog << "[INFO] Finished Loading." << std::endl;
}