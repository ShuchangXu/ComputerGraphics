//
//  HalfEdge.hpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/5.
//  Copyright © 2019 许书畅. All rights reserved.
//

#ifndef HalfEdge_hpp
#define HalfEdge_hpp

#include <vector>

#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "lighting.hpp"

class HalfEdge;
class Vertex;
class Face;
class MeshHDS;

class HalfEdge{
private:
    double contract_cost;
    glm::vec4 Vt;
    
    void updateVt(glm::mat4 Q);
    glm::mat4 getQsum();
    
public:
    HalfEdge *next, *prev, *oppo;
    Face *f;
    Vertex *v;
    
    HalfEdge(){}
    ~HalfEdge(){}
    
    bool checkManifold();
    void updateCost();
    double getCost(){return contract_cost;};
    glm::vec4 getVt(){return Vt;};
    
    static bool minHeapCmp(HalfEdge* e1, HalfEdge* e2){return e1->getCost() > e2->getCost();};
    
};

class Vertex{
private:
    glm::mat4 Q;
    
public:
    HalfEdge *e;
    glm::vec3 position;
    glm::vec3 normal;
    
    Vertex():Q(0.0f){}
    ~Vertex(){}
    
    bool isAdjacentTo(Vertex* v_in);
    void updateNormal();
    void updateQ();
    glm::mat4 getQ(){return this->Q;};
    int getInDegree();
};

struct VertexIndices{
    std::vector<int> indices_v;
};

class Face{
private:
    glm::mat4 Kp;
    glm::vec4 getPlaneCofficients(bool normalized = true);
    
public:
    HalfEdge *e;
    
    Face():Kp(0.0f){}
    ~Face(){}
    
    glm::vec3 getWeightedNormal(){return glm::vec3(getPlaneCofficients(false));};
    void updateKp();
    glm::mat4 getKp(){return Kp;};
    bool containsEdge(HalfEdge* e_in);
};

class MeshHDS{
private:
    const char* _path;
    int init_face_size;
    
    std::vector<Face*> _fs;
    std::vector<Vertex*> _vs;
    std::vector<HalfEdge*> _es;
    GLuint VAO, VBO, EBO;
    int indicesSize;
    Shader lighting_shader;
    Shader constClr_shader;
    
    bool loadDataFromFile(const char* path, std::vector<Vertex*>& vs, std::vector<VertexIndices*>& fs);
    void constructFromData(std::vector<Vertex*> vs, std::vector<VertexIndices*> fs);
    void setupBuffers();
    bool contractPair();
    
public:
    MeshHDS(const char* path);
    ~MeshHDS(){};
    
    void ReloadModel();
    void SimplifyOnce();
    void SimplifyBy(double percent);
    void Render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 color);
    void Render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos, Light light, Material material);
};

#endif /* HalfEdge_hpp */
