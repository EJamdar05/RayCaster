#include "ofApp.h"
// Intersect Ray with Plane  (wrapper on glm::intersect*)
//
bool Plane::intersect(const Ray& ray, glm::vec3& point, glm::vec3& normalAtIntersect) {
    float dist;
    bool insidePlane = false;
    bool hit = glm::intersectRayPlane(ray.p, ray.d, position, this->normal, dist);
    if (hit) {
        Ray r = ray;
        point = r.evalPoint(dist);
        normalAtIntersect = this->normal;
        glm::vec2 xrange = glm::vec2(position.x - width / 2, position.x + width / 2);
        glm::vec2 yrange = glm::vec2(position.y - width / 2, position.y + width / 2);
        glm::vec2 zrange = glm::vec2(position.z - height / 2, position.z + height / 2);

        // horizontal
        //
        if (normal == glm::vec3(0, 1, 0) || normal == glm::vec3(0, -1, 0)) {
            if (point.x < xrange[1] && point.x > xrange[0] && point.z < zrange[1] && point.z > zrange[0]) {
                insidePlane = true;
            }
        }
        // front or back
        //
        else if (normal == glm::vec3(0, 0, 1) || normal == glm::vec3(0, 0, -1)) {
            if (point.x < xrange[1] && point.x > xrange[0] && point.y < yrange[1] && point.y > yrange[0]) {
                insidePlane = true;
            }
        }
        // left or right
        //
        else if (normal == glm::vec3(1, 0, 0) || normal == glm::vec3(-1, 0, 0)) {
            if (point.y < yrange[1] && point.y > yrange[0] && point.z < zrange[1] && point.z > zrange[0]) {
                insidePlane = true;
            }
        }
    }
    return insidePlane;
}

// Convert (u, v) to (x, y, z)
// We assume u,v is in [0, 1]
//
glm::vec3 ViewPlane::toWorld(float u, float v) {
    float w = width();
    float h = height();
    return (glm::vec3((u * w) + min.x, (v * h) + min.y, position.z));
}

// Get a ray from the current camera position to the (u, v) position on
// the ViewPlane
//
Ray RenderCam::getRay(float u, float v) {
    glm::vec3 pointOnPlane = view.toWorld(u, v);
    return(Ray(position, glm::normalize(pointOnPlane - position)));
}

/*
    ofApp::lambert
    p: some point of intersection
    norm: normal of the surface
    diffuse: color of obj
    
    return: ofColor value with ambient light added
*/
ofColor ofApp::lambert(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse){
    ofColor result = ambient; //result is the total color value of some surface
    
    //loop through every light in the scene
    for(auto lightObj : lights){
        Light *light = dynamic_cast<Light*>(lightObj); //light object
        //if the shadow ray is not blocked
        if(!isShadowed(p, light->position)){
            glm::vec3 l = normalize(light->position - p); //get direction of the light from the object
            float dotProduct = dot(norm, l); //take dot product of dir of light and normal vector
            //take the max of the dot product
            dotProduct = glm::max(0.0f, dotProduct);
            //add to result the value of ambient * dotProduct * intensity of the light
            result+=diffuse * dotProduct * light->intensity;
        }else{
            //else color the shadowed area as black
            result += ofColor::black;
        }
    }
    return result;
}

/*/
    isShdaowed()
    p: some point on a surface
    l: position of light
    built on top of Lambert shading
*/
bool ofApp::isShadowed(const glm::vec3 &p, const glm::vec3 &l){
    glm::vec3 r = glm::normalize(l-p); //take the normal of light post - p of object from camera pov
    
    //ray of a point on the surface in respect to the light distance
    Ray shadow((p+ 0.001 * r), r); //little bit off the surface (0.001 * r)
    
    for(auto obj : scene){ //for every scene object
        glm::vec3 intersectionPoint;
        glm::vec3 normal;
        //get the intersection of the shadowRay and if it does intersect
        if(obj->intersect(shadow, intersectionPoint, normal)){
            return true;
        }
    }
    return false;
}
/*
    phong()
        vec3 p: point on the surface
        vec3 n: normal of the surface
        ofColor diffuse: object color
        ofColor: specular color vaue
        float power: power for speculaer color coefficient
    returns resulting color for a phong shader for some point
 */
