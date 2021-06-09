#include <Keypad.h>
#include <TM1637Display.h>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

const int CLK = 11; //Set the CLK pin connection to the display
const int DIO = 10; //Set the DIO pin connection to the display

TM1637Display display(CLK, DIO);

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

void setup(){
  display.setBrightness(0x0a);
  Serial.begin(9600);
}

void loop(){
  char key = keypad.getKey();

  if (key){
    display.showNumberDec(key);
    Serial.println(key);
  }
}
