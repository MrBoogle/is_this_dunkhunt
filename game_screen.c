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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Begin part1.s for Lab 7
//void draw_line(int x0,int y0,int x1,int y1,short int line_color);
//void draw_pixel(int x,int y,short int line_color);
//void swap(int *x,int *y);
//void clear_screen();
	
volatile int pixel_buffer_start; // global variable

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
	letter_T(100-40,20);
	letter_E(114-40,20);
	letter_R(128-40,20);
	letter_M(142-40,20);
	letter_I(154-40,20);
	letter_N(168-40,20);
	letter_A(182-40,20);
	letter_T(196-40,20);
	letter_O(210-40,20);
	letter_R(224-40,20);
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
	ptr_ = "HIGHEST SCORE: "; // include some number
	draw_text(40,20,ptr_);
}


/*Draw Arrow for direction*/
//(x,y) are the tip of the arrow, pixel block will be 7x10
void draw_arrow(int x,int y,char* direction){
	if(strcmp(direction,"up")){}
	if(strcmp(direction,"down")){}
	if(strcmp(direction,"right")){}
	if(strcmp(direction,"left")){}
}

//renders status bar in the top right corner of the game page
//update highest score global variable
void status_bar(int score,int highScore,int strike){
	char *ptr_ = "STATUS BAR:";
	draw_text(65,3,ptr_);
	ptr_ = "SCORE: ";
	draw_text(65,6,ptr_);
    ptr_ = "HIGHSCORE: ";
	draw_text(65,8,ptr_);
	ptr_ = "STRIKE: ";
	draw_text(65,10,ptr_);
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

//draw tiangles
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

// code not shown for clear_screen() and draw_line() subroutines
/*Clears the screen*/
void clear_screen(){
	short int line_color = 0x0000;
	for(int i=0;i<320;i++){
		for(int j=0;j<240;j++){
		*(short int*)(pixel_buffer_start+(j<<10)+(i<<1)) = line_color;
		}
	}
}
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
/*****/
void draw_pixel(int x,int y,short int line_color){
	*(short int*)(pixel_buffer_start+(y<<10)+(x<<1)) = line_color;
}
void swap(int *x,int *y){
	int temp = *x;
	*x = *y;
	*y = temp;
}

void draw_line(int x0,int y0,int x1,int y1,short int line_color){
	bool is_steep = ABS(y1-y0)>ABS(x1-x0);
	if(is_steep){
		swap(&x0,&y0);
		swap(&x1,&y1);
	}
	if(x0>x1){
		swap(&x0,&x1);
		swap(&y0,&y1);
	}
	int delta_x = x1-x0;
	int delta_y = ABS(y1-y0);
	int error = - (delta_x/2);
	int y = y0;
	int y_step;
	if(y0<y1){ y_step = 1;}
	else { y_step = -1;}
	
	for(int x = x0; x<=x1;x++){
		if(is_steep){ draw_pixel(y,x,line_color);}
		else{
			draw_pixel(x,y,line_color);
			error = error + delta_y;
		}
		if(error>=0){
			y += y_step;
			error -= delta_x;
		}
	
	}
}

