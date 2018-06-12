#include "ofApp.h"

#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowShape(WIDTH, HEIGHT);
	ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
	ofDisableArbTex();
	ofSetFrameRate(60);
	//ofSetVerticalSync(true);


	{
		ofLog(OF_LOG_NOTICE, "application start with resolution: %u x %u", ofGetWidth(), ofGetHeight());
	}
	
	// allocate fbo
    {
        ofFbo::Settings s;
        s.width = FBO_WIDTH;
		s.height = FBO_HEIGHT;
        s.useDepth = true;
		s.colorFormats = { GL_RGBA8 };
        
        fbo.allocate(s);
    }
    
	// setup gui
    {
		gui.setup("GUI", gui_filename);

		ofParameterGroup infos;
		infos.setName("information");
		infos.add(memory_total.set("total", ""));
		infos.add(memory_avail.set("available", ""));
		infos.add(memory_used.set("used", ""));
		infos.setSerializable(false);
		gui.add(infos);
		
		ofParameterGroup params;
		params.setName("setting");
		params.add(time_step.set("time_step", 1.0f / 120.0f, 0.0f, 1.0f / 30.0f));
		time_step.setSerializable(false);
		params.add(elapsed_time.set("elapsed_time", ofGetElapsedTimef()));
		elapsed_time.setSerializable(false);
		params.add(time_value.set("time_value", 0, 0, 1));
		time_value.setSerializable(false);
		gui.add(params);

		
    }

	//gui.minimizeAll();
	gui.loadFromFile(gui_filename);

	reset();
}

//--------------------------------------------------------------
void ofApp::update()
{
	ofSetWindowTitle("oF Application: " + ofToString(ofGetFrameRate(), 1));

	// update params and ubo
	updateParameters();
	
	// update main fbo
	{
		fbo.begin();
		auto viewport = ofGetCurrentViewport();
		ofClear(0);

		shader->begin();
		shader->end();

		fbo.end();
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofClear(0);
	auto viewport = ofGetCurrentViewport();
	
	// draw debug things
	if (is_debug_visible)
	{
		gui.draw();
	}

}

//--------------------------------------------------------------
void ofApp::exit() 
{
	ofLog(OF_LOG_NOTICE, "application exit");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{	
	string text = "";
	switch (key)
	{
	case OF_KEY_F1:
		is_debug_visible = !is_debug_visible;
		text = "toggle debug";
		break;
	case OF_KEY_F5:
		reset();
		text = "reset";
		break;
	case OF_KEY_F11:
		ofToggleFullscreen();
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

	if (!text.empty()) std::cout << "[KeyDown] " << text << std::endl;
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 
	
}

///////////////////////////////////////////////////////////////

void ofApp::reset()
{
	ofLog(OF_LOG_NOTICE, "%s reset", ofGetTimestampString("%H:%M:%S").c_str());

	shader.reset(new ofShader);
	shader->setupShaderFromFile(GL_VERTEX_SHADER, "shaders/basic.vert");
	shader->setupShaderFromFile(GL_FRAGMENT_SHADER, "shaders/basic.frag");
	shader->linkProgram();
}

void ofApp::updateParameters()
{
	// update gui params
	static GLint total_mem_kb = 0;
	glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_mem_kb);
	static GLint cur_avail_mem_kb = 0;
	glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb);
	static float used_mem = 0.0f;
	used_mem = (total_mem_kb > 0) ? (100.0f * (total_mem_kb - cur_avail_mem_kb) / total_mem_kb) : 0.0f;
	memory_total.set(ofVAArgsToString("%u kb", total_mem_kb));
	memory_avail.set(ofVAArgsToString("%u kb", cur_avail_mem_kb));
	memory_used.set(ofVAArgsToString("%2.2f %%", used_mem));
	
	float current_time = ofGetElapsedTimef();
	time_step = glm::clamp(current_time - elapsed_time, time_step.getMin(), time_step.getMax());
	elapsed_time = current_time;
	time_value = glm::sin(current_time * 0.5f) * 0.5f + 0.5f;

}