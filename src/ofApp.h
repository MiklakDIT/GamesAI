#pragma once

#include "ofMain.h"
#include "ofLight.h"
#include "ofxBullet.h"
#include "ship.h"
#include "BlueShip.h"
#include "CyanShip.h"
#include "PurpleShip.h"
#include "RedShip.h"
#include "YellowShip.h"
#include "GreenShip.h"
#include "ofxGrabCam.h"
#include "ffmpeg.h"
#include "bahn.h"
#include <thread>
#include <atomic>
#include "Projectile.h"
#include <unordered_set>

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
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void seek(ofPoint, float);
		void createShip(int color, int team);
		void shootShoot(float intensity);
		ofVec3f randomStar();
		int randomCoordinate();
		unordered_set<shared_ptr<Projectile>> projectiles;

		float previous_time;
		ofLight light;
		//ofEasyCam cam;
		ofxGrabCam cam1;

		vector<ofVec3f> stars;
		
		int scoreA, scoreB;
		
		void openNewFile();
		void startGame();
		void nextCamera();
		void updateCamera();
		int current_camera;

		void collideShips();

		ofTrueTypeFont font;
		ofSoundPlayer sound; // wraps FMOD
		FFmpeg ffmpeg; // used to open and decode audio files with FFmpeg
		Bahn bahn; // used to analyse audio data 
		int window, lastWindow; // the current window (sound frame) (used for matching the game with sound)
		float interpolate; // value used to infer the values between the windows 
		atomic<bool> finishedLoading; // flag to indicate whether the analysis thread has terminated
		bool isTwoPlayers, isNinja, isBlack; // game options
		string filename; // path to the file that is being analysed

		vector<shared_ptr<Ship>> teams[2];

		// function used to interpolate between two values
		inline static float interpol(float a, float b, float i) {
			return a * (1 - i) + b * i;
		}
};
