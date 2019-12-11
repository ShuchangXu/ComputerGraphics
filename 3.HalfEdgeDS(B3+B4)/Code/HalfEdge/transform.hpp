//
//  transform.hpp
//  LearningOpenGL
//
//  Created by 许书畅 on 2019/12/6.
//  Copyright © 2019 许书畅. All rights reserved.
//

#ifndef transform_hpp
#define transform_hpp

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Transform{
private:
    const glm::vec3 position_init;
    const glm::vec3 rotation_init;
    const glm::vec3 scale_init;
    
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 model;
    
    void updateModelMatrix(){
        glm::mat4 mat(1);
        mat = glm::scale(mat, scale);
        mat = glm::translate(mat, position);
        mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f,0.0f,0.0f));
        mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f,1.0f,0.0f));
        mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f,0.0f,1.0f));
        model = mat;
    }
    
public:
    Transform():position_init(0.0f), rotation_init(0.0f), scale_init(1.0f), model(1){
        setTransform(position_init, rotation_init, scale_init);
    }
    
    Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl):position_init(pos), rotation_init(rot), scale_init(scl){
        setTransform(position_init, rotation_init, scale_init);
    }
    
    void setTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl){
        position = pos;
        rotation = rot;
        scale = scl;
        updateModelMatrix();
    }
    
    void scaleBy(double factor){
        scale = scale_init * glm::vec3(factor, factor, factor);
        updateModelMatrix();
    }
    
    void Rotate(glm::vec3 rot_delta){
        rotation += rot_delta;
        updateModelMatrix();
    }
    
    glm::mat4 getModelMat(){
        return model;
    }
};

#endif /* transform_hpp */
