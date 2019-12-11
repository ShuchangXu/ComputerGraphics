//
//  terrain.hpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/1.
//  Copyright © 2019 许书畅. All rights reserved.
//

#ifndef terrain_hpp
#define terrain_hpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.hpp"

enum RenderType{
    visible,
    reflection,
    refraction
};

class Terrain
{
public:
    Terrain(float gridSize, float heightScale, float heightOffset);
    ~Terrain();

    void loadHeightmap(const GLchar* pathname);
    void loadTextures(const GLchar* diffuse, const GLchar* detail);
    
    void Render(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos, RenderType renderType);

private:
    int faceSize;
    float gridSize;//Size Per Pixel
    float heightScale;
    float heightOffset;

    GLuint VAO, VBO, EBO;
    GLuint texDiffuse, texDetail;
    Shader terrainShader;
};

#endif /* terrain_hpp */
