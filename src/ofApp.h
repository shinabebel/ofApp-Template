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
	ofParameter<float> g_threshold;

	// uniforms
	ofParameter<float> time_step;
	ofParameter<float> elapsed_time;
	ofParameter<float> time_value;

	struct UniformBlock
	{
		float time_step;
		float elapsed_time;
		float time_value;
		float a;
	} uniform_block;
	ofBufferObject ubo;
	bool is_uniform_updated = false;
	
	void updateUbo()
	{
		if (is_uniform_updated) return;

		uniform_block.time_step = time_step;
		uniform_block.elapsed_time = elapsed_time;
		uniform_block.time_value = time_value;
		uniform_block.a = 0.0f;
		is_uniform_updated = true;

		if (!ubo.isAllocated())
		{
			ubo.allocate();
			ubo.setData(sizeof(UniformBlock), &uniform_block, GL_DYNAMIC_DRAW);
		}
		else
		{
			ubo.updateData(sizeof(UniformBlock), &uniform_block);
		}
	}

	template<typename T>
	void onUniformChanged(T& t)
	{
		is_uniform_updated = false;
	}

	template<typename T, typename U>
	void addParameter(ofParameterGroup& group, ofParameter<T>& param, const string& name, const U& value, const U& min, const U& max)
	{
		group.add(param.set(name, T(value), T(min), T(max)));
		param.addListener(this, &ofApp::onUniformChanged<T>);
	}
};


