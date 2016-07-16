#pragma once

#include "ofMain.h"

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