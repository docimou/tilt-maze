/* 05-833 Gadgets, Sensors, and Activity Recognition in HCI
 *
 * @Author: Doci Mou (dmou@andrew.cmu.edu)
 * Completion Date: February 26, 2013
 *
 * This project utilizes an Arduino Nano to drive an LEDMS88R, using
 * four tilt sensors arranged along a digital-analog converter circuit
 * divider.  With this code, it creates a six level maze game, with the
 * last level being an invisible level.  The brightest cell on the board 
 * will be the player's position, the slightly less bright cell is the
 * target, and the dimly lit cells are the walls of the maze.
 *
 * Correct orientation of the tilt sensors is necessary to achieve proper
 * orientation.
 */

#define F_CPU 16000000L
#include <avr/io.h>

//set initial gameplay
const int MAXLEVEL = 3;
boolean winner = false;
int location[2] = {1, 1};
int level = 0;
int currentBoard[8][8]; 
int currLocation[2];

//initial controller values
long tiltValue = 0;
long timeLastSeenUp = 0;
long timeLastSeenDown = 0;
long timeLastSeenLeft = 0;
long timeLastSeenRight = 0;
int lastTriggered;

//board setups for individual levels
int BOARD0[8][8] = { \
  {1, 1, 1, 1, 1, 1, 1, 1}, \
  {1, 0, 0, 0, 0, 0, 2, 1}, \
  {1, 0, 0, 0, 0, 0, 0, 1}, \
  {1, 0, 0, 0, 0, 0, 0, 1}, \
  {1, 0, 0, 0, 0, 0, 0, 1}, \
  {1, 0, 0, 0, 0, 0, 0, 1}, \
  {1, 0, 0, 0, 0, 0, 0, 1}, \
  {1, 1, 1, 1, 1, 1, 1, 1} \
};
  
int BOARD1[8][8] = { \
  {1, 1, 1, 1, 1, 1, 1, 1}, \
  {1, 0, 0, 0, 1, 0, 2, 1}, \
  {1, 1, 1, 0, 1, 0, 1, 1}, \
  {1, 0, 0, 0, 1, 0, 1, 1}, \
  {1, 0, 1, 1, 1, 0, 0, 1}, \
  {1, 0, 0, 1, 1, 1, 0, 1}, \
  {1, 1, 0, 0, 0, 0, 0, 1}, \
  {1, 1, 1, 1, 1, 1, 1, 1} \
};
  
int BOARD2[8][8] = { \
  {1, 1, 1, 1, 1, 1, 1, 1}, \
  {1, 0, 1, 0, 1, 2, 0, 1}, \
  {1, 0, 1, 0, 1, 1, 0, 1}, \
  {1, 0, 0, 0, 0, 1, 0, 1}, \
  {1, 1, 1, 1, 0, 1, 0, 1}, \
  {1, 0, 0, 0, 0, 1, 0, 1}, \
  {1, 0, 1, 1, 0, 0, 0, 1}, \
  {1, 1, 1, 1, 1, 1, 1, 1} \
};

int BOARD3[8][8] = { \
  {1, 1, 1, 1, 1, 1, 1, 1}, \
  {1, 0, 0, 0, 0, 0, 0, 1}, \
  {1, 1, 1, 1, 1, 1, 0, 1}, \
  {1, 0, 0, 0, 1, 1, 0, 1}, \
  {1, 0, 1, 0, 2, 1, 0, 1}, \
  {1, 0, 1, 1, 1, 1, 0, 1}, \
  {1, 0, 0, 0, 0, 0, 0, 1}, \
  {1, 1, 1, 1, 1, 1, 1, 1} \
};

int BOARD4[8][8] = { \
  {1, 1, 1, 1, 1, 1, 1, 1}, \
  {1, 0, 1, 2, 1, 1, 1, 1}, \
  {1, 0, 1, 0, 0, 0, 0, 1}, \
  {1, 0, 1, 1, 1, 1, 0, 1}, \
  {1, 0, 1, 0, 0, 0, 0, 1}, \
  {1, 0, 1, 0, 1, 1, 0, 1}, \
  {1, 0, 0, 0, 0, 0, 0, 1}, \
  {1, 1, 1, 1, 1, 1, 1, 1} \
};

int BOARD5[8][8] = { \
  {1, 1, 1, 1, 1, 1, 1, 1}, \
  {1, 0, 1, 0, 1, 0, 2, 1}, \
  {1, 0, 1, 0, 0, 0, 1, 1}, \
  {1, 0, 1, 0, 1, 1, 0, 1}, \
  {1, 0, 1, 0, 0, 0, 0, 1}, \
  {1, 0, 1, 1, 1, 1, 0, 1}, \
  {1, 0, 0, 0, 0, 0, 0, 1}, \
  {1, 1, 1, 1, 1, 1, 1, 1} \
};

