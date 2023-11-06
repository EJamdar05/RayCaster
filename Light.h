//
//  Light.h
//  RayTrace
//
//  Created by Eshaq Jamdar on 11/6/23.
//

#ifndef Light_h
#define Light_h
#include "SceneObject.h"

class Light : public SceneObject {
public:
    float intensity;
    Light(){}
    Light(glm::vec3 p, float i){
        position = p;
        intensity = i;
    };
    
    void draw(){
        ofDrawSphere(position, 0.5);
    }
};

#endif /* Light_h */
