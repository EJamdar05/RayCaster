#ifndef SceneObject_h
#define SceneObject_h

class SceneObject {
public:
    virtual void draw() = 0;    // pure virtual funcs - must be overloaded
    virtual bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) { cout << "SceneObject::intersect" << endl; return false; }

    // any data common to all scene objects goes here
    glm::vec3 position = glm::vec3(0, 0, 0);

    // material properties (we will ultimately replace this with a Material class - TBD)
    //
    ofColor diffuseColor = ofColor::grey;    // default colors - can be changed.
    ofColor specularColor = ofColor::lightGray;
};

#endif /* SceneObject_h */