void setup(){ 
  //one-time initialization for each level, called at beginning and when entering a new level
  
  //declares data direction for all pins
  DDRD = B11111100; 
  DDRB = B011111;
  DDRC = B011111;

  //sets initial values as all high --> all off
  PORTD = B0;
  PORTB = B0;
  PORTC = B0;
  
  //setup tilt sensor values
  PINC = B100000;

  //initialize values
  if (level == 0) {
    location[0] = 1;
    location[1] = 1;
  } else if (level == 1) {
    location[0] = 1;    
    location[1] = 1;
  } else if (level == 2) {
    location[0] = 1;
    location[1] = 1;    
  } else if (level == 3) {
    location[0] = 1;
    location[1] = 1; 
  } else if (level == 4) {
    location[0] = 1;
    location[1] = 1;
  } else if (level == 5) {
    location[0] = 1;
    location[1] = 1;
  }
  
  // set 2d array (board) for current level
  if (level == 0) {
    for (int i = 1; i <=8; i++) {
      for (int j = 1; j <= 8; j++) {
        currentBoard[i-1][j-1] = BOARD0[i-1][j-1];
      }
    }
  } else if (level == 1) {
     for (int i = 1; i <=8; i++) {
      for (int j = 1; j <= 8; j++) {
        currentBoard[i-1][j-1] = BOARD1[i-1][j-1];
      }
    }
  } else if (level == 2) {
     for (int i = 1; i <=8; i++) {
      for (int j = 1; j <= 8; j++) {
        currentBoard[i-1][j-1] = BOARD2[i-1][j-1];
      }
    }
  } else if (level == 3) {
     for (int i = 1; i <=8; i++) {
      for (int j = 1; j <= 8; j++) {
        currentBoard[i-1][j-1] = BOARD3[i-1][j-1];
      }
    }
  } else if (level == 4) {
    for (int i = 1; i <=8; i++) {
      for (int j = 1; j <= 8; j++) {
        currentBoard[i-1][j-1] = BOARD4[i-1][j-1];
      }
    }
  } else if (level == 5) {
    for (int i = 1; i <=8; i++) {
      for (int j = 1; j <= 8; j++) {
        currentBoard[i-1][j-1] = BOARD5[i-1][j-1];
      }
    }
  }
}

//play!
void loop() {
  //read voltage divided tilt sensor voltages, ADC of DAC-ed signals
  tiltValue = analogRead(A5);
  
  //set up viewer of board
  if (level == 0) {
    map0();
  } else if (level == 1) {
    map1();
  } else if (level == 2) {
    map2();  
  } else if (level == 3) {
    map3();
  } else if (level == 4) {
    map4();
  } else if (level == 5) {
    map5();
  }
 
  //save the current location in case of errorenous movement
  currLocation[0] = location[0];
  currLocation[1] = location[1];
  
  //set all rows high and all cols low
  PORTD = B11010000;
  PORTC = B011111;
  PORTB = B000000;
 
  //internal counter to slow down player movement
  if (lastTriggered > 50) {
    lastTriggered = 0;
  } else {
    lastTriggered++;
  } 
  
  //movement activates only after a given time
  if ((85 < tiltValue) & (tiltValue < 105)) {
    if ((millis() - timeLastSeenUp > 200) & (lastTriggered == 0)) {  // has been held down for a while
      location[1] -= 1; 
      timeLastSeenUp = millis();
    }       
  } else if ((690 < tiltValue) & (tiltValue < 710)) {
    if ((millis() - timeLastSeenDown > 200) & (lastTriggered == 0)) {     
      location[1] += 1;
      timeLastSeenDown = millis();
    }
  } else if ((170 < tiltValue) & (tiltValue < 190)) {
    if ((millis() - timeLastSeenLeft > 200) & (lastTriggered == 0)) {     
      location[0] -= 1;
      timeLastSeenLeft = millis();
    }
  } else if ((505 < tiltValue) & (tiltValue < 525)) {
    if ((millis() - timeLastSeenRight> 200) & (lastTriggered == 0)) {     
      location[0] += 1;
      timeLastSeenRight = millis();
    }
  }
  
  //resets location to previously saved location if moves into wall
  //sets winner flag to true if moved into target
  if (currentBoard[location[0]][location[1]] == 1) {
    location[0] = currLocation[0];
    location[1] = currLocation[1];
  } if (currentBoard[location[0]][location[1]] == 2) {
    winner = true;
  }  
  
  lightPlayerPosition(location[0], location[1]);
  
  if (winner) {
    level += 1;
    winner = false;
    setup();
  }
  if (level == MAXLEVEL + 1) {
    for (int i = 1; i <=5; i++) {
      winGame();
    }
  }
}