ofColor ofApp::phong(const glm::vec3 &p, const glm::vec3 &norm, const ofColor diffuse, const ofColor specular, float power){
    ofColor result = ambient; //start with ambient color value

    //loop through all the lights
    for(auto lightObj : lights){
        Light *light = dynamic_cast<Light*>(lightObj); //light obj cast

        //if the light is not blocked
        if(!isShadowed(p, light->position)){
            glm::vec3 l = normalize(light->position - p);//get the normal between the light position and point on surface
            float dotProduct = dot(norm, l); //take the dot product of
            dotProduct = max(0.0f, dotProduct);

            //lambert shader
            result+=diffuse * dotProduct * light->intensity;
            //specular color shader
            result += specular * pow(dotProduct, power) * light->intensity;

        }
    }
    return result;
    
}

// Your main ray trace loop
// before phong shading, l and v and n must be (light, view and normal)
void ofApp::rayTrace() {
    
    //itterate through every pixel
    for(int j = 0 ; j < imageHeight ; j++){ //height (y)
        for(int i = 0 ; i < imageWidth ; i++){ //width (x)
            //u and v coord
            double u = (i+0.5) / imageWidth;
            double v = (j+0.5) / imageHeight;
            
            //get the current ray from u,v coord
            Ray ray = renderCam.getRay(u,v); //xyz space (position dir)
            //controls what gets colored on screen
            bool hit = false;
            
            //get the biggest value possible
            double distance = std::numeric_limits<double>::infinity();
            SceneObject *closestObj = nullptr;
            
            //empty objects that get initialized via obj->intersect
            glm::vec3 intersectionPoint;
            glm::vec3 normal;
            
            //loop through each object in the scene
            for(auto obj : scene){
                //if we can intersect
                if(obj->intersect(ray, intersectionPoint, normal)){
                    //subtract the diff between origin and intersection
                    glm::vec3 intersectionDist = intersectionPoint - ray.p;
                    
                    //if the intersection of the ray is closer than the farthest obj
                    //then we have the closest object
                    if(glm::length(intersectionDist) < distance){
                        distance = glm::length(intersectionDist); //set the farthest distance to the intersection of the ray
                        closestObj = obj; //closetsObj is the curr obj
                    }
                }
                //if closestObj is not null, we hit something
                if(closestObj) hit = true;
            }
            
            //set the color based off the closest object the ray has hit
            if(hit){
                if(dynamic_cast<Plane*>(closestObj)){
                    Plane * plane = dynamic_cast<Plane*>(closestObj);
                    if(plane->normal == glm::vec3(0.0f, 1.0f, 0.0f)){
                        glm::vec2 uv = getUVCoord(intersectionPoint, *plane);
                        ofColor textureColor = textureSearch(texture, uv, *plane);
                        ofColor specularColor = textureSearch(textureSpecular, uv, *plane);
                        closestObj->diffuseColor = textureColor;
                        closestObj->specularColor = specularColor;
                    }else if(plane->normal == glm::vec3(0.0f, 0.0f, 1.0f)){
                        glm::vec2 uv = getUVCoord(intersectionPoint, *plane);
                        ofColor textureColor = textureSearch(textureW, uv, *plane);
                        ofColor specularColor = textureSearch(textureSpecularW, uv, *plane);
                        closestObj->diffuseColor = textureColor;
                        closestObj->specularColor = specularColor;
                    }
                }
                ofColor shaderColor;
                if(togglePhong){
                    shaderColor = phong(intersectionPoint, normal, closestObj->diffuseColor, ofColor::white, 20.0f);
                }else if (toggleLambert){
                    shaderColor = lambert(intersectionPoint, normal, closestObj->diffuseColor);
                }
                image.setColor(i, j, shaderColor);
            }else{
                image.setColor(i, j, colorGenerator.getRandomColor());
            }
            
        }
    }
    image.mirror(true, false);
    
    
}

