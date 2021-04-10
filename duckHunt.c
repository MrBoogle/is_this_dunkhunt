//#include"address_map_arm.h"/*function prototypes*/
int HEX3_HEX0_BASE = 0xFF200020;
int HEX5_HEX4_BASE = 0xFF200030;
PS2_BASE = 0xFF200100;
void HEX_PS2(char,char,char);/********************************************************************************This program demonstrates use of the PS/2 port by displaying the last three*bytes of data received from the PS/2 port on the HEX displays.******************************************************************************/


/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

#define CURSOR_CLR 0
/* Constants for animation */
#define BOX_LEN 4
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1
#define SHOT_RANGE 10
#define BULLET_CLR YELLOW

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
// Begin part1.s for Lab 7
void plot_pixel(int x, int y, short int line_color);

volatile int pixel_buffer_start; // global variable

int score = 0;
int highScore = 0;
int strike = 0;


void wait_for_vsync() {
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	register int status;
	*pixel_ctrl_ptr = 1;
	
	status = *(pixel_ctrl_ptr+3);
	while ((status & 0x01) != 0) {
		status = *(pixel_ctrl_ptr + 3);
	}
}


int abs(int a) {
	if (a < 0) return -a;
	return a;
}

void swap (int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

//Each point holds the x and y location of a color to be drawn
struct point {
	int xPos;
	int yPos;
	int color;
};

typedef struct point point;

point playerLoc = {RESOLUTION_X/2, RESOLUTION_Y/2 + 80, 0};


struct target {
	int xPos;
	int yPos;
	int xVel;
	int yVel;
	int timer;
	int shot; //1 if shot, 0 if not shot
	int spawned; //Only draw and register shots when set to 1
	point image[25];
	point previous[25];
	point toDelete[25];

};

typedef struct target target;
void initTarget(target* gameT, int t);
target TARGETS[NUM_BOXES];


struct cursor {
	int xPos;
	int yPos;
	int shot; //1 if shot has been fired, 0 otherwise, reset to 0 once shot is registered on/off target
	point image[8];
	point previous[8];
	point toDelete[8];
};



struct gun {
	int xPos;
	int yPos;
	int shot; //1 if shot has been fired, 0 otherwise, reset to 0 once shot is registered on/off target
	point image[8];
	point previous[8];
	point toDelete[8];
};



struct bullet {
	int xPos;
	int yPos;
	int xVel;
	int yVel;
	int shot; //1 if shot has been fired, 0 otherwise, reset to 0 once shot is registered on/off target
	point image[25];
	point previous[25];
	point toDelete[25];
};

point bulletImage[25] = {
	{-2, 2, BULLET_CLR}, {-1, 2, BULLET_CLR}, {0, 2, BULLET_CLR}, {1, 2, BULLET_CLR}, {2, 2, BULLET_CLR},	
	{-2, 1, BULLET_CLR}, {-1, 1, BULLET_CLR}, {0, 1, BULLET_CLR}, {1, 1, BULLET_CLR}, {2, 1, BULLET_CLR},
	{-2, 0, BULLET_CLR}, {-1, 0, BULLET_CLR}, {0, 0, BULLET_CLR}, {1, 0, BULLET_CLR}, {2, 0, BULLET_CLR},
	{-2, -1, BULLET_CLR}, {-1, -1, BULLET_CLR}, {0, -1, BULLET_CLR}, {1, -1, BULLET_CLR}, {2, -1, BULLET_CLR},
	{-2, -2, BULLET_CLR}, {-1, -2, BULLET_CLR}, {0, -2, BULLET_CLR}, {1, -2, BULLET_CLR}, {2, -2, BULLET_CLR}
	
};


typedef struct bullet bullet;

bullet BULLETS[NUM_BOXES];

typedef struct gun gun;

point gunImage[8] = {{0, 2, CURSOR_CLR}, {0, 1, CURSOR_CLR}, {-2, 0, CURSOR_CLR}, {-1, 0, CURSOR_CLR}, {1, 0, CURSOR_CLR}, {2, 0, CURSOR_CLR}, {0, -1, CURSOR_CLR}, {0, -2, CURSOR_CLR}};

typedef struct cursor cursor;
void initTargets() {
	srand(time(0));
	for (int i = 0; i < NUM_BOXES; i++) {
		
		
		initTarget(&TARGETS[i], rand());  
	}
	
}

point cursorImage[8] = {{0, 2, CURSOR_CLR}, {0, 1, CURSOR_CLR}, {-2, 0, CURSOR_CLR}, {-1, 0, CURSOR_CLR}, {1, 0, CURSOR_CLR}, {2, 0, CURSOR_CLR}, {0, -1, CURSOR_CLR}, {0, -2, CURSOR_CLR}};

point targetImage[25] = {
	{-2, 2, RED}, {-1, 2, RED}, {0, 2, RED}, {1, 2, RED}, {2, 2, RED},	
	{-2, 1, RED}, {-1, 1, WHITE}, {0, 1, WHITE}, {1, 1, WHITE}, {2, 1, RED},
	{-2, 0, RED}, {-1, 0, WHITE}, {0, 0, RED}, {1, 0, WHITE}, {2, 0, RED},
	{-2, -1, RED}, {-1, -1, WHITE}, {0, -1, WHITE}, {1, -1, WHITE}, {2, -1, RED},
	{-2, -2, RED}, {-1, -2, RED}, {0, -2, RED}, {1, -2, RED}, {2, -2, RED}
	
};


void initCursor(cursor* gameC) {
	//Initialize cursor
	gameC->xPos = RESOLUTION_X/2;
	gameC->yPos = RESOLUTION_Y/2;
	for (int i = 0; i < 8; i++) {
		gameC->image[i] = cursorImage[i];
		gameC->previous[i] = cursorImage[i];
		gameC->toDelete[i] = cursorImage[i];
	}
	
}


void initBullet(bullet* gameB) {
	//Initialize cursor
	gameB->xPos = -5;
	gameB->yPos = -5;
	gameB->yPos = 0;
	gameB->xVel = 1;
	gameB->yVel = 1;
	for (int i = 0; i < 8; i++) {
		gameB->image[i] = bulletImage[i];
		gameB->previous[i] = bulletImage[i];
		gameB->toDelete[i] = bulletImage[i];
	}
	
}

void initBullets() {
	for (int b = 0; b < NUM_BOXES; b++) {
		initBullet(&BULLETS[b]);
	}
}

void initTarget(target* gameT, int t) {
	//Initialize cursor
	
	gameT->xPos = t%RESOLUTION_X;
	gameT->yPos = t%RESOLUTION_Y;
	gameT->xVel = (t%2)*2 - 1;
	gameT->yVel = (t%2)*2 - 1;
	gameT->shot = 0;
	gameT->timer = t%81 + 80;
	for (int i = 0; i < 25; i++) {
		gameT->image[i] = targetImage[i];
		gameT->previous[i] = targetImage[i];
		gameT->toDelete[i] = targetImage[i];
	}
	
}

void initGun(gun* gameG) {
	//Initialize cursor
	gameG->xPos = RESOLUTION_X/2;
	gameG->yPos = RESOLUTION_Y/2+60;
	for (int i = 0; i < 8; i++) {
		gameG->image[i] = gunImage[i];
		gameG->previous[i] = gunImage[i];
		gameG->toDelete[i] = gunImage[i];
	}
	
}



void flushPS2() {
	volatile int* PS2_ptr = (int*)PS2_BASE;
	//*(PS2_ptr) = 0xFF;
	int PS2_data =*(PS2_ptr);
	int byte3 = PS2_data & 0xFF;
}

int checkShot(cursor *check) {
	
	for (int i = 0; i < NUM_BOXES; i++) {
		int deltaX = abs((*check).xPos - TARGETS[i].xPos);
		int deltaY = abs(check->yPos - TARGETS[i].yPos);
		if (deltaY < SHOT_RANGE && deltaX < SHOT_RANGE) {
			score++;
			check->shot = 1;
			return 1;
		} 
	}
	//strike++;
	return 0;
}

void renderCursor(cursor* gameC, int valid) { /**Rendering is for graphics, this fucntion take user input to calculate position to be rendered**/
	int n = sizeof(gameC->toDelete)/sizeof(gameC->toDelete[0]);
	//Delete toDel and shift previous into del
	//Shidt current into prev
	for (int i = 0; i < n; i++){
		if (valid) plot_pixel(gameC->toDelete[i].xPos, gameC->toDelete[i].yPos, gameC->toDelete[i].color);
		//plot_pixel(0, 0, RED);
		gameC->toDelete[i] = gameC->previous[i];
		gameC->previous[i].xPos = gameC->xPos + gameC->image[i].xPos;
		gameC->previous[i].yPos = gameC->yPos + gameC->image[i].yPos;
		gameC->previous[i].color = *(short int *)(pixel_buffer_start + (gameC->yPos + gameC->image[i].yPos << 10) + (gameC->xPos + gameC->image[i].xPos << 1));
		flushPS2();
	}

	//Draw current
	for (int i = 0; i < n; i++) {
		plot_pixel(gameC->xPos + gameC->image[i].xPos, gameC->yPos + gameC->image[i].yPos, gameC->image[i].color);
		flushPS2();
	}
}

void renderGun(gun* gameG, int valid, int flash) { /**Rendering is for graphics, this fucntion take user input to calculate position to be rendered**/
	int n = sizeof(gameG->toDelete)/sizeof(gameG->toDelete[0]);
	//Delete toDel and shift previous into del
	//Shidt current into prev
	for (int i = 0; i < n; i++){
		if (valid) plot_pixel(gameG->toDelete[i].xPos, gameG->toDelete[i].yPos, gameG->toDelete[i].color);
		//plot_pixel(0, 0, RED);
		gameG->toDelete[i] = gameG->previous[i];
		gameG->previous[i].xPos = gameG->xPos + gameG->image[i].xPos;
		gameG->previous[i].yPos = gameG->yPos + gameG->image[i].yPos;
		gameG->previous[i].color = *(short int *)(pixel_buffer_start + (gameG->yPos + gameG->image[i].yPos << 10) + (gameG->xPos + gameG->image[i].xPos << 1));
		flushPS2();
	}

	//Draw current
	for (int i = 0; i < n; i++) {
		
		if (flash && valid) {
			plot_pixel(gameG->xPos + gameG->image[i].xPos, gameG->yPos + gameG->image[i].yPos, gameG->image[i].color);
			
		}
			else if (valid){
				plot_pixel(gameG->xPos + gameG->previous[i].xPos, gameG->yPos + gameG->previous[i].yPos, gameG->previous[i].color);
			}
		
		flushPS2();
	}
	
}



void renderTarget(target* gameT, int valid) {/**Similar comment as above, just change the names so that it reflects this.**/
	int n = sizeof(gameT->toDelete)/sizeof(gameT->toDelete[0]);
	//Delete toDel and shift previous into del
	//Shidt current into prev
	for (int i = 0; i < n; i++){
		int deleteValid = gameT->toDelete[i].xPos <= 320 && gameT->toDelete[i].xPos >= 0 && gameT->toDelete[i].yPos <= 240 && gameT->toDelete[i].yPos >= 0;
		if (valid && deleteValid) plot_pixel(gameT->toDelete[i].xPos, gameT->toDelete[i].yPos, gameT->toDelete[i].color);
		//plot_pixel(0, 0, RED);
		gameT->toDelete[i] = gameT->previous[i];
		gameT->previous[i].xPos = gameT->xPos + gameT->image[i].xPos;
		gameT->previous[i].yPos = gameT->yPos + gameT->image[i].yPos;
		gameT->previous[i].color = CYAN;// *(short int *)(pixel_buffer_start + (gameT->yPos + gameT->image[i].yPos << 10) + (gameT->xPos + gameT->image[i].xPos << 1));
		flushPS2();
	}

	//Draw current
	for (int i = 0; i < n; i++) {
		int x = gameT->xPos + gameT->image[i].xPos;
		int y = gameT->yPos + gameT->image[i].yPos;
		int drawValid = x <= 320 && x >= 0 && y <= 240 && y >= 0;
		if (drawValid) plot_pixel(x, y, gameT->image[i].color);
		flushPS2();
	}
}


void renderBullet(bullet* gameT, int valid) {/**Similar comment as above, just change the names so that it reflects this.**/
	int n = sizeof(gameT->toDelete)/sizeof(gameT->toDelete[0]);
	//Delete toDel and shift previous into del
	//Shidt current into prev
	for (int i = 0; i < n; i++){
		int deleteValid = gameT->toDelete[i].xPos <= 320 && gameT->toDelete[i].xPos >= 0 && gameT->toDelete[i].yPos <= 240 && gameT->toDelete[i].yPos >= 0;
		if (valid && deleteValid) plot_pixel(gameT->toDelete[i].xPos, gameT->toDelete[i].yPos, gameT->toDelete[i].color);
		//plot_pixel(0, 0, RED);
		gameT->toDelete[i] = gameT->previous[i];
		gameT->previous[i].xPos = gameT->xPos + gameT->image[i].xPos;
		gameT->previous[i].yPos = gameT->yPos + gameT->image[i].yPos;
		gameT->previous[i].color = CYAN;// *(short int *)(pixel_buffer_start + (gameT->yPos + gameT->image[i].yPos << 10) + (gameT->xPos + gameT->image[i].xPos << 1));
		flushPS2();
	}

	//Draw current
	for (int i = 0; i < n; i++) {
		int x = gameT->xPos + gameT->image[i].xPos;
		int y = gameT->yPos + gameT->image[i].yPos;
		int drawValid = x <= 320 && x >= 0 && y <= 240 && y >= 0;
		if (drawValid) plot_pixel(x, y, gameT->image[i].color);
		flushPS2();
	}
}


void draw_line(int x1, int y1, int x2, int y2, short int color) {
	int is_steep;// = abs(y2 - y1) > abs(x2 - x1)? 1: 0;
	if (abs(y2 - y1) > abs(x2 - x1)) {
		is_steep = 1;
	} else {
		is_steep = 0;
	}
	if (is_steep) {
		swap(&x1, &y1);
		swap(&x2, &y2);
	}
	if (x1 > x2) {
		swap(&x1, &x2);
		swap(&y1, &y2);
		
	}
	int deltax = x2 - x1;
	int deltay = abs(y2 - y1);
	int error = -(deltax / 2);
	int y = y1;
	int y_step;// = y1 < y2 ? 1 : -1;
	if (y1 < y2) {
		y_step = 1;
	} else {
		y_step = -1;
	}
	
	for (int i = x1; i < x2; i++) {
		if (is_steep) {
			plot_pixel(y, i, color);
		} else {
			plot_pixel(i, y, color);
		}
		error = error + deltay;
		if (error >= 0) {
			y += y_step;
			error = error - deltax;
		}
	}
}

void clear_screen(int color) {
	for (int x = 0; x < 320; x++) {
		for (int y = 0; y < 240; y++) {
			*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = color;
		}
	}
}



void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void drawBox (int x, int y, int color) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; i++) {
			if (x+i < RESOLUTION_X && y+j < RESOLUTION_Y)plot_pixel(x+i, y+j, color);
		}
	}
}

