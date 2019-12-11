//
//  skybox.hpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/1.
//  Copyright © 2019 许书畅. All rights reserved.
//

#ifndef skybox_hpp
#define skybox_hpp

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.hpp"

class SkyBox
{
public:
    SkyBox();
    ~SkyBox();
    void Render(glm::mat4 view, glm::mat4 projection);
    GLuint getTexSkybox() {return cubemapTexture;};
    
private:
    GLuint skyboxVAO, skyboxVBO;
    GLuint cubemapTexture;
    Shader skyboxShader;
};

#endif /* skybox_hpp */
