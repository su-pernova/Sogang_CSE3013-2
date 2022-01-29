#include "ofApp.h"
#include <cmath>

//----------- Set up preconditions for the game -----------
void ofApp::setup(){
	// setFrameRate and Background color, LineWidth
	ofSetFrameRate(1000);
	ofBackground(255, 255, 255);
	ofSetLineWidth(1);

	// initialize flags
	start_flag = 0;
	game_over = 0;
	pipe_flag = 0;
	complete = 0;
	score = 0;
	end_game = 0;

	// make header node for pipe nodes
	struct pipe *p;
	p = (struct pipe*)malloc(sizeof(struct pipe));
	p->pipe_type = 1;
	p->rotation = 1;
	p->x = 160; p->y = 0;
	p->next = NULL;
	startP = p;
	currentP = p;
	
	// make header node for water nodes
	struct water *w;
	w = (struct water*)malloc(sizeof(struct water));
	w->x = 40; w->y = 60;
	w->next = NULL;
	startW = w;
	endW = w;
	tempW = w;

	// save path of water along the initial pipe
	for (int i = 1; i <= 120; i++) {
		struct water *w;
		w = (struct water*)malloc(sizeof(struct water));
		w->x = (endW->x) + 1; w->y = endW->y;
		w->next = NULL;
		endW->next = w;
		endW = w;
	}
}

//----------- Draw game elements on the screen -----------
void ofApp::draw() {
	// background grid
	ofSetColor(230, 230, 230);
	ofDrawLine(0, 100, 0, 140);
	for (int i = 0; i <= 840; i += 40) { ofDrawLine(0, i, 1640, i); }
	for (int i = 0; i <= 1640; i += 40) { ofDrawLine(i, 0, i, 840); }

	// start and finish point
	ofSetColor(0, 0, 255);
	ofDrawRectangle(0, 40, 40, 40);
	ofDrawRectangle(1600, 760, 40, 40);

	// current score
	ofSetColor(0, 0, 255);
	myfont.load("Arial.ttf", 20);
	myfont.drawString("current score : ", 1290, 30);
	std::stringstream ssInt;
	ssInt << score;
	myfont.drawString(ssInt.str(), 1490, 31);

	// initial pipe
	ofSetColor(190, 190, 190);
	ofDrawRectangle(40, 40, 120, 40);
	ofDrawRectangle(40, 30, 15, 60);
	ofDrawRectangle(145, 30, 15, 60);

	// getColor : used for checking overlapped pipe
	screen.grabScreen(0, 0, 1640, 840);
	pix = screen.getPixels();
	c1 = pix.getColor(41, 41);
	c2 = pix.getColor(0, 41);

	// game ready
	if (start_flag == 0) {
		ofSetColor(255, 0, 0);
		myfont.load("Arial.ttf", 80);
		myfont.drawString("Press S to start", 445, 290);
		myfont.load("Arial.ttf", 30);
		myfont.drawString("- left/right key : change pipe", 560, 370);
		myfont.drawString("- up/down key : rotate pipe", 560, 450);
		myfont.drawString("- space key : place pipe", 560, 530);
		myfont.drawString("- Q key : quit game", 560, 610);
	}

	// S pressed : game start
	if (start_flag == 1) {
		// draw placed pipe (grey)
		if (pipe_flag == 1) {
			currentP = startP;
			while (currentP->next != NULL) {
				placePipe();
				currentP = currentP->next; }
		}

		// draw next pipe (pink)
		if (complete == 0) drawPipe();

		// draw water and add score
		// draw every wather path saved in water linked list
		currentW = startW;
		ofSetColor(0, 0, 255);
		while (1) {
			ofDrawCircle(currentW->x, currentW->y, 11);
			if (currentW == tempW) break;
			currentW = currentW->next;
		}
		int check = 0;
		for (int i = 0; i < 8; i++) {
			if (tempW->next != NULL && game_over != 1) {
				// check rather the water path is cut off
				if (abs((tempW->next->x) - (tempW->x)) > 2 || abs((tempW->next->y) - (tempW->y)) > 2) {
					game_over = 1;
					check = 1;
				}
				if (check == 0) {
					tempW = tempW->next;
					score++;
				}
			}
		}

		// water arrived destination : game ends
		if (tempW->next == NULL) {
			if (currentP->x != 1600 || currentP->y != 720) game_over = 1;
			else end_game = 1;
		}
	}

	// game over
	if (game_over == 1) {
		ofSetColor(255, 0, 0);
		myfont.load("Arial.ttf", 80);
		myfont.drawString("Game Over", 530, 340);
		myfont.load("Arial.ttf", 30);
		myfont.drawString("- Press R to restart", 630, 420);
		myfont.drawString("- Press C to close", 630, 500);
	}

	// game succeeded
	if (end_game == 1) {
		ofSetColor(255, 0, 0);
		myfont.load("Arial.ttf", 80);
		myfont.drawString("Your score : ", 390, 370);
		myfont.drawString(ssInt.str(), 985, 380);
		myfont.load("Arial.ttf", 30);
		myfont.drawString("- Press R to restart", 630, 480);
		myfont.drawString("- Press C to close", 630, 560);
	}

}