void draw_text(int x,int y,char* text_ptr){
	int offset;
	volatile char*character_buffer =(char*)FPGA_CHAR_BASE;
	offset = (y << 7) + x;
	while(*(text_ptr)) {
		*(character_buffer + offset) =*(text_ptr);
		++text_ptr;
		++offset;
	}
}

//------------LETTERS
void letter_E(int x,int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i < 3 || i > 26 || i>6 && i<9){
				plot_pixel(x+j,y+i,0xffff);
			}
		}
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
	}
}

void letter_V(int x,int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i==26){plot_pixel(x+j,y+i,0xffff);}
			if(i==27 && (j!=0 && j!=9) ){plot_pixel(x+j,y+i,0xffff);}
			if(i==28 && (j!=0 && j!=9 && j!=1 && j!=8) ){plot_pixel(x+j,y+i,0xffff);}
			if(i==29 && (j!=0 && j!=9 && j!=1 && j!=8 && j!=2 && j!=7) ){plot_pixel(x+j,y+i,0xffff);}
			//if(i==29 && (j!=0 && j!=9 && j!=1 && j!=8 && j!=2 && j!=7 && j!=3 && j!=6) ){plot_pixel(x+j,y+i,0xffff);}
		}
		if(i<26){
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
		plot_pixel(x+8,y+i,0xffff);	
		plot_pixel(x+9,y+i,0xffff);
		}
	}
}

