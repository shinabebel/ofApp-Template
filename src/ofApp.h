#pragma once

#include "ofMain.h"
#include "ofxGui.h"

template<typename T>
static std::shared_ptr<T> create() { return std::shared_ptr<T>(new T); }

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

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

	void loadShaders();

protected:


private:
	enum {
		WIDTH = 1280,
		HEIGHT = 720
	};
	
	bool bDebugVisible = true;
	std::shared_ptr<ofxGuiGroup> mGui;

	ofParameterGroup mUniforms;
	ofParameter<float> uElapsedTime;
	
	std::shared_ptr<ofFbo> mFbo;
	std::shared_ptr<ofShader> mShader;
};