//----------- Execute an order allocated on each alphabet keys -----------
void ofApp::keyPressed(int key){
	// start the game
	if (key == 's' || key == 'S') start_flag = 1;

	// quit the game
	if (key == 'q' || key == 'Q') {
		if(game_over==0 && end_game==0) game_over = 1;
	}

	// restart the game
	if (key == 'r' || key == 'R') {
		// free allocated memories
		if (game_over == 1 || end_game == 1) {
			struct pipe *p;
			while (startP->next != NULL) {
				p = startP;
				startP = startP->next;
				free(p);
			} free(startP);

			struct water *w;
			while (startW->next != NULL) {
				w = startW;
				startW = startW->next;
				free(w);
			} free(startW);

			// get ready to start the game again
			setup();
			game_over = 0;
			end_game = 0;
			start_flag = 0;
		}
	}

	// exit the game
	if (key == 'c' || key == 'C') {
		// free allocated memories
		if (game_over == 1 || end_game == 1) {
			struct pipe *p;
			while (startP->next != NULL) {
				p = startP;
				startP = startP->next;
				free(p);
			} free(startP);

			struct water *w;
			while (startW->next != NULL) {
				w = startW;
				startW = startW->next;
				free(w);
			} free(startW);
			// close the game screen
			_Exit(0);
		}
	}
}

//----------- Execute an order allocated on each arrow keys and spacebar -----------
void ofApp::keyReleased(int key){
	// only operate when game is on the way
	if (start_flag == 1 && game_over != 1) {
		// change pipe type
		if (key == OF_KEY_RIGHT) {
			currentP->pipe_type++;
			if (currentP->pipe_type > 3) currentP->pipe_type = 1;
		}
		if (key == OF_KEY_LEFT) {
			currentP->pipe_type--;
			if (currentP->pipe_type < 1) currentP->pipe_type = 3;
		}

		// change pipe rotation
		if (key == OF_KEY_UP) {
			currentP->rotation++;
			if (currentP->rotation > 4) currentP->rotation = 1;
		}
		if (key == OF_KEY_DOWN) {
			currentP->rotation--;
			if (currentP->rotation < 1) currentP->rotation = 4;
		}

		// place pipe on current location
		if (key == OF_KEY_SPACE) {
			if (complete == 0) {
				addPath();

				// make new pipe node and link to the pipe linkedlist
				struct pipe *p;
				p = (struct pipe*)malloc(sizeof(struct pipe));
				p->pipe_type = currentP->pipe_type;
				p->rotation = currentP->rotation;
				p->x = pxNew; p->y = pyNew;
				p->next = NULL;
				currentP->next = p;
				currentP = p;
				pipe_flag = 1;
				if (pxNew == 1600 && pyNew == 720) complete = 1;

				// check rather the pipe is overlapped
				// check by confirming the color of specific location of the screen
				screen.grabScreen(0, 0, 1640, 840);
				pix = screen.getPixels();
				c3 = pix.getColor(pxNew + 60, pyNew + 60);
				if (c1 == c3) game_over = 1;
				if (c2 == c3) game_over = 1;
				if (pxNew > 1640 || pxNew < 0) game_over = 1;
				if (pyNew > 840 || pyNew < 0) game_over = 1;
			}
		}
	}
}

