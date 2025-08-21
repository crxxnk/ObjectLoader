#pragma once
#include <string>
#include <optional>
#include <exception>
#include "Logger.h"
#include "Mesh.h"
#include "Obj_Prefix.h"

class MtlLoader
{
public:
    void load(const std::string &path);
    template<typename T>
    const std::optional<T> parseElement(const std::string &line);
};