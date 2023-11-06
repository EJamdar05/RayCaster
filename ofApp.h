#pragma once

#include "ofMain.h"
#include "ofxGui.h"
//  These classes provide a simple render camera which can can return a ray starting from
//  it's position to a (u, v) coordinate on the view plane.
//
//  The view plane is where we can locate our photorealistic image we are rendering.
//  The field-of-view of the camera by moving it closer/further
//  from the view plane.  The viewplane can be also resized.  When ray tracing an image, the aspect
//  ratio of the view plane should the be same as your image. So for example, the current view plane
//  default size is ( 6.0 width by 4.0 height ).   A 1200x800 pixel image would have the same
//  aspect ratio.
//
//  This is not a complete ray tracer - just a set of skelton classes to start.  The current
//  base scene object only stores a value for the diffuse/specular color of the object (defaut is gray).
//  at some point, we will want to replace this with a Material class that contains these (and other
//  parameters)
//
//  (c) Kevin M. Smith  - 24 September 2018
//
#pragma once

#include "ofMain.h"
#include "Ray.h"
#include "SceneObject.h"
#include "Sphere.h"
#include "Mesh.h"
#include "Light.h"
#include "Plane.h"
#include "RandomColor.hpp"
#include "ViewPlane.h"
#include "RenderCam.h"
#include <glm/gtx/intersect.hpp>


class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void rayTrace();
    void drawGrid() {}
    void guiSetUp();
    void sceneObjectSetUp();
    void lightSetUp();
    void cameraSetUp();
    void rayTraceOperation();
    void changeColor();
    void changeEasyCameraString();
    void changePreviewCameraString();
    void changeSideCameraString();
    void changeRenderCameraString();
    void changePlaneDirection();
    void toggleP(bool&);
    void toggleL(bool&);
    void changePosition(glm::vec3&);
    string generateFileName();
    void drawAxis(glm::vec3 position) {}
    ofColor lambert(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse);
    bool isShadowed(const glm::vec3 &p, const glm::vec3 &l);
    ofColor phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power);
    glm::vec2 getUVCoord(const glm::vec3&, const Plane& plane);
    ofColor textureSearch(const ofImage& texture, const glm::vec2& uv, const Plane& plane);
    ofColor ambient = ofColor::black;

    bool bHide = true;
    bool bShowImage = false;
    bool changePlane = false;
    
    bool isEasyCam = true;
    bool isPreviewCam = false;
    bool isSideCam = false;
    bool isRenderCam = false;
    ofImage texture;
    ofImage textureSpecular;
    
    ofImage textureW;
    ofImage textureSpecularW;
    ofEasyCam  mainCam;
    ofCamera sideCam;
    ofCamera previewCam;
    ofCamera imageCam;
    ofCamera* theCam;    // set to current camera either mainCam or sideCam


    RandomColor colorGenerator;
    
    ofxFloatSlider lightSlider1; //slider to show shared verticies
    ofxFloatSlider lightSlider2; //slider to show shared verticies
    ofxFloatSlider lightSlider3; //slider to show shared verticies
//    ofxFloatSlider lightSlider4; //slider to show shared verticies

    ofxToggle togglePhong;
    ofxToggle toggleLambert;
    ofxToggle moveCameras;
    ofxPanel gui; //gui panel
    ofxButton changeColorButton;
    ofxButton changePlaneDir;
    
    ofParameter<glm::vec3> lightPos1;
    ofParameter<glm::vec3> lightPos2;
    ofParameter<glm::vec3> lightPos3;
    ofParameter<glm::vec3> lightPos4;
    ofParameter<glm::vec3> lightPos5;
    ofParameter<glm::vec3> lightPos6;
    ofParameter<glm::vec3> lightPos7;
    ofParameter<glm::vec3> lightPos8;
    

    // set up one render camera to render image throughn
    //
    RenderCam renderCam;
    ofImage image;
    
    Plane plane;
    ViewPlane vp;
    Light light;

    vector<Sphere> spheres;

    vector<SceneObject*> scene;
    vector<SceneObject*> lights;

    int imageWidth = 600;
    int imageHeight = 400;
    
    ofSoundPlayer ring;
    ofSoundPlayer menu;
    ofSoundPlayer renderSound;
    ofSoundPlayer selectCamera;
    
    string currentFile = "";
    
    
};