void letter_T(int x,int y){
	for(int i=0;i<30;i++){
		if(i<3){
			for(int k=0;k<10;k++){
				plot_pixel(x+k,y+i,0xffff);
			}
		}
		plot_pixel(x+4,y+i,0xffff);
		plot_pixel(x+5,y+i,0xffff);
		plot_pixel(x+6,y+i,0xffff);	
	}
}
void letter_R(int x,int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i<3){plot_pixel(x+j,y+i,0xffff);}
			if( (i == 7 || i == 9) && j != 9){plot_pixel(x+j,y+i,0xffff);}
			if(i == 8 && (j != 9 && j != 8 ) ){plot_pixel(x+j,y+i,0xffff);}
		}
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
		if(i<=6 || i>=10){
			plot_pixel(x+7,y+i,0xffff);
			plot_pixel(x+8,y+i,0xffff);	
			plot_pixel(x+9,y+i,0xffff);
		}
	}

}

void letter_M(int x, int y){
	for(int i=0;i<30;i++){
		
		for(int j=0;j<10;j++){
			if( i==0 && (j<=2 || j>=8) ){plot_pixel(x+j,y+i,0xffff);}
			if( i==1 && (j<=3 || j>=7) ){plot_pixel(x+j,y+i,0xffff);}
			if( i==2 && j != 5){plot_pixel(x+j,y+i,0xffff);}
			if(i==3){plot_pixel(x+j,y+i,0xffff);}
			if( i==4 && (j>2 && j<8) ){plot_pixel(x+j,y+i,0xffff);}
			if( i==5 && (j>3 && j<7) ){plot_pixel(x+j,y+i,0xffff);}
			if( i==6 && j == 5){plot_pixel(x+j,y+i,0xffff);}
		}
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
		plot_pixel(x+2,y+i,0xffff);
		plot_pixel(x+8,y+i,0xffff);	
		plot_pixel(x+9,y+i,0xffff);
		
	}
}
	
