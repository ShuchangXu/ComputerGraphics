//
//  texture.hpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/1.
//  Copyright © 2019 许书畅. All rights reserved.
//

#ifndef texture_hpp
#define texture_hpp

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <SOIL/SOIL.h>

GLuint loadTexture(GLchar const * path);
GLuint loadCubemap(std::vector<std::string> faces);
GLint frameBufferTexture(GLuint frameBuffer, int screenWidth, int screenHeight);
#endif /* texture_hpp */
