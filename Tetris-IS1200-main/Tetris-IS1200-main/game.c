#include <pic32mx.h>
#include <stdint.h>
#include <stdbool.h>

/* Code written by Pontus Rydén and Daniel Hartman 2022
*/

// getDecStr made by @Lundin on StackOverflow
void getDecStr (uint8_t* str, uint8_t len, uint32_t val) {
	uint8_t i;

	for(i = 1; i <= len; i++) {
		str[len-i] = (uint8_t) ((val % 10UL) + '0');
		val = val/10;
	}

	str[i-1] = '\0';

}

int globalcount = 0;
int s = 0;
int score = 0;

// Matrix for gameover screen, sets every pixel white
uint8_t death[] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};

// Struct for keeping all data for shapes
struct Shape {
	int piece[4][4];
};

// Declares the shapes into the struct
struct Shape shapes[6] = {{{{0, 0, 0, 0},
						   {0, 0, 0, 0},
						   {1, 1, 0, 0},
						   {1, 1, 0, 0}}},

						  {{{0, 0, 0, 0},
						   {1, 0, 0, 0},
						   {1, 0, 0, 0},
						   {1, 1, 0, 0}}},

						   {{{0, 0, 0, 0},
						   {0, 1, 0, 0},
						   {0, 1, 0, 0},
						   {1, 1, 0, 0}}},

						  {{{1, 0, 0, 0},
						   {1, 0, 0, 0},
						   {1, 0, 0, 0},
						   {1, 0, 0, 0}}},

						  {{{0, 0, 0, 0},
						   {0, 0, 0, 0},
						   {0, 1, 0, 0},
						   {1, 1, 1, 0}}},

						  {{{0, 0, 0, 0},
						   {0, 0, 0, 0},
						   {0, 1, 1, 0},
						   {1, 1, 0, 0}}}};

int arena[10][32];

const int ARENAYOFFSET = 31; // should be the same as playerPosY
const int ARENAHEIGHT = 32;
const int ARENAWIDTH = 10;

int playerPosX = 13;
int playerPosY = 31;

// Starts timer for ticking the update function
void timer_init() {
	T2CONSET = 0x70; // 1:256 prescale, internal clock
  	PR2 = ((80000000/256)/100);
  	TMR2 = 0; // reset counter
  	T2CONSET = 0x8000;  // timer on
}

// Draws the border of the playing field
void draw_borders(void) {
	int i;
	int j;
    for (i = 0; i <= ARENAWIDTH*3; ++i)
    {
        for (j = 0; j < ARENAHEIGHT*3; ++j)
        {
            if(i == 0 || i == (ARENAWIDTH*3)) {
                setPixel(i,j+32);
            }

            if(j == 0 || j == ARENAHEIGHT*3-1) {
                setPixel(i,j+32);
            }
        }
    }
    
}

// Draw the active shapes to the gamescreen
void drawMatrix( int playerPosX,int playerPosY) {
	int x = 0;
	int y = 0;
	for ( x = 0; x < 4; x++)
	{
		for ( y = 0; y < 4; y++)
		{
			if(shapes[s].piece[x][y] == 1)
			{
				fillSquare((y*3) + playerPosX, (3*x) + playerPosY-3);
			}
		}
	}
	}


int dropInterval = 1000;
int dropInterval2 = 200;
int dropCounter = 0;
int dropCounter2 = 0;
int lastTime = 0;
float time = 0;

// Checks if the player piece is colliding a floor (blocks or border)
bool collisionFloor() {
	if((playerPosY > ((ARENAHEIGHT*3)+ARENAYOFFSET)-9))
				{
					return true;
				}
				int x = 0;
				int y = 0;
				for(x = 0; x < 4; x++) {
					for(y = 0; y < 4; y++) {
						if(shapes[s].piece[y][x] != 0 &&
                    (arena[x + (playerPosX)/3][y-1 + (playerPosY-ARENAYOFFSET)/3]) != 2) { //Sätt till två?
                    return true;
                }
					}
				}
				return false;
}

// Checks if the player piece is colliding with a wall (blocks or border)
bool collisionWalls() {
	int x;
	int y;
	for(x = 0; x < 4; x++) {
		for(y = 0; y < 4; y++) {
			if(shapes[s].piece[y][x] != 0) {
				if(((playerPosX/3)+x > 9) || ((playerPosX + 3*x) < 0)) {
				return true;
			}
				if(arena[((playerPosX)/3)+x][((playerPosY-ARENAYOFFSET)/3)+y-1] != 2) {
					return true;
				}
			}
		}
	}
	return false; 
}