void letter_G(int x, int y){
	for(int i=0;i<30;i++){
		if(i<3 || i>26){
			for(int k=0;k<10;k++){
				plot_pixel(x+k,y+i,0xffff);
			}
		}
		if(i!=3 && i!=4){
			plot_pixel(x+8,y+i,0xffff);	
			plot_pixel(x+9,y+i,0xffff);
		}
		if(i == 5 || i == 6){
			plot_pixel(x+5,y+i,0xffff);	
			plot_pixel(x+6,y+i,0xffff);	
			plot_pixel(x+7,y+i,0xffff);	
			plot_pixel(x+8,y+i,0xffff);	
			plot_pixel(x+9,y+i,0xffff);
		}
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
		
	}
}
void letter_O(int x,int y){
	for(int i=0;i<30;i++){
		if(i<3 || i>26){
			for(int k=0;k<10;k++){
				plot_pixel(x+k,y+i,0xffff);
			}
		}
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
		plot_pixel(x+8,y+i,0xffff);	
		plot_pixel(x+9,y+i,0xffff);
	}
}
void letter_I(int x,int y){
	for(int i=0;i<30;i++){
		if(i<3 || i>26){
			for(int k=0;k<10;k++){
				plot_pixel(x+k,y+i,0xffff);
			}
		}
		plot_pixel(x+4,y+i,0xffff);
		plot_pixel(x+5,y+i,0xffff);
		plot_pixel(x+6,y+i,0xffff);	
	}
}
void letter_N(int x,int y){
	for(int i=0;i<30;i++){
		if(i<3){
			for(int k=0;k<10;k++){
				plot_pixel(x+k,y+i,0xffff);
			}
		}
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
		plot_pixel(x+8,y+i,0xffff);	
		plot_pixel(x+9,y+i,0xffff);
	}
}
void letter_U(int x, int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i > 26){plot_pixel(x+j,y+i,0xffff);}
		}
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
		plot_pixel(x+8,y+i,0xffff);	
		plot_pixel(x+9,y+i,0xffff);
	}
}

