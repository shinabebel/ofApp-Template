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

	void loadGuiTheme(ofxGuiGroupRef gui, string path);
	string getGuiFilename(ofxGuiGroupRef gui);
	ofShaderRef autoLoader(string s1, string s2 = "", string s3 = "");
	void loadShaders();
	
	void drawRectangle(float x, float y, float w, float h);
	void drawRectangle(const ofRectangle& rect) { drawRectangle(rect.x, rect.y, rect.width, rect.height); }

protected:
	ofRectangle getCenteredRect(int srcWidth, int srcHeight, int otherWidth, int otherHeight, bool isFill = true)
	{
		auto other = ofRectangle(0, 0, otherWidth, otherHeight);
		ofRectangle result;
		result.setFromCenter(other.getCenter(), srcWidth, srcHeight);
		float scaleBy;
		auto aspectAspect = result.getAspectRatio() / other.getAspectRatio();

		if ((isFill && aspectAspect <= 1.0f) || (!isFill && aspectAspect >= 1.0f))
			scaleBy = other.getWidth() / result.getWidth();
		else
			scaleBy = other.getHeight() / result.getHeight();

		result.scaleFromCenter(scaleBy);
		return result;
	}

private:
	enum {
		WIDTH = 1280,
		HEIGHT = 720,
        FBO_SIZE = 1920
	};
	
	bool bDebugVisible = true;
	std::vector<ofxGuiGroupRef> mGui;

	ofParameterGroup mSettings;
	ofParameter<float> gThreshold;

	ofParameterGroup mUniforms;
	ofParameter<float> uDeltaTime;
	ofParameter<float> uElapsedTime;
	
	ofFboRef mFbo;
	ofShaderRef mShader;
};
