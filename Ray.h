#ifndef Ray_h
#define Ray_h
class Ray {
public:
    Ray(glm::vec3 p, glm::vec3 d) { this->p = p; this->d = d; }
    void draw(float t) { ofDrawLine(p, p + t * d); }

    glm::vec3 evalPoint(float t) {
        return (p + t * d);
    }

    glm::vec3 p, d;
};

#endif /* Ray_h */