void letter_C(int x, int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i < 3 || i > 27){plot_pixel(x+j,y+i,0xffff);}
		}
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
	}
}

void letter_A(int x, int y){
    for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i < 3){plot_pixel(x+j,y+i,0xffff);}
			if(i == 25 || i == 26){plot_pixel(x+j,y+i,0xffff);}	
		}
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
		plot_pixel(x+8,y+i,0xffff);	
		plot_pixel(x+9,y+i,0xffff);
	}
}
//letter D
void letter_D(int x, int y){
	int shift = 7;
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i < 3){ /*if first few rows*/ plot_pixel(x+j,y+i,0xffff);}		
		}
		
		plot_pixel(x,y+i,0xffff);
		plot_pixel(x+1,y+i,0xffff);
		if(i<=24){
			plot_pixel(x+7,y+i,0xffff);
			plot_pixel(x+8,y+i,0xffff);	
			plot_pixel(x+9,y+i,0xffff);
		}
		if(i>24){
			plot_pixel(x+shift,y+i,0xffff);
			plot_pixel(x+shift+1,y+i,0xffff);	
			plot_pixel(x+(shift+2),y+i,0xffff);
			--shift;
		}
	}
	plot_pixel(x+2,y+29,0xffff);	
	plot_pixel(x+3,y+29,0xffff);
}
//---------


