#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Utilities.h"

//#define SHIPPING

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void reset();
	void updateParameters();

private:
	enum {
		WIDTH = 1280,
		HEIGHT = 720,
		FBO_WIDTH = 1920,
		FBO_HEIGHT = 1080
	};
	
	// gl
	ofFbo fbo;
	ofShader shader;

	// utilities
	Log logger;
	PixelSaverRef pixel_saver;

	bool is_debug_visible = true;
	const string gui_filename = "settings/gui_setting.xml";
	ofxGuiGroup gui;

	// infos
	ofParameter<string> g_total_mem;
	ofParameter<string> g_avail_mem;
	ofParameter<string> g_used_mem;

	// settings
	ofParameter<float> time_step;
	ofParameter<float> elapsed_time;
	ofParameter<float> time_value;
	ofParameter<float> g_threshold;

	struct UniformBlock
	{
		float time_step;
		float elapsed_time;
		float time_value;
		float threshold;
	};
	ofBufferObject ubo;
};


