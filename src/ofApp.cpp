#include "ofApp.h"
#include "FT.h"
#include <cmath>
#include <string>
#include "bandPass.h"
#include "bandReject.h"
#include "brillance.h"
#include "constante.h"

//--------------------------------------------------------------
void ofApp::setup(){

	gui.setup(); // to draw sliders
	gui.setPosition(32,550);
	gui.setSize(900, 50);
	gui.add(x1.setup("Frequence basse", 0, 0, sampling_rate / 2.0)); // to draw x1 filter slider
	gui.add(x2.setup("Frequence haute", sampling_rate / 2.0, 0, sampling_rate / 2.0)); // to draw x2 filter slider
	gui.add(brillance.setup("Brillance", 0, 0, 2)); // to draw brillance slider



	ofBackground(34, 34, 34);
	
	int bufferSize		= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	// phaseAdderTarget 	= 0.0f;
	volume				= 0.1f;
	bNoise 				= false;
	
	octave				= 3;
	pan 				= 0.5;
	FreqPlayed 			= 30.0;

	lAudio.assign(bufferSize, 0.0);
	rAudio.assign(bufferSize, 0.0);
	spectre.assign(bufferSize, 0.0);
	spectreInverse.assign(bufferSize, 0.0);
	cplx_spectrum.assign(bufferSize, {0.0,0.0});

	rawValues.assign(3, 0.0);
	filteredValues.assign(3, 0.0);

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

	gui.draw(); // to draw sliders

	ofSetColor(225);
	ofDrawBitmapString("Artek - Prototype de Synthetiseur v0.4.6", 32, 32);
	ofDrawBitmapString("Press 'x' to unpause the audio\nPress 'c' to pause the audio", 31, 92);
	ofDrawBitmapString("Press 'o' to increase the octave\nPress 'l' to decrease the octave", 31, 120);
	
	ofNoFill();
		
	// draw the left channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 150, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Son", 4, 18);
		
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
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 350, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Spectre", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(100);
		ofFill();
		ofDrawRectangle(x1*900 / sampleRate * 2.0, 0, (x2-x1)*900 / sampleRate * 2.0, 200);
		ofNoFill();
		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
		
			ofBeginShape();
			FT(lAudio,lAudio.size(), spectre, n_bands, cplx_spectrum);
			for (unsigned int i = 0; i < n_bands; i++){
				float x =  ofMap(i, 0, n_bands, 0, 900, true);
				ofVertex(x, 200 -std::abs(cplx_spectrum[i])*180.0f*0.1f);
				// ofVertex(x, 200 -spectre[i]*180.0f*0.1f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
	
	// FT inverse affichage

	// ofPushStyle();
	// 	ofPushMatrix();
	// 	ofTranslate(32, 550, 0);
			
	// 	ofSetColor(225);
	// 	ofDrawBitmapString("son", 4, 18);
		
	// 	ofSetLineWidth(1);	
	// 	ofDrawRectangle(0, 0, 900, 200);

	// 	ofSetColor(245, 58, 135);
	// 	ofSetLineWidth(3);
					
	// 		ofBeginShape();
	// 		ftinverse(cplx_spectrum,n_bands, spectreInverse, spectreInverse.size());
	// 		for (unsigned int i = 0; i < n_bands; i++){
	// 			float x =  ofMap(i, 0, spectreInverse.size(), 0, 900, true);
	// 			ofVertex(x, 100 -spectreInverse[i]*180.0f);
	// 		}
	// 		ofEndShape(false);
			
	// 	ofPopMatrix();
	// ofPopStyle();

		
	ofSetColor(225);
	string reportString = "Volume: ("+ofToString(volume, 2)+") modify with -/+ keys\nOctave: ("+ofToString(octave, 0)+") modify with O/L keys\nSynthesis: ";
	if( !bNoise ){
		reportString += "Sine wave (" + ofToString(FreqPlayed, 2) + ")\nKey played : " + note;
	}else{
		reportString += "Noise";	
	}
	ofDrawBitmapString(reportString, 32, 649);

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
	
	// to associate a key played on the keyboard to a frequency to play
	
	if( key == 'q' ){
		FreqPlayed = 32.703 * pow(2,octave);
		note = "DO";
	
	}
	
	if( key == 'z' ){
		FreqPlayed = 34.648 * pow(2,octave);
		note = "DO#";
	}

	if( key == 's' ){
		FreqPlayed = 36.708 * pow(2,octave);
		note = "RE";
	}
	
	if( key == 'e' ){
		FreqPlayed = 38.891 * pow(2,octave);
		note = "RE#";
	}

	if( key == 'd' ){
		FreqPlayed = 41.203 * pow(2,octave);
		note = "MI";
	}
	
	if( key == 'f' ){
		FreqPlayed = 43.654 * pow(2,octave);
		note = "FA";
	}

	if( key == 't' ){
		FreqPlayed = 46.249 * pow(2,octave);
		note ="FA#";
	}
	
	if( key == 'g' ){
		FreqPlayed = 48.999 * pow(2,octave);
		note = "SOL";
	}

	if( key == 'y' ){
		FreqPlayed = 51.913 * pow(2,octave);
		note = "SOL#";
	}
	
	if( key == 'h' ){
		FreqPlayed = 55.000 * pow(2,octave);
		note = "LA";
	}

	if( key == 'u' ){
		FreqPlayed = 58.270 * pow(2,octave);
		note = "LA#";
	}
	
	if( key == 'j' ){
		FreqPlayed = 61.735 * pow(2,octave);
		note = "SI";
	}

	// to change the octave
	if (key == 'o'){
		octave += 1;
		octave = MIN(octave, 6);
	} else if (key == 'l'){
		octave -= 1;
		octave = MAX(octave, 0);
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
	bNoise = true;
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bNoise = false;
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
	pan = 0.5f;
	float leftScale = pan;
	float rightScale = pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}

	if ( bNoise == true){
		// ---------------------- noise --------------
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			float sample = ofRandom(0, 1) * volume * leftScale;
			rawValues[0] = sample;
			sample = bandPass(rawValues, filteredValues, x1, x2);
			lAudio[i] = buffer[i*buffer.getNumChannels()    ] = sample;
			// lAudio[i] = buffer[i*buffer.getNumChannels()    ] = ofRandom(0, 1) * volume * leftScale;
			rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = sample;
			// rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = ofRandom(0, 1) * volume * rightScale;
		}
	}
	else{
	
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			phase += TWO_PI * FreqPlayed * (1/44100.0); // i transformé en temps t
			float sample = representation(phase, brillance);
			rawValues[0] = sample;
			if(x1 <= x2)
				sample = bandPass(rawValues, filteredValues, x1, x2);
			else
				sample = bandReject(rawValues, filteredValues, x1, x2);
			lAudio[i] = sample * volume * pan ; //sortie visuelle
			buffer[i*buffer.getNumChannels()    ] = sample * volume * pan ; // sortie audio

			rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = sample * volume * pan;
		}
	}

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

