#pragma once

#include "ofMain.h"

class PlantComponent {
public:
    ofVec2f position;
    ofVec2f direction;
    float length;
    float thickness;
    float age;
    float maxAge;
    ofColor color;
    bool isLeaf;
    bool isFlower;

    ofImage* leafTexture;
    ofImage* flowerTexture;
    float branchingProbability;
    ofColor stemColor, leafColor, flowerColor;

    std::vector<PlantComponent*> children;

    PlantComponent(ofVec2f pos, ofVec2f dir, float len, float thick, ofColor col,
                   bool leaf, bool flower,
                   ofImage* leafTex, ofImage* flowerTex,
                   float branchingProb, ofColor stemCol, ofColor leafCol, ofColor flowerCol);

    ~PlantComponent();
    void update(float deltaTime);
    void draw();
    void grow();
    bool isDead();
};

class Plant {
public:
    PlantComponent* root;
    ofVec2f position;
    float age;
    float growthRate;
    bool flowering;

    Plant(ofVec2f pos, ofImage* leafTex, ofImage* flowerTex,
          float branchingProb, ofColor stemCol, ofColor leafCol, ofColor flowerCol);
    ~Plant();

    void update(float deltaTime);
    void draw();
    bool isDead();
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);

    std::vector<Plant*> plants;
    int plantLimit;

    ofImage leafTexture, flowerTexture, flowerTexture2;
    ofShader shader;
    ofFbo fbo;
    float time;
    float windStrength;
    float gravity;
    float sunlightDirection;
    bool useShader;
    bool nightMode;
    ofColor backgroundColor;
};
