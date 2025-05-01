#include "ofApp.h"

//--------------------------------------------------------------
PlantComponent::PlantComponent(ofVec2f pos, ofVec2f dir, float len, float thick, ofColor col,
                               bool leaf, bool flower,
                               ofImage* leafTex, ofImage* flowerTex,
                               float branchingProb, ofColor stemCol, ofColor leafCol, ofColor flowerCol)
    : position(pos), direction(dir), length(len), thickness(thick),
      color(col), isLeaf(leaf), isFlower(flower),
      leafTexture(leafTex), flowerTexture(flowerTex),
      branchingProbability(branchingProb),
      stemColor(stemCol), leafColor(leafCol), flowerColor(flowerCol), age(0)
{
    maxAge = ofRandom(50, 100);
}

PlantComponent::~PlantComponent() {
    for (auto child : children) delete child;
}

void PlantComponent::update(float deltaTime) {
    age += deltaTime;
    for (int i = children.size() - 1; i >= 0; i--) {
        children[i]->update(deltaTime);
        if (children[i]->isDead()) {
            delete children[i];
            children.erase(children.begin() + i);
        }
    }
    if (!isLeaf && age > 2.0 && children.size() < 3 && ofRandom(1.0) < 0.03) grow();
}

void PlantComponent::draw() {
    ofPushMatrix();
    ofPushStyle();
    ofSetColor(color);
    ofSetLineWidth(thickness);

    if (isLeaf) {
        ofTranslate(position);
        float angle = atan2(direction.y, direction.x) * RAD_TO_DEG;
        ofRotateDeg(angle);
        float scale = 10.0;

        if (isFlower && flowerTexture && flowerTexture->isAllocated()) {
            flowerTexture->draw(-scale/2, -scale/2, scale, scale);
        } else if (leafTexture && leafTexture->isAllocated()) {
            leafTexture->draw(-scale/2, -scale/2, scale, scale);
        }
    } else {
        ofDrawLine(position, position + direction * length);
        for (auto child : children) child->draw();
    }

    ofPopStyle();
    ofPopMatrix();
}

void PlantComponent::grow() {
    int totalDescendants = children.size();
    for (auto child : children) totalDescendants += child->children.size();
    bool tooDense = totalDescendants > 10 || thickness < 0.5;

    bool createLeaf = tooDense || ofRandom(1.0) > branchingProbability;
    bool createFlower = createLeaf && ofRandom(1.0) < 0.3;

    ofVec2f newDir = direction.getRotated(ofRandom(-35, 35));
    ofVec2f newPos = position + direction * length;

    float newLength = length * (createLeaf ? 0.4 : 0.8);
    float newThickness = thickness * 0.8;

    ofColor newColor = createLeaf
        ? (createFlower ? flowerColor : leafColor)
        : stemColor;

    PlantComponent* newComponent = new PlantComponent(
        newPos, newDir, newLength, newThickness, newColor,
        createLeaf, createFlower, leafTexture, flowerTexture,
        branchingProbability, stemColor, leafColor, flowerColor
    );
    children.push_back(newComponent);
}

bool PlantComponent::isDead() {
    return age > maxAge;
}

//--------------------------------------------------------------
Plant::Plant(ofVec2f pos, ofImage* leafTex, ofImage* flowerTex,
             float branchingProb, ofColor stemCol, ofColor leafCol, ofColor flowerCol)
    : position(pos), age(0), flowering(false)
{
    growthRate = ofRandom(0.5, 1.5);
    root = new PlantComponent(
        position, ofVec2f(0, -1), ofRandom(40, 70), ofRandom(3, 6),
        stemCol, false, false,
        leafTex, flowerTex,
        branchingProb, stemCol, leafCol, flowerCol
    );
}

Plant::~Plant() {
    delete root;
}

void Plant::update(float deltaTime) {
    age += deltaTime * growthRate;
    root->update(deltaTime * growthRate);
    if (!flowering && age > 5) flowering = true;
}

void Plant::draw() {
    root->draw();
}