void render_title(int front){
	if(front){
	letter_T(60,20);
	letter_E(74,20);
	letter_R(88,20);
	letter_M(102,20);
	letter_I(116,20);
	letter_N(130,20);
	letter_A(144,20);
	letter_T(158,20);
	letter_O(172,20);
	letter_R(186,20);
	}
	else{
	letter_G(99,105);
	letter_A(113,105);
	letter_M(127,105);
	letter_E(141,105);
	letter_O(159,105);
	letter_V(173,105);
	letter_E(187,105);
	letter_R(201,105);
	}
}

void status_bar(){
	char buf[100];
	char *ptr_ = "STATUS BAR:";
	draw_text(60,3,ptr_);
	
	snprintf(buf, 100, "SCORE: %d", score);
	draw_text(60,6,buf);
	
	snprintf(buf, 100, "HIGHSCORE: %d", highScore);
	draw_text(60,8,buf);
	
        snprintf(buf, 100, "STRIKE: %d", strike);
	draw_text(60,10,buf);
}

void info_MainPage(){
	char *ptr_ = "INSTRUCTIONS:";
	draw_text(5,30,ptr_);
	ptr_ = "1) CONTROL THE CURSOR USING 'W' 'A' 'S' 'D' ";
	draw_text(5,35,ptr_);
        ptr_ = "2) START/CONTINUE USING ENTER KEY";
	draw_text(5,38,ptr_);
	ptr_ = "3) PRESS SPACE TO SHOOT";
	draw_text(5,41,ptr_);
	ptr_ = "4) PRESS ESC TO RETURN TO THE START PAGE";
	draw_text(5,44,ptr_);
	
	//Highest Saved score
	char buf[100];
	snprintf(buf, 100, "HIGHEST SCORE: %d", highScore); // puts string into buffer
	draw_text(40,20,buf);
}

void clear_text(){
	int offset;
	volatile char*character_buffer =(char*)FPGA_CHAR_BASE;
	//offset = (y << 7) + x;
	for(int i=0;i<80;i++){
		for(int j=0;j<60;j++){
			*(character_buffer + (j<<7)+i) =' ';
		}
	}
}

point findShot(target* gameT) {
	point result;
	result.xPos = (gameT->xPos - playerLoc.xPos)/5;
	result.yPos = (gameT->yPos - playerLoc.yPos)/5;
	return result;
}

volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

void erase_screen(int color) {
	pixel_buffer_start = *pixel_ctrl_ptr;
	clear_screen(color); // pixel_buffer_start points to the pixel buffer


	/* set back pixel buffer to start of SDRAM memory */
	clear_text();

	*(pixel_ctrl_ptr + 1) = 0xC0000000;
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); 

	clear_screen(color);

	clear_text();

}



