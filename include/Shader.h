#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public :
    unsigned int ID;

    Shader(const char* VertexPath, const char* fragmentPath);

    void use() {glUseProgram(ID); }

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;
};

#endif
