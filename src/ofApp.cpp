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
		mSettings.setName("Settings");
		mSettings.add(gThreshold.set("threshold", 128.0f, 0.0f, 255.0f));

        mUniforms.setName("Uniforms");
        mUniforms.add(uElapsedTime.set("uElapsedTime", ofGetElapsedTimef()));
        
		mGui = shared_ptr<ofxGuiGroup>(new ofxGuiGroup);
		loadGuiTheme(mGui, "fonts/theme.xml");

		mGui->setup("GUI");
		mGui->add(mSettings);
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

///////////////////////////////////////////////////////////////

void ofApp::loadGuiTheme(std::shared_ptr<ofxGuiGroup> gui, string path)
{
	ofXml xml(path);
	gui->loadFont(xml.getValue("FONT_NAME"), xml.getIntValue("FONT_SIZE"));
	gui->setDefaultTextPadding(xml.getIntValue("TEXT_PADDING"));
	gui->setDefaultHeight(xml.getIntValue("HEIGHT"));

	string theme_name = xml.getValue("THEME_NAME");
	if (xml.exists(theme_name))
	{
		xml.setTo(theme_name);
		auto hexHeaderBackgroundColor = ofColor::fromHex(ofHexToInt(xml.getValue("HeaderBackgroundColor")));
		auto hexBackgroundColor = ofColor::fromHex(ofHexToInt(xml.getValue("BackgroundColor")));
		auto hexBorderColor = ofColor::fromHex(ofHexToInt(xml.getValue("BorderColor")));
		auto hexFillColor = ofColor::fromHex(ofHexToInt(xml.getValue("FillColor")));
		auto hexTextColor = ofColor::fromHex(ofHexToInt(xml.getValue("TextColor")));
		gui->setHeaderBackgroundColor(hexHeaderBackgroundColor);
		gui->setBackgroundColor(hexBackgroundColor);
		gui->setBorderColor(hexBorderColor);
		gui->setFillColor(hexFillColor);
		gui->setTextColor(hexTextColor);
		gui->setDefaultHeaderBackgroundColor(hexHeaderBackgroundColor);
		gui->setDefaultBackgroundColor(hexBackgroundColor);
		gui->setDefaultBorderColor(hexBorderColor);
		gui->setDefaultFillColor(hexFillColor);
		gui->setDefaultTextColor(hexTextColor);
	}
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