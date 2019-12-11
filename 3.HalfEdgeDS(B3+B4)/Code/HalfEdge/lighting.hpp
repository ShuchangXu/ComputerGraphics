//
//  lighting.hpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/6.
//  Copyright © 2019 许书畅. All rights reserved.
//

#ifndef lighting_hpp
#define lighting_hpp

// GL Includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

struct Light {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    
    Light(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular){
        position = _position;
        ambient = _ambient;
        diffuse = _diffuse;
        specular = _specular;
    }
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    
    Material(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _shininess){
        ambient = _ambient;
        diffuse = _diffuse;
        specular = _specular;
        shininess = _shininess;
    }
};


#endif /* lighting_hpp */
