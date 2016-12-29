#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "Log.h"
#include "PixelSaver.h"

//#define SHIPPING

class ofApp : public ofBaseApp{

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
	ofFbo fbo;
	ofShader shader;
	ofShader compute_shader;


	// utilities
	generative::Log logger;
	generative::PixelSaverRef pixel_saver;

	bool is_debug_visible = true;
	const string gui_filename = "settings/gui_setting.xml";
	ofxGuiGroup gui;

	// infos
	ofParameter<string> memory_total;
	ofParameter<string> memory_avail;
	ofParameter<string> memory_used;

	// settings
	ofParameter<float> time_step;
	ofParameter<float> elapsed_time;
	ofParameter<float> time_value;

};



