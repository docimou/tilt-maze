void test_cols(){ 
  PORTD = 0;             // turn everything off
  PORTB = 0;
  PORTC = 0;
  PORTB = B001000;       //test col1
  delay(500);
  PORTB = 0;
  PORTD = B00100000;     //test col2
  delay(500);
  PORTD = B00001000;     //test col3
  delay(500);
  PORTD = 0;
  PORTB = B000100;       //test col4
  delay(500);
  PORTB = 0;
  PORTD = B00000100;     //test col5
  delay(500);
  PORTD = 0;
  PORTB = B000010;       //test col6
  delay(500);
  PORTB = B010000;       //test col7
  delay(500);
  PORTB = B000001;       //test col8
  delay(500);
}

void test_rows(){
  PORTD = B11111100;     // turn everything off
  PORTB = B011111;
  PORTC = B011111;
  delay(500);
  PORTD = B11101100;     // turn row1 on
  delay(500);
  PORTD = B10111100;     // turn row2 on
  delay(500);
  PORTD = B11111100;     // turn row3 on
  PORTC = B011110;
  delay(500);
  PORTC = B011111;       // turn row4 on
  PORTD = B01111100;
  delay(500);
  PORTD = B11111100;     // turn row5 on
  PORTC = B001111;
  delay(500);
  PORTC = B011101;       // turn row6 on
  delay(500);
  PORTC = B010111;       // turn row7 on
  delay(500);
  PORTC = B011011;       // turn row8 on
  delay(500);
}
