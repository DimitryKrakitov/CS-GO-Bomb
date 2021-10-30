#include <Keypad.h>
#include <TM1637Display.h>
// #include <string>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

const int CLK = 11; //Set the CLK pin connection to the display
const int DIO = 10; //Set the DIO pin connection to the display

TM1637Display display(CLK, DIO);

const int Offset = 48;

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// ----- letters -------

const uint8_t int_mark = 0b01011010;
const uint8_t g = 0b00101111;
const uint8_t t_int[] = { int_mark, 0b00000000, 0b00001000, 0b00111000};
const uint8_t s_int[] = { int_mark, 0b00000000, 0b00000000, 0b01101101};
const uint8_t g_int[] = { int_mark, 0b00000000, 0b00000000, g};


const uint8_t Succ[] = { 0b11111111, 0b11111111, 0b11111111, 0b11111111}; //WiP
const uint8_t FAiL[] = { 0b00000000, 0b00000000, 0b00000000, 0b00000000}; //WiP
const uint8_t HELP[] = { 0b01010101, 0b01010101, 0b01010101, 0b01010101}; //WiP

// -----------------------

// ---- Game modes -----
/*
  0 - 

*/
// -----------------------



int addNumber(int value, int key){
  return value * 10 + key;
  // return value + key;
}

int remNumber(int value){
  return (value)/10;
  // value.remove(value.length()-1);
  // return value;
}


int code;
int defuse_code;
int time;
int game;
char last_key = '#';
int state = 0;
int internal_timer;

// states:
/* 
  0 = Game?
  1 = Code?
  2 = Time?
  3 = Countdown
  4 = Defusing
  5 = Analysis  
  6 = Succ
  7 = FAiL
*/


// ------- SETTERS -------- //


int setKey (char key, int current_value) {
  char newKey = key - Offset;
  if (key == '*') {
    current_value = remNumber(current_value);
    display.showNumberDec(current_value);
  } else if (key == '#') {
    if (last_key == '*') {
      max(0, state--);
    } else if (last_key == '#' && state != 3) {
      // do nothing: ignore repetition
      Serial.println("Repeated '#'");
    } else {
      if (state >= 6) {
        state = 0;
      } else {
        state++;
      }
    }
  } else if (current_value <= 999) {
    current_value = addNumber(current_value, newKey);
    display.showNumberDec(current_value);
  }
  // Serial.println(current_value);
  serialSummary(current_value, key);
  last_key = key;
  return current_value;
}

void setGame(int key) {
  display.setSegments(g_int);
  game = setKey (key, game);
  // delay(1000);
}

void setCode(int key) {
  display.setSegments(s_int);
  code = setKey (key, code);
}

void setTime(int key) {
  display.setSegments(t_int);
  time = setKey (key, time);
}


// ----------------------- //


void flashAndReboot(char key) {
  switch(state) {
    case 6:
      display.setSegments(Succ);
      break;
    case 7:
      display.setSegments(FAiL);
      break;
    default:
      display.setSegments(HELP);
  }
  if (key == '#') {
    state = 0;
  } else {
    Serial.println("Waiting for '#'");
    Serial.println("State: ");
    Serial.println(state);
    delay(1000);
  }
}

void checkTimeout() {
  if ( (millis()-internal_timer) > 1000) {
    internal_timer = millis();
    // time--;
    display.showNumberDec(time--);   
    if (time <= 0) {
      state = 7;
    }
  }
  // delay(1000);
  // flashAndReboot(key);
}

void serialSummary(int value, char key){
  
  Serial.println("PrevKey: ");
  Serial.println(last_key);
  Serial.println("Key: ");
  Serial.println(key);
  Serial.println("Value: ");
  Serial.println(value);
  Serial.println("State: ");
  Serial.println(state);
  Serial.println("\n\r\n\r");
}

void game0(char key) {
  switch(state) {
    case 3:
      display.showNumberDec(time);      
      Serial.println("\nTime:");
      Serial.println(time);
      if (key) { 
        Serial.println("\n Pressed key while in state 3. Key: ");
        Serial.println(key);        
      }
      if (key == '#') {
        Serial.println("\n Will update state.");
        state++;
      }
      break;
    case 4:
      defuse_code = setKey(key, defuse_code);
      break;
    case 5:
      state++;
      if (defuse_code != code) state++;
      break;
    default:
      flashAndReboot(key);
  }
}

void setup(){
  display.setBrightness(0x0a);
  Serial.begin(9600);
  while (state < 3) {
    char key = keypad.getKey();
    if (key){
      switch(state) {
        case 0:
          Serial.println("state 0");
          setGame(key);
          break;
        case 1:
          Serial.println("state 1");
          setCode(key);
          break;
        default:
          Serial.println("state 2");
          setTime(key);
          internal_timer = millis();
      }
    } else if (last_key == '#') {
      switch(state) {
        case 0:
          display.setSegments(g_int);
          break;
        case 1:
          display.setSegments(s_int);
          break;
        default:
          display.setSegments(t_int);
      }
    }
  }
}

void loop(){
  checkTimeout();
  char key = keypad.getKey();

  switch(game) {
    case 1:
      // code block
      break;
    default:
      game0(key);
  }

}