glm::vec2 ofApp::getUVCoord(const glm::vec3& p, const Plane& plane){
    cout<<"Normal: "<<plane.normal<<endl;
    //get the horizontal plane u,v
    if(plane.normal == glm::vec3(0.0f, 1.0f, 0.0f)) {
        float u = fmod((p.x - plane.position.x) / plane.width, 1.0f);
        float v = fmod((p.z - plane.position.z) / plane.height, 1.0f);
        return glm::vec2(u, v);
        
    //get the vertical plane u,v
    }else if(plane.normal == glm::vec3(0.0f, 0.0f, 1.0f)) {
        float u = fmod((p.x - plane.position.x) / plane.width, 1.0f);
        float v = fmod((p.y - plane.position.y) / plane.height, 1.0f);
        return glm::vec2(u, v);
    }
    return glm::vec2(0.0f, 0.0f);
}

ofColor ofApp::textureSearch(const ofImage& texture, const glm::vec2& uv, const Plane& plane){
    float x = fmod((uv.x * 10) * texture.getWidth(), texture.getWidth()); //x coord
    float y = fmod((uv.y * 10) * texture.getHeight(), texture.getHeight()); //y coord
    return texture.getColor(x,y);
}

void ofApp::guiSetUp(){
    gui.setup();
    gui.add(lightSlider1.setup("Light 1 Intensity", 0.3, 0.05, 1));
    gui.add(lightSlider2.setup("Light 2 Intensity", 0.3, 0.05, 1));
    gui.add(lightSlider3.setup("Light 3 Intensity", 0.3, 0.05, 1));
    gui.add(togglePhong.setup("Phong Shader", true));
    gui.add(toggleLambert.setup("Lambert Shader", false));
    gui.add(moveCameras.setup("Move Cameras", true));
    gui.add(lightPos1.set("Adjust Light 1 Position", ofVec3f(10, 5, 0), ofVec3f(-100,-100,-100), ofVec3f(100, 100, 100)));
    gui.add(lightPos2.set("Adjust Light 2 Position", ofVec3f(-10,3,10), ofVec3f(-100,-100,-100), ofVec3f(100, 100, 100)));
    gui.add(lightPos3.set("Adjust Light 3 Position", ofVec3f(-3,1,10), ofVec3f(-100,-100,-100), ofVec3f(100, 100, 100)));
    gui.add(lightPos4.set("Adjust Plane Vert", ofVec3f(0.0f, -4.08f, 2.0f), ofVec3f(-100,-100,-100), ofVec3f(100, 100, 100)));
    gui.add(lightPos5.set("Adjust Plane Hori", ofVec3f(0.0f, -0.0255f, 0.0f), ofVec3f(-100,-100,-100), ofVec3f(100, 100, 100)));
    
    gui.add(lightPos6.set("Ball 1", ofVec3f(0.5,1.05,2.0), ofVec3f(-100,-100,-100), ofVec3f(100, 100, 100)));
    gui.add(lightPos7.set("Ball 2", ofVec3f(1.0,0.198,2.5), ofVec3f(-100,-100,-100), ofVec3f(100, 100, 100)));
    gui.add(lightPos8.set("Ball 3", ofVec3f(.1,0.322,3.0), ofVec3f(-100,-100,-100), ofVec3f(100, 100, 100)));
    gui.add(changeColorButton.setup("Change Colors"));
    gui.add(changePlaneDir.setup("Change Plane Direction"));
    changeColorButton.addListener(this, &ofApp::changeColor);
    changePlaneDir.addListener(this, &ofApp::changePlaneDirection);
    ring.load("confirm.wav");
    menu.load("menu.wav");
    menu.play();
    renderSound.load("render.wav");
    selectCamera.load("select.wav");

}

