//
//  terrain.cpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/1.
//  Copyright © 2019 许书畅. All rights reserved.
//

#include "terrain.hpp"

Terrain::Terrain(float _gridSize, float _heightScale, float _heightOffset):terrainShader("shader/terrain.vert.glsl", "shader/terrain.frag.glsl"){
    gridSize = _gridSize;
    heightScale = _heightScale;
    heightOffset = _heightOffset;
}

Terrain::~Terrain(){
    
}

void Terrain::loadTextures(const GLchar *diffuse, const GLchar *detail){
    if(diffuse != nullptr)
        texDiffuse = loadTexture(diffuse);
    if(detail != nullptr)
        texDetail = loadTexture(detail);
}

void Terrain::loadHeightmap(const GLchar *pathname){
    //Note:
    //1. the heightMap defines height per pixel
    //2. hightMap is stored in x(row)-first order
    //therefore h = heightMap[z][x]
    int lenX, lenZ;
    unsigned char* heightMap = SOIL_load_image(pathname, &lenX, &lenZ, nullptr, SOIL_LOAD_L);
    
    int verticesSize = lenX * lenZ;
    this->faceSize = (lenX - 1) * (lenZ - 1) * 2;
    
    // PosX, PosY, PosZ, Nx, Ny, Nz, S, T
    GLfloat* vertices = new GLfloat[verticesSize * 8];
    GLuint* indices = new GLuint[faceSize * 3];
    
    for(int i = 0; i < verticesSize; i++){
        int h = (int)heightMap[i];
        int vertIndex = 8 * i;
        
        // i = z * lenX + x
        int x = i % lenX;
        int z = i / lenX;
        
        vertices[vertIndex] = (x - lenX/2.0) * gridSize;//PosX
        vertices[vertIndex + 1] = h * heightScale + heightOffset;//PosY
        vertices[vertIndex + 2] = (z - lenZ/2.0) * gridSize;//PosZ
        
        vertices[vertIndex + 6] = x / (float) lenX;//S
        vertices[vertIndex + 7] = z / (float) lenZ;//T
        
        if(z == 0 || z == lenZ - 1 || x == 0 || x == lenX - 1){
            vertices[vertIndex + 3] = 0;//Nx
            vertices[vertIndex + 4] = 1;//Ny
            vertices[vertIndex + 5] = 0;//Nz
        }else{
            float h_w = (int)heightMap[i - 1];//x_neg
            float h_e = (int)heightMap[i + 1];//x_pos
            float h_n = (int)heightMap[i - lenX];//z_neg
            float h_s = (int)heightMap[i + lenX];//z_pos
            
            glm::vec3 vec_w = glm::vec3(-gridSize, (h_w - h) * heightScale, 0);
            glm::vec3 vec_e = glm::vec3(gridSize, (h_e - h) * heightScale, 0);
            glm::vec3 vec_n = glm::vec3(0, (h_n - h) * heightScale, -gridSize);
            glm::vec3 vec_s = glm::vec3(0, (h_s - h) * heightScale, gridSize);
            
            glm::vec3 normal_nw = glm::cross(vec_n, vec_w);
            glm::vec3 normal_ws = glm::cross(vec_w, vec_s);
            glm::vec3 normal_se = glm::cross(vec_s, vec_e);
            glm::vec3 normal_en = glm::cross(vec_e, vec_n);
            
            glm::vec3 normal = glm::normalize(normal_nw + normal_ws + normal_se + normal_en);
            
            vertices[vertIndex + 3] = normal.x;
            vertices[vertIndex + 4] = normal.y;
            vertices[vertIndex + 5] = normal.z;
        }
    }
    
    for(int i = 0; i < faceSize/2; i++){
        int index = 6 * i;
        int x = i % (lenX - 1);
        int z = i / (lenX - 1);
        int vert_nw = x + z * lenX;
        
        indices[index] = vert_nw;
        indices[index + 1] = vert_nw + 1;
        indices[index + 2] = vert_nw + lenX;
        indices[index + 3] = vert_nw + 1;
        indices[index + 4] = vert_nw + 1 + lenX;
        indices[index + 5] = vert_nw + lenX;
    }
    
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize * 8 * sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceSize * 3 * sizeof(GL_UNSIGNED_INT), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    
    delete [] heightMap;
    delete [] vertices;
    delete [] indices;
    
    heightMap = nullptr;
    vertices = nullptr;
    indices = nullptr;
}

void Terrain::Render(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos, RenderType renderType){
    terrainShader.Use();
    
    glm::mat4 model(1);
    glm::vec4 clipping;
    switch(renderType){
        case RenderType::visible:
            clipping = glm::vec4(0, 1, 0, 0);
            break;
        case RenderType::reflection:
            model = glm::scale(model, glm::vec3(1,-1,1));
            clipping = glm::vec4(0, -1, 0, 0);
            break;
        case RenderType::refraction:
            clipping = glm::vec4(0, -1, 0, 0);
    }
    terrainShader.setMat4("model", model);
    terrainShader.setMat4("view", view);
    terrainShader.setMat4("projection", projection);
    terrainShader.setVec3("ViewPos", viewPos);
    terrainShader.setVec4("clipping", clipping);
    
    //TODO: tidy up light settings
    terrainShader.setVec3("AmbientColor", glm::vec3(0.2f,0.2f,0.2f));
    terrainShader.setVec3("dirlight.lightColor", glm::vec3(1.0f,1.0f,1.0f));
    terrainShader.setVec3("dirlight.direction", glm::vec3(-1.0f,-1.0f,-1.0f));
    terrainShader.setFloat("dirlight.intensity", 0.65f);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texDiffuse);
    terrainShader.setTexture("texture_diffuse", 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texDetail);
    terrainShader.setTexture("texture_detail", 1);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, faceSize * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
