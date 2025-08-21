#include "ObjectLoader.h"
#include "MaterialLoader.h"

//TODO Handle faces with missing texture/normal indices
//TODO Handle points and lines with missing texture indices
//TODO Add v, vt, vn, vp, l, p... etc in objects and groups
//TODO Put every parser in a function and call it in parseElement
//TODO Make the degree default value changeable and change it in parseDegree
//TODO Fix and research about d_interp and c_interp
//TODO Handle invalid indices in faces curves etc
//TODO Do even more error handling

/**
 * @brief Parses a single line of the .obj file into the corresponding element.
 * 
 * Uses template specialization to determine which type of element to parse:
 * Vertex, Normal, Texture, Smoothing, Object, Group, or Face.
 * 
 * @tparam T The type of element to parse.
 * @param line The line from the .obj file.
 * @return std::optional<T> The parsed element, or nullopt if parsing fails.
 */
template<typename T>
[[nodiscard]] const std::optional<T> ObjLoader::parseElement(const std::string &line)
{
    //* Geometry

    //? Vertex
    if constexpr (std::is_same_v<T, Vertex>)
    {
        Vertex vertex;
        std::stringstream ss(line);
        std::string prefix; // skip 'v'
        ss >> prefix >> vertex.x >> vertex.y >> vertex.z;
        logger.log("Parsing vertex...");
        return vertex;
    }

    //? Normal
    else if constexpr (std::is_same_v<T, Normal>)
    {
        Normal normal;
        std::stringstream ss(line);
        std::string prefix; // skip 'vn'
        ss >> prefix >> normal.x >> normal.y >> normal.z;
        logger.log("Parsing normal...");
        return normal;
    }

    //? Texture
    else if constexpr (std::is_same_v<T, Texture>)
    {
        Texture texture;
        std::stringstream ss(line);
        std::string prefix; // skip 'vt'
        ss >> prefix >> texture.u >> texture.v;
        logger.log("Parsing texture...");
        return texture;
    }

    //? Parameter Space Vertex
    else [[unlikely]] if constexpr (std::is_same_v<T, ParameterSpaceVertex>)
    {
        ParameterSpaceVertex psv;
        std::stringstream ss(line);
        std::string prefix; // skip 'vp'
        ss >> prefix >> psv.x >> psv.y >> psv.z;
        logger.log("Parsing Parameter Space Vertex...");
        return psv;
    }

    //* Elements

    //? Face
    else if constexpr (std::is_same_v<T, std::shared_ptr<Face>>)
    {
        //! Face index starts at 1
        Face face;
        std::stringstream ss(line);
        std::string prefix; // skip 'f'
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
                logger.log(std::string("Face Index out of bounds ") + e.what(), logger.ERROR);
            }
        }
        std::shared_ptr<Face> facePtr = std::make_shared<Face>(face);

        logger.log("Parsing face...");
        return facePtr;
    }

    //? Point
    else if constexpr (std::is_same_v<T, std::shared_ptr<Point>>)
    {
        Point point;
        std::stringstream ss(line);
        std::string prefix; // skip 'p'
        char slash;
        int vertexIndex, textureIndex;

        ss >> prefix;
        std::string vertexData;

        while(ss >> vertexData)
        {
            int vIndex = 0, tIndex = 0;
            std::stringstream vss(vertexData);

            vss >> vIndex >> slash >> tIndex;

            try{
                point.vertices.push_back(mesh.vertices.at(vIndex - 1));
                point.textures.push_back(mesh.textures.at(tIndex - 1));
            } catch(const std::out_of_range& e) {
                logger.log(std::string("Point Index out of bounds ") + e.what(), logger.ERROR);
            }
        }
        std::shared_ptr<Point> pointPtr = std::make_shared<Point>(point);

        logger.log("Parsing points..");
        return pointPtr;
    }

    //? Line
    else if constexpr (std::is_same_v<T, std::shared_ptr<Line>>)
    {
        Line _line;
        std::stringstream ss(line);
        std::string prefix; // skip 'p'
        char slash;
        int vertexIndex, textureIndex;

        ss >> prefix;
        std::string vertexData;

        while(ss >> vertexData)
        {
            int vIndex = 0, tIndex = 0;
            std::stringstream vss(vertexData);

            vss >> vIndex >> slash >> tIndex;

            try{
                _line.vertices.push_back(mesh.vertices.at(vIndex - 1));
                _line.textures.push_back(mesh.textures.at(tIndex - 1));
            } catch(const std::out_of_range& e) {
                logger.log(std::string("Line Index out of bounds ") + e.what(), logger.ERROR);
            }
        }
        std::shared_ptr<Line> linePtr = std::make_shared<Line>(_line);

        logger.log("Parsing lines...");
        return linePtr;
    }

    //* Groups & Objects

    //? Group
    else if constexpr (std::is_same_v<T, Group>)
    {
        Group group;
        std::stringstream ss(line);
        std::string prefix; // skip 'g'
        ss >> prefix >> group.name;
        logger.log("Parsing group...");
        return group;
    }

    //? Object
    else if constexpr (std::is_same_v<T, Object>)
    {
        Object object;
        std::stringstream ss(line);
        std::string prefix; // skip 'o'
        ss >> prefix >> object.name;
        logger.log("Parsing object...");
        return object;
    }

    //? Smoothing
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
            logger.log("Smoothness level not specified! Set to 0.", logger.WARNING);
        }

        logger.log("Parsing smoothness...");
        return smooth;
    }
    
    
    //* Freeform Curves & Surfaces
    
    //? Curve/Surface type & Material Loading
    else if constexpr (std::is_same_v<T, std::string>)
    {
        std::stringstream ss(line);
        std::string prefix; // skip 'cstype'
        ss >> prefix;
        
        if(prefix == "cstype")
        {
            std::string type;
            std::string first;
            if (!(ss >> first))
                throw std::runtime_error("Expected a curve-surface type after 'cstype'");
            
            if (first == "rat") {
                std::string second;
                if (!(ss >> second))
                throw std::runtime_error("Expected curve type after 'rat'");
                type = "rat " + second;
            } else
                type = first;
            
            if(type != "bezier" && type != "rat bezier" && type != "b-spline" && type != "rat b-spline" && type != "cardinal" && type != "rat cardinal" && type != "taylor" && type != "rat taylor")
                throw std::runtime_error("Expected any of the following curve-surface types after 'cstype':\nbezier\nrat bezier\nb-spline\nrat b-spline\ncardinal\nrat cardinal\ntaylor\nrat taylor");
            logger.log("Parsing curve-surface type...");
            return type;
        }
        else if(prefix == "mtllib")
        {
            std::string path;
            if(!(ss >> path))
                throw std::runtime_error("Expected a .mtl file after mtllib.");
            return path;
        }

        //! Dont use
        //? Curve interpolation method
        // else if(prefix == "cinterp")
        // {
        //     std::string method;
        //     if (!(ss >> method))
        //         throw std::runtime_error("Expected interpolation method after 'cinterp'");
            
        //     if(method != "line" && method != "cubic" && method != "tangent" && method != "spline")
        //         throw std::runtime_error("Expected any of the following interpolation methods after 'cinterp':\nline\ncubic\ntangent\nspline");
        //     logger.log("Parsing color interpolation method...");
        //     return method;
        // }

        return ""; // Trigger warning
    }

    //? Color & Dissolve Interpolation
    // else if constexpr (std::is_same_v<T, bool>)
    // {
    //     std::stringstream ss(line);
    //     std::string prefix; // skip 'c_interp' or 'd_interp'
    //     std::string onOff;
    //     ss >> prefix;

    //     if(prefix == "c_interp")
    //     {
    //         if (!(ss >> onOff))
    //             throw std::runtime_error("Expected 'on' or 'off' after 'c_interp'");
    //         if(onOff == "on")
    //             return true;
    //         else if(onOff == "off")
    //             return false;
    //         else
    //             throw std::runtime_error("Expected 'on' or 'off' after 'c_interp'");
    //     }

    //     if(prefix == "d_interp")
    //     {
    //         if (!(ss >> onOff))
    //             throw std::runtime_error("Expected 'on' or 'off' after 'd_interp'");
    //         if(onOff == "on")
    //             return true;
    //         else if(onOff == "off")
    //             return false;
    //         else
    //             throw std::runtime_error("Expected 'on' or 'off' after 'd_interp'");
    //     }

    //     return false;
    // }

    //? Degree
    else if constexpr (std::is_same_v<T, int>)
    {
        int degree;
        std::stringstream ss(line);
        std::string prefix; // skip 'deg'
        ss >> prefix;
        if(!(ss >> degree))
            throw std::runtime_error("Expected integer after 'deg'");
        if(degree < 1) {
            throw std::runtime_error("Curve degree cannot be less than 1 - set to default value(3)");
            degree = 3;
        }
        logger.log("Parsing degree...");
        return degree;
    }
    
    //? Additional parameters
    else if constexpr (std::is_same_v<T, std::vector<float>>)
    {
        std::vector<float> params;
        float value;
        std::stringstream ss(line);
        std::string prefix; // skip 'parm'
        ss >> prefix;

        if(!(ss >> value))
            throw std::runtime_error("Expected a float parameter between 0.0 and 1.0 for each vertex in the curve above after 'parm'");
        else
            params.push_back(value);

        while(ss >> value) {
            if(value < 0.0 || value > 1.0) {
                std::fill(params.begin(), params.end(), 0.0);
                throw std::runtime_error("Parameter must be between 0.0 and 1.0.");
            }
            params.push_back(value);
        }
                
        logger.log("Parsing parameters...");
        return params;
    }

    //? Curve
    else if constexpr (std::is_same_v<T, std::shared_ptr<Curve>>)
    {
        Curve curve;
        std::stringstream ss(line);
        std::string prefix; // skip 'curv'
        std::string temp;
        int vertexIndex;
        std::string glbParmRange1, glbParmRange2;

        ss >> prefix;

        if(!(ss >> temp))
            throw std::runtime_error("Expected a curve definition after 'curv'.");
        try {
            float f = std::stof(temp);
        } catch (const std::exception&) {
            throw std::runtime_error("Non-numeric value found after 'curv': expected 'float' or 'int'.");
        }

        std::string token;
        while (ss >> token) {
            try {
                float f = std::stof(token);
            } catch (const std::exception&) {
                throw std::runtime_error("Non-numeric value found after 'curv': expected 'float' or 'int'.");
            }
        }

        ss.clear();
        ss.seekg(0);

        ss >> prefix;

        ss >> glbParmRange1 >> glbParmRange2;

        auto isDecimal = [](const std::string& s) {
            return s.find('.') != std::string::npos;
        };

        if (isDecimal(glbParmRange1) && isDecimal(glbParmRange2)) {
            float gpr = std::stof(glbParmRange1);
            float _gpr = std::stof(glbParmRange2);

            if(gpr < 0.0 || gpr > 1.0 || _gpr > 1.0 || _gpr < 0.0) {
                logger.log("Global parameter range must be between 0.0 and 1.0: both set to -1.0.", logger.WARNING);
                gpr = -1.0;
                _gpr = -1.0;
            } else if(gpr > _gpr) {
                logger.log("First global parameter range attribute must be less than the second: both set to -1.0.", logger.WARNING);
                gpr = -1.0;
                _gpr = -1.0;
            }

            curve.globalParameterRange.at(0) = gpr;
            curve.globalParameterRange.at(1) = _gpr;
        } else [[unlikely]] if(!isDecimal(glbParmRange1) ^ !isDecimal(glbParmRange2)) {
            logger.log("Only 1 global parameter range attribute specified: both set to -1.0.", logger.WARNING);
            curve.globalParameterRange.at(0) = -1.0;
            curve.globalParameterRange.at(1) = -1.0;
            ss.clear();
            ss.seekg(0);
            ss >> prefix;
            ss >> temp;
        } else {
            logger.log("No global parameter range attribute specified: both set to -1.0.", logger.WARNING);
            curve.globalParameterRange.at(0) = -1.0;
            curve.globalParameterRange.at(1) = -1.0;
            ss.clear();
            ss.seekg(0);
            ss >> prefix;
        }

        std::string vertexData;

        while(ss >> vertexData)
        {
            int vIndex = 0;
            std::stringstream vss(vertexData);

            if(vss >> vIndex) {
                try{
                    curve.controlPoints.push_back(mesh.vertices.at(vIndex - 1));
                    curve.vertexCount = curve.controlPoints.size();
                } catch(const std::out_of_range& e) {
                    logger.log(std::string("Curve Index out of bounds ") + e.what(), logger.ERROR);
                }
            }
        }
        std::shared_ptr<Curve> curvePtr = std::make_shared<Curve>(curve);

        logger.log("Parsing curve...");
        return curvePtr;
    }

    else [[unlikely]]
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
    else [[unlikely]] if constexpr (std::is_same_v<T, ParameterSpaceVertex>)
        mesh.psvs.push_back(*element);
    else if constexpr (std::is_same_v<T, Smoothing>)
        mesh.smooths.push_back(*element);
    else if constexpr (std::is_same_v<T, Object>)
        mesh.objects.push_back(*element);
    else if constexpr (std::is_same_v<T, Group>)
        mesh.groups.push_back(*element);
    else if constexpr (std::is_same_v<T, std::shared_ptr<Face>>)
        mesh.faces.push_back(*element);
    else if constexpr (std::is_same_v<T, std::shared_ptr<Point>>)
        mesh.points.push_back(*element);
    else if constexpr (std::is_same_v<T, std::shared_ptr<Line>>)
        mesh.lines.push_back(*element);
    else if constexpr (std::is_same_v<T, std::shared_ptr<Curve>>)
        mesh.curves.push_back(*element);
    else [[unlikely]]
        throw std::runtime_error("Cannot store this type of element");
}

