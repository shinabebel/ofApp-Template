#include "ofApp.h"

#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowShape(WIDTH, HEIGHT);
	ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
	ofDisableArbTex();
	ofSetFrameRate(60);
	//ofSetVerticalSync(true);

#ifdef SHIPPING
	ofLogToFile(ofVAArgsToString("logs/%s_log.txt", ofGetTimestampString("%Y-%m-%d-%H-%M-%S").c_str()));
	ofHideCursor();
	ofToggleFullscreen();
	is_debug_visible = false;
#endif

	{
		ofLog(OF_LOG_NOTICE, "application start with resolution: %u x %u", ofGetWidth(), ofGetHeight());
	}
	
	// allocate fbo
    {
        ofFbo::Settings s;
        s.width = FBO_WIDTH;
		s.height = FBO_HEIGHT;
        s.useDepth = true;
        s.colorFormats.emplace_back(GL_RGBA);
        
        fbo.allocate(s);
    }
    
	// setup gui
    {
		loadGuiTheme(&gui, "fonts/theme.xml");
		gui.setup("gui");

		ofParameterGroup infos;
		infos.setName("informations");
		infos.add(g_total_mem.set("total_memory", ""));
		infos.add(g_avail_mem.set("avail_memory", ""));
		infos.add(g_used_mem.set("used_memory", ""));
		infos.setSerializable(false);
		gui.add(infos);

		ofParameterGroup g_settings;
		g_settings.setName("settings");
		g_settings.add(g_threshold.set("threshold", 0.5f, 0, 1));
		gui.add(g_settings);

		g_uniforms.setName("uniforms");
		g_uniforms.add(uTimeStep.set("uTimeStep", 1.0f / 60.0f, 0.0f, 1.0f / 30.0f));
		uTimeStep.setSerializable(false);
		g_uniforms.add(uElapsedTime.set("uElapsedTime", ofGetElapsedTimef()));
		uElapsedTime.setSerializable(false);
		g_uniforms.add(uTimeValue.set("uTimeValue", 0, 0, 1));
		uTimeValue.setSerializable(false);
		gui.add(g_uniforms);

		gui.minimizeAll();
		gui.loadFromFile(gui_filename);
    }
	

	loadShaders();
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("oF Application: " + ofToString(ofGetFrameRate(), 1));

	// update gui params
	{
		GLint total_mem_kb = 0;
		glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_mem_kb);
		GLint cur_avail_mem_kb = 0;
		glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb);
		float used_mem = float(total_mem_kb - cur_avail_mem_kb) * 100 / total_mem_kb;
		g_total_mem.set(ofVAArgsToString(" %u kb", total_mem_kb));
		g_avail_mem.set(ofVAArgsToString(" %u kb", cur_avail_mem_kb));
		g_used_mem.set(ofVAArgsToString(" %2.2f%%", used_mem));

		uTimeStep = ofGetElapsedTimef() - uElapsedTime;
		uElapsedTime = ofGetElapsedTimef();
		int interval = 100000;
		float value = ofGetElapsedTimeMillis() % interval / float(interval);
		uTimeValue = sin(value * TWO_PI) * 0.5f + 0.5f; // continuous sin value
	}
	
	// update main fbo
	{
		fbo.begin();
		auto viewport = ofGetCurrentViewport();
		ofClear(0);
		shader.begin();
		shader.setUniforms(g_uniforms);
		//drawRectangle(viewport);
		shader.end();
		fbo.end();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
	auto viewport = ofGetCurrentViewport();
	
	auto rect = getCenteredRect(fbo.getWidth(), fbo.getHeight(), viewport.width, viewport.height);
	fbo.draw(rect);


	logger.draw();

	// draw debug things
	if (is_debug_visible)
	{
		gui.draw();
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
	
	string text = "";

	switch (key)
	{
	case OF_KEY_F1:
		is_debug_visible = !is_debug_visible;
		text = "toggle debug";
		break;
	case OF_KEY_F5:
		loadShaders();
		text = "load shaders";
		break;
	case OF_KEY_F11:
		toggleFullscreen();
		text = "toggle full screen";
		break;
	case 's':
		gui.saveToFile(gui_filename);
		text = "save gui settings";
		break;
	case 'l':
		gui.loadFromFile(gui_filename);
		text = "load gui settings";
		break;
	}

	if (!text.empty()) logger << text << endl;
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

void ofApp::loadGuiTheme(ofxGuiGroup* gui, string path)
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

void ofApp::loadShaders()
{
	ofLog(OF_LOG_NOTICE, "%s load shaders", ofGetTimestampString("%H:%M:%S").c_str());

	shader.unload();
	shader.setupShaderFromFile(GL_VERTEX_SHADER, "shaders/basic.vert");
	shader.setupShaderFromFile(GL_FRAGMENT_SHADER, "shaders/basic.frag");
	shader.linkProgram();
}

void ofApp::drawRectangle(float x, float y, float w, float h)
{
	static ofVbo vbo;
	vector<ofVec3f> vertices = { ofVec3f(x, y, 0), ofVec3f(x + w, y, 0), ofVec3f(x + w, y + h, 0) , ofVec3f(x, y + h, 0) };
	vector<ofFloatColor> colors(4, ofGetStyle().color);
	if (!vbo.getIsAllocated())
	{
		vector<ofVec2f> texCoords = { ofVec2f(0, 0), ofVec2f(1, 0), ofVec2f(1, 1), ofVec2f(0, 1) };
		vector<ofVec3f> normals(4, ofVec3f(0, 0, 1));
		vector<ofIndexType> indices = { 0, 2, 1, 0, 3, 2 };
		vbo.setVertexData(&vertices[0].x, 3, vertices.size(), GL_DYNAMIC_DRAW);
		vbo.setColorData(&colors[0].r, colors.size(), GL_DYNAMIC_DRAW);
		vbo.setTexCoordData(&texCoords[0].x, texCoords.size(), GL_STATIC_DRAW);
		vbo.setNormalData(&normals[0].x, normals.size(), GL_STATIC_DRAW);
		vbo.setIndexData(&indices[0], indices.size(), GL_STATIC_DRAW);
	}
	else
	{
		vbo.updateVertexData(&vertices[0].x, vertices.size());
		vbo.updateColorData(&colors[0].r, colors.size());
	}
	vbo.drawElements(GL_TRIANGLES, vbo.getNumIndices());
}