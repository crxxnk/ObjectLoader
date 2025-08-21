#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

class Descriptor
{
protected:
    static std::unordered_map<std::string, std::string> descriptions;

    static void loadDescriptions() {
        if(!descriptions.empty()) return;
        std::ifstream file("MaterialSpecification.txt");
        if(!file) { std::cerr << "Could not open description file." << std::endl; return; }

        std::string line;
        while(std::getline(file, line)) {
            size_t pos = line.find(':');
            if(pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                descriptions[key] = value;
            }
        }
    }

public:
    [[nodiscard]] virtual std::string className() const = 0;
    [[nodiscard]] virtual std::string what() const {
        loadDescriptions();
        auto it = descriptions.find(className());
        if(it != descriptions.end())
            return it->second + "\n";
        else
            return "No description found for " + className() + "\n";
    }
};

std::unordered_map<std::string, std::string> Descriptor::descriptions;

struct Color// : public Descriptor //! Don't use (for now)
{
    float r,g,b;
};

// struct AmbientColor
// {
//     float r,g,b;
// };

// struct DiffuseColor
// {
//     float r,g,b;
// };

// struct SpecularColor
// {
//     float r,g,b;
// };

// struct EmissiveColor
// {
//     float r,g,b;
// };

// struct TransmissionFilterColor
// {
//     float r,g,b;
// };

struct AmbientMap
{

};

struct DiffuseMap
{

};

struct SpecularMap
{

};

struct ShininessMap
{

};

struct DissolveMap
{

};

struct BumpMap
{

};

struct Decal
{

};