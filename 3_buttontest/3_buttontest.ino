#include <Keypad.h>
#include <Joystick.h>

//DEFINITIONS
#define ENABLE_PULLUPS
#define NUMROTARIES 0 //replace "?" with number of rotary encoders you are using
#define NUMBUTTONS 3 //replace "?"with number of buttong you are using
#define NUMROWS 3 //replace "?" with number of rows you have
#define NUMCOLS 1 //replace "?" with number of columns you have

//BUTTON MATRIX
//first change number of rows and columns to match your button matrix, 
//then replace all "?" with numbers (starting from 0)
byte buttons[NUMROWS][NUMCOLS] = {
  {0},
  {1},
  {2}
  
 
 
};

struct rotariesdef {
  byte pin1;
  byte pin2;
  int ccwchar;
  int cwchar;
  volatile unsigned char state;
};





#define DIR_CCW 0x10
#define DIR_CW 0x20
#define R_START 0x0

#ifdef HALF_STEP
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#else
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif

//BUTTON MATRIX PART 2
byte rowPins[NUMROWS] = {A1,A2,A3}; //change "?" to the pins the rows of your button matrix are connected to
byte colPins[NUMCOLS] = {12}; //change "?" to the pins the rows of your button matrix are connected to

Keypad buttbx = Keypad( makeKeymap(buttons), rowPins, colPins, NUMROWS, NUMCOLS);

//JOYSTICK SETTINGS
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  32, //number of buttons
  0, //number of hat switches
  //Set as many axis to "true" as you have potentiometers for
  false, // y axis
  false, // x axis
  false, // z axis
  true, // rx axis
  true, // ry axis
  false, // rz axis
  false, // rudder
  false, // throttle
  false, // accelerator
  false, // brake
  false); // steering wheel

const int numReadings = 20;
 
int readings[numReadings];      // the readings from the analog input
int index = 0;              // the index of the current reading
int total = 0;                  // the running total
int currentOutputLevel = 0;

//POTENTIOMETERS PART 1
//add all the axis' which are enabled above
int RyAxis_ = 0;
int RxAxis_ = 0;   

               
//POTENTIOMETERS  PART 2
//Which pins are your potentiometers connected to?
int potentiometerPin1 = A4; //Change "?" to the pin your potentiometer is connected to
int potentiometerPin2 = A5;
const bool initAutoSendState = true;

void setup() {
  Joystick.begin();
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}


void loop() {

  
  CheckAllButtons();
  CheckAllPotentiometers();
 
}

//POTENTIOMETERS PART 3
//change the details to match teh details above for each potentiometer you are using
void CheckAllPotentiometers(){
                           
  //potentiometer 1
  currentOutputLevel = getAverageOutput(potentiometerPin1);
  RyAxis_ = map(currentOutputLevel,0,1023,0,255);
  Joystick.setRyAxis(RyAxis_); 

  //potentiometer 2
  currentOutputLevel = getAverageOutput(potentiometerPin2);
  RxAxis_ = map(currentOutputLevel,0,1023,0,255);
  Joystick.setRxAxis(RxAxis_);


}

int getAverageOutput(int pinToRead){
  index = 0;
  total = 0; 
 
  while (index < numReadings){
    readings[index] = analogRead(pinToRead);
    total = total + readings[index];
    index = index + 1;
    //delay (1);
  }
  return total / numReadings;
}


void CheckAllButtons(void) {
      if (buttbx.getKeys())
    {
       for (int i=0; i<LIST_MAX; i++)   
        {
           if ( buttbx.key[i].stateChanged )   
            {
            switch (buttbx.key[i].kstate) { 
                    case PRESSED:
                    case HOLD:
                              Joystick.setButton(buttbx.key[i].kchar, 1);
                              break;
                    case RELEASED:
                    case IDLE:
                              Joystick.setButton(buttbx.key[i].kchar, 0);
                              break;
            }
           }   
         }
     }
}
