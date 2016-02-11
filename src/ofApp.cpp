#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowShape(WIDTH, HEIGHT);
	ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
	ofSetFrameRate(60);
	ofDisableArbTex();
	//ofSetVerticalSync(true);
    
    {
        ofFbo::Settings s;
        s.width = s.height = FBO_SIZE;
        s.useDepth = true;
        s.colorFormats.emplace_back(GL_RGBA);
        
        mFbo = shared_ptr<ofFbo>(new ofFbo);
        mFbo->allocate(s);
    }
    
    {
        mUniforms.setName("Uniforms");
        mUniforms.add(uElapsedTime.set("uElapsedTime", ofGetElapsedTimef()));
        
        mGui = shared_ptr<ofxGuiGroup>(new ofxGuiGroup);
        mGui->setup("GUI");
        mGui->add(mUniforms);
    }
	

	loadShaders();
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("oF Application: " + ofToString(ofGetFrameRate(), 1));
	uElapsedTime = ofGetElapsedTimef();
	
	mFbo->begin();
	ofClear(0);
    if (mShader)
    {
        mShader->begin();
        mShader->setUniforms(mUniforms);
    }
	
    if (mShader)
    {
        mShader->end();
    }
	mFbo->end();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);

	mFbo->draw(0, (ofGetHeight() - ofGetWidth()) / 2, ofGetWidth(), ofGetWidth());

	if (bDebugVisible)
	{
		mGui->draw();
	}
		
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	auto toggleFullscreen = [&]()
	{
		ofToggleFullscreen();
		if (!(ofGetWindowMode() == OF_FULLSCREEN))
		{
			ofSetWindowShape(WIDTH, HEIGHT);
			auto pos = ofVec2f(ofGetScreenWidth() - WIDTH, ofGetScreenHeight() - HEIGHT) * 0.5f;
			ofSetWindowPosition(pos.x, pos.y);
		}
	};
	
	switch (key)
	{
	case OF_KEY_F1:
		bDebugVisible = !bDebugVisible;
		break;
	case OF_KEY_F5:
		loadShaders();
		break;
	case OF_KEY_F11:
		toggleFullscreen();
		break;
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

void ofApp::loadShaders()
{
	printf("%s load shaders\n", ofGetTimestampString().c_str());

	mShader.reset();
	mShader = shared_ptr<ofShader>(new ofShader);
	mShader->setupShaderFromFile(GL_VERTEX_SHADER, "shaders/basic.vert");
	mShader->setupShaderFromFile(GL_FRAGMENT_SHADER, "shaders/basic.frag");
	mShader->linkProgram();
}