void ObjLoader::load(const std::string &path)
{
    std::ifstream file(path);
    if(!path.ends_with(".obj"))
        throw std::invalid_argument("File '" + path + "' is not an OBJ file.");
    if(!file)
        throw std::runtime_error("Cannot open .obj file.");

    logger.log("Loading file: " + path);
    std::string line;

    Group* currentGroup = nullptr;
    Object* currentObject = nullptr;
    Smoothing* currentSmoothing = nullptr;

    std::optional<Vertex> vertex;
    std::optional<Normal> normal;
    std::optional<Texture> texture;
    std::optional<ParameterSpaceVertex> psv;
    std::optional<std::shared_ptr<Face>> face;
    std::optional<std::shared_ptr<Point>> point;
    std::optional<std::shared_ptr<Line>> _line;
    std::optional<std::shared_ptr<Curve>> curve;
    std::optional<Group> group;
    std::optional<Object> object;
    std::optional<Smoothing> smoothing;
    std::optional<int> degree;
    std::optional<std::string> cstype;
    std::optional<std::vector<float>> parameters;

    std::optional<std::string> materialPath;
    MtlLoader mtlLoader;

    // bool c_interp = false;
    // bool d_interp = false;

    // std::optional<std::string> cinterp;

    while(std::getline(file, line)) 
    {
        if(line.empty() || line[0] == '#') continue;
        if(line[0] == VERTEX_PREFIX && line[1] != NORMAL_PREFIX && line[1] != TEXTURE_PREFIX && line[1] != POINT_PREFIX) {
            try {
                vertex = parseElement<Vertex>(line);
                storeElement(vertex);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if(line[0] == VERTEX_PREFIX && line[1] == NORMAL_PREFIX) {
            try {
                normal = parseElement<Normal>(line);
                storeElement(normal);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if(line[0] == VERTEX_PREFIX && line[1] == TEXTURE_PREFIX) {
            try {
                texture = parseElement<Texture>(line);
                storeElement(texture);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if(line[0] == FACE_PREFIX) {
            try {
                face = parseElement<std::shared_ptr<Face>>(line);
                storeElement(face);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
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
                logger.log(e.what(), logger.ERROR);
            }
            currentGroup = &mesh.groups.back();
        }
        else if(line[0] == OBJECT_PREFIX) {
            try {
                object = parseElement<Object>(line);
                storeElement(object);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
            currentObject = &mesh.objects.back();
        }
        else if(line[0] == SMOOTHING_PREFIX) {
            try {
                smoothing = parseElement<Smoothing>(line);
                storeElement(smoothing);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
            currentSmoothing = &mesh.smooths.back();
        }
        else [[unlikely]] if(line[0] == VERTEX_PREFIX && line[1] == POINT_PREFIX) {
            try {
                psv = parseElement<ParameterSpaceVertex>(line);
                storeElement(psv);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if (line[0] == POINT_PREFIX && line[1] == ' ') {
            try {
                point = parseElement<std::shared_ptr<Point>>(line);
                storeElement(point);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if(line[0] == LINE_PREFIX) {
            try {
                _line = parseElement<std::shared_ptr<Line>>(line);
                storeElement(_line);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if (line.rfind(CURVE_PREFIX, 0) == 0) {
            try {
                curve = parseElement<std::shared_ptr<Curve>>(line);
                storeElement(curve);
                curve.value()->degree = degree.value();
                curve.value()->type = cstype.value();

                curve.value()->hasParameters = false;
                // curve.value()->hasInterpMethod = false;

            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if (line.rfind(DEGREE_PREFIX, 0) == 0) {
            try {
                degree = parseElement<int>(line);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if (line.rfind(CUR_SUR_TYPE_PREFIX, 0) == 0) {
            try {
                cstype = parseElement<std::string>(line);
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if (line.rfind(PARAMETER_PREFIX, 0) == 0) {
            try {
                parameters = parseElement<std::vector<float>>(line);
                if (curve.has_value()) {
                    if(curve.value()->vertexCount == parameters.value().size()) {
                        curve.value()->hasParameters = true;
                        curve.value()->parameters = parameters.value();
                    } else [[unlikely]] {
                        logger.log("Parameter list does not match the number of control points", logger.ERROR);
                    }
                } else [[unlikely]] {
                    logger.log("Cannot assign parameters: no curve defined yet", logger.ERROR);
                }
                
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        else if (line.rfind(MATERIAL_LIB_PREFIX, 0) == 0) {
            try {
                materialPath = parseElement<std::string>(line);
                mtlLoader.load(materialPath.value());
            } catch (const std::exception &e) {
                logger.log(e.what(), logger.ERROR);
            }
        }
        // else if(line.rfind(COLOR_INTERPOLATION_PREFIX, 0) == 0) {
        //     c_interp = parseElement<bool>(line).value();
        //     mesh.c_interp = c_interp;
        // }
        // else if(line.rfind(DISSOLVE_INTERPOLATION_PREFIX, 0) == 0) {
        //     d_interp = parseElement<bool>(line).value();
        //     mesh.d_interp = d_interp;
        // }
    
    }
    logger.log("Finished Loading.");
    logger.logFinish();
}

/* DEPRECATED

! Wrong interpolation but still nice
else if(line.rfind(COLOR_INTERPOLATION_PREFIX, 0) == 0) {
    try {
        cinterp = parseElement<std::string>(line);
        if (curve.has_value()) {
            curve.value()->hasInterpMethod = true;
            curve.value()->interpMethod = cinterp.value();
        } else [[unlikely]] {
            logger.log(cinterp.value(), logger.DEBUG);
            logger.log("Cannot assign interpolation method: no curve defined yet", logger.ERROR);
        }
    } catch (const std::exception &e) {
        logger.log(e.what(), logger.ERROR);
    }
}


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