//----------- Function for drawing pink pipe -----------
// consider : type of pipe, rotation, location
// pink pipe : a pipe whilch is waiting to be placed by user.
void ofApp::drawPipe() {
	if (currentP->pipe_type == 1) {
		float x, y;
		switch (currentP->rotation) {
		case 1:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 120, 40);
			ofSetColor(255, 0, 0);
			ofDrawRectangle(x + 105, y - 10, 15, 60);
			break;
		case 2:
			x = currentP->x + 40;
			y = currentP->y;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 120);
			ofSetColor(255, 0, 0);
			ofDrawRectangle(x - 10, y + 105, 60, 15);
			break;
		case 3:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 120, 40);
			ofSetColor(255, 0, 0);
			ofDrawRectangle(x, y - 10, 15, 60);
			break;
		case 4:
			x = currentP->x + 40;
			y = currentP->y;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 120);
			ofSetColor(255, 0, 0);
			ofDrawRectangle(x - 10, y, 60, 15);
			break;
		}
	}

	else if (currentP->pipe_type == 2) {
		float x, y;
		switch (currentP->rotation) {
		case 1:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x + 40, y + 40, 40, 40);
			ofSetColor(252, 0, 0);
			ofDrawRectangle(x + 30, y + 65, 60, 15);
			break;
		case 2:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x + 40, y - 40, 40, 40);
			ofSetColor(252, 0, 0);
			ofDrawRectangle(x, y - 10, 15, 60);
			break;
		case 3:
			x = currentP->x + 40;
			y = currentP->y;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x, y + 40, 40, 40);
			ofDrawRectangle(x + 40, y + 40, 40, 40);
			ofSetColor(252, 0, 0);
			ofDrawRectangle(x - 10, y, 60, 15);
			break;
		case 4:
			x = currentP->x + 40;
			y = currentP->y + 40;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x, y + 40, 40, 40);
			ofSetColor(252, 0, 0);
			ofDrawRectangle(x + 65, y - 10, 15, 60);
			break;
		}
	}

	else if (currentP->pipe_type == 3) {
		float x, y;
		switch (currentP->rotation) {
		case 1:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x + 40, y + 40, 40, 40);
			ofSetColor(252, 0, 0);
			ofDrawRectangle(x, y - 10, 15, 60);
			break;
		case 2:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x + 40, y - 40, 40, 40);
			ofSetColor(252, 0, 0);
			ofDrawRectangle(x + 30, y - 40, 60, 15);
			break;
		case 3:
			x = currentP->x + 40;
			y = currentP->y;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x, y + 40, 40, 40);
			ofDrawRectangle(x + 40, y + 40, 40, 40);
			ofSetColor(252, 0, 0);
			ofDrawRectangle(x + 65, y + 30, 15, 60);
			break;
		case 4:
			x = currentP->x + 40;
			y = currentP->y + 40;
			ofSetColor(252, 172, 175);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x, y + 40, 40, 40);
			ofSetColor(252, 0, 0);
			ofDrawRectangle(x - 10, y + 65, 60, 15);
			break;
		}
	}
}

//----------- Function for drawing grey pipe -----------
// consider every pipes saved in pipe linked list
// grey pipe = pipes already placed on the screen
void ofApp::placePipe() {
	if (currentP->pipe_type == 1) {
		float x, y;
		switch (currentP->rotation) {
		case 1:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 120, 40);
			ofDrawRectangle(x + 105, y - 10, 15, 60);
			break;
		case 2:
			x = currentP->x + 40;
			y = currentP->y;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 120);
			ofDrawRectangle(x - 10, y + 105, 60, 15);
			break;
		case 3:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 120, 40);
			ofDrawRectangle(x, y - 10, 15, 60);
			break;
		case 4:
			x = currentP->x + 40;
			y = currentP->y;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 120);
			ofDrawRectangle(x - 10, y, 60, 15);
			break;
		}
	}

	else if (currentP->pipe_type == 2) {
		float x, y;
		switch (currentP->rotation) {
		case 1:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x + 40, y + 40, 40, 40);
			ofDrawRectangle(x + 30, y + 65, 60, 15);
			break;
		case 2:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x + 40, y - 40, 40, 40);
			ofDrawRectangle(x, y - 10, 15, 60);
			break;
		case 3:
			x = currentP->x + 40;
			y = currentP->y;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x, y + 40, 40, 40);
			ofDrawRectangle(x + 40, y + 40, 40, 40);
			ofDrawRectangle(x - 10, y, 60, 15);
			break;
		case 4:
			x = currentP->x + 40;
			y = currentP->y + 40;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x, y + 40, 40, 40);
			ofDrawRectangle(x + 65, y - 10, 15, 60);
			break;
		}
	}

	else if (currentP->pipe_type == 3) {
		float x, y;
		switch (currentP->rotation) {
		case 1:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x + 40, y + 40, 40, 40);
			ofDrawRectangle(x, y - 10, 15, 60);
			break;
		case 2:
			x = currentP->x;
			y = currentP->y + 40;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x + 40, y - 40, 40, 40);
			ofDrawRectangle(x + 30, y - 40, 60, 15);
			break;
		case 3:
			x = currentP->x + 40;
			y = currentP->y;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x, y + 40, 40, 40);
			ofDrawRectangle(x + 40, y + 40, 40, 40);
			ofDrawRectangle(x + 65, y + 30, 15, 60);
			break;
		case 4:
			x = currentP->x + 40;
			y = currentP->y + 40;
			ofSetColor(190, 190, 190);
			ofDrawRectangle(x, y, 40, 40);
			ofDrawRectangle(x + 40, y, 40, 40);
			ofDrawRectangle(x, y + 40, 40, 40);
			ofDrawRectangle(x - 10, y + 65, 60, 15);
			break;
		}
	}
}

