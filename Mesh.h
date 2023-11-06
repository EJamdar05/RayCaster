//
//  Mesh.h
//  RayTrace
//
//  Created by Eshaq Jamdar on 11/6/23.
//

#ifndef Mesh_h
#define Mesh_h
#include "SceneObject.h"
class Mesh : public SceneObject {
    bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) { return false; }
    void draw() { }
};

#endif /* Mesh_h */
