//
//  water.cpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/2.
//  Copyright © 2019 许书畅. All rights reserved.
//

#include "water.hpp"

Water::Water(float size,float height):waterShader("shader/water.vert.glsl", "shader/water.frag.glsl"){
    GLfloat vertices[] = {
        size, height, size, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -size, height, size, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -size, height, -size, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        -size, height, -size, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        size, height, -size, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        size, height, size, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    };
    this->size = size;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

Water::~Water()
{
}

void Water::loadTextures(const GLchar* diffuse, const GLchar* normal0, const GLchar* normal1, const GLchar* flow, const GLchar* noise){
    if(diffuse != nullptr)
        texDiffuse = loadTexture(diffuse);
    if(normal0 != nullptr)
        texNormal0 = loadTexture(normal0);
    if(normal1 != nullptr)
        texNormal1 = loadTexture(normal1);
    if(flow != nullptr)
        texFlow = loadTexture(flow);
    if(noise != nullptr)
        texNoise = loadTexture(noise);
}

void Water::Render(glm::mat4 view,
                   glm::mat4 projection,
                   glm::vec3 viewPos,
                   GLuint texSkybox,
                   GLuint reflecTex,
                   GLuint refracTex,
                   float time){
    waterShader.Use();
    
    glm::mat4 model(1);
    waterShader.setMat4("model", model);
    waterShader.setMat4("view", view);
    waterShader.setMat4("projection", projection);
    
    waterShader.setFloat("tiling", size/5);
    waterShader.setFloat("time", time);
    waterShader.setVec3("ViewPos", viewPos);
    //TODO: tidy up light settings
    waterShader.setVec3("dirlight.lightColor", glm::vec3(1.0f,1.0f,1.0f));
    waterShader.setVec3("dirlight.direction", glm::vec3(-1.0f,-1.0f,-1.0f));
    waterShader.setFloat("dirlight.intensity", 1.0f);
    
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texDiffuse);
    waterShader.setTexture("texture_diffuse", 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texNormal0);
    waterShader.setTexture("texture_normal0", 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texNormal1);
    waterShader.setTexture("texture_normal1", 2);
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texFlow);
    waterShader.setTexture("texture_flowmap", 3);
    
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texNoise);
    waterShader.setTexture("texture_noise", 4);
    
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texSkybox);
    waterShader.setTexture("skybox", 5);
    
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, reflecTex);
    waterShader.setTexture("texture_reflect", 6);
    
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, refracTex);
    waterShader.setTexture("texture_refract", 7);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