void lightPlayerPosition(int x, int y) {
  //light up cols and arrays according to curent location
  switch (x) { //array grows down
    case 0: // SHOULD NEVER GET HERE
      PORTD &= ~B00010000; // writes 0 in row1
      break;
    case 1:
      PORTD &= ~B01000000; // writes 0 in row2
      break;
    case 2:
      PORTC &= ~B000001; // writes 0 in row3
      break;
    case 3:
      PORTD &= ~B10000000; // writes 0 in row4
      break;
    case 4:
      PORTC &= ~B010000; // writes 0 in row5
      break;
    case 5:
      PORTC &= ~B000010; // writes 0 in row6
      break;
    case 6:
      PORTC &= ~B001000; // writes 0 in row7
      break;
    case 7:
      PORTC &= ~B000100; // writes 0 in row8
      break;
  }
  switch (y) { //array grows right
    case 0:
      PORTB ^= B001000; // writes 1 in col1
      break;
    case 1:
      PORTD ^= B00100000; // writes 1 in col2
      break;
    case 2:
      PORTD ^= B00001000; // writes 1 in col3
      break;
    case 3:
      PORTB ^= B000100; // writes 1 in col4
      break;
    case 4:
      PORTD ^= B00000100; // writes 1 in col5
      break;
    case 5:
      PORTB ^= B000010; // writes 1 in col6
      break;
    case 6:
      PORTB ^= B010000; // writes 1 in col7
      break;
    case 7:
      PORTB ^= B000001; // writes 1 in col8
      break;
  }    
}

//relating to board matrices, the visual representation of the board
void map0() {  
  PORTD = 0;
  PORTC = 0;
  PORTB = B001000;
  delayMicroseconds(5);
  PORTD = B11100000;
  PORTC = B011011;
  PORTB = B000000;
  delayMicroseconds(2);
  PORTD = B11001000;
  PORTC = B011011;
  PORTB = B000100;
  delayMicroseconds(2);
  PORTD = B11000000;
  PORTC = B011011;
  PORTB = B000100;
  delayMicroseconds(1);
  PORTD = B11000100;
  PORTC = B011011;
  PORTB = B000000;
  delayMicroseconds(2);
  PORTD = B11000000;
  PORTC = B011011;
  PORTB = B000010;
  delayMicroseconds(2);
  PORTD = B11000000;
  PORTC = B011011;
  PORTB = B010000;
  delayMicroseconds(2);
  PORTD = 0;
  PORTC = 0;
  PORTB = B000001;
  delayMicroseconds(5);  
  PORTD = B10010000;
  PORTC = B011111;
  PORTB = B010000;
  delayMicroseconds(6);
}

void map1() {
  PORTD = 0;
  PORTC = 0;
  PORTB = B001000;
  delayMicroseconds(5);
  PORTD = B11100000;
  PORTC = B010010;
  PORTB = B000000;
  delayMicroseconds(2);
  PORTD = B11001000;
  PORTC = B001010;
  PORTB = B000000;
  delayMicroseconds(2);
  PORTD = B11000000;
  PORTC = B001001;
  PORTB = B000100;
  delayMicroseconds(2);    
  PORTD = B00000100;
  PORTC = B001000;
  PORTB = B000000;
  delayMicroseconds(4);
  PORTD = B11000000;
  PORTC = B011001;
  PORTB = B000010;
  delayMicroseconds(1);    
  PORTD = B01000000;
  PORTC = B011010;
  PORTB = B010000;
  delayMicroseconds(2);    
  PORTD = 0;
  PORTC = 0;
  PORTB = B000001;
  delayMicroseconds(4);  
  PORTD = B10010000;
  PORTC = B011111;
  PORTB = B010000;
  delayMicroseconds(6);
}

void map2() {
  PORTD = 0;
  PORTC = 0;
  PORTB = B001000;
  delayMicroseconds(4);
  PORTD = B11100000;
  PORTC = B001011;
  PORTB = B000000;
  delayMicroseconds(1);
  PORTD = B10001000;
  PORTC = B000010;
  PORTB = B000000;
  delayMicroseconds(3);
  PORTD = B11000000;
  PORTC = B000011;
  PORTB = B000100;
  delayMicroseconds(2);
  PORTD = B10000100;
  PORTC = B011010;
  PORTB = B000000;
  delayMicroseconds(2);
  PORTD = B01000000;
  PORTC = B001000;
  PORTB = B000010;
  delayMicroseconds(3);
  PORTD = B11000000;
  PORTC = B011011;
  PORTB = B010000;
  delayMicroseconds(1);
  PORTD = 0;
  PORTC = 0;
  PORTB = B000001;
  delayMicroseconds(3);  
  PORTD = B10010000;
  PORTC = B011111;
  PORTB = B000010;
  delayMicroseconds(2);  
}