bool Plant::isDead() {
    return age > 100;
}

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofBackground(255);
    time = 0;
    useShader = false;
    plantLimit = 10;
    windStrength = 0.2;
    gravity = 0.1;
    sunlightDirection = 45;
    nightMode = false;
    backgroundColor = ofColor(240, 240, 240);

    leafTexture.load("leaf.png");
    flowerTexture.load("flower.png");
    flowerTexture2.load("flower2.png");

    shader.load("shaders/plant/plant.vert", "shaders/plant/plant.frag");
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    for (int i = 0; i < 3; i++) {
        float branching = ofRandom(0.2f, 0.9f);
        ofColor stem = ofColor(139, ofRandom(60, 100), 19);
        ofColor leaf = ofColor(ofRandom(20, 70), ofRandom(130, 200), ofRandom(20, 70));
        ofColor flower = ofColor(ofRandom(180, 255), ofRandom(100, 200), ofRandom(180, 255));
        ofImage* selectedFlower = ofRandom(1.0) < 0.5 ? &flowerTexture : &flowerTexture2;

        plants.push_back(new Plant(
            ofVec2f(ofRandom(100, ofGetWidth() - 100), ofGetHeight() - 50),
            &leafTexture, selectedFlower,
            branching, stem, leaf, flower
        ));

    }
}

void ofApp::update() {
    time += 0.01;
    for (int i = plants.size() - 1; i >= 0; i--) {
        plants[i]->update(0.1);
        if (plants[i]->isDead()) {
            delete plants[i];
            plants.erase(plants.begin() + i);
        }
    }

    if (plants.size() < plantLimit && ofRandom(1.0) < 0.01) {
        float branching = ofRandom(0.2f, 0.9f);
        ofColor stem = ofColor(139, ofRandom(60, 100), 19);
        ofColor leaf = ofColor(ofRandom(20, 70), ofRandom(130, 200), ofRandom(20, 70));
        ofColor flower = ofColor(ofRandom(180, 255), ofRandom(100, 200), ofRandom(180, 255));
        plants.push_back(new Plant(
            ofVec2f(ofRandom(100, ofGetWidth() - 100), ofGetHeight() - 50),
            &leafTexture, &flowerTexture,
            branching, stem, leaf, flower
        ));
    }
}

void ofApp::draw() {
    if (useShader && shader.isLoaded()) fbo.begin();
    ofBackground(backgroundColor);
    for (auto plant : plants) plant->draw();

    ofSetColor(0);
    ofDrawBitmapString("Generative Plant Art", 20, 20);
    ofDrawBitmapString("Plants: " + ofToString(plants.size()), 20, 40);
//    ofDrawBitmapString("Press 'n' for night mode, 's' to toggle shader", 20, 60);
    ofDrawBitmapString("Click to plant a new seed", 20, 60);

    if (useShader && shader.isLoaded()) {
        fbo.end();
        shader.begin();
        shader.setUniform1f("time", time);
        shader.setUniform1f("windStrength", windStrength);
        shader.setUniform1i("nightMode", nightMode ? 1 : 0);
        shader.setUniformTexture("tex0", fbo.getTexture(), 0);
        fbo.draw(0, 0);
        shader.end();
    }
}

void ofApp::keyPressed(int key) {
    if (key == 'n') {
        nightMode = !nightMode;
        backgroundColor = nightMode ? ofColor(20, 20, 40) : ofColor(240, 240, 240);
    } else if (key == 's') {
        useShader = !useShader;
    } else if (key == '+' || key == '=') {
        plantLimit++;
    } else if (key == '-' && plantLimit > 1) {
        plantLimit--;
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if (plants.size() < plantLimit * 2) {
        float branching = ofRandom(0.2f, 0.9f);
        ofColor stem = ofColor(139, ofRandom(60, 100), 19);
        ofColor leaf = ofColor(ofRandom(20, 70), ofRandom(130, 200), ofRandom(20, 70));
        ofColor flower = ofColor(ofRandom(180, 255), ofRandom(100, 200), ofRandom(180, 255));
        ofImage* selectedFlower = ofRandom(1.0) < 0.5 ? &flowerTexture : &flowerTexture2;

        plants.push_back(new Plant(
            ofVec2f(ofRandom(100, ofGetWidth() - 100), ofGetHeight() - 50),
            &leafTexture, selectedFlower,
            branching, stem, leaf, flower
        ));

    }
}
