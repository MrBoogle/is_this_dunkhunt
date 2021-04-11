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
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1
#define pixelTarget 49 //number of pixels that make up a target/**NEW**/
#define higher 81

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Begin part1.s for Lab 7
//void draw_line(int x0,int y0,int x1,int y1,short int line_color);
//void draw_pixel(int x,int y,short int line_color);
//void swap(int *x,int *y);
//void clear_screen();
	
volatile int pixel_buffer_start; // global variable
/*Used inside initilizetarget functions*/
point target[pixelTarget];
point biggerTarget[higher];

/*****/
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

/**/
int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
	/*Game Name: Terminator*/
    clear_screen();
	clear_text();
	//render_title(true);
	//info_MainPage();
	status_bar(0,0,0);
	
	
}
//if front is true, displays "Terminator", if !front displays "Game Over"
void render_title(bool front){
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
//prints game information and instructions onto the screen at predefined positions
/*******************************************************************/
/*Complete highScore is used to show present highest score*/
/*******************************************************************/
void info_MainPage(){
	char *ptr_ = "INSTRUCTIONS:";
	draw_text(5,30,ptr_);
	ptr_ = "1) CONTROL THE CURSOR USING 'W' 'A' 'S' 'D' ";
	draw_text(5,35,ptr_);
        ptr_ = "2) START/CONTINUE USING ENTER KEY";
	draw_text(5,38,ptr_);
	ptr_ = "3) PRESS SPACE TO SHOT";
	draw_text(5,41,ptr_);
	ptr_ = "4) PRESS ESC TO RETURN TO THE START PAGE";
	draw_text(5,44,ptr_);
	
	//Highest Saved score
	char buf[100];
	snprintf(buf, 100, "HIGHEST SCORE: %d", highScore); // puts string into buffer
	draw_text(40,20,buf);
}


/*Draw Arrow for direction*/
/*Incomplete*/
//(x,y) are the tip of the arrow, pixel block will be 7x10
void draw_arrow(int x,int y,char* direction){
	if(strcmp(direction,"up")){}
	if(strcmp(direction,"down")){}
	if(strcmp(direction,"right")){}
	if(strcmp(direction,"left")){}
}

//renders status bar in the top right corner of the game page
//update highest score global variable
/*******************************************************************/
/*Complete, uses gloabl variable to present values*/
/*******************************************************************/
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

void letter_E(int x,int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i < 3 || i > 26 || i>6 && i<9){
				draw_pixel(x+j,y+i,0xffff);
			}
		}
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
	}
}

void letter_V(int x,int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i==26){draw_pixel(x+j,y+i,0xffff);}
			if(i==27 && (j!=0 && j!=9) ){draw_pixel(x+j,y+i,0xffff);}
			if(i==28 && (j!=0 && j!=9 && j!=1 && j!=8) ){draw_pixel(x+j,y+i,0xffff);}
			if(i==29 && (j!=0 && j!=9 && j!=1 && j!=8 && j!=2 && j!=7) ){draw_pixel(x+j,y+i,0xffff);}
			//if(i==29 && (j!=0 && j!=9 && j!=1 && j!=8 && j!=2 && j!=7 && j!=3 && j!=6) ){draw_pixel(x+j,y+i,0xffff);}
		}
		if(i<26){
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
		draw_pixel(x+8,y+i,0xffff);	
		draw_pixel(x+9,y+i,0xffff);
		}
	}
}

void letter_T(int x,int y){
	for(int i=0;i<30;i++){
		if(i<3){
			for(int k=0;k<10;k++){
				draw_pixel(x+k,y+i,0xffff);
			}
		}
		draw_pixel(x+4,y+i,0xffff);
		draw_pixel(x+5,y+i,0xffff);
		draw_pixel(x+6,y+i,0xffff);	
	}
}
void letter_R(int x,int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i<3){draw_pixel(x+j,y+i,0xffff);}
			if( (i == 7 || i == 9) && j != 9){draw_pixel(x+j,y+i,0xffff);}
			if(i == 8 && (j != 9 && j != 8 ) ){draw_pixel(x+j,y+i,0xffff);}
		}
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
		if(i<=6 || i>=10){
			draw_pixel(x+7,y+i,0xffff);
			draw_pixel(x+8,y+i,0xffff);	
			draw_pixel(x+9,y+i,0xffff);
		}
	}

}

