#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowShape(WIDTH, HEIGHT);
	ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
	ofSetFrameRate(60);
	ofDisableArbTex();
	//ofSetVerticalSync(true);

#ifdef SHIPPING
	ofLogToFile(ofVAArgsToString("logs/%s_log.txt", ofGetTimestampString("%Y-%m-%d-%H-%M-%S").c_str()));
	ofHideCursor();
	ofToggleFullscreen();
	bDebugVisible = false;
#endif

	{
		ofLog(OF_LOG_NOTICE, "application start with resolution: %u x %u", ofGetWidth(), ofGetHeight());
	}
	

    {
        ofFbo::Settings s;
        s.width = FBO_WIDTH;
		s.height = FBO_HEIGHT;
        s.useDepth = true;
        s.colorFormats.emplace_back(GL_RGBA);
        
        mFbo.reset(new ofFbo);
        mFbo->allocate(s);
    }
    
    {
		const string theme_path = "fonts/theme.xml";
		ofVec2f gui_pos;
		ofxGuiGroupRef gui;
		auto getNextPosition = [&]() -> ofVec2f { return gui->getPosition() + ofVec2f(gui->getWidth(), 0); };		

		mSettings.setName("settings");
		mSettings.add(gThreshold.set("threshold", 128.0f, 0.0f, 255.0f));

		mUniforms.setName("uniforms");
		mUniforms.add(uDeltaTime.set("uDeltaTime", 0.0f, 0.0f, 1.0f));
		mUniforms.add(uElapsedTime.set("uElapsedTime", ofGetElapsedTimef()));

		gui.reset(new ofxGuiGroup);
		loadGuiTheme(gui, theme_path);
		gui->setup("gui");
		gui->add(mSettings);
		gui->add(mUniforms);
		gui_pos = getNextPosition();
		mGui.push_back(gui);


		gui.reset(new ofxGuiGroup);
		loadGuiTheme(gui, theme_path);
		ofParameterGroup infos;
		infos.setName("hot key");
		infos.add(ofParameter<string>().set("ESC", "exit"));
		infos.add(ofParameter<string>().set("F1", "gui"));
		infos.add(ofParameter<string>().set("F5", "reload shaders"));
		infos.add(ofParameter<string>().set("F11", "fullscreen"));
		infos.add(ofParameter<string>().set("S", "save settings"));
		infos.add(ofParameter<string>().set("L", "load settings"));
		infos.setSerializable(false);
		gui->setup("info");
		gui->add(infos);
		gui->setPosition(gui_pos);
		gui_pos = getNextPosition();
		mGui.push_back(gui);


		for (auto& g : mGui)
		{
			g->loadFromFile(getGuiFilename(g));
			g->saveToFile(getGuiFilename(g));
		}
			
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
		for (auto& gui : mGui)
			gui->draw();
	}
		
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofLog(OF_LOG_NOTICE, "application exit");
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
	case 's':
		for (auto& g : mGui) g->saveToFile(getGuiFilename(g));
		break;
	case 'l':
		for (auto& g : mGui) g->loadFromFile(getGuiFilename(g));
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

void ofApp::loadGuiTheme(ofxGuiGroupRef gui, string path)
{
	ofXml xml;
	if (!xml.load(path))
	{
		ofLog(OF_LOG_WARNING, "fail to load gui theme settings from %s\n", path.c_str());
		return;
	}

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
	else
	{
		ofLog(OF_LOG_WARNING, "gui theme [%s] is missing.\n", theme_name.c_str());
	}
}

string ofApp::getGuiFilename(ofxGuiGroupRef gui)
{
	return ofVAArgsToString("settings/%s_setting.xml", gui->getName().c_str());
}

ofShaderRef ofApp::autoLoader(string s1, string s2, string s3)
{
	int count = !s2.empty() + !s3.empty();
	bool isComputeShader = false;

	ofShader glsl;
	if (s2.empty() && s3.empty())
	{
		glsl.setupShaderFromFile(GL_COMPUTE_SHADER, s1);
		isComputeShader = true;
	}
	else if (s3.empty())
	{
		glsl.setupShaderFromFile(GL_VERTEX_SHADER, s1);
		glsl.setupShaderFromFile(GL_FRAGMENT_SHADER, s2);
	}
	else
	{
		glsl.setupShaderFromFile(GL_VERTEX_SHADER, s1);
		glsl.setupShaderFromFile(GL_FRAGMENT_SHADER, s2);
		glsl.setupShaderFromFile(GL_GEOMETRY_SHADER, s3);
	}
	glsl.linkProgram();

	if (glsl.getUniformLocation("modelViewProjectionMatrix") < 0 && !isComputeShader) return nullptr;
	else return make_shared<ofShader>(glsl);
}

void ofApp::loadShaders()
{
	ofLog(OF_LOG_NOTICE, "%s load shaders", ofGetTimestampString("%H:%M:%S").c_str());

	mShader = autoLoader("shaders/basic.vert", "shaders/basic.frag");
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