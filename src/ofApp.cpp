#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowShape(WIDTH, HEIGHT);
	ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
	//ofSetVerticalSync(true);

	mFbo.allocate(1920, 1920);

	mUniforms.setName("Uniforms");
	mUniforms.add(uElapsedTime.set("uElapsedTime", ofGetElapsedTimef()));

	mGui.setup("Settings");
	mGui.add(mUniforms);
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("oF Application: " + ofToString(ofGetFrameRate(), 1));
	uElapsedTime = ofGetElapsedTimef();
	
	mFbo.begin();
	ofClear(0);
	mShader.begin();
	mShader.setUniforms(mUniforms);

	mShader.end();
	mFbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);

	mFbo.draw(0, (ofGetHeight() - ofGetWidth()) / 2, ofGetWidth(), ofGetWidth());

	if (bGuiVisible)
		mGui.draw();
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
		bGuiVisible = !bGuiVisible;
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

	mShader.unload();
	mShader.setupShaderFromFile(GL_VERTEX_SHADER, "shaders/basic.vert");
	mShader.setupShaderFromFile(GL_FRAGMENT_SHADER, "shaders/basic.frag");
	mShader.linkProgram();
}