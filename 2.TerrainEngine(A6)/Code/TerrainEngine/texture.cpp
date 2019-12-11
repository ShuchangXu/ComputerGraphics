//
//  texture.cpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/1.
//  Copyright © 2019 许书畅. All rights reserved.
//

#include "texture.hpp"

GLint frameBufferTexture(GLuint frameBuffer, int screenWidth, int screenHeight)
{
    //Generate a texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2 * screenWidth, 2 * screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //Bind the texture to input frameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return textureID;
}

GLuint loadTexture(GLchar const * path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = SOIL_load_image(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        SOIL_free_image_data(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        SOIL_free_image_data(data);
    }

    return textureID;
}

//GLuint loadTexture(GLchar* path, GLboolean alpha)
//{
//    //Generate texture ID and load texture data
//    GLuint textureID;
//    glGenTextures(1, &textureID);
//    int width,height;
//    unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
//    // Assign texture to ID
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    
//    // Parameters
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    SOIL_free_image_data(image);
//    return textureID;
//}

GLuint loadCubemap(std::vector<std::string> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height;
    for (GLuint i = 0; i < faces.size(); i++)
    {
        unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            SOIL_free_image_data(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            SOIL_free_image_data(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    return textureID;
}
