#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetFullscreen(true);
    ofSetVerticalSync(true);
    
    astra.setup();
    //astra.enableRegistration(true);
    
    astra.initColorStream();
    astra.initDepthStream();
    astra.initPointStream();
    
    gui.setup();
    gui.add( near.set("near", 30, 0, 8000) );
    gui.add( far.set("far", 1800, 0, 8000) );
    
    
    grayImage.allocate(1280, 720);
    
    mesh.setMode(OF_PRIMITIVE_POINTS);
    myFont.load("OpenSans-Semibold.ttf", 6);
    boughtfont.load("OpenSans-Semibold.ttf", 100);
    
    
    bDrawPointCloud = false;
    bPointCloudUseColor = false;
    bUseRegistration = true;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    
    astra.setup();
    astra.enableRegistration(bUseRegistration);
    
    astra.initColorStream();
    
    astra.initDepthStream();
    astra.initPointStream();
    astra.initHandStream();
    
    click.setSpeed(1.0f);
    click.setLoop(false);
    
    background.load("website2.png");
    cursor.load ("cursor.png");
    click.load("mouse.wav");
    buy.load("h-bigButton.png");
    buy1.load("h-buyButton.png");
    error.load ("error.png");
    
    
    }

//--------------------------------------------------------------
void ofApp::reset() {
    startTime = (int)ofGetElapsedTimeMillis();
    preTime = (int)ofGetElapsedTimeMillis();
    nowTime = (int)ofGetElapsedTimeMillis();
    for (int i = 0; i < 144; i++) {
        for (int j = 0; j < 90; j++) {
            count[i][j] = j * 100 + ofRandom(0, 4000);
            p[i][j] = ofVec3f(i * 10, j * 10, 0);
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
   // ofSoundUpdate();
    astra.setDepthClipping(near.get(), far.get());
    astra.update();
    
    //update stages---------------------------------------------
    for (auto& hand : astra.getHandsDepth()) {
        auto& pos_real = hand.second;
        ofPoint pos = ofPoint((int)(pos_real.x*2.25),(int)(pos_real.y*1.875));
        if(pos.x > 47 && pos.x < 409 && pos.y > 399 && pos.y < 437) {
            stage = 2;
        }
        else if(pos.x > 38 && pos.x < 204 && pos.y > 730 && pos.y < 768) {
            stage = 3;
        }
        else if(pos.x > 296 && pos.x < 462 && pos.y > 730 && pos.y < 768) {
            stage = 4;
        } else if(pos.x > 552 && pos.x < 718 && pos.y > 730 && pos.y < 768) {
            stage = 5;
        } else if(pos.x > 805 && pos.x < 971 && pos.y > 730 && pos.y < 768) {
            stage = 6;
        } else if(pos.x > 1054 && pos.x < 1220 && pos.y > 730 && pos.y < 768) {
            stage = 7;
        }
        else{
            stage=0;
        }
    }
    
    for (auto& hand : astra.getHandsWorld()) {
        auto& pos_real = hand.second;
       ofVec3f pos = ofVec3f((int)(pos_real.x*2.25),(int)(pos_real.y*1.875),pos_real.z);
            if (pos.z < 800){
                stage = 1;
            }
    }
    
    
    
    //mesh---------------------------------------------
    
    if (astra.isFrameNew() && bDrawPointCloud) {
        mesh.clear();
        
        int maxDepth = 1500;
        int w = astra.getDepthImage().getWidth();
        int h = astra.getDepthImage().getHeight();
        
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                ofVec3f p = astra.getWorldCoordinateAt(x, y);
                
                if (p.z == 0) continue;
                if (p.z > maxDepth) continue;
                
                
                mesh.addVertex(p);
                
                if (bPointCloudUseColor) {
                    mesh.addColor(astra.getColorImage().getColor(x, y));
                } else {
                    float hue  = ofMap(p.z, 0, maxDepth, 0, 255);
                    mesh.addColor(ofColor::fromHsb(hue, 255, 255));
                }
            }
        }
    }
    
    
    
    if(astra.isFrameNew()) {
        
        grayImage.setFromPixels(astra.getDepthImage());
        grayImage.threshold(80);
        mesh.clear();
        
        nowTime = (int)ofGetElapsedTimeMillis();
        for (int i = 0; i < 144; i++) {
            for (int j = 0; j < 90; j++) {
                if(count[i][j]<10000)
                    count[i][j] += (nowTime - preTime);
                
                ofVec3f p_astra = astra.getWorldCoordinateAt((int)(i*4.44), (int)(j*5.33));
                if (p_astra.z < near.get() || p_astra.z > far.get()) {
                    p[i][j].z = 0;
                    continue;
                }else {
                    p[i][j].z = p_astra.z;
                    mesh.addVertex(p_astra);
                }
            }
        }
        preTime = nowTime;
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    background.draw(0,0, ofGetWindowWidth(), ofGetWindowHeight());
    
    ofSetFullscreen(true);
    int a = ofMap(nowTime - startTime, 0, 10000, 0, 255);
    ofSetColor(255, 255, 255, 255-a);
    astra.drawDepth(0, 0, ofGetWidth(), ofGetHeight());
    //astra.draw(640, 0);
    
    //mesh---------------------------------------------
    
    for (int i = 0; i < 144; i++) {
        for (int j = 0; j < 90; j++) {
            if (count[i][j] > 10000 && (p[i][j].z!=0)) {
                ofSetColor(0, 255, 0, a);
                myFont.drawString(ofToString((int)ofRandom(0, 2)), p[i][j].x, p[i][j].y);
                //ofDrawBitmapString(ofToString((int)ofRandom(0, 2)), p[i][j].x, p[i][j].y);
            }
        }
    }
    ofSetColor (255);
    
    //GUI----------------------------------------------

    //contourFinder.draw(640, 0);
  //  gui.draw();
    
    //ofSetColor(255, 0, 0,100);
    //ofDrawBitmapString("r = reset", 10, 10);
    //ofDrawBitmapString(mesh.getNumVertices(), 10, 20);
    
    //stages--------------------------------------------
    
    if (stage == 0){
 
    }else if (stage == 1){
       
//        ofSetColor(255, 0, 0);
//        ofDrawRectangle(ofGetWidth()/2.0-173, ofGetHeight()/2.0-110, 400, 125);
//        ofSetColor(255);
//        boughtfont.drawString("SOLD", ofGetWindowWidth()/2.0-150, ofGetWindowHeight()/2.0);
        timer += ofGetLastFrameTime();
        if (timer <= 1){
       // click.play();
        error.draw(ofGetWindowWidth()/2.0-460, ofGetWindowHeight()/2.0-230);
        timer = 0;
        }
        reset();

    }else if (stage==2){
        buy.draw(47,399);
    }else if (stage==3){
        buy1.draw(38,730);
    }else if (stage==4){
        buy1.draw(296,731);
    }else if (stage==5){
        buy1.draw(552,730);
    }else if (stage==6){
        buy1.draw(805,730);
    }else if (stage==7){
        buy1.draw(1054,730);
    }
    
    //hand tracking-----------------------------------------------------------------------
    if (!bDrawPointCloud) {
        ofSetColor(255);
        for (auto& hand : astra.getHandsDepth()) {
            auto& pos_real = hand.second;
            ofPoint pos = ofPoint((int)(pos_real.x*2.25),(int)(pos_real.y*1.875));
            cursor.draw (pos);
        }
    } else {
        cam.begin();
        ofEnableDepthTest();
        ofRotateY(180);
        ofScale(1.5, 1.5);
        
        mesh.draw();
        
        for (auto& hand : astra.getHandsWorld()) {
            auto& pos_real = hand.second;
            ofPoint pos = ofPoint((int)(pos_real.x*2.25),(int)(pos_real.y*1.875));
            cursor.draw (pos);
        }
        
        ofDisableDepthTest();
        cam.end();
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'r')
        reset();
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
