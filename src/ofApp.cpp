#include "ofApp.h"
#include <cmath>

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);
	
	int bufferSize		= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	// phaseAdderTarget 	= 0.0f;
	volume				= 0.1f;
	bNoise 				= false;
	octave				= 0;
	pan 				= 0.5;
	FreqPlayed 			= 30.0;

	lAudio.assign(bufferSize, 0.0);
	rAudio.assign(bufferSize, 0.0);
	
	soundStream.printDeviceList();

	ofSoundStreamSettings settings;

	// if you want to set the device id to be different than the default:
	//
	//	auto devices = soundStream.getDeviceList();
	//	settings.setOutDevice(devices[3]);

	// you can also get devices for an specific api:
	//
	//	auto devices = soundStream.getDeviceList(ofSoundDevice::Api::PULSE);
	//	settings.setOutDevice(devices[0]);

	// or get the default device for an specific api:
	//
	// settings.api = ofSoundDevice::Api::PULSE;

	// or by name:
	//
	//	auto devices = soundStream.getMatchingDevices("default");
	//	if(!devices.empty()){
	//		settings.setOutDevice(devices[0]);
	//	}

#ifdef TARGET_LINUX
	// Latest linux versions default to the HDMI output
	// this usually fixes that. Also check the list of available
	// devices if sound doesn't work
	auto devices = soundStream.getMatchingDevices("default");
	if(!devices.empty()){
		settings.setOutDevice(devices[0]);
	}
#endif

	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.numOutputChannels = 2;
	settings.numInputChannels = 0;
	settings.bufferSize = bufferSize;
	soundStream.setup(settings);

	// on OSX: if you want to use ofSoundPlayer together with ofSoundStream you need to synchronize buffersizes.
	// use ofFmodSetBuffersize(bufferSize) to set the buffersize in fmodx prior to loading a file.
}


//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(225);
	ofDrawBitmapString("AUDIO OUTPUT EXAMPLE", 32, 32);
	ofDrawBitmapString("press 'x' to unpause the audio\npress 'c' to pause the audio", 31, 92);
	
	ofNoFill();
	
	// draw the left channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 150, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Left Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < lAudio.size(); i++){
				float x =  ofMap(i, 0, lAudio.size(), 0, 900, true);
				ofVertex(x, 100 -lAudio[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();

	// draw the right channel:
	// ofPushStyle();
	// 	ofPushMatrix();
	// 	ofTranslate(32, 350, 0);
			
	// 	ofSetColor(225);
	// 	ofDrawBitmapString("Right Channel", 4, 18);
		
	// 	ofSetLineWidth(1);	
	// 	ofDrawRectangle(0, 0, 900, 200);

	// 	ofSetColor(245, 58, 135);
	// 	ofSetLineWidth(3);
					
	// 		ofBeginShape();
	// 		for (unsigned int i = 0; i < rAudio.size(); i++){
	// 			float x =  ofMap(i, 0, rAudio.size(), 0, 900, true);
	// 			ofVertex(x, 100 -rAudio[i]*180.0f);
	// 		}
	// 		ofEndShape(false);
			
	// 	ofPopMatrix();
	// ofPopStyle();
	
		
	ofSetColor(225);
	string reportString = "volume: ("+ofToString(volume, 2)+") modify with -/+ keys\npan: ("+ofToString(pan, 2)+") modify with mouse x\nsynthesis: ";
	if( !bNoise ){
		reportString += "sine wave (" + ofToString(FreqPlayed, 2) + "hz) modify with mouse y";
	}else{
		reportString += "noise";	
	}
	ofDrawBitmapString(reportString, 32, 579);

}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
	if (key == '-' || key == '_' ){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+' || key == '=' ){
		volume += 0.05;
		volume = MIN(volume, 1);
	}
	
	if( key == 'x' ){
		soundStream.start();
	}
	
	if( key == 'c' ){
		soundStream.stop();
	}
	
}

//--------------------------------------------------------------
void ofApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	// int width = ofGetWidth();
	// pan = (float)x / (float)width;
	// float height = (float)ofGetHeight();
	// float heightPct = ((height-y) / height);
	// targetFrequency = 2000.0f * heightPct;
	// phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	// int width = ofGetWidth();
	// pan = (float)x / (float)width;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	// bNoise = true;
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	// bNoise = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer & buffer){
	//pan = 0.5f;
	// float leftScale = pan;
	// float rightScale = pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}

	// if ( bNoise == true){
		// ---------------------- noise --------------
		// for (size_t i = 0; i < buffer.getNumFrames(); i++){
		// 	lAudio[i] = buffer[i*buffer.getNumChannels()    ] = ofRandom(0, 1) * volume * leftScale;
		// 	rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = ofRandom(0, 1) * volume * rightScale;
		// }
	 
	
	for (size_t i = 0; i < buffer.getNumFrames(); i++){
		phase += TWO_PI * FreqPlayed * (i/44100); // i transformé en temps t
		float sample = sin(phase);
		lAudio[i] = sample * volume * pan ; //sortie visuelle
		buffer[i*buffer.getNumChannels()    ] = sample * volume * pan ; // sortie audio


		// rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = sample * volume * pan;
		// printf("%.4f ",)
	}
	

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::keyNoteAssociation(int key){
	// associates a key played on the keyboard to a frequency to play
	
	if( key == 'q' ){
		FreqPlayed = 32.703 * pow(2,octave);
	
	}
	
	if( key == 'z' ){
		FreqPlayed = 34.648 * pow(2,octave);
	}

	if( key == 's' ){
		FreqPlayed = 36.708 * pow(2,octave);
	}
	
	if( key == 'e' ){
		FreqPlayed = 38.891 * pow(2,octave);
	}

	if( key == 'd' ){
		FreqPlayed = 41.203 * pow(2,octave);
	}
	
	if( key == 'f' ){
		FreqPlayed = 43.654 * pow(2,octave);
	}

	if( key == 't' ){
		FreqPlayed = 46.249 * pow(2,octave);
	}
	
	if( key == 'g' ){
		FreqPlayed = 48.999 * pow(2,octave);
	}

	if( key == 'y' ){
		FreqPlayed = 51.913 * pow(2,octave);
	}
	
	if( key == 'h' ){
		FreqPlayed = 55.000 * pow(2,octave);
	}

	if( key == 'u' ){
		FreqPlayed = 58.270 * pow(2,octave);
	}
	
	if( key == 'j' ){
		FreqPlayed = 61.735 * pow(2,octave);
	}
}

