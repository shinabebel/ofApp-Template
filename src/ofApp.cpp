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
		mUniforms.add(uDeltaTime.set("uDeltaTime", 0.0f, 0.0f, 1.0f));
        mUniforms.add(uElapsedTime.set("uElapsedTime", ofGetElapsedTimef()));
        
		const string theme_path = "fonts/theme.xml";

		ofxGuiGroup gui;
		loadGuiTheme(gui, theme_path);
		gui.setup("GUI");
		gui.add(mSettings);
		gui.add(mUniforms);
		mGui.push_back(std::make_shared<ofxGuiGroup>(gui));

		ofxGuiGroup info_gui;
		loadGuiTheme(info_gui, theme_path);
		ofParameterGroup infos;
		infos.setName("Hot Key");
		infos.add(ofParameter<string>().set("F1", "Gui"));
		infos.add(ofParameter<string>().set("F5", "Reload Shaders"));
		infos.add(ofParameter<string>().set("F11", "Fullscreen"));
		info_gui.setup("INFO");
		info_gui.add(infos);
		mGui.push_back(std::make_shared<ofxGuiGroup>(info_gui));
    }
	

	loadShaders();
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("oF Application: " + ofToString(ofGetFrameRate(), 1));
	uDeltaTime = ofGetElapsedTimef() - uElapsedTime;
	uElapsedTime = ofGetElapsedTimef();
	
	mFbo->begin();
	ofClear(0);
    mShader->begin();
    mShader->setUniforms(mUniforms);
    
    mShader->end();
	mFbo->end();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
	auto viewport = ofGetCurrentViewport();
	
	auto rect = getCenteredRect(mFbo->getWidth(), mFbo->getHeight(), viewport.width, viewport.height);
	mFbo->draw(rect);

	if (bDebugVisible)
	{
		ofPushMatrix();
		for (auto& gui : mGui)
		{
			gui->draw();
			ofTranslate(gui->getWidth(), 0.0f);
		}
		ofPopMatrix();
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

void ofApp::loadGuiTheme(ofxGuiGroup& gui, string path)
{
	ofXml xml(path);
	gui.loadFont(xml.getValue("FONT_NAME"), xml.getIntValue("FONT_SIZE"));
	gui.setDefaultTextPadding(xml.getIntValue("TEXT_PADDING"));
	gui.setDefaultHeight(xml.getIntValue("HEIGHT"));

	string theme_name = xml.getValue("THEME_NAME");
	if (xml.exists(theme_name))
	{
		xml.setTo(theme_name);
		auto hexHeaderBackgroundColor = ofColor::fromHex(ofHexToInt(xml.getValue("HeaderBackgroundColor")));
		auto hexBackgroundColor = ofColor::fromHex(ofHexToInt(xml.getValue("BackgroundColor")));
		auto hexBorderColor = ofColor::fromHex(ofHexToInt(xml.getValue("BorderColor")));
		auto hexFillColor = ofColor::fromHex(ofHexToInt(xml.getValue("FillColor")));
		auto hexTextColor = ofColor::fromHex(ofHexToInt(xml.getValue("TextColor")));
		gui.setHeaderBackgroundColor(hexHeaderBackgroundColor);
		gui.setBackgroundColor(hexBackgroundColor);
		gui.setBorderColor(hexBorderColor);
		gui.setFillColor(hexFillColor);
		gui.setTextColor(hexTextColor);
		gui.setDefaultHeaderBackgroundColor(hexHeaderBackgroundColor);
		gui.setDefaultBackgroundColor(hexBackgroundColor);
		gui.setDefaultBorderColor(hexBorderColor);
		gui.setDefaultFillColor(hexFillColor);
		gui.setDefaultTextColor(hexTextColor);
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

void ofApp::drawRectangle(float x, float y, float w, float h)
{
	static ofVbo vbo;
	vector<ofVec3f> vertices;
	vertices.emplace_back(x, y, 0);
	vertices.emplace_back(x + w, y, 0);
	vertices.emplace_back(x + w, y + h, 0);
	vertices.emplace_back(x, y + h, 0);
	vector<ofFloatColor> colors(4, ofGetStyle().color);
	if (!vbo.getIsAllocated())
	{
		vector<ofVec2f> texCoords;
		texCoords.emplace_back(0, 0);
		texCoords.emplace_back(1, 0);
		texCoords.emplace_back(1, 1);
		texCoords.emplace_back(0, 1);
		vector<ofVec3f> normals(4, ofVec3f(0, 0, 1));

		vbo.setVertexData(&vertices[0].x, 3, vertices.size(), GL_DYNAMIC_DRAW);
		vbo.setColorData(&colors[0].r, colors.size(), GL_DYNAMIC_DRAW);
		vbo.setTexCoordData(&texCoords[0].x, texCoords.size(), GL_STATIC_DRAW);
		vbo.setNormalData(&normals[0].x, normals.size(), GL_STATIC_DRAW);
	}
	else
	{
		vbo.updateVertexData(&vertices[0].x, vertices.size());
		vbo.updateColorData(&colors[0].r, colors.size());
	}
	vbo.draw(GL_TRIANGLE_FAN, 0, vertices.size());
}