void letter_M(int x, int y){
	for(int i=0;i<30;i++){
		
		for(int j=0;j<10;j++){
			if( i==0 && (j<=2 || j>=8) ){draw_pixel(x+j,y+i,0xffff);}
			if( i==1 && (j<=3 || j>=7) ){draw_pixel(x+j,y+i,0xffff);}
			if( i==2 && j != 5){draw_pixel(x+j,y+i,0xffff);}
			if(i==3){draw_pixel(x+j,y+i,0xffff);}
			if( i==4 && (j>2 && j<8) ){draw_pixel(x+j,y+i,0xffff);}
			if( i==5 && (j>3 && j<7) ){draw_pixel(x+j,y+i,0xffff);}
			if( i==6 && j == 5){draw_pixel(x+j,y+i,0xffff);}
		}
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
		draw_pixel(x+2,y+i,0xffff);
		draw_pixel(x+8,y+i,0xffff);	
		draw_pixel(x+9,y+i,0xffff);
		
	}
}
	
void letter_G(int x, int y){
	for(int i=0;i<30;i++){
		if(i<3 || i>26){
			for(int k=0;k<10;k++){
				draw_pixel(x+k,y+i,0xffff);
			}
		}
		if(i!=3 && i!=4){
			draw_pixel(x+8,y+i,0xffff);	
			draw_pixel(x+9,y+i,0xffff);
		}
		if(i == 5 || i == 6){
			draw_pixel(x+5,y+i,0xffff);	
			draw_pixel(x+6,y+i,0xffff);	
			draw_pixel(x+7,y+i,0xffff);	
			draw_pixel(x+8,y+i,0xffff);	
			draw_pixel(x+9,y+i,0xffff);
		}
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
		
	}
}
void letter_O(int x,int y){
	for(int i=0;i<30;i++){
		if(i<3 || i>26){
			for(int k=0;k<10;k++){
				draw_pixel(x+k,y+i,0xffff);
			}
		}
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
		draw_pixel(x+8,y+i,0xffff);	
		draw_pixel(x+9,y+i,0xffff);
	}
}
void letter_I(int x,int y){
	for(int i=0;i<30;i++){
		if(i<3 || i>26){
			for(int k=0;k<10;k++){
				draw_pixel(x+k,y+i,0xffff);
			}
		}
		draw_pixel(x+4,y+i,0xffff);
		draw_pixel(x+5,y+i,0xffff);
		draw_pixel(x+6,y+i,0xffff);	
	}
}
void letter_N(int x,int y){
	for(int i=0;i<30;i++){
		if(i<3){
			for(int k=0;k<10;k++){
				draw_pixel(x+k,y+i,0xffff);
			}
		}
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
		draw_pixel(x+8,y+i,0xffff);	
		draw_pixel(x+9,y+i,0xffff);
	}
}
void letter_U(int x, int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i > 26){draw_pixel(x+j,y+i,0xffff);}
		}
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
		draw_pixel(x+8,y+i,0xffff);	
		draw_pixel(x+9,y+i,0xffff);
	}
}

void letter_C(int x, int y){
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i < 3 || i > 27){draw_pixel(x+j,y+i,0xffff);}
		}
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
	}
}

void letter_A(int x, int y){
    for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i < 3){draw_pixel(x+j,y+i,0xffff);}
			if(i == 25 || i == 26){draw_pixel(x+j,y+i,0xffff);}	
		}
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
		draw_pixel(x+8,y+i,0xffff);	
		draw_pixel(x+9,y+i,0xffff);
	}
}
//letter D
void letter_D(int x, int y){
	int shift = 7;
	for(int i=0;i<30;i++){//rows
		
		for(int j=0;j<10;j++){//columns 
			if(i < 3){ /*if first few rows*/ draw_pixel(x+j,y+i,0xffff);}		
		}
		
		draw_pixel(x,y+i,0xffff);
		draw_pixel(x+1,y+i,0xffff);
		if(i<=24){
			draw_pixel(x+7,y+i,0xffff);
			draw_pixel(x+8,y+i,0xffff);	
			draw_pixel(x+9,y+i,0xffff);
		}
		if(i>24){
			draw_pixel(x+shift,y+i,0xffff);
			draw_pixel(x+shift+1,y+i,0xffff);	
			draw_pixel(x+(shift+2),y+i,0xffff);
			--shift;
		}
	}
	draw_pixel(x+2,y+29,0xffff);	
	draw_pixel(x+3,y+29,0xffff);
}

