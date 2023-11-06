//
//  RenderCam.h
//  RayTrace
//
//  Created by Eshaq Jamdar on 11/6/23.
//

#ifndef RenderCam_h
#define RenderCam_h
#include "SceneObject.h"
#include "ViewPlane.h"
class RenderCam : public SceneObject {
public:
    RenderCam() {
        position = glm::vec3(0, 0.5, 9);
        aim = glm::vec3(0, 0, -5);
    }
    Ray getRay(float u, float v);
    void draw() { ofDrawBox(position, 1.0); };
    void drawFrustum();

    glm::vec3 aim;
    ViewPlane view;          // The camera viewplane, this is the view that we will render
};
#endif /* RenderCam_h */