void ofApp::changePlaneDirection(){
    ofColor color = scene[0]->diffuseColor;
    if(!changePlane){
        scene[0] = new Plane(glm::vec3(0.0f, -4.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), color);
        changePlane = true;
    }else{
        scene[0] = new Plane(glm::vec3(0.0f, -4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), color);
        changePlane = false;
    }
}

void ofApp::sceneObjectSetUp(){
    float sphereRadius = 0.5f;
    scene.push_back(new Plane(glm::vec3(0.0f, -4.08f, 2.0f), glm::vec3(0.0f, 0.0f, 1.0f), colorGenerator.getRandomColor()));
    scene.push_back(new Plane(glm::vec3(0.0f, -0.0255f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), colorGenerator.getRandomColor()));

    scene.push_back(new Sphere(glm::vec3(0.5,-.3,2.0), sphereRadius, colorGenerator.getRandomColor()));
    scene.push_back(new Sphere(glm::vec3(1.0,-1.0,2.5), sphereRadius, colorGenerator.getRandomColor()));
    scene.push_back(new Sphere(glm::vec3(.1,-0.8,3.0), sphereRadius, colorGenerator.getRandomColor()));
}

void ofApp::cameraSetUp(){
    theCam = &mainCam; //set camer ptr to the easy cam (current camera)

    // setup cameras
    // setting up sideCam and topCam from a certain point from the origin
    mainCam.setDistance(10);
    mainCam.setNearClip(.1);
    
    renderCam = RenderCam();
    
    
    sideCam.setPosition(glm::vec3(15, 0, 0));
    sideCam.lookAt(glm::vec3(0, 0, 0)); //origin
    sideCam.setNearClip(.1);
    
    previewCam.setPosition(glm::vec3(0, 0, 10));
    previewCam.setNearClip(.1);
}

void ofApp::lightSetUp(){
    lights.push_back(new Light(glm::vec3(10,5,0), 0.2f));
    lights.push_back(new Light(glm::vec3(-10,3,10), 0.5f));
    lights.push_back(new Light(glm::vec3(-3,1,10), 0.8f));
    lights.push_back(new Light(glm::vec3(2,4,-10), 0.8f));
    lights.push_back(new Light(glm::vec3(-1,3,-5), 0.8f));
}

void ofApp::changeColor(){
    for(auto obj : scene){
        obj->diffuseColor = colorGenerator.getRandomColor();
    }
    ring.play();
}
//--------------------------------------------------------------
void ofApp::setup(){
    cameraSetUp();
    ofNoFill();
    sceneObjectSetUp();
    image.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);
    lightSetUp();
    texture.load(ofToDataPath("wall.jpg", true));
    textureSpecular.load(ofToDataPath("spec.jpg", true));
    textureW.load(ofToDataPath("wallpaper.jpg", true));
    textureSpecularW.load(ofToDataPath("wallpaper_specular.jpg", true));
    guiSetUp();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    theCam->begin(); //start the main cam
    cout<<theCam->getPosition()<<endl;
    for (auto obj : scene) {
        //go through every obj in the scene
        ofFill(); //fill the obj
        ofSetColor(obj->diffuseColor); //set color based off obj color prop
        obj->draw(); //draw obj
    }
    
    for(auto obj : lights){
        ofFill();
        ofSetColor(ofColor::yellow);
        obj->draw();
    }
    
    ofSetColor(ofColor::white); //set the cam and view plane white
    ofNoFill();
    vp.draw();
    renderCam.draw();
    
    if(bShowImage){
        cout<<"Current file: "<<currentFile<<endl;
        ofImage renderedImage;
        renderedImage.load(ofToDataPath(currentFile, true));
        renderedImage.draw(-10,-10);
        theCam = &imageCam;
        ofSetColor(ofColor::white);
        ofDrawBitmapString("Image Saved to: "+currentFile, -10, -30);
        imageCam.setPosition(glm::vec3(302, 197.302, 668));
    }
    
    if(lightSlider1){
        dynamic_cast<Light*>(lights[0])->intensity = lightSlider1;
    }
    if (lightSlider2){
        dynamic_cast<Light*>(lights[1])->intensity = lightSlider2;
    }
    if (lightSlider3){
        dynamic_cast<Light*>(lights[2])->intensity = lightSlider3;
    }
    
    if(moveCameras){
        dynamic_cast<Light*>(lights[0])->position = lightPos1;
        dynamic_cast<Light*>(lights[1])->position = lightPos2;
        dynamic_cast<Light*>(lights[2])->position = lightPos3;
        dynamic_cast<Plane*>(scene[0])->position = lightPos4;
        dynamic_cast<Plane*>(scene[1])->position = lightPos5;
        dynamic_cast<Sphere*>(scene[2])->position = lightPos6;
        dynamic_cast<Sphere*>(scene[3])->position = lightPos7;
        dynamic_cast<Sphere*>(scene[4])->position = lightPos8;
    }
    
    

    theCam->end();
    gui.draw();
    
    ofSetColor(ofColor::green);
    if(isEasyCam){
        ofDrawBitmapString("Main Camera (Easy Cam)", 10, 10);
    }else if (isSideCam){
        ofDrawBitmapString("Side Camera", 10, 10);
    }else if (isPreviewCam){
        ofDrawBitmapString("Preview Camera", 10, 10);
    }else if (isRenderCam){
        ofDrawBitmapString("Rendered Image", 10, 10);
    }
}