//draw tiangles, pointing down. Thinking of using it as divided between title and instruction section of main page
void draw_triangle(int x, int y){
	for(int i=0;i<5;i++)
	for(int j=0;j<10;j++){//columns 
			if(i==0){draw_pixel(x+j,y+i,0xffff);}
			if(i==1 && (j!=0 && j!=9) ){draw_pixel(x+j,y+i,0xffff);}
			if(i==2 && (j!=0 && j!=9 && j!=1 && j!=8) ){draw_pixel(x+j,y+i,0xffff);}
			if(i==3 && (j!=0 && j!=9 && j!=1 && j!=8 && j!=2 && j!=7) ){draw_pixel(x+j,y+i,0xffff);}
			if(i==4 && (j!=0 && j!=9 && j!=1 && j!=8 && j!=2 && j!=7 && j!=3 && j!=6) ){draw_pixel(x+j,y+i,0xffff);}
	}
}

/***NEW***/
//7x7 pixel block for target, note the target pixels are organized with elements in the same column 
//togehter one after the other.
void print_7x7(){
	for(int i=0;i<pixelTarget;i++){
		draw_pixel(target[i].xPos,target[i].yPos,target[i].color);
	}
}
void print_9x9(){
	for(int i=0;i<higher;i++){
		draw_pixel(biggerTarget[i].xPos,biggerTarget[i].yPos,biggerTarget[i].color);
	}
}
void initialize_target_7x7(int x,int y){
	bool flip = false;
	for(int j=0;j<7;j++){
		target[j+21].xPos = x;
		target[j+21].yPos = y-3+j;
		if(!flip){target[j+21].color = 0xf800; flip=true;}
		else{target[j+21].color = 0xffff; flip=false;}
	}
	
	for(int j=0;j<7;j++){
		if(j==0 ||j==3|| j==6){target[j+14].color = 0xf800;target[j+28].color = 0xf800;}//set color to red
		else{target[j+14].color = 0xffff;target[j+28].color = 0xffff;}//set color to be background color 
		target[j+14].xPos = x-1; 
		target[j+14].yPos = y-3+j;
		target[j+28].xPos = x+1; 
		target[j+28].yPos = y-3+j;
	}
	
	for(int j=0;j<7;j++){
		if(j==1 || j==5){target[j+7].color = 0xf800;target[j+35].color = 0xf800;}//set color to red
		else if(j==0||j==6){target[j+7].color = 0x0000;target[j+35].color = 0x0000;}//set color to be background color
		else{target[j+7].color = 0xffff;target[j+35].color = 0xffff;}//set color to be white
		target[j+7].xPos = x-2; 
		target[j+7].yPos = y-3+j;
		target[j+35].xPos = x+2; 
		target[j+35].yPos = y-3+j;
	}

	for(int j=0;j<7;j++){
		//code for column x-3
		if(j == 2 || j == 3 || j==4){target[j].xPos = x-3; target[j].yPos = y-3+j; target[j].color = 0xf800;}
		else{target[j].xPos = x-3; target[j].yPos = y-3+j; target[j].color = 0x0000 ;}
		//code for column x+3
		if(j == 2 || j == 3 || j==4){target[j+42].xPos = x+3; target[j+42].yPos = y-3+j; target[j+42].color = 0xf800;}
		else{target[j+42].xPos = x+3; target[j+42].yPos = y-3+j; target[j+42].color = 0x0000;}
	}
}

void initilize_target_9x9(int x,int y){
	//column x-4 and x+4
	for(int j=0;j<9;j++){
		biggerTarget[j].xPos = x-4;
		biggerTarget[j].yPos = y-4+j;
		biggerTarget[j+72].xPos = x+4;
		biggerTarget[j+72].yPos = y-4+j;
		if(j==0||j==1||j==7||j==8){biggerTarget[j].color = 0x0000;biggerTarget[j+72].color = 0x000;}
		else{biggerTarget[j].color = 0xf800;biggerTarget[j+72].color = 0xf800;}
	}
	//column x-3 and x+3
	for(int j=0;j<9;j++){
		biggerTarget[j+9].xPos = x-3;
		biggerTarget[j+9].yPos = y-4+j;
		biggerTarget[j+63].xPos = x+3;
		biggerTarget[j+63].yPos = y-4+j;
		if(j==0||j==8){biggerTarget[j+9].color = 0x0000;biggerTarget[j+63].color = 0x000;}
		else if(j==1||j==7){biggerTarget[j+9].color = 0xf800;biggerTarget[j+63].color = 0xf800;}
		else{biggerTarget[j+9].color = 0xffff;biggerTarget[j+63].color = 0xffff;}
	}
	//column x-2 and x+2
	for(int j=0;j<9;j++){
		biggerTarget[j+18].xPos = x-2;
		biggerTarget[j+18].yPos = y-4+j;
		biggerTarget[j+54].xPos = x+2;
		biggerTarget[j+54].yPos = y-4+j;
		if(j==0||j==4||j==8){biggerTarget[j+18].color = 0xf800;biggerTarget[j+54].color = 0xf800;}
		else{biggerTarget[j+18].color = 0xffff;biggerTarget[j+54].color = 0xffff;}
	}
	//column x-1 and x+1
	for(int j=0;j<9;j++){
		biggerTarget[j+27].xPos = x-1;
		biggerTarget[j+27].yPos = y-4+j;
		biggerTarget[j+45].xPos = x+1;
		biggerTarget[j+45].yPos = y-4+j;
		if(j==0||j==3||j==5||j==8){biggerTarget[j+27].color = 0xf800;biggerTarget[j+45].color = 0xf800;}
		else{biggerTarget[j+27].color = 0xffff;biggerTarget[j+45].color = 0xffff;}
	}
	//column x
	bool flip = false;
	for(int j=0;j<9;j++){
		biggerTarget[j+36].xPos = x;
		biggerTarget[j+36].yPos = y-4+j;
		if(!flip){biggerTarget[j+36].color = 0xf800; flip=true;}
		else{biggerTarget[j+36].color = 0xffff; flip=false;}
	}
}
/*******************************************************************/
/********************Beginnig of Robot Code************************/
/*******************************************************************/

