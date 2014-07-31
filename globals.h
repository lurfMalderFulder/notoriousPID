#ifndef GLOBALS_H  // ensure this file is included only once
#define GLOBALS_H

#define DEBUG true    // debug flag tells compiler to include serial output debuging code via #if compiler directives
const int EEPROM_VER = 11;  // eeprom data tracking

byte delta[8] =  // custom delta character for LCD
{
  B00000,
  B00000,
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B00000
};

byte rightArrow[8] =
{
  B11000,
  B10100,
  B10010,
  B10001,
  B10010,
  B10100,
  B11000,
  B00000
};

byte disc[8] =
{
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000,
  B00000,
  B00000
};

byte dot[8] =
{
  B00000,
  B00000,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte circle[8] =
{
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000,
  B00000,
  B00000
};

byte inverted[8] =
{
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

enum opState {  // fridge operation states
  IDLE,
  COOL,
  HEAT,
};

// arduino pin declarations:
const byte encoderPinA = 3;   // rotary encoder A channel **interrupt pin**
const byte encoderPinB = 2;   // rotary encoder B channel **interrupt pin**
const byte lcd_d7 = 4;        // lcd D7
const byte lcd_d6 = 5;        // lcd D6
const byte lcd_d5 = 6;        // lcd D5
const byte lcd_d4 = 7;        // lcd D4
const byte lcd_enable = 8;    // lcd enable
const byte lcd_rs = 9;        // lcd RS
const byte chipSelect = 10;   // data logging shield
const byte mosi = 11;         // sd i/o (MOSI)
const byte miso = 12;         // sd i/o (MISO)
const byte sck = 13;          // sd i/o (SCK)
const byte pushButton = A0;   // rotary encoder pushbutton
const byte onewireData = A1;  // one-wire data
const byte relay1 = A2;       // relay 1 (fridge compressor)
const byte relay2 = A3;       // relay 2 (heating element)

volatile char encoderPos = 0;          // a counter for the rotary encoder dial
volatile byte encoderState = 0b000;    // 3 bit-flag encoder state (A Channel)(B Channel)(is rotating)

byte fridgeState[2] = { IDLE, IDLE };    // [0] - current fridge state; [1] - fridge state t - 1 history
const double fridgeIdleDiff = 0.5;       // constrain fridge temperature to +/- 0.5 deg F differential
const double fridgePeakDiff = 1;         // constrain allowed peak error to +/- 1 deg F differential
const unsigned int coolMinOff = 300;     // minimum compressor off time, seconds (5 min)
const unsigned int coolMinOn = 120;      // minimum compressor on time, seconds (2 min)
const unsigned int coolMaxOn = 2700;     // maximum compressor on time, seconds (45 min)
const unsigned int peakMaxTime = 1200;   // maximum runTime to consider for peak estimation, seconds (20 min)
const unsigned int peakMaxWait = 1800;   // maximum wait on peak, seconds (30 min)
const unsigned int heatMinOff = 300;     // minimum HEAT off time, seconds (5 min)
const unsigned int heatWindow = 300000;  // window size for HEAT time proportioning, ms (5 min)
double peakEstimator = 5;     // to predict COOL overshoot; units of deg F per hour (always positive)
double peakEstimate = 0;      // to determine prediction error = (estimate - actual)
unsigned long startTime = 0;  // timing variables for enforcing min/max cycling times
unsigned long stopTime = 0;

OneWire onewire(onewireData);  // define instance of the OneWire class to communicate with onewire sensors
probe beer(&onewire), fridge(&onewire);

byte programState;  // 6 bit-flag program state -- (mainPID manual/auto)(heatPID manual/auto)(temp C/F)(fermentation profile on/off)(data capture on/off)(file operations) = 0b000000
double Input, Setpoint, Output, Kp, Ki, Kd;  // SP, PV, CO, tuning params for main PID
double heatInput, heatOutput, heatSetpoint, heatKp, heatKi, heatKd;  // SP, PV, CO tuning params for HEAT PID
PID mainPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);  // main PID instance for beer temp control (DIRECT: beer temperature ~ fridge(air) temperature)
PID heatPID(&heatInput, &heatOutput, &heatSetpoint, heatKp, heatKi, heatKd, DIRECT);   // create instance of PID class for cascading HEAT control (HEATing is a DIRECT process)

LiquidCrystal lcd(lcd_rs, lcd_enable, lcd_d4, lcd_d5, lcd_d6, lcd_d7);  // define instance of the LiquidCrystal class for 20x4 LCD
RTC_DS1307 RTC;           // define instance of Real-time Clock class
File LogFile;             // define datalogging File object
File ProFile;                      // define fermentation profile File object
QueueList <profileStep> profile;   // dynamic queue (FIFO) linked list; contains steps for temperature profile

#endif