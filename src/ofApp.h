#pragma once

#include "ofMain.h"
#include "ofxGui.h"

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

	void loadGuiTheme(std::shared_ptr<ofxGuiGroup> gui, string path);
	void loadShaders();
	
	void drawRectangle(float x, float y, float w, float h);
	void drawRectangle(const ofRectangle& rect) { drawRectangle(rect.x, rect.y, rect.width, rect.height); }

protected:


private:
	enum {
		WIDTH = 1280,
		HEIGHT = 720,
        FBO_SIZE = 1920
	};
	
	bool bDebugVisible = true;
	std::shared_ptr<ofxGuiGroup> mGui;

	ofParameterGroup mSettings;
	ofParameter<float> gThreshold;

	ofParameterGroup mUniforms;
	ofParameter<float> uDeltaTime;
	ofParameter<float> uElapsedTime;
	
	std::shared_ptr<ofFbo> mFbo;
	std::shared_ptr<ofShader> mShader;
};
