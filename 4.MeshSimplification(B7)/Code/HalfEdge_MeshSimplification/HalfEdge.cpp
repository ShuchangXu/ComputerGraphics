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
#include <unordered_map>
#include <glm/gtx/string_cast.hpp>

glm::vec4 Face::getPlaneCofficients(bool normalized){
    //counter-clockwise: v0, v1, v2
    glm::vec3 v0 = this->e->v->position;
    glm::vec3 v1 = this->e->next->v->position;
    glm::vec3 v2 = this->e->prev->v->position;
    glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);
    if(normalized) normal = glm::normalize(normal);
    return glm::vec4(normal, -glm::dot(normal, v0));
}

void Face::updateKp(){
    glm::vec4 p = getPlaneCofficients(true);
    double a=p.x, b=p.y, c=p.z, d=p.w;
    Kp = glm::mat4(
                    a*a, a*b, a*c, a*d,
                    a*b, b*b, b*c, b*d,
                    a*c, b*c, c*c, d*c,
                    a*d, b*d, c*d, d*d
                    );
}

bool Face::containsEdge(HalfEdge *e_in){
    HalfEdge* pe = e;
    do{
        if(pe == e_in) return true;
        pe = pe -> next;
    }while(pe != e);
    return false;
}

int Vertex::getInDegree(){
    int indegree = 0;
    HalfEdge* pe = this->e;
    do{
        indegree++;
        pe = pe->next->oppo;
    }while(pe!=nullptr && pe!=this->e);
    return indegree;
}

void Vertex::updateNormal(){
    glm::vec3 normal = glm::vec3(0.0f,0.0f,0.0f);
    
    HalfEdge* pe = this->e;
    
    do{
        normal += pe->f->getWeightedNormal();
        pe = pe->next->oppo;
    }while(pe != nullptr && pe != this->e);
    
    this->normal = glm::normalize(normal);
}

void Vertex::updateQ(){
    Q = glm::mat4(0.0f);
    
    HalfEdge* pe = this->e;
    
    do{
        Q += pe->f->getKp();
        pe = pe->next->oppo;
    }while(pe != nullptr && pe != this->e);
}

glm::mat4 HalfEdge::getQsum(){
    return v->getQ() + prev->v->getQ();
}

void HalfEdge::updateVt(glm::mat4 Q){
    //glm::mat4 is in column-major order
    //glm::mat4 is accessed by mat[col][row]
    glm::mat4 D = glm::mat4(Q[0][0], Q[1][0], Q[2][0], 0,
                            Q[0][1], Q[1][1], Q[2][1], 0,
                            Q[0][2], Q[1][2], Q[2][2], 0,
                            Q[0][3], Q[1][3], Q[2][3], 1);
    
    if(fabs(glm::determinant(D))<1e-10){
        Vt = glm::vec4((v->position + prev->v->position)/2.0f, 1.0f);
    }else{
        Vt = glm::inverse(D) * glm::vec4(0, 0, 0, 1);
    }
}

bool HalfEdge::checkManifold(){
    if(this->v->getInDegree() < 4) return false;
    if(this->next->v->getInDegree() < 4) return false;
    if(this->prev->v->getInDegree() < 4) return false;
    if(this->oppo->next->v->getInDegree() < 4) return false;
    return true;
}

void HalfEdge::updateCost(){
    //prevent non-manifold mesh
    contract_cost = DBL_MAX;
    if(!checkManifold()) return;
    
    glm::mat4 Qsum = getQsum();
    updateVt(Qsum);
    contract_cost = glm::dot(Vt, Qsum * Vt);
}

