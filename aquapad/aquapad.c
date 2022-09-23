// AquaPad - Mattel Aquarius Pad tester
//         ,
// Dan Doore 2022
//
// Tester for Mini/Micro Expansion Pads with AY-3-8910 
//
// Made with Z88DK (https://www.z88dk.org/
//
// Thanks: M. v.d. Steenoven - https://www.vdsteenoven.com/aquarius/hndcntrl.html
//
// Requires 4Kb expansion (8K total)
//
// Tape files: zcc +aquarius aquapad.c -clib=ansi -lm -create-app -o aquapad
//
// ROM file: zcc +aquarius aquapad.c -subtype=rom -clib=ansi -lm -create-app -o aquapad

// Libraries

#include <conio.h>

// Macros

#define SCREENY 24 // Screen y rows
#define SCREENX 40 // Screen x rows
#define AYSELECT 247 // AY-3-8910 Register select
#define AYDATA 246 // AY-3-8910 Data
#define LEFTPADPORT 15 // AY Data port 2
#define RIGHTPADPORT 14 // AY Data port 1
#define LEFTPADXOFFSET 2 // Printing offset
#define RIGHTPADXOFFSET 24 // Printing offset
#define LEFTPADYOFFSET 1 // Printing offset
#define RIGHTPADYOFFSET 1 // Printing offset

// Prototypes

void setuppad(int xoffset, int yoffset, char * name);
int readpad(int port);
int keypress();
void clearscreen();
void centre(int line, char * text);
void intro();
void showpad(int port, int xoffset, int yoffset);

// Main

void main() {
  intro();
  clearscreen();
  setuppad(LEFTPADXOFFSET, LEFTPADYOFFSET, "Left");
  setuppad(RIGHTPADXOFFSET, RIGHTPADYOFFSET, "Right");
  centre(22, "                                       ,");
  centre(23, "AquaPad Pad Tester v1.1 - 2022 Dan Doore");
  while (1) {
    showpad(LEFTPADPORT, LEFTPADXOFFSET, LEFTPADYOFFSET);
    showpad(RIGHTPADPORT, RIGHTPADXOFFSET, RIGHTPADYOFFSET);
  }
}
}

// Setup & print unchanging pad elements

void setuppad(int xoffset, int yoffset, char * name) {
  int count;

  gotoxy(xoffset + 5, yoffset + 15);
  printf("%s", name);

  gotoxy(xoffset + 1, yoffset + 17);
  for (count = 7; count >= 0; count--) {
    printf("%u", count);
  }
}

// Read Pad data (8 bits) from AY-3-8910 port
// Data from  https://www.vdsteenoven.com/aquarius/hndcntrl.html

int readpad(int port) {
  int value;
  outp(AYSELECT, 7);
  outp(AYDATA, 63);
  outp(AYSELECT, port);
  return (inp(AYDATA));
}

// Detect Keypress Routine - like kbhit() but gets character from buffer and just dumps it

int keypress() {
  char dummyval;
  if (kbhit()) {
    dummyval = getch();
    return (1);
  } else {
    return (0);
  }
}

// Clear Screen

void clearscreen() {
  int row, col;

  // Set ANSI Colours for foreground and background

  printf("%c[%um", 27, 30); // Black ink
  printf("%c[%um", 27, 46); // White paper

  // Clear screen

  for (row = 0; row < SCREENY; row++) {
    for (col = 0; col < SCREENX; col++) {
      gotoxy(col, row);
      putchar(' ');
    }
  }
}

// Centre Text Print

void centre(int line, char * text) {
  gotoxy((SCREENX - strlen(text)) / 2, line);
  printf("%s", text);
}

// Intro

void intro() {
  int val;

  clearscreen();
  centre(5, "AquaPad v1.1");
  centre(8, "Aquarius Pad Tester");
  centre(9, "             ,");
  centre(10, "2022 Dan Doore");
  centre(12, "Thanks: M. v.d. Steenoven");

  // Detect AY Chip

  outp(AYSELECT, 8); // Select A Envelope/Volume control
  outp(AYDATA, 31); // Set to 00011111 (Max Volume)
  outp(AYSELECT, 9); // Select B Envelope/Volume control
  val = inp(AYDATA); // Read back value (and ignore)
  outp(AYSELECT, 8); // Select A again
  val = inp(AYDATA); // Read back value

  if ((val == 31)) {
    centre(14, "Press any key!");
  } else {
    centre(14, "Interface/AY-3-8910 not detected!");
  }
  while (!keypress()) {}
}

// Show Pad

