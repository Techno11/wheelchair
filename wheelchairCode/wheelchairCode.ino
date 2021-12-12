#include <Servo.h> 

// Analog Inputs
#define JOYSTICK_Y = 0
#define JOYSTICK_X = 1

// Servo Outputs
#define RIGHT_DRIVE 3
#define LEFT_DRIVE 5

// Speed Selectors
#define SPEED_HIGH 7
#define SPEED_LOW 9

// Motor Controllers (basically Contonous-rotation servos)
Servo leftDrive;
Servo rightDrive;

/************ Configurables ************/

// Max Speed of Cart
double slow_maxSpeed = 0.15;
double medium_maxSpeed = 0.2;
double fast_maxSpeed = 0.25;

// Max Reverse Speed (Compounded on top of max speed of card)
double reverseSpeed = 0.75;

// Max Reverse Speed (Compounded on top of max speed of card)
double turnSpeed = 0.7;

// Joystick Deadzone
double deadzone = 0.3;

/************ End Configurables ************/

void setup() {
  Serial.begin(115200);

  // Init drives
  rightDrive.attach(RIGHT_DRIVE);
  leftDrive.attach(LEFT_DRIVE);
  rightDrive.writeMicroseconds(1500);
  leftDrive.writeMicroseconds(1500);

  // Init Speed Selector
  pinMode(SPEED_LOW, INPUT_PULLUP);
  pinMode(SPEED_HIGH, INPUT_PULLUP);
}

void loop() {
  // Read Digital Pins for Speed
  int high = digitalRead(SPEED_HIGH);
  int low = digitalRead(SPEED_LOW);
  
  // Read analog  inputs, 0 - 1023
  int rawX = 1023 - analogRead(0);
  int rawY = analogRead(1);

  double normX = calcSpeed(rawX);
  double normY = calcSpeed(rawY);

  // Deadzone
  normX = deadzoneify(normX, deadzone);
  normY = deadzoneify(normY, deadzone);

  // Slower Reverse
  if(normX > 0) {
    normX = normX * reverseSpeed;
  }

  // Slower Turn
  normY = normY * turnSpeed;

  // Arcade Drive
  double rawLeft = normY + normX;
  double rawRight = normY - normX;
  
  if(rawLeft > 1) {rawLeft = 1;}
  if(rawLeft < -1) {rawLeft = -1;}
  if(rawRight > 1) {rawRight = 1;}
  if(rawRight < -1) {rawRight = -1;}
  
  // Speed Limiter
  if(high == 0) { // Fast Speed
    rawLeft = rawLeft * fast_maxSpeed;
    rawRight = rawRight * fast_maxSpeed;
  } else if (low == 0) { // Slow Speed
    rawLeft = rawLeft * slow_maxSpeed;
    rawRight = rawRight * slow_maxSpeed;
  } else { // Medium Speed
    rawLeft = rawLeft * medium_maxSpeed;
    rawRight = rawRight * medium_maxSpeed;
  }

  double calcLeft = (rawLeft + 1) * 500;
  double calcRight = (rawRight + 1) * 500;
  
  // Command speed  
  leftDrive.writeMicroseconds(calcLeft + 1000);
  rightDrive.writeMicroseconds(calcRight + 1000);
}

double deadzoneify(double val, double deadzone) {
  double offset = val / (1 - deadzone);
  if(abs(val) < deadzone) {
    return 0;
  } else {
    if(val > 0) {
      return offset - deadzone;
    } else {
      return offset + deadzone;
    }
  }
}

double calcSpeed(int rawJoystick){
  return (((rawJoystick / 1023.0) - 0.5) * 2.0);
}