point robotImage[525];

typedef struct robot {
	int xPos;
	int yPos;
	int xVel;
	int yVel;
	int timer;
	int shot; //1 if shot, 0 if not shot
	int spawned; //Only draw and register shots when set to 1
	point image[525];
	point previous[525];
	point toDelete[525];

}robot;	
	
void initRobot(robot* gameR, int t) {
	//Initialize Robot
	
	gameR->xPos = t%RESOLUTION_X;
	gameR->yPos = t%RESOLUTION_Y;
	robot_initHelper(gameR->xPos,gameR->yPos);
	gameR->xVel = (t%2)*2 - 1;
	gameR->yVel = (t%2)*2 - 1;
	gameR->shot = 0;
	gameR->timer = t%81 + 80;
	for (int i = 0; i < 525; i++) {
		gameR->image[i] = robotImage[i];
		gameR->previous[i] = robotImage[i];
		gameR->toDelete[i] = robotImage[i];
	}
}


//this function fills the robotImage array at the location defined by (x,y) point
void robot_initHelper(int x,int y){
	//colummn x-10 and x+10
	for(int i=0;i<25;i++){
		if(i==14||i==15||i==16){robotImage[i].color = RED;robotImage[i+500].color = RED;}
		else{robotImage[i].color = 0x0000;robotImage[i+500].color = 0x0000;}//->set to background color
		robotImage[i].xPos = x-10;
		robotImage[i].yPos = y-12+i;

		robotImage[i+500].xPos = x+10;
		robotImage[i+500].yPos = y-12+i;
	}
	//colummn x-9 and x+9 and x-8 and x+8
	for(int i=0;i<25;i++){
		if(i==8||i==9||i==10){robotImage[i+25].color = ROBO_BLUE;robotImage[i+475].color = ROBO_BLUE;robotImage[i+50].color = ROBO_BLUE;robotImage[i+450].color = ROBO_BLUE;}
		else if(i==11||i==12||i==13){robotImage[i+25].color = GREY;robotImage[i+475].color = GREY;robotImage[i+50].color = GREY;robotImage[i+450].color = GREY;}
		else if(i==14||i==15){robotImage[i+25].color = RED;robotImage[i+475].color = RED;robotImage[i+50].color = RED;robotImage[i+450].color = RED;}
		else{robotImage[i+25].color = 0x0000;robotImage[i+475].color = 0x0000;robotImage[i+50].color = 0x0000;robotImage[i+450].color = 0x0000;}//->set to background color
		robotImage[i+25].xPos = x-9;
		robotImage[i+25].yPos = y-12+i;

		robotImage[i+475].xPos = x+9;
		robotImage[i+475].yPos = y-12+i;
		//for column x-8 and x+8
		robotImage[i+50].xPos = x-8;
		robotImage[i+50].yPos = y-12+i;

		robotImage[i+450].xPos = x+8;
		robotImage[i+450].yPos = y-12+i;
	}
	//column x-7 and x+7
	for(int i=0;i<25;i++){
		if(i==8||i==9||i==10){robotImage[i+75].color = ROBO_BLUE;robotImage[i+425].color = ROBO_BLUE;}
		else if(i==14||i==15||i==16){robotImage[i+75].color = RED;robotImage[i+425].color = RED;}
		else{robotImage[i+75].color = 0x0000;robotImage[i+425].color = 0x0000;}//->set to background color
		robotImage[i+75].xPos = x-7;
		robotImage[i+75].yPos = y-12+i;

		robotImage[i+425].xPos = x+7;
		robotImage[i+425].yPos = y-12+i;
	}
	//column x-6 and x+6
	for(int i=0;i<25;i++){
		if(i>5&&i<18){robotImage[i+100].color = GREY;robotImage[i+400].color = GREY;}
		else{robotImage[i+100].color = 0x0000;robotImage[i+400].color = 0x0000;}//->set to background color
		robotImage[i+100].xPos = x-6;
		robotImage[i+100].yPos = y-12+i;

		robotImage[i+400].xPos = x+6;
		robotImage[i+400].yPos = y-12+i;
	}
	//column x-5 and x-5
	for(int i=0;i<25;i++){
		if(i>5&&i<18){robotImage[i+125].color = GREY;robotImage[i+375].color = GREY;}
		else if(i==23||i==24){robotImage[i+125].color = RED;robotImage[i+375].color = RED;}
		else{robotImage[i+125].color = 0x0000;robotImage[i+375].color = 0x0000;}//->set to background color
		robotImage[i+125].xPos = x-5;
		robotImage[i+125].yPos = y-12+i;

		robotImage[i+375].xPos = x+5;
		robotImage[i+375].yPos = y-12+i;
	}
	//column x-4 and x+4
	for(int i=0;i<25;i++){
		if(i>5&&i<23){robotImage[i+150].color = GREY;robotImage[i+350].color = GREY;}
		else if(i==2||i==3||i==23||i==24){robotImage[i+150].color = RED;robotImage[i+350].color = RED;}
		else{robotImage[i+125].color = 0x0000;robotImage[i+350].color = 0x0000;}//->set to background color
		robotImage[i+150].xPos = x-4;
		robotImage[i+150].yPos = y-12+i;

		robotImage[i+350].xPos = x+4;
		robotImage[i+350].yPos = y-12+i;
	}
	//column x-3 and x+3
	for(int i=0;i<25;i++){
		if(i==23||i==24){robotImage[i+175].color = RED;robotImage[i+325].color = RED;}
		else {robotImage[i+175].color = GREY;robotImage[i+325].color = GREY;}
		robotImage[i+175].xPos = x-3;
		robotImage[i+175].yPos = y-12+i;

		robotImage[i+325].xPos = x+3;
		robotImage[i+325].yPos = y-12+i;
	}
	//column x-2 and x+2
	for(int i=0;i<25;i++){
		if(i==23||i==24){robotImage[i+200].color = RED;robotImage[i+300].color = RED;}
		else if(i==1){robotImage[i+200].color = WHITE;robotImage[i+300].color = 0x0000;}
		else if(i==2){robotImage[i+200].color = 0x0000;robotImage[i+300].color = 0x0000;}
		else {robotImage[i+200].color = GREY;robotImage[i+300].color = GREY;}
		robotImage[i+200].xPos = x-2;
		robotImage[i+200].yPos = y-12+i;

		robotImage[i+300].xPos = x+2;
		robotImage[i+300].yPos = y-12+i;
	}
	//column x-1 and x+1
	for(int i=0;i<25;i++){
		if(i==0||i==3|| i>4&&i<18){robotImage[i+225].color = GREY;robotImage[i+275].color = GREY;}
		else if(i==4){robotImage[i+225].color = WHITE;robotImage[i+275].color = WHITE;}
		else if(i==1){robotImage[i+225].color = 0x0000;robotImage[i+275].color = WHITE;}
		else{robotImage[i+225].color = 0x0000;robotImage[i+275].color = 0x0000;}//->set to background color
		robotImage[i+225].xPos = x-1;
		robotImage[i+225].yPos = y-12+i;
		
		robotImage[i+275].xPos = x+1;
		robotImage[i+275].yPos = y-12+i;
	}
	//column x
	for(int i=0;i<25;i++){
		if(i<4 || i>4&&i<18){robotImage[i+250].color = GREY;}
		else if(i==4){robotImage[i+250].color = WHITE;}
		else{robotImage[i+250].color = 0x0000;}//->set to background color
		robotImage[i+250].xPos = x;
		robotImage[i+250].yPos = y-12+i;
	}
}

void print_robot(){
	for(int i=0;i<525;i++){
		draw_pixel(robotImage[i].xPos,robotImage[i].yPos,robotImage[i].color);
	}	
}