void map3() {
  PORTD = 0;
  PORTC = 0;
  PORTB = B001000;
  delayMicroseconds(3);
  PORTD = B11100000;
  PORTC = B011010;
  PORTB = B000000;
  delayMicroseconds(1);  
  PORTD = B11001000;
  PORTC = B001000;
  PORTB = B000000;
  delayMicroseconds(2); 
  PORTD = B11000000;
  PORTC = B011000;
  PORTB = B000100;
  delayMicroseconds(2); 
  PORTD = B01000100;
  PORTC = B011000;
  PORTB = B000000;
  delayMicroseconds(2);  
  PORTD = B01000000;
  PORTC = B001000;
  PORTB = B000010;
  delayMicroseconds(2);  
  PORTD = B11000000;
  PORTC = B011011;
  PORTB = B010000;
  delayMicroseconds(1);
  PORTD = 0;
  PORTC = 0;
  PORTB = B000001;
  delayMicroseconds(3);  
  PORTD = B11010100;
  PORTC = B001111;
  PORTB = B000000;
  delayMicroseconds(2);    
}

void map4() {  
  PORTD = 0;
  PORTC = 0;
  PORTB = B001000;
  delayMicroseconds(4);
  PORTD = B11100000;
  PORTC = B011011;
  PORTB = B000000;
  delayMicroseconds(1);
  PORTD = B00001000;
  PORTC = B001000;
  PORTB = B000000;
  delayMicroseconds(4);
  PORTD = B01000000;
  PORTC = B011011;
  PORTB = B000100;
  delayMicroseconds(2);
  PORTD = B00000100;
  PORTC = B011001;
  PORTB = B000000;
  delayMicroseconds(2);
  PORTD = B00000000;
  PORTC = B011001;
  PORTB = B000010;
  delayMicroseconds(2);
  PORTD = B10000000;
  PORTC = B011011;
  PORTB = B010000;
  delayMicroseconds(2);
  PORTD = 0;
  PORTC = 0;
  PORTB = B000001;
  delayMicroseconds(4);  
  PORTD = B10010000;
  PORTC = B011111;
  PORTB = B000100;
  delayMicroseconds(4);  
}

void map5() {
  //invisible!
}

void winGame() {
  // flashy game winning thing
  // TONOTDO: speakers?
  PORTD = B00000000;
  PORTB = B001000;
  PORTC = B000000;
  delay(100);
  PORTD = B00100000;
  delay(100);
  PORTD = B00101000;
  delay(100);
  PORTB = B001100;
  delay(100);
  PORTD = B00101100;
  delay(100);
  PORTB = B001110;
  delay(100);
  PORTB = B011110;
  delay(100);
  PORTB = B011111;
  delay(100);

  PORTD = 0;
  PORTC = 0;
  PORTB = 0;
  delay(200);
  
  for (int i = 1; i < 100; i++) {
    PORTD = B10101000;
    PORTB = B001101;
    PORTC = B010011;
    delay(1);
    PORTD = B00000100;
    PORTC = B000011;
    PORTB = B0;
    delay(2);
    PORTD = 0;
    PORTC = 0;
    PORTB = B000010;
    delay(2);
    PORTD = B10000000;
    PORTC = B010000;
    PORTB = B010000;
    delay(2);
  }
  for (int j = 1; j < 80; j++) {
    PORTD = B01111100;
    PORTC = B001111;
    PORTB = B011111;
    delay(1);
    PORTD = 0; 
    PORTC = 0;
    PORTB = 0;
    delay(10);
  }
  
  for (int k = 1; k < 150; k++) {
    PORTD = B10000000;
    PORTB = B101000;
    PORTC = B010011;
    delay(1);
    PORTD = B10100000;
    PORTB = B000000;
    PORTC = B010001;
    delay(1);
    PORTD = B10001000;
    PORTB = B000000;
    PORTC = B000001;
    delay(1);
    PORTD = B00000000;
    PORTB = B000100;
    PORTC = B000001;
    delay(1);
    PORTD = B00000100;
    PORTB = B000000;
    PORTC = B000010;
    delay(1);
    PORTD = B00000000;
    PORTB = B000010;
    PORTC = B010010;
    delay(1);
    PORTD = B10000000;
    PORTB = B010000;
    PORTC = B010010;
    delay(1);
    PORTD = B10000000;
    PORTB = B000001;
    PORTC = B010011;
    delay(1);
  }
}