bool MeshHDS::contractPair(){
    if(_fs.size() < 0.1 * init_face_size) return false;
    std::pop_heap(_es.begin(),_es.end(),HalfEdge::minHeapCmp);
    HalfEdge* e2contract = _es.back();
    
    if(e2contract->getCost() > DBL_MAX / 2) {
        return false;
    }
    
    Vertex* v1 = e2contract -> prev -> v;
    Vertex* v2 = e2contract -> v;
    glm::vec4 vt = e2contract -> getVt();
    
    /* -------------------Step A: Preparation------------------- */
    //for vertices linked to v1, make sure its pointed e won't be erased
    HalfEdge* pe = e2contract->prev;
    do{
        pe = pe -> next;
        Vertex* tmpV = pe->v;
        HalfEdge* tmpE = tmpV->e;
        do{
            if(!e2contract->f->containsEdge(tmpE) &&
               !e2contract->oppo->f->containsEdge(tmpE)){
                tmpV->e = tmpE;
                break;
            }
            tmpE = tmpE -> next -> oppo;
        }while(tmpE != nullptr && tmpE != tmpV->e);
        pe = pe -> oppo;
    }while(pe != nullptr && pe != e2contract->prev);
    
    /* -------------------Step B: Rebuild Connections------------------- */
    //for half-edges linked to v1, relink them to v2
    pe = e2contract->prev;
    do{
        pe -> v = v2;
        pe = pe -> next -> oppo;
    }while(pe != nullptr && pe != e2contract->prev);
        
    //mind the order:
    //0. deal with e2contract_oppo before deleting e2contract
    //1. relink oppo
    //2. remove v1 - v2 faces
    //3. remove half_edges on v1 - v2 faces
    if(e2contract -> oppo != nullptr){
        HalfEdge* e2contract_oppo = e2contract -> oppo;
        e2contract_oppo->prev->oppo->oppo = e2contract_oppo->next->oppo;
        e2contract_oppo->next->oppo->oppo = e2contract_oppo->prev->oppo;
        _fs.erase(std::remove(_fs.begin(), _fs.end(), e2contract_oppo->f), _fs.end());
        _es.erase(std::remove(_es.begin(), _es.end(), e2contract_oppo->prev), _es.end());
        _es.erase(std::remove(_es.begin(), _es.end(), e2contract_oppo->next), _es.end());
        _es.erase(std::remove(_es.begin(), _es.end(), e2contract_oppo), _es.end());
        delete e2contract_oppo->f;
        delete e2contract_oppo->prev;
        delete e2contract_oppo->next;
        delete e2contract_oppo;
    }
    
    e2contract->prev->oppo->oppo = e2contract->next->oppo;
    e2contract->next->oppo->oppo = e2contract->prev->oppo;
    _fs.erase(std::remove(_fs.begin(), _fs.end(), e2contract->f), _fs.end());
    _es.erase(std::remove(_es.begin(), _es.end(), e2contract->prev), _es.end());
    _es.erase(std::remove(_es.begin(), _es.end(), e2contract->next), _es.end());
    _es.erase(std::remove(_es.begin(), _es.end(), e2contract), _es.end());
    delete e2contract->f;
    delete e2contract->prev;
    delete e2contract->next;
    delete e2contract;
    
    //remove v1
    _vs.erase(std::remove(_vs.begin(), _vs.end(), v1), _vs.end());
    delete v1;
    
    /* -------------------Step C: Update Parameters------------------- */
    /*update all simplification related parameters
      update normals
      make sure _es is still heap*/
    
    //update v2 position
    v2->position = glm::vec3(vt);
    
    //update Kp for v2-adjacent faces
    pe = v2->e;
    do{
        pe -> f -> updateKp();
        pe = pe -> next -> oppo;
    }while(pe != nullptr && pe != v2->e);
    
    //update (Q & Normal) for v2
    v2 -> updateQ();
    v2 -> updateNormal();
    
    //update (Q & Normal) for v2-adjacent vertices
    pe = v2->e;
    do{
        pe = pe -> next;
        pe -> v -> updateQ();
        pe -> v -> updateNormal();
        pe = pe -> oppo;
    }while(pe != nullptr && pe != v2->e);
    
    //update cost for v2-linked half-edges
    pe = v2->e;
    do{
        pe -> updateCost();
        pe = pe -> next;
        pe -> updateCost();
        pe = pe -> oppo;
    }while(pe != nullptr && pe != v2->e);
    
    //make sure _es is still heap
    std::make_heap(_es.begin(),_es.end(),HalfEdge::minHeapCmp);
    
    return true;
}

MeshHDS::MeshHDS(const char* path):
lighting_shader("shaders/lighting.vert.glsl", "shaders/lighting.frag.glsl"),
constClr_shader("shaders/constColor.vert.glsl", "shaders/constColor.frag.glsl")
{
    _path = path;
    ReloadModel();
}

