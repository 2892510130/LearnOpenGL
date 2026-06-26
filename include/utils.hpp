#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

std::string readShaderFile(const std::string &file_path)
{
    std::ifstream file;
    std::stringstream buffer;
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        file.open(file_path);
        buffer << file.rdbuf();
        file.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return buffer.str();
}