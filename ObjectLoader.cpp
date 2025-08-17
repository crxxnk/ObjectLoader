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

template<typename T>
[[nodiscard]] const std::optional<T> ObjLoader::parseElement(const std::string &line)
{
    if constexpr (std::is_same_v<T, Vertex>)
    {
        Vertex vertex;
        std::stringstream ss(line);
        std::string prefix; // skip 'v'
        ss >> prefix >> vertex.x >> vertex.y >> vertex.z;
        std::clog << "[INFO] Parsing vertex..." << std::endl;
        return vertex;
    }
    else if constexpr (std::is_same_v<T, Normal>)
    {
        Normal normal;
        std::stringstream ss(line);
        std::string prefix; // skip 'vn'
        ss >> prefix >> normal.x >> normal.y >> normal.z;
        std::clog << "[INFO] Parsing normal..." << std::endl;
        return normal;
    }
    else if constexpr (std::is_same_v<T, Texture>)
    {
        Texture texture;
        std::stringstream ss(line);
        std::string prefix; // skip 'vt'
        ss >> prefix >> texture.u >> texture.v;
        std::clog << "[INFO] Parsing texture..." << std::endl;
        return texture;
    }
    else if constexpr (std::is_same_v<T, Smoothing>)
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

        std::clog << "[INFO] Parsing smoothness..." << std::endl;
        return smooth;
    }
    else if constexpr (std::is_same_v<T, Object>)
    {
        Object object;
        std::stringstream ss(line);
        std::string prefix; // skip 'o'
        ss >> prefix >> object.name;
        std::clog << "[INFO] Parsing object..." << std::endl;
        return object;
    }
    else if constexpr (std::is_same_v<T, Group>)
    {
        Group group;
        std::stringstream ss(line);
        std::string prefix; // skip 'g'
        ss >> prefix >> group.name;
        std::clog << "[INFO] Parsing group..." << std::endl;
        return group;
    }
    else if constexpr (std::is_same_v<T, std::shared_ptr<Face>>)
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
        std::shared_ptr<Face> facePtr = std::make_shared<Face>(face);

        std::clog << "[INFO] Parsing face..." << std::endl;
        return facePtr;
    }
    else
        throw std::runtime_error("Cannot parse this type of element");
    
}

template<typename T>
void ObjLoader::storeElement(const std::optional<T> &element)
{
    if (!element.has_value())
        throw std::runtime_error("Tried to store uninitialized element - Did you mean to call ParseElement?");
    if constexpr (std::is_same_v<T, Vertex>)
        mesh.vertices.push_back(*element);
    else if constexpr (std::is_same_v<T, Normal>)
        mesh.normals.push_back(*element);
    else if constexpr (std::is_same_v<T, Texture>)
        mesh.textures.push_back(*element);
    else if constexpr (std::is_same_v<T, Smoothing>)
        mesh.smooths.push_back(*element);
    else if constexpr (std::is_same_v<T, Object>)
        mesh.objects.push_back(*element);
    else if constexpr (std::is_same_v<T, Group>)
        mesh.groups.push_back(*element);
    else if constexpr (std::is_same_v<T, std::shared_ptr<Face>>)
        mesh.faces.push_back(*element);
    else
        throw std::runtime_error("Cannot store this type of element");
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

    std::optional<Vertex> vertex;
    std::optional<Normal> normal;
    std::optional<Texture> texture;
    std::optional<std::shared_ptr<Face>> face;
    std::optional<Group> group;
    std::optional<Object> object;
    std::optional<Smoothing> smoothing;

    while(std::getline(file, line)) 
    {
        if(line.empty() || line[0] == '#') continue;
        if(line[0] == VERTEX_PREFIX && line[1] != NORMAL_PREFIX && line[1] != TEXTURE_PREFIX) {
            try {
                vertex = parseElement<Vertex>(line);
                storeElement(vertex);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
        }
        else if(line[0] == VERTEX_PREFIX && line[1] == NORMAL_PREFIX) {
            try {
                normal = parseElement<Normal>(line);
                storeElement(normal);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
        }
        else if(line[0] == VERTEX_PREFIX && line[1] == TEXTURE_PREFIX) {
            try {
                texture = parseElement<Texture>(line);
                storeElement(texture);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
        }
        else if(line[0] == FACE_PREFIX) {
            try {
                face = parseElement<std::shared_ptr<Face>>(line);
                storeElement(face);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }

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

            currentGroup->faces.push_back(*face);
            currentSmoothing->faces.push_back(*face);

            auto it = std::find_if(currentObject->groups.begin(), currentObject->groups.end(),
                [&](const Group& g){ return g.name == currentGroup->name; });
            if(it == currentObject->groups.end())
                currentObject->groups.push_back(*currentGroup);

            currentObject->faces.push_back(*face);
        }
        else if(line[0] == GROUP_PREFIX) {
            try {
                group = parseElement<Group>(line);
                storeElement(group);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
            currentGroup = &mesh.groups.back();
        }
        else if(line[0] == OBJECT_PREFIX) {
            try {
                object = parseElement<Object>(line);
                storeElement(object);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
            currentObject = &mesh.objects.back();
        }
        else if(line[0] == SMOOTHING_PREFIX) {
            try {
                smoothing = parseElement<Smoothing>(line);
                storeElement(smoothing);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
            currentSmoothing = &mesh.smooths.back();
        }
    }

    std::clog << "[INFO] Finished Loading." << std::endl;
}

/* DEPRECATED


void ObjLoader::parseVertex(const std::string &line)
{
    Vertex vertex;
    std::stringstream ss(line);
    std::string prefix; // skip 'v'
    ss >> prefix >> vertex.x >> vertex.y >> vertex.z;
    // mesh.vertices.push_back(vertex);
    std::clog << "[INFO] Parsing vertex..." << std::endl;
}

[[nodiscard]] std::shared_ptr<Face> ObjLoader::parseFace(const std::string &line)
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
    std::shared_ptr<Face> facePtr = std::make_shared<Face>(face);

    // mesh.faces.push_back(facePtr);

    std::clog << "[INFO] Parsing face..." << std::endl;
    return facePtr;
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
*/