void MeshHDS::ReloadModel(){
    std::vector<Vertex*> vs;
    std::vector<VertexIndices*> fs;
    if(loadDataFromFile(_path, vs, fs)){
        constructFromData(vs, fs);
    }
    init_face_size = (int)fs.size();
    fs.clear();
    std::cout<<init_face_size<<std::endl;
}

bool MeshHDS::loadDataFromFile(const char* path, std::vector<Vertex*>& vs, std::vector<VertexIndices*>& fs)
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
            VertexIndices* f = new VertexIndices();
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

void MeshHDS::constructFromData(std::vector<Vertex*> vs, std::vector<VertexIndices*> fs){
    _vs = vs;
    _fs.clear();
    _es.clear();
    
    std::map<std::pair<int, int>, HalfEdge*> Vs2HF;
    std::map<HalfEdge*, std::pair<int, int>> HF2Vs;
    
    for(int i = 0; i < fs.size(); i++){
        int e_curSize = (int)_es.size();
        int v_size = (int)fs[i] -> indices_v.size();
        
        //avoid wrong indices in Obj file
        bool isVerticesInRange = true;
        
        for(int j = 0; j < v_size; j++){
            if(fs[i]->indices_v[j] > _vs.size()-1){
                isVerticesInRange = false;
                break;
            }
        }
        
        if(!isVerticesInRange) continue;
        
        Face* f = new Face();
        
        //set up cross references of e, f, v, and HF
        for(int j = 0; j < v_size; j++){
            int vj = fs[i]->indices_v[j];
            
            HalfEdge* e = new HalfEdge();
            e->f = f;
            e->v = _vs[vj];
            _es.push_back(e);
            
            f->e = e;
            _vs[vj]->e = e;
            
            int pj = (v_size+j-1) % v_size;
            int vpj = fs[i]->indices_v[pj];
            Vs2HF[std::make_pair(vpj, vj)] = e;
            HF2Vs[e] = std::make_pair(vpj, vj);
        }
        
        //set up 'prev' and 'next' for all half edges
        for(int j = 0; j < v_size; j++){
            _es[e_curSize + j] -> prev = _es[e_curSize + (v_size+j-1)%v_size];
            _es[e_curSize + j] -> next = _es[e_curSize + (j+1)%v_size];
        }
        
        //Add face to faces
        _fs.push_back(f);
    }
    
    //set up opposite half edges
    for(int i = 0; i < _es.size(); i++){
        std::pair<int, int> vs = HF2Vs[_es[i]];
        HalfEdge* tmp = Vs2HF[std::make_pair(vs.second, vs.first)];
        if(tmp!= nullptr) _es[i]->oppo = tmp;
        else std::cout<<"non-manifold"<<std::endl;
    }
    
    //update Simplification Related Parameters
    for(int i = 0; i < _fs.size(); i++){ _fs[i]->updateKp(); }
    for(int i = 0; i < _vs.size(); i++){ _vs[i]->updateQ(); }
    for(int i = 0; i < _es.size(); i++){ _es[i]->updateCost(); }
    std::make_heap(_es.begin(),_es.end(),HalfEdge::minHeapCmp);
    //update normal
    for(int i = 0; i < _vs.size(); i++){ _vs[i]->updateNormal(); }
    
    setupBuffers();
}

void MeshHDS::setupBuffers(){
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    std::unordered_map<Vertex*,int> verticesIndex;
    
    for (int i = 0; i < _vs.size(); i++){
        vertices.push_back(_vs[i]->position.x);
        vertices.push_back(_vs[i]->position.y);
        vertices.push_back(_vs[i]->position.z);
        vertices.push_back(_vs[i]->normal.x);
        vertices.push_back(_vs[i]->normal.y);
        vertices.push_back(_vs[i]->normal.z);
        
        verticesIndex[_vs[i]]=i;
    }
    
    for (int i = 0; i < _fs.size(); i++){
        HalfEdge* e0 = _fs[i]->e;
        HalfEdge* e = e0->next;
        indices.push_back(verticesIndex[e->v]);
        
        while(e!=e0){
            e = e->next;
            indices.push_back(verticesIndex[e->v]);
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

void MeshHDS::SimplifyOnce(){
    contractPair();
    setupBuffers();
}

void MeshHDS::SimplifyBy(double percent){
    for(int i = 0; i < init_face_size * percent/2; i++){
        contractPair();
        std::cout<<i<<std::endl;
    }
    setupBuffers();
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
