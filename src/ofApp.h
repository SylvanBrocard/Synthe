#pragma once

#include "ofMain.h"
#include <string>
#include <complex>

#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
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
				
		void audioOut(ofSoundBuffer & buffer);
		
		
		ofSoundStream soundStream;

		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;

		vector <float> lAudio;
		vector <float> rAudio;
		
		//------------------- for the simple sine wave synthesis
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
		int n_bands=512;
		vector <float> spectre;
		vector <float> spectreInverse;
		vector <std::complex<float>> cplx_spectrum;
		vector <float> rawValues, filteredValues;

		float   octave;
		float	FreqPlayed;
		std::string	note;	
	
		ofxFloatSlider brillance; // to draw brillance slider
		ofxPanel gui; // to draw brillance slider
		ofxFloatSlider x1; // to draw slider for filter
		ofxFloatSlider x2; // to draw slider for filter




};