//----------- Function for addning water nodes in water linked list -----------
// save every location water will go by : according to the pipes place on the screen
// on every location the small blue circle will be drawn(water path)
void ofApp::addPath() {
	if (currentP->pipe_type == 1) {
		struct water *w;
		w = (struct water*)malloc(sizeof(struct water));
		switch (currentP->rotation) {
		case 1:
			w->x = currentP->x + 1; w->y = currentP->y + 60;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 120; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x + 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x + 120;
			pyNew = currentP->y;
			break;
		case 2:
			w->x = currentP->x + 60; w->y = currentP->y + 1;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 120; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y + 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x;
			pyNew = currentP->y + 120;
			break;
		case 3:
			w->x = currentP->x + 119; w->y = currentP->y + 60;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 120; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x - 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x - 120;
			pyNew = currentP->y;
			break;
		case 4:
			w->x = currentP->x + 60; w->y = currentP->y + 119;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 120; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y - 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x;
			pyNew = currentP->y - 120;
			break;
		}
	}

	else if (currentP->pipe_type == 2) {
		struct water *w;
		w = (struct water*)malloc(sizeof(struct water));
		switch (currentP->rotation) {
		case 1:
			w->x = currentP->x + 1; w->y = currentP->y + 60;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x + 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y + 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x;
			pyNew = currentP->y + 120;
			break;
		case 2:
			w->x = currentP->x + 60; w->y = currentP->y + 1;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y + 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x - 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x - 120;
			pyNew = currentP->y;
			break;
		case 3:
			w->x = currentP->x + 119; w->y = currentP->y + 60;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x - 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y - 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x;
			pyNew = currentP->y - 120;
			break;
		case 4:
			w->x = currentP->x + 60; w->y = currentP->y + 119;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y - 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x + 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x + 120;
			pyNew = currentP->y;
			break;
		}
	}
	else if (currentP->pipe_type == 3) {
		struct water *w;
		w = (struct water*)malloc(sizeof(struct water));
		switch (currentP->rotation) {
		case 1:
			w->x = currentP->x + 60; w->y = currentP->y + 119;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y - 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x - 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x - 120;
			pyNew = currentP->y;
			break;
		case 2:
			w->x = currentP->x + 1; w->y = currentP->y + 60;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x + 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y - 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x;
			pyNew = currentP->y - 120;
			break;
		case 3:
			w->x = currentP->x + 60; w->y = currentP->y + 1;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y + 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x + 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x + 120;
			pyNew = currentP->y;
			break;
		case 4:
			w->x = currentP->x + 119; w->y = currentP->y + 60;
			w->next = NULL;
			endW->next = w; endW = w;
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x - 1; w->y = endW->y;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			for (int i = 1; i < 60; i++) {
				struct water *w;
				w = (struct water*)malloc(sizeof(struct water));
				w->x = endW->x; w->y = endW->y + 1;
				w->next = NULL;
				endW->next = w; endW = w;
			}
			pxNew = currentP->x;
			pyNew = currentP->y + 120;
			break;
		}
	}
}