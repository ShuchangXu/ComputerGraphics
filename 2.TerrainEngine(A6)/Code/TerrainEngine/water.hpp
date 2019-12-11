//
//  water.hpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/2.
//  Copyright © 2019 许书畅. All rights reserved.
//

#ifndef water_hpp
#define water_hpp

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.hpp"

class Water
{
public:
    Water(float size,float height);
    ~Water();

    void Render(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos, GLuint texSkybox, GLuint reflecTex, GLuint refracTex, float time);
    void loadTextures(const GLchar* diffuse, const GLchar* normal0, const GLchar* normal1, const GLchar* flow, const GLchar* noise);

private:
    float size;
    Shader waterShader;
    GLuint VAO, VBO;
    GLuint texDiffuse, texNormal0, texNormal1, texFlow, texNoise;
};


#endif /* water_hpp */