// Clears a row and moves rows above down 1 step
void clearRow(int row) {
	int i;
	int j;
	for(j = row; j > 0; j--) {
		for(i = 0; i < ARENAWIDTH; i++) {
		arena[i][j] = arena[i][j-1];
		}
	}
	for(i = 0; i < ARENAWIDTH; i++) {
		arena[i][0] = 2;
	}
}

// Checks of a row has been filled
void clear() {
    int x = 0;
    int y = 0;
   bool isFilled = true;

    for ( y = 0; y < ARENAHEIGHT; y++) {
         isFilled = true;
        for ( x = 0; x < ARENAWIDTH; x++) {
            if (arena[x][y] == 2){
                isFilled = false;
            }
            }
            if(isFilled) {
            	clearRow(y); 
            	score++;       
        }
    }
}

// Initialises the arena
void createArena() {
	int i;
	int j;
	for(i = 0; i < 10; i++) {
	for(j = 0; j < 32; j++) {
		arena[i][j] = 2;
	}
}
}

// Draws the arena by filling squares
void drawArena() {

	int y = 0;
	int x = 0;

	for (x = 0; x < ARENAWIDTH; x++) {
		for (y = 0; y < ARENAHEIGHT; y++) {
			if(arena[x][y] != 2)
			{
				fillSquare((x * 3)+1, (ARENAYOFFSET + (y * 3)));
			}
		}		
	}
}

// Adds blocks to the arena where they will stay put
void addToArena() {
	int y = 0;
	int x = 0;
	for (x = 0; x < 4; x++) {
		for (y = 0; y < 4; y++) {
			if(shapes[s].piece[y][x] != 0) {
					arena[((playerPosX)/3)+x][((playerPosY-ARENAYOFFSET-3)/3)+y] = 1;
				} 
			}
		}
	}

// clears the entire display
	void display_clear()  {
    int i;
    for(i = 0; i < 128; i+= 32) {
        display_image(i, death);
    }
}

// Stops the game and displays the score (rows cleared)
void game_over() {
	int i;
	char str[3];
	getDecStr(str, 3, score);
	for(i = 0; i < ARENAWIDTH; i++) {
		if(arena[i][1] == 1) {
			display_clear();
			while(1) {
				display_string(3, &str);
				display_update();
			}
		}
	}
}

// Adds a new user piece to the top of the screen
void pieceReset() {
	addToArena();
	game_over();
	clear();
	s++;
	if(s == 6) {
		s = 0;
	}
	playerPosY = 31;
	playerPosX = 13;
}

// Increases player drop speed
void playerDrop(void) {
	playerPosY += 3;
	dropCounter = 0;
}

// Moves the player piece depending on user input from buttons
void playerMove(int value) {
	playerPosX += (value*3);

	if(collisionWalls()){
		playerPosX += -value*3;
	}

}

// Checks which input buttons are pressed
void input(int btns) {
	if((btns & 0x1) == 1) {
		playerDrop();

		if(collisionFloor()) {
			playerPosY += -3;
			pieceReset();
		}
	}

	if(dropCounter2 > dropInterval2){
		if((btns & 0x2) == 2) {
			playerMove(-1);
			dropCounter2 = 0;
		}

		if((btns & 0x4) == 4) {
			playerMove(1);
			dropCounter2 = 0;
		}
		
	}
	
	
	
}

// Initializes the game by creating the arena (playing area)
void start_game() {
	createArena();
}

// runs every tick to update game data
void update() {
	int btns = getbtns();
	time += 0.01;


	if (IFS(0) & 0x100) {
		input(btns);

		const int deltaTime = (int)time;
 		dropCounter += deltaTime;
 		dropCounter2 += deltaTime;
 		time = 0;

 		if(dropCounter > dropInterval){
			playerDrop();

			if(collisionFloor()){
	 			playerPosY += -3;
	 			pieceReset();
	 		}
 		}


		IFS(0) = 0;

		draw_borders();

		drawArena();
		drawMatrix( playerPosX, playerPosY);

		render();
	}
}