int main(void) {
	int gameMode = 0;// 0 = start, 1 is game, 2 is game over
	int it = 0;
	int flash = 0;
	cursor gameCursor;
	gun gameGun;
	gameCursor.xPos = RESOLUTION_X/2;
	gameCursor.yPos = RESOLUTION_Y/2;
	gameCursor.shot = 0;
	initCursor(&gameCursor);
	initGun(&gameGun);
	initTargets();
	initBullets();
	
	/*Declare volatile pointers to I/O registers (volatile means that IO loadand store instructions will be used to access these pointer locations,instead of regular memory loads and stores)*/
	
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    erase_screen(0);
	//draw_text(gameGun.xPos,gameGun.yPos,ptr_);
	volatile int*PS2_ptr = (int*)PS2_BASE;
	int PS2_data, RVALID;
	char byte1 = 0, byte2 = 0, byte3 = 0;// PS/2 mouse needs to be reset (must be already plugged in)
	*(PS2_ptr) = 0xFF;// reset
	
	
	
	int lineRedrawX[8];
	int lineRedrawY[8];
	int drawRay;
	int count = 0;
	while(1) {
		
		while (!gameMode) {
			
			info_MainPage();
			render_title(1);
			PS2_data =*(PS2_ptr);// read the Data register in the PS/2 
			RVALID   = PS2_data & 0x8000;// extract the RVALID field
			if(RVALID) {
				/*shift the next data byte into the display*/
				byte1 = byte2;
				byte2 = byte3;
				byte3 = PS2_data & 0xFF;

				HEX_PS2(strike, 0, 0);
				if((byte2 == (char)0xAA) && (byte3 == (char)0x00))// mouse inserted; initialize sending of data
					*(PS2_ptr) = 0xF4;
				if (byte3 == 90) {
				  	erase_screen(CYAN);
					gameMode = 1;
				
				}
		}
		
		} 
		if (gameMode == 2) {
			erase_screen(0);
			
			status_bar();
			render_title(0);
			
			while (gameMode == 2) {
			}
			score = 0;
			strike = 0;
		}
		while (gameMode == 1) {
		status_bar();
		HEX_PS2(highScore, score, strike);
		for (int tg = 0; tg < NUM_BOXES; tg++) {
			TARGETS[tg].xPos += TARGETS[tg].xVel;
			TARGETS[tg].yPos += TARGETS[tg].yVel;
			TARGETS[tg].timer--;
			printf("Timer: %d\n", TARGETS[tg].timer); 
			if (TARGETS[tg].timer <= 0) {
				
			if (!TARGETS[tg].timer) {
			strike++;
			BULLETS[tg].xPos = TARGETS[tg].xPos;
			BULLETS[tg].yPos = TARGETS[tg].yPos;
				lineRedrawX[tg] = TARGETS[tg].xPos;
				lineRedrawY[tg] = TARGETS[tg].xPos;
			drawRay = lineRedrawX[tg] <= 320 && lineRedrawX[tg] >= 0 && lineRedrawY[tg] <= 240 && lineRedrawY[tg] >= 0;
			if (drawRay) draw_line(lineRedrawX[tg], lineRedrawY[tg], gameGun.xPos, gameGun.yPos, RED);
			} 
			if (TARGETS[tg].timer == -6 || TARGETS[tg].timer == -7) {
				drawRay = lineRedrawX[tg] <= 320 && lineRedrawX[tg] >= 0 && lineRedrawY[tg] <= 240 && lineRedrawY[tg] >= 0;
				if (drawRay) draw_line(lineRedrawX[tg], lineRedrawY[tg], gameGun.xPos, gameGun.yPos, CYAN);
			}
			//findShot(&TARGETS[tg]);
			BULLETS[tg].xPos += BULLETS[tg].xVel;
			BULLETS[tg].yPos += BULLETS[tg].yVel;
			
			}
			if (TARGETS[tg].timer == 0) BULLETS[tg].shot = 1;
			if (TARGETS[tg].xPos == 0 || TARGETS[tg].xPos == 320-5) TARGETS[tg].xVel *= -1;
			if (TARGETS[tg].yPos == 0 || TARGETS[tg].yPos == 240-5) TARGETS[tg].yVel *= -1; 
		}
	
		
		//HEX_PS2(byte1, 0, 0);
		PS2_data =*(PS2_ptr);// read the Data register in the PS/2 
		RVALID   = PS2_data & 0x8000;// extract the RVALID field
		if(RVALID) {
			/*shift the next data byte into the display*/
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
			
			HEX_PS2(strike, 0, 0);
			if((byte2 == (char)0xAA) && (byte3 == (char)0x00))// mouse inserted; initialize sending of data
				*(PS2_ptr) = 0xF4;
			//If A is pressed
				if (byte3 == 28) {
				gameCursor.xPos-=5;
				if (gameCursor.xPos < 0) {
					gameCursor.xPos = 0;
				}
			}
			//If D is pressed
			if (byte3 == 35) {
				gameCursor.xPos+=5;
				if (gameCursor.xPos > RESOLUTION_X-1) {
					gameCursor.xPos = RESOLUTION_X-1;
				}
			}
			//If W is pressed
			if (byte3 == 27) {
				gameCursor.yPos+=5;
				if (gameCursor.yPos > RESOLUTION_Y-1) {
					gameCursor.yPos = RESOLUTION_Y-1;
				}
			}
			//If S is pressed
			if (byte3 == 29) {
				gameCursor.yPos-=5;
				if (gameCursor.yPos < 0) {
					gameCursor.yPos = 0;
				}
			}
			//If space is pressed
			if (byte3 == 41) {	
				if (!gameCursor.shot) gameCursor.shot = 1;
				checkShot(&gameCursor);
				flash = 1;
				
			}
			if (byte3 == 118) {	
				gameMode = 0;
				erase_screen(0);
				
			}
			
			
			//renderGun(&gameGun, it > 1, flash);
			//draw_line(gameCursor.xPos, gameCursor.yPos, gameGun.xPos, gameGun.yPos, RED);
			flash = 0;
			
			
			
			
			
		}
		for (int t = 0; t < 8; t++) {
			renderTarget(&TARGETS[t], it > 1);
			//renderBullet(&BULLETS[t], it > 1);
			
		}
		renderCursor(&gameCursor, it > 1);
		
		if (strike == 8) {
			gameMode = 2;
			if (highScore < score) highScore = score;
			erase_screen(0);
			
		}
		
		it++;
		wait_for_vsync();
         // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1);
		

		
	}
	}
}

