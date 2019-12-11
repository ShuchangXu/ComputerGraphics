//
//  HalfEdge.cpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/5.
//  Copyright © 2019 许书畅. All rights reserved.
//

#include "HalfEdge.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

MeshHDS::MeshHDS(const char* path) : MeshHDS(){
    std::vector<Vertex*> vs;
    std::vector<Face*> fs;
    if(loadDataFromFile(path, vs, fs)){
        constructFromData(vs, fs);
    }
}

MeshHDS::MeshHDS(std::vector<Vertex*> vs, std::vector<Face*> fs) : MeshHDS(){
    constructFromData(vs, fs);
}

bool MeshHDS::loadDataFromFile(const char* path, std::vector<Vertex*>& vs, std::vector<Face*>& fs)
{
    vs.clear();
    fs.clear();
    
    std::ifstream file(path);
    if (!file)
    {
        std::cerr << "Error::ObjLoader, could not open obj file:"<< path << " for reading." << std::endl;
        return false;
    }
    std::string line;
    while (getline(file, line))
    {
        if (line.substr(0, 1) == "v") // 顶点位置数据
        {
            Vertex* v = new Vertex();
            std::istringstream s(line.substr(2));
            s >> v->position.x; s >> v->position.y; s >> v->position.z;
            vs.push_back(v);
        }
        else if (line.substr(0, 1) == "f") // 面数据
        {
            Face* f = new Face();
            std::istringstream s(line.substr(2));
            
            int iv;
            while(s >> iv){
                f->indices_v.push_back(iv - 1);
            }
            
            fs.push_back(f);
        }
    }
    return true;
}

void MeshHDS::constructFromData(std::vector<Vertex*> vs, std::vector<Face*> fs){
    _es.clear();
    _vs = vs;
    _fs = fs;
    
    std::map<std::pair<int, int>, HalfEdge*> Vs2HF;
    std::map<HalfEdge*, std::pair<int, int>> HF2Vs;
    
    for(int i = 0; i < _fs.size(); i++){
        int e_curSize = (int)_es.size();
        int v_size = (int)_fs[i] -> indices_v.size();
        
        //set up cross references of e, f, v, and HF
        for(int j = 0; j < v_size; j++){
            HalfEdge* e = new HalfEdge();
            int vj = _fs[i]->indices_v[j];
            e->f = _fs[i];
            e->v = _vs[vj];
            _es.push_back(e);
            
            _fs[i]->e = e;
            _vs[vj]->e = e;
            
            int pj = (v_size+j-1) % v_size;
            int vpj = _fs[i]->indices_v[pj];
            Vs2HF[std::make_pair(vpj, vj)] = e;
            HF2Vs[e] = std::make_pair(vpj, vj);
        }
        
        //set up 'next' for all half edges
        for(int j = 0; j < v_size; j++){
            _es[e_curSize + j] -> next = _es[e_curSize + (j+1)%v_size];
        }
    }
    
    //set up opposite half edges
    for(int i = 0; i < _es.size(); i++){
        std::pair<int, int> vs = HF2Vs[_es[i]];
        _es[i]->oppo = Vs2HF[std::make_pair(vs.second, vs.first)];
    }
    
    computeNormals();
    setupBuffers();
}

void MeshHDS::computeNormals(){
    auto crossOppoWithNext = [](HalfEdge* e) -> glm::vec3{
        glm::vec3 v0 = e->v->position;
        glm::vec3 v1 = e->oppo->v->position;
        glm::vec3 v2 = e->next->v->position;
        return glm::cross(v2 - v0, v1 - v0);
    };
    
    for(int i = 0; i < _vs.size(); i++){
        glm::vec3 normal = glm::vec3(0.0f,0.0f,0.0f);
        HalfEdge* e0 = _vs[i]->e;
        
        HalfEdge* e = e0;
        if(e->oppo == nullptr) continue;//dealing with imperfect meshes
        normal += crossOppoWithNext(e);
        if(e->next->oppo == nullptr) continue;//dealing with imperfect meshes
        e = e->next->oppo;
        
        while(e != e0){
            normal += crossOppoWithNext(e);
            if(e->next->oppo == nullptr) break;//dealing with imperfect meshes
            e = e->next->oppo;
        }
        
        _vs[i]->normal = glm::normalize(normal);
    }
}

void MeshHDS::setupBuffers(){
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    
    for (int i = 0; i < _vs.size(); i++){
        vertices.push_back(_vs[i]->position.x);
        vertices.push_back(_vs[i]->position.y);
        vertices.push_back(_vs[i]->position.z);
        vertices.push_back(_vs[i]->normal.x);
        vertices.push_back(_vs[i]->normal.y);
        vertices.push_back(_vs[i]->normal.z);
    }
    
    for (int i = 0; i < _fs.size(); i++){
        for (int j = 0; j < _fs[i]->indices_v.size(); j++){
            indices.push_back(_fs[i]->indices_v[j]);
        }
    }
    
    indicesSize = (int)indices.size();
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void MeshHDS::Render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos, Light light, Material material){
    lighting_shader.Use();
    
    lighting_shader.setVec3("viewPos", viewPos);
        
    lighting_shader.setVec3("light.position", light.position);
    lighting_shader.setVec3("light.ambient", light.ambient);
    lighting_shader.setVec3("light.diffuse", light.diffuse);
    lighting_shader.setVec3("light.specular", light.specular);
        
        
    // material properties
    lighting_shader.setVec3("material.ambient", material.ambient);
    lighting_shader.setVec3("material.diffuse", material.diffuse);
    lighting_shader.setVec3("material.specular", material.specular);
    lighting_shader.setFloat("material.shininess", material.shininess);
    
    // model transform properties
    lighting_shader.setMat4("model", model);
    lighting_shader.setMat4("view", view);
    lighting_shader.setMat4("projection", projection);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void MeshHDS::Render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 color){
    constClr_shader.Use();
    
    constClr_shader.setVec3("Color", color);
    
    // model transform properties
    constClr_shader.setMat4("model", model);
    constClr_shader.setMat4("view", view);
    constClr_shader.setMat4("projection", projection);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}
