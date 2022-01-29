#pragma once

#include "ofMain.h"
#include "ofTrueTypeFont.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void draw();
		void keyPressed(int key);
		void keyReleased(int key);

		void drawPipe();
		void placePipe();
		void addPath();

		const int OF_KEY_SPACE = 32;
		ofTrueTypeFont myfont;
		ofImage screen;
		ofPixels pix;
		ofColor c1;
		ofColor c2;
		ofColor c3;

		float limit = 40;
		int start_flag = 0;
		int game_over = 0;
		int pipe_flag = 0;
		int complete = 0;
		int end_game = 0;
		int score = 0;
		float pxNew, pyNew;
		
		int rotation = 1;
		int pipe_type = 1;
		float init_x = 80;
		float init_y = 0;
		
		typedef struct pipe {
			int pipe_type;
			int rotation;
			float x; float y;
			struct pipe *next;
		} pipe;
		struct pipe *currentP;
		struct pipe *startP;
		struct pipe *tempP1;
		struct pipe *tempP2;

		typedef struct water {
			float x; float y;
			struct water *next;
		} water;
		struct water *startW;
		struct water *currentW;
		struct water *tempW;
		struct water *endW;
		
};
