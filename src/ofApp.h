#pragma once

#include "ofMain.h"
#include "ofxGui.h"

//#define SHIPPING

using ofShaderRef = std::shared_ptr<ofShader>;
using ofFboRef = std::shared_ptr<ofFbo>;
using ofxGuiGroupRef = std::shared_ptr<ofxGuiGroup>;

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

	void loadGuiTheme(ofxGuiGroup* gui, string path);
	void loadShaders();
	
	void drawRectangle(float x, float y, float w, float h);
	void drawRectangle(ofVec2f pos, float w, float h) { drawRectangle(pos.x, pos.y, w, h); }
	void drawRectangle(const ofRectangle& rect) { drawRectangle(rect.x, rect.y, rect.width, rect.height); }

protected:
	ofRectangle getCenteredRect(int srcWidth, int srcHeight, int otherWidth, int otherHeight, bool isFill = true)
	{
		ofRectangle other(0, 0, otherWidth, otherHeight);
		ofRectangle result; result.setFromCenter(other.getCenter(), srcWidth, srcHeight);
		float scaleBy = other.getHeight() / result.getHeight();
		auto aspectAspect = result.getAspectRatio() / other.getAspectRatio();
		if ((isFill && aspectAspect <= 1.0f) || (!isFill && aspectAspect >= 1.0f))
			scaleBy = other.getWidth() / result.getWidth();
		result.scaleFromCenter(scaleBy);
		return result;
	}

private:
	enum {
		WIDTH = 1280,
		HEIGHT = 720,
		FBO_WIDTH = 1920,
		FBO_HEIGHT = 1080
	};
	
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
	ofParameterGroup g_uniforms;
	ofParameter<float> uTimeStep;
	ofParameter<float> uElapsedTime;
	ofParameter<float> uTimeValue;
	
	ofFbo fbo;
	ofShader shader;
};