//generates a file name based off current time and date
string ofApp::generateFileName(){
    string baseFileName = "RayCastedImage_";
    time_t now = time(0);
    string currentTime = ctime(&now);
    currentTime.substr(0, currentTime.length()-1);
    baseFileName+=currentTime;
    return baseFileName+"_.png";
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case OF_KEY_F1:
            if(bShowImage) bShowImage = false;
            theCam = &sideCam;
            selectCamera.play();
            changeSideCameraString();
            break;
        case OF_KEY_F2:
            if(bShowImage) bShowImage = false;
            theCam = &mainCam;
            selectCamera.play();
            changeEasyCameraString();
            break;
        case OF_KEY_F3:
            if(bShowImage){
                bShowImage = false;}
            theCam = &previewCam;
            selectCamera.play();
            changePreviewCameraString();
            break;
        case 'r':{
            string fileName = generateFileName();
            rayTrace();
            image.save(fileName);
            currentFile = fileName;
            bShowImage = true;
            renderSound.play();
            changeRenderCameraString();
            break;
        }
        default:
            cout<<"Not recognized input"<<endl;
    }
}

void ofApp::changeSideCameraString(){
    if(isEasyCam){
        isEasyCam = false;
        isSideCam = true;
    }else if(isPreviewCam){
        isPreviewCam = false;
        isSideCam = true;
    }else if(isRenderCam){
        isRenderCam = false;
        isSideCam = true;
    }
}

void ofApp::changeEasyCameraString(){
    if(isSideCam){
        isEasyCam = true;
        isSideCam = false;
    }else if(isPreviewCam){
        isEasyCam = true;
        isPreviewCam = false;
    }else if(isRenderCam){
        isRenderCam = false;
        isEasyCam = true;
    }
}

void ofApp::changePreviewCameraString(){
    if(isEasyCam){
        isEasyCam = false;
        isPreviewCam = true;
    }else if(isSideCam){
        isPreviewCam = true;
        isSideCam = false;
    }else if(isRenderCam){
        isRenderCam = false;
        isSideCam = true;
    }
}

void ofApp::changeRenderCameraString(){
    if(isEasyCam){
        isEasyCam = false;
        isRenderCam = true;
    }else if(isSideCam){
        isSideCam = false;
        isRenderCam = true;
    }else if(isPreviewCam){
        isPreviewCam = false;
        isRenderCam = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
