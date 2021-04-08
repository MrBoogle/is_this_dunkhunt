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
	point image[5];
	point toDelete[5];

};

typedef struct target target;

target TARGETS[NUM_BOXES];

struct cursor {
	int xPos;
	int yPos;
	int shot; //1 if shot has been fired, 0 otherwise, reset to 0 once shot is registered on/off target
	point image[5];
	point toDelete[5];
};

typedef struct cursor cursor;
void initTargets() {
	for (int i = 0; i < NUM_BOXES; i++) {
		//Initialize target  
	}
	
}

void renderer(point* draw, point* toDel) {
	for (int i = 0; i < sizeof(toDel)/sizeof(toDel[0]); i++) {
		plot_pixel(toDel[i].xPos, toDel[i].yPos, toDel[i].color);
	}
	for (int i = 0; i < sizeof(draw)/sizeof(draw[0]); i++) {
		plot_pixel(draw[i].xPos, draw[i].yPos, draw[i].color);
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

void clear_screen() {
	for (int x = 0; x < 320; x++) {
		for (int y = 0; y < 240; y++) {
			*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = 0x0;
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
	cursor gameCursor;
	gameCursor.xPos = RESOLUTION_X/2;
	gameCursor.yPos = RESOLUTION_Y/2;
	gameCursor.shot = 0;
	/*Declare volatile pointers to I/O registers (volatile means that IO loadand store instructions will be used to access these pointer locations,instead of regular memory loads and stores)*/
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
	
	int x = RESOLUTION_X/2;
	int y = RESOLUTION_Y/2;
	clear_screen();
	volatile int*PS2_ptr = (int*)PS2_BASE;
	int PS2_data, RVALID;
	char byte1 = 0, byte2 = 0, byte3 = 0;// PS/2 mouse needs to be reset (must be already plugged in)
	*(PS2_ptr) = 0xFF;// reset
	while(1) {
		PS2_data =*(PS2_ptr);// read the Data register in the PS/2 
		RVALID   = PS2_data & 0x8000;// extract the RVALID field
		if(RVALID) {
			/*shift the next data byte into the display*/
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
			HEX_PS2(byte1, byte2, byte3);
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
			//drawBox(gameCursor.xPos, gameCursor.yPos, RED);
			
			
		}
		plot_pixel(gameCursor.xPos, gameCursor.yPos, RED);
		
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
	
	
