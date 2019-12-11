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
public:
    HalfEdge *next, *oppo;
    Face *f;
    Vertex *v;
    
    HalfEdge(){}
    ~HalfEdge(){}
};

class Vertex{
public:
    HalfEdge *e;
    glm::vec3 position;
    glm::vec3 normal;
    Vertex(){}
    ~Vertex(){}
};

class Face{
public:
    HalfEdge *e;
    std::vector<int> indices_v;
    Face(){}
    ~Face(){}
};

class MeshHDS{
private:
    std::vector<Face*> _fs;
    std::vector<Vertex*> _vs;
    std::vector<HalfEdge*> _es;
    GLuint VAO, VBO, EBO;
    int indicesSize;
    Shader lighting_shader;
    Shader constClr_shader;
    
    bool loadDataFromFile(const char* path, std::vector<Vertex*>& vs, std::vector<Face*>& fs);
    void constructFromData(std::vector<Vertex*> vs, std::vector<Face*> fs);
    void computeNormals();
    void setupBuffers();
    
public:
    MeshHDS():lighting_shader("shaders/lighting.vert.glsl", "shaders/lighting.frag.glsl"),
              constClr_shader("shaders/constColor.vert.glsl", "shaders/constColor.frag.glsl"){};
    MeshHDS(const char* path);
    MeshHDS(std::vector<Vertex*> vs, std::vector<Face*> fs);
    ~MeshHDS(){};
    
    void Render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 color);
    void Render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos, Light light, Material material);
};

#endif /* HalfEdge_hpp */
