#pragma once

#include "ofMain.h"
#include "ofxGui.h"

namespace fs = boost::filesystem;

using ofShaderRef = std::shared_ptr<ofShader>;
using ofFboRef = std::shared_ptr<ofFbo>;
using ofEasyCamRef = std::shared_ptr<ofEasyCam>;

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key) {}
	void mouseMoved(int x, int y ) {}
	void mouseDragged(int x, int y, int button) {}
	void mousePressed(int x, int y, int button) {}
	void mouseReleased(int x, int y, int button) {}
	void mouseEntered(int x, int y) {}
	void mouseExited(int x, int y) {}
	void windowResized(int w, int h) {}
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg) {}

	void reset();
	void updateParameters();

private:
	enum {
		WIDTH = 1280,
		HEIGHT = 720,
		FBO_WIDTH = 1920,
		FBO_HEIGHT = 1080,
		WORK_GROUP_SIZE = 1 << 10
	};
	
	// gl
	ofFboRef fbo = nullptr;
	ofShaderRef shader = nullptr;
	ofShaderRef compute_shader = nullptr;


	bool is_debug_visible = true;
	const std::string gui_filename = "gui_settings.xml";
	ofxPanel gui;

	// infos
	ofParameter<string> memory_total;
	ofParameter<string> memory_avail;
	ofParameter<string> memory_used;

	// settings
	ofParameter<float> time_step;
	ofParameter<float> elapsed_time;
	ofParameter<float> time_value;

};

namespace scoped
{
	class Shader
	{
	public:
		Shader(ofShaderRef shader) :s(shader) { s->begin(); }
		~Shader() { s->end(); }
	private:
		ofShaderRef s = nullptr;
	};

	class Fbo
	{
	public:
		Fbo(ofFboRef fbo) :f(fbo) { f->begin(); }
		~Fbo() { f->end(); }
	private:
		ofFboRef f = nullptr;
	};

	class EasyCam
	{
	public:
		EasyCam(ofEasyCamRef cam) :c(cam) { c->begin(); }
		~EasyCam() { c->end(); }
	private:
		ofEasyCamRef c = nullptr;
	};

	class Matrix
	{
	public:
		Matrix() { ofPushMatrix(); }
		~Matrix() { ofPopMatrix(); }
	};

	class Style
	{
	public:
		Style() { ofPushStyle(); }
		~Style() { ofPopStyle(); }
	};

	class BlendMode
	{
	public:
		BlendMode(ofBlendMode mode) { ofEnableBlendMode(mode); }
		~BlendMode() { ofEnableBlendMode(OF_BLENDMODE_ALPHA); }
	};

	class DepthTest
	{
	public:
		DepthTest() { ofEnableDepthTest(); }
		~DepthTest() { ofDisableDepthTest(); }
	};
}


