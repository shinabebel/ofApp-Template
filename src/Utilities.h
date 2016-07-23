#pragma once

#include "ofMain.h"

namespace
{
	void toggleFullscreen(size_t width, size_t height)
	{
		ofToggleFullscreen();
		if (!(ofGetWindowMode() == OF_FULLSCREEN))
		{
			ofSetWindowShape(width, height);
			auto pos = ofVec2f(ofGetScreenWidth() - width, ofGetScreenHeight() - height) * 0.5f;
			ofSetWindowPosition(pos.x, pos.y);
		}
	}

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


	void drawRectangle(float x, float y, float w, float h)
	{
		static ofVbo vbo;
		vector<ofVec3f> vertices = { ofVec3f(x, y, 0), ofVec3f(x + w, y, 0), ofVec3f(x + w, y + h, 0) , ofVec3f(x, y + h, 0) };
		vector<ofFloatColor> colors(4, ofGetStyle().color);
		if (!vbo.getIsAllocated())
		{
			vector<ofVec2f> texCoords = { ofVec2f(0, 0), ofVec2f(1, 0), ofVec2f(1, 1), ofVec2f(0, 1) };
			vector<ofVec3f> normals(4, ofVec3f(0, 0, 1));
			vector<ofIndexType> indices = { 0, 2, 1, 0, 3, 2 };
			vbo.setVertexData(&vertices[0].x, 3, vertices.size(), GL_DYNAMIC_DRAW);
			vbo.setColorData(&colors[0].r, colors.size(), GL_DYNAMIC_DRAW);
			vbo.setTexCoordData(&texCoords[0].x, texCoords.size(), GL_STATIC_DRAW);
			vbo.setNormalData(&normals[0].x, normals.size(), GL_STATIC_DRAW);
			vbo.setIndexData(&indices[0], indices.size(), GL_STATIC_DRAW);
		}
		else
		{
			vbo.updateVertexData(&vertices[0].x, vertices.size());
			vbo.updateColorData(&colors[0].r, colors.size());
		}
		vbo.drawElements(GL_TRIANGLES, vbo.getNumIndices());
	}
	void drawRectangle(ofVec2f pos, float w, float h) { drawRectangle(pos.x, pos.y, w, h); }
	void drawRectangle(const ofRectangle& rect) { drawRectangle(rect.x, rect.y, rect.width, rect.height); }


	void loadGuiTheme(ofxGuiGroup* gui, string path)
	{
		ofXml xml;
		if (!xml.load(path))
		{
			ofLog(OF_LOG_WARNING, "fail to load gui theme settings from %s\n", path.c_str());
			return;
		}

		gui->loadFont(xml.getValue("FONT_NAME"), xml.getIntValue("FONT_SIZE"));
		gui->setDefaultTextPadding(xml.getIntValue("TEXT_PADDING"));
		gui->setDefaultHeight(xml.getIntValue("HEIGHT"));

		string theme_name = xml.getValue("THEME_NAME");
		if (xml.exists(theme_name))
		{
			xml.setTo(theme_name);
			auto hexHeaderBackgroundColor = ofColor::fromHex(ofHexToInt(xml.getValue("HeaderBackgroundColor")));
			auto hexBackgroundColor = ofColor::fromHex(ofHexToInt(xml.getValue("BackgroundColor")));
			auto hexBorderColor = ofColor::fromHex(ofHexToInt(xml.getValue("BorderColor")));
			auto hexFillColor = ofColor::fromHex(ofHexToInt(xml.getValue("FillColor")));
			auto hexTextColor = ofColor::fromHex(ofHexToInt(xml.getValue("TextColor")));
			gui->setHeaderBackgroundColor(hexHeaderBackgroundColor);
			gui->setBackgroundColor(hexBackgroundColor);
			gui->setBorderColor(hexBorderColor);
			gui->setFillColor(hexFillColor);
			gui->setTextColor(hexTextColor);
			gui->setDefaultHeaderBackgroundColor(hexHeaderBackgroundColor);
			gui->setDefaultBackgroundColor(hexBackgroundColor);
			gui->setDefaultBorderColor(hexBorderColor);
			gui->setDefaultFillColor(hexFillColor);
			gui->setDefaultTextColor(hexTextColor);
		}
		else
		{
			ofLog(OF_LOG_WARNING, "gui theme [%s] is missing.\n", theme_name.c_str());
		}
	}
}



