//
//  Sphere.h
//  RayTrace
//
//  Created by Eshaq Jamdar on 11/6/23.
//

#ifndef Sphere_h
#define Sphere_h

#include "SceneObject.h"
#include <glm/gtx/intersect.hpp>

class Sphere : public SceneObject {
public:
    Sphere(glm::vec3 p, float r, ofColor diffuse = ofColor::lightGray) { position = p; radius = r; diffuseColor = diffuse; }
    Sphere() {}
    bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {
        return (glm::intersectRaySphere(ray.p, ray.d, position, radius, point, normal));
    }
    void draw() {
        ofDrawSphere(position, radius);
    }

    float radius = 1.0;
};

#endif /* Sphere_h */