void showpad(int port, int xoffset, int yoffset) {
  int count, bit[8], val, padval;

  // Read port value

  val = readpad(port);

  // Place bits into array - https://www.includehelp.com/c/how-to-check-a-particular-bit-is-set-or-not-using-c-program.aspx

  for (count = 0; count < 8; count++) {
    if ((val & (1 << count))) {
      bit[count] = 1;
    } else {
      bit[count] = 0;
    }
  }

  // Controller value is 255 (all bits set) for no data, the pads pull data lines down when keys activated setting bit to zero
  //
  // Decoding of all combinations of K & P keys not possible due to keys sharing data lines e .g P5 is D0 =0, but D0 also used on K6
  //
  // K keys use D7,D6,D5 as root data lines with K1 (D6) and K4 (D5) being unique bits so these two keys can be used with the direction pad
  // If not using the direction pad keys K1-6 can be used but not chorded together plus will trigger P values due to sharing.
  //
  // Data from  https://www.vdsteenoven.com/aquarius/hndcntrl.html

  // Print Pad Controller values in BIN and DEC

  gotoxy(xoffset + 1, yoffset + 18);
  for (count = 7; count >= 0; count--) {
    printf("%u", bit[count]);
  }
  printf(" = %3d", val);

  // Check for non unique values and warn
  //
  // Note that value 95 (K3) is both valid and non unique as it overlaps with K4 if pressed together
  // Data from  https://www.vdsteenoven.com/aquarius/hndcntrl.html
  /*
  Combinations
  generating the
  same code 		Code 	Data Bus Grounded
  -------------------------------------------
  (I) 	P5 K5 		124 	D0,1,7
  		P4 K5
  		K6 K5
  		K6 P1
  		K6 P4
  (II) 	P5 K2 		122 	D0,2,7
  		K6 K2
  		K6 P13
  (III) 	K5 P16 		121 	D1,2,7
  		K5 P13
  		K5 K2
  		P1 K2
  		P16 K2
  (IV) 	K6 P16 		120 	D0,1,2,7
  		K2 P4
  (V) 	K3 			95 	D7,5
  		K3 K4
  */

  if ((val >= 120 && val <= 122) || (val == 95) || (val == 124)) {
    gotoxy(xoffset + 1, yoffset + 19);
    printf("NON UNIQUE");
  } else {
    gotoxy(xoffset + 1, yoffset + 19);
    printf("          ");
  }

  // Print Controller Data based on bit pattern

  // Buttons

  gotoxy(xoffset, yoffset);
  if (!bit[6]) {
    printf("**");
  } else {
    printf("K1");
  }
  gotoxy(xoffset + 6, yoffset);
  if (!bit[7] && !bit[2]) {
    printf("**");
  } else {
    printf("K2");
  }
  gotoxy(xoffset + 12, yoffset);
  if (!bit[7] && !bit[5]) {
    printf("**");
  } else {
    printf("K3");
  }
  gotoxy(xoffset, yoffset + 1);
  if (!bit[5]) {
    printf("**");
  } else {
    printf("K4");
  }
  gotoxy(xoffset + 6, yoffset + 1);
  if (!bit[7] && !bit[1]) {
    printf("**");
  } else {
    printf("K5");
  }
  gotoxy(xoffset + 12, yoffset + 1);
  if (!bit[7] && !bit[0]) {
    printf("**");
  } else {
    printf("K6");
  }

  // Pad

  // Drop front 2 bits (dedicated to K buttons) to get pad bits only

  padval = (val & 31);

  // If the compiler supports GCC binary literals it might make more sense to use them e.g. 0b00011' instead of '3' here, YMMV.

  // Some pad directions still need to front 2 bits to distinguish the presses of the K buttons (P01,P05,P13)

  gotoxy(xoffset + 12, yoffset + 8);
  if (padval == 29 && bit[7]) {
    printf("***");
  } else {
    printf("P01");
  }
  gotoxy(xoffset + 11, yoffset + 9);
  if (padval == 13) {
    printf("***");
  } else {
    printf("P02");
  }
  gotoxy(xoffset + 10, yoffset + 11);
  if (padval == 12) {
    printf("***");
  } else {
    printf("P03");
  }
  gotoxy(xoffset + 8, yoffset + 12);
  if (padval == 28) {
    printf("***");
  } else {
    printf("P04");
  }
  gotoxy(xoffset + 6, yoffset + 13);
  if (padval == 30 && bit[7]) {
    printf("***");
  } else {
    printf("P05");
  }
  gotoxy(xoffset + 4, yoffset + 12);
  if (padval == 14) {
    printf("***");
  } else {
    printf("P06");
  }
  gotoxy(xoffset + 2, yoffset + 11);
  if (padval == 6) {
    printf("***");
  } else {
    printf("P07");
  }
  gotoxy(xoffset + 1, yoffset + 9);
  if (padval == 22) {
    printf("***");
  } else {
    printf("P08");
  }
  gotoxy(xoffset, yoffset + 8);
  if (padval == 23) {
    printf("***");
  } else {
    printf("P09 ");
  }
  gotoxy(xoffset + 1, yoffset + 7);
  if (padval == 7) {
    printf("***");
  } else {
    printf("P10");
  }
  gotoxy(xoffset + 2, yoffset + 5);
  if (padval == 3) {
    printf("***");
  } else {
    printf("P11");
  }
  gotoxy(xoffset + 4, yoffset + 4);
  if (padval == 19) {
    printf("***");
  } else {
    printf("P12");
  }
  gotoxy(xoffset + 6, yoffset + 3);
  if (padval == 27 && bit[7]) {
    printf("***");
  } else {
    printf("P13");
  }
  gotoxy(xoffset + 8, yoffset + 4);
  if (padval == 11) {
    printf("***");
  } else {
    printf("P14");
  };
  gotoxy(xoffset + 10, yoffset + 5);
  if (padval == 9) {
    printf("***");
  } else {
    printf("P15");
  }
  gotoxy(xoffset + 11, yoffset + 7);
  if (padval == 25) {
    printf("***");
  } else {
    printf("P16");
  };
}