class Log
{
public:
	float elapsed_time = 5.0f;
	float fade_out_time = 1.5f;

	template <typename T>
	Log& operator<<(const T& x)
	{
		_log << x;
		return *this;
	}

	void draw(size_t numLines = 15, int x = 10, int y = ofGetHeight() - 10)
	{
		time_stamps.resize(numLines, 0.0f);

		std::vector<std::string> elems;
		stringstream ss(_log.str());
		std::string buff;
		while (getline(ss, buff)) elems.push_back(buff);
		numLines = MIN(elems.size(), numLines);
		int to_line = (int)elems.size() - numLines;

		if (elems.size() <= 0) return;

		ofPushStyle();
		float current_time = ofGetElapsedTimef();
		for (int i = elems.size() - 1; i >= to_line; i--) 
		{
			int idx = elems.size() - i - 1;
			float delta_time = current_time - time_stamps[idx];
			float opacity = 1.0f - CLAMP(delta_time / fade_out_time, 0.0f, 1.0f);
			ofSetColor(ofFloatColor(1, opacity));
			ofDrawBitmapString(elems[i], x, y - idx * 12);
		}
		ofPopStyle();

		if (time_stamps.size() > 0 && current_time - time_stamps[0] > fade_out_time)
			_log.str(std::string());
	}

private:
	stringstream _log;
	std::deque<float> time_stamps;

	// function that takes a custom stream, and returns it
	typedef Log& (*StreamManipulator)(Log&);

	// take in a function with the custom signature
	Log& operator<<(StreamManipulator manip) { return manip(*this); }

	// define the custom endl for this stream.
	// note how it matches the `StreamManipulator`
	// function signature
	Log& endl(Log& stream)
	{
		// print a new line
		_log << std::endl;
		stream << "Called StreamManipulator::endl!" << std::endl;
		return stream;
	}

	// this is the type of std::cout
	typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

	// this is the function signature of std::endl
	typedef CoutType& (*StandardEndLine)(CoutType&);

public:
	// define an operator<< to take in std::endl
	Log& operator<<(StandardEndLine manip)
	{
		manip(_log);
		time_stamps.push_front(ofGetElapsedTimef() + elapsed_time);
		return *this;
	}
};



using PixelSaverRef = std::shared_ptr<class PixelSaver>;

class PixelSaver
{
public:
	static PixelSaverRef create(string path = "print_screen", bool bNameAsSequence = false)
	{
		return PixelSaverRef(new PixelSaver(path, bNameAsSequence));
	}
	void savePixels(ofPixels pix)
	{
		std::lock_guard<mutex> guard(m);
		buffer.push_back(pix);
	}
	void saveTerxture(const ofTexture& tex)
	{
		std::lock_guard<mutex> guard(m);
		buffer.push_back(ofPixels());
		tex.readToPixels(buffer.back());
	}
	void saveFbo(const ofFbo& fbo)
	{
		saveTerxture(fbo.getTexture());
	}
	~PixelSaver()
	{
		is_thread_running = false;
		th.join();
	}

private:
	PixelSaver(string path, bool bNameAsSequence)
		:is_sequence(bNameAsSequence), path(path)
	{
		ofDirectory dir(path);
		if (!dir.exists()) dir.createDirectory(path);
		th = std::thread(&PixelSaver::threadFunction, this);
	}
	void threadFunction()
	{
		while (is_thread_running)
		{
			if (buffer.size() == 0) continue;

			ofPixels pix;
			{
				std::lock_guard<mutex> guard(m);
				pix = buffer[0];
				buffer.pop_front();
			}
			string file_path = path + ofVAArgsToString("/%s.png", ofGetTimestampString().c_str());
			if (is_sequence)
			{
				file_path = path + ofVAArgsToString("/sequence-%04d.png", sequence_index);
				sequence_index++;
			}
			ofSaveImage(pix, file_path);
		}
	}

	std::thread th;
	mutex m;
	std::deque<ofPixels> buffer;
	bool is_sequence;
	int sequence_index = 0;
	bool is_thread_running = true;
	string path;
};