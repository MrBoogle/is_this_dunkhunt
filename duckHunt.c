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

#include <stdlib.h>
#include <stdio.h>

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

struct target {
	int xPos;
	int yPos;
	int shot; //1 if shot, 0 if not shot
	int spawned; //Only draw and register shots when set to 1
	point image[25];
	point previous[25];
	point toDelete[25];

};

typedef struct target target;

target TARGETS[NUM_BOXES];

struct cursor {
	int xPos;
	int yPos;
	int shot; //1 if shot has been fired, 0 otherwise, reset to 0 once shot is registered on/off target
	point image[8];
	point previous[8];
	point toDelete[8];
};

typedef struct cursor cursor;
void initTargets() {
	for (int i = 0; i < NUM_BOXES; i++) {
		//Initialize target  
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

void initTarget(target* gameT) {
	//Initialize cursor
	gameT->xPos = RESOLUTION_X/2+40;
	gameT->yPos = RESOLUTION_Y/2+40;
	for (int i = 0; i < 25; i++) {
		gameT->image[i] = targetImage[i];
		gameT->previous[i] = targetImage[i];
		gameT->toDelete[i] = targetImage[i];
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
		if (deltaY < 3 && deltaX < 3) {
			score++;
			return 1;
		} 
	}
	strike++;
	return 0;
}

void renderCursor(cursor* gameC, int valid) {
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



void renderTarget(target* gameT, int valid) {
	int n = sizeof(gameT->toDelete)/sizeof(gameT->toDelete[0]);
	//Delete toDel and shift previous into del
	//Shidt current into prev
	for (int i = 0; i < n; i++){
		if (valid) plot_pixel(gameT->toDelete[i].xPos, gameT->toDelete[i].yPos, gameT->toDelete[i].color);
		//plot_pixel(0, 0, RED);
		gameT->toDelete[i] = gameT->previous[i];
		gameT->previous[i].xPos = gameT->xPos + gameT->image[i].xPos;
		gameT->previous[i].yPos = gameT->yPos + gameT->image[i].yPos;
		gameT->previous[i].color = *(short int *)(pixel_buffer_start + (gameT->yPos + gameT->image[i].yPos << 10) + (gameT->xPos + gameT->image[i].xPos << 1));
		flushPS2();
	}

	//Draw current
	for (int i = 0; i < n; i++) {
		plot_pixel(gameT->xPos + gameT->image[i].xPos, gameT->yPos + gameT->image[i].yPos, gameT->image[i].color);
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










int main(void) {
	int it = 0;
	cursor gameCursor;
	gameCursor.xPos = RESOLUTION_X/2;
	gameCursor.yPos = RESOLUTION_Y/2;
	gameCursor.shot = 0;
	initCursor(&gameCursor);
	
	initTarget(&TARGETS[0]);
	
	/*Declare volatile pointers to I/O registers (volatile means that IO loadand store instructions will be used to access these pointer locations,instead of regular memory loads and stores)*/
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(CYAN); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); 
	
	clear_screen(CYAN);
	volatile int*PS2_ptr = (int*)PS2_BASE;
	int PS2_data, RVALID;
	char byte1 = 0, byte2 = 0, byte3 = 0;// PS/2 mouse needs to be reset (must be already plugged in)
	*(PS2_ptr) = 0xFF;// reset
	
	
	
	
	int count = 0;
	while(1) {
		
		//HEX_PS2(byte1, 0, 0);
		PS2_data =*(PS2_ptr);// read the Data register in the PS/2 
		RVALID   = PS2_data & 0x8000;// extract the RVALID field
		if(RVALID) {
			/*shift the next data byte into the display*/
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
			
			HEX_PS2(score, 0, 0);
			if((byte2 == (char)0xAA) && (byte3 == (char)0x00))// mouse inserted; initialize sending of data
				*(PS2_ptr) = 0xF4;
			//If A is pressed
				if (byte3 == 28) {
				gameCursor.xPos--;
				if (gameCursor.xPos < 0) {
					gameCursor.xPos = 0;
				}
			}
			//If D is pressed
			if (byte3 == 35) {
				gameCursor.xPos++;
				if (gameCursor.xPos > RESOLUTION_X-1) {
					gameCursor.xPos = RESOLUTION_X-1;
				}
			}
			//If W is pressed
			if (byte3 == 27) {
				gameCursor.yPos++;
				if (gameCursor.yPos > RESOLUTION_Y-1) {
					gameCursor.yPos = RESOLUTION_Y-1;
				}
			}
			//If S is pressed
			if (byte3 == 29) {
				gameCursor.yPos--;
				if (gameCursor.yPos < 0) {
					gameCursor.yPos = 0;
				}
			}
			//If space is pressed
			if (byte3 == 41) {	
				if (!gameCursor.shot) gameCursor.shot = 1;
				checkShot(&gameCursor);
			}
			renderTarget(&TARGETS[0], it > 1);
			renderCursor(&gameCursor, it > 1);
			
			
			
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
	
	

