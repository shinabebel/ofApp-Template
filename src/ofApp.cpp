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

		pixel_saver = PixelSaver::create();
	}
	
	// allocate fbo
    {
        ofFbo::Settings s;
        s.width = FBO_WIDTH;
		s.height = FBO_HEIGHT;
        s.useDepth = true;
		s.colorFormats = { GL_RGBA };
        
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

		gui.minimizeAll();

		ofParameterGroup g_settings;
		g_settings.setName("settings");
		g_settings.add(time_step.set("time_step", 1.0f / 120.0f, 0.0f, 1.0f / 30.0f));
		time_step.setSerializable(false);
		g_settings.add(elapsed_time.set("elapsed_time", ofGetElapsedTimef()));
		elapsed_time.setSerializable(false);
		g_settings.add(time_value.set("time_value", 0, 0, 1));
		time_value.setSerializable(false);
		g_settings.add(g_threshold.set("threshold", 0.5f, 0, 1));
		gui.add(g_settings);

		gui.loadFromFile(gui_filename);
    }
	
	reset();
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("oF Application: " + ofToString(ofGetFrameRate(), 1));

	// update params and ubo
	updateParameters();
	
	// update main fbo
	{
		fbo.begin();
		auto viewport = ofGetCurrentViewport();
		ofClear(0);

		shader.begin();
		//drawRectangle(viewport);
		shader.end();

		fbo.end();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
	auto viewport = ofGetCurrentViewport();
	
	{
		auto rect = getCenteredRect(fbo.getWidth(), fbo.getHeight(), viewport.width, viewport.height, false);
		auto& tex = fbo.getTexture();
		tex.bind();
		drawRectangle(rect);
		tex.unbind();
	}
	

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
		toggleFullscreen(WIDTH, HEIGHT);
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
	case 'p':
		pixel_saver->saveFbo(fbo);
		text = "save pixels";
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

void ofApp::reset()
{
	ofLog(OF_LOG_NOTICE, "%s reset", ofGetTimestampString("%H:%M:%S").c_str());

	int ubo_binding_point = 0;
	string ubo_name = "uniform_block";
	string header = ofBufferFromFile("shaders/header.glsl").getText();

	string ubo_layout = ofVAArgsToString(R"(
layout(std140, binding = %u) uniform %s { 
	float time_step;
	float elapsed_time;
	float time_value;
	float threshold;
};
)", ubo_binding_point, ubo_name.c_str());

	// general shader
	{
		string vs = ofBufferFromFile("shaders/basic.vert").getText();
		string fs = ofBufferFromFile("shaders/basic.frag").getText();
		shader.unload();
		shader.setupShaderFromSource(GL_VERTEX_SHADER, header + ubo_layout + vs);
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, header + ubo_layout + fs);
		shader.linkProgram();
		bindUniformBlock(shader, ubo_name, ubo_binding_point);
	}

	// compute shader
	{
		string ssbo_layout = ofVAArgsToString(R"(
layout(std430, binding = %u) buffer %s { 
	Particle particle[];
};
layout(local_size_x = %u, local_size_y = %u, local_size_z = %u) in;
)", 0, "particle_buffer", WORK_GROUP_SIZE, 1, 1);
		string cs = ofBufferFromFile("shaders/basic.comp").getText();
		compute_shader.unload();
		compute_shader.setupShaderFromSource(GL_COMPUTE_SHADER, header + ubo_layout + ssbo_layout + cs);
		compute_shader.linkProgram();
		bindUniformBlock(compute_shader, ubo_name, ubo_binding_point);
	}
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
	g_total_mem.set(" " + toStringWithCommas(total_mem_kb) + " kb");
	g_avail_mem.set(" " + toStringWithCommas(cur_avail_mem_kb) + " kb");
	g_used_mem.set(ofVAArgsToString(" %2.2f%%", used_mem));

	float current_time = ofGetElapsedTimef();
	time_step = ofClamp(current_time - elapsed_time, time_step.getMin(), time_step.getMax());
	elapsed_time = current_time;
	int interval = 100000;
	float value = ofGetElapsedTimeMillis() % interval / float(interval);
	time_value = sin(value * TWO_PI) * 0.5f + 0.5f; // continuous sin value

	// update ubo
	static UniformBlock ub;
	ub.time_step = time_step;
	ub.elapsed_time = elapsed_time;
	ub.time_value = time_value;
	ub.threshold = g_threshold;

	if (!ubo.isAllocated())
	{
		ubo.allocate();
		ubo.setData(sizeof(UniformBlock), &ub, GL_DYNAMIC_DRAW);
		ubo.bindBase(GL_UNIFORM_BUFFER, 0);
	}
	else
	{
		ubo.updateData(sizeof(UniformBlock), &ub);
	}
}