void HEX_PS2(char b1,char b2,char b3) {
	volatile int*HEX3_HEX0_ptr = (int*)HEX3_HEX0_BASE;
	volatile int*HEX5_HEX4_ptr = (int*)HEX5_HEX4_BASE;/*SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in*a single 7-seg display in the DE1-SoC Computer, for the hex digits 0 - F*/
	unsigned char seven_seg_decode_table[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int shift_buffer, nibble;
	unsigned char code;
	int i;
	shift_buffer = (b1 << 16) | (b2 << 8) | b3;
	for(i = 0; i < 6; ++i) {
		nibble = shift_buffer & 0x0000000F;// character is in rightmost nibble
		code   = seven_seg_decode_table[nibble];
		hex_segs[i]  = code;
		shift_buffer = shift_buffer >> 4;
	}/*drive the hex displays*/
	*(HEX3_HEX0_ptr) =*(int*)(hex_segs);
	*(HEX5_HEX4_ptr) =*(int*)(hex_segs + 4);
}










/* This program demonstrates memory-mapped I/O using C code * * It performs the following:  * 1. displays SW on LEDR * 2. displays hex digits corresponding to SW on HEX3-0 */
char seg7[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
void HEX(int val) {/* Declare volatile pointers to I/O registers (volatile means that IO load * and store instructions will be used to access these pointer locations,  * instead of regular memory loads and stores)*/
	volatile int * LEDR_ptr = (int *)0xFF200000;// red LED address
	volatile int * SW_ptr= (int *)0xFF200040;// SW slide switch address
	volatile int * HEX3_0_ptr= (int *)0xFF200020;// HEX3_HEX0 address
	int value;
	while(1) {
		val++;
		
		value = *(SW_ptr);// read the SW slider switch values
						*(LEDR_ptr) = val; // light up the red LEDs
		int hun = val/100;
		int tens = (val - hun*100)/10;
		int ones = (val - tens*10);
						*(HEX3_0_ptr) = seg7[ones] | seg7[tens] << 8 | seg7[hun] << 16;}}
	
	

