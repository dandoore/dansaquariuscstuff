//               ,
// Life! Dan Doore '96
//
// Life was thieved from a PC C Compiler disc (PD, of course) and is John Conway's life algorithm.
//
// 2022 remix for Z88DK (https://www.z88dk.org/
//
// Requires 4Kb expansion (8K total)
//
// Tape files: zcc +aquarius life.c -clib=ansi -lm -create-app -o life
//
// ROM file: zcc +aquarius life.c -subtype=rom -clib=ansi -lm -create-app -o life

#include <stdio.h>

#include <stdlib.h>

#include <conio.h>

#include <graphics.h>

// Macros

#define SCREENY 24 // Screen y rows
#define SCREENX 40 // Screen x rows
#define ROWS 21 // Life size y rows
#define COLS 40 // Life size x columns
#define LIFENUM 80 // Number of life cells

// Prototypes

int keypress();
void setuplife();
void cyclelife();
void addd(int x, int y);
void outputlife();

// Globals

char world[ROWS][COLS];
int popul, gen;

// Main

void main() {
  while (1) // Infinate loop
  {
    setuplife();
    while (!keypress() && popul) {
      gen++;
      gotoxy(0, 23);
      printf("Generation:%4d Population:%3d", gen, popul);
      cyclelife();
      outputlife();
    }
  }
}

// RND Function for integers 0-32768

int rnd(int range) {
  return abs(abs(rand()) / (32768 / range));
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

// Setup Life function

void setuplife() {
  int rnumber, i, row, col;

  // Set ANSI Colours for foreground and background

  printf("%c[%um", 27, 30);  // Black ink
  printf("%c[%um", 27, 46);  // White paper

  // Clear screen

  for (row = 0; row < SCREENY; row++) {
    for (col = 0; col < SCREENX; col++) {
      gotoxy(col, row);
      putchar(' ');
    }
  }
  
  // Clear world

  for (row = 0; row < ROWS; row++) {
    for (col = 0; col < COLS; col++) {
      world[row][col] = 0;
    }
  }

  gen = 0;
  rnumber = rnd(LIFENUM) + LIFENUM;
  popul = rnumber;

  gotoxy(0, 22);
  printf("Press any key to restart!\n");
  printf("Generation:%4d Population:%3d", gen, popul);

  for (i = 0; i < rnumber; i++) {
    row = rnd(ROWS);
    col = rnd(COLS);
    world[row][col] = 'X'; // putchar in a cell
    gotoxy(col, row);
    putchar('O');
  }
}

// Update screen and world

void outputlife() {
  int row, col;
  char cell;

  popul = 0;
  for (row = 0; row < ROWS; row++) {
    for (col = 0; col < COLS; col++) {
      cell = world[row][col];
      if (cell && (cell == 3 || cell == 'X' + 2 || cell == 'X' + 3)) {
        popul++;

        if (cell < 'X') {
          gotoxy(col, row);
          putchar('O');
        }
        cell = 'X';
      } else {
        if (cell >= 'X') {
          gotoxy(col, row);
          putchar(' ');
        }
        cell = 0;
      }
      world[row][col] = cell;
    }
  }
}

// Addd

void addd(int row, int col) {
  int rrow, ccol, rr, cc;

  for (rr = row - 1; rr <= row + 1; rr++) {
    for (cc = col - 1; cc <= col + 1; cc++) {
      rrow = rr != -1 ? rr : ROWS - 1;
      ccol = cc != -1 ? cc : COLS - 1;
      if (rrow >= ROWS) rrow = 0;
      if (ccol >= COLS) ccol = 0;
      world[rrow][ccol]++;
    }
  }
  world[row][col]--;
}

// Generate next life cycle

void cyclelife() {
  int row, col;

  // Take care of left and right column first

  for (row = 0; row < ROWS; row++) {
    if (world[row][0] >= 'X') addd(row, 0);
    if (world[row][COLS - 1] >= 'X') addd(row, COLS - 1);
    if (keypress()) break;
  }

  // Take care of top and bottom line

  for (col = 1; col < COLS - 1; col++) {
    if (world[0][col] >= 'X') addd(0, col);
    if (world[ROWS - 1][col] >= 'X') addd(ROWS - 1, col);
    if (keypress()) break;
  }

  // Fill in the box, ignoring border conditions

  for (row = 1; row < ROWS - 1; row++) {
    for (col = 1; col < COLS - 1; col++) {
      if (world[row][col] >= 'X') {
        world[row - 1][col - 1]++;
        world[row - 1][col]++;
        world[row - 1][col + 1]++;
        world[row][col - 1]++;
        world[row][col + 1]++;
        world[row + 1][col - 1]++;
        world[row + 1][col]++;
        world[row + 1][col + 1]++;
        if (keypress()) break;
      }
      if (keypress()) break;
    }
  }
}
