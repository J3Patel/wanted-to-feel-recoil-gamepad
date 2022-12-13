#include <Gamepad.h>

int rightXcenter = 500;
int rightYcenter = 500;
int leftXcenter = 500;
int leftYcenter = 500;
double multiplierRX = 0.254; //127 / 500
double multiplierRY = 0.254;
double multiplierLX = 0.254;
double multiplierLY = 0.254;
bool fireState = false;
int const sol = 7;
int rpm = 500;
unsigned long calDelay = 0;
int shooting = 0;
int lastShooting = 0;
  unsigned long gunStateChangeMili = 0UL;
Gamepad gp;

void setup() {

  Serial.begin(115200); // opens serial port, sets data rate to 9600 bps
  
  pinMode(sol, OUTPUT);
  
  digitalWrite(sol, LOW);  
  calDelay =  400UL;

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(14,  INPUT_PULLUP); //UPPER RIGHT
  pinMode(15,  INPUT_PULLUP); //UPPER LEFT
  pinMode(1,  INPUT_PULLUP); //LEFTBUTTON
  pinMode(0,  INPUT_PULLUP); //RIGHTBUTTON
  pinMode(4,  INPUT_PULLUP); //UP
  pinMode(5,  INPUT_PULLUP); //DOWN
  pinMode(6,  INPUT_PULLUP); //LEFT
//  pinMode(7,  INPUT_PULLUP); //RIGHT
  pinMode(8,  INPUT_PULLUP); //Y
  pinMode(9,  INPUT_PULLUP); //X
  pinMode(10,  INPUT_PULLUP); //A
  pinMode(16,  INPUT_PULLUP); //B  
  
  calibrate();
}

void loop() {
  int lx, ly, rx, ry;
  lx = analogRead(A3);
  ly = analogRead(A2);
  rx = analogRead(A1);
  ry = analogRead(A0);
  //we need to convert a 0-1000 to -127 - 127
  lx = floor((lx - leftXcenter) * multiplierLX);
  ly = floor((ly - leftYcenter) * multiplierLY);
  rx = floor((rx - rightXcenter) * multiplierRX);
  ry = floor((ry - rightYcenter) * multiplierRY);
  if(lx > 127) lx = 127;
  if(ly > 127) ly = 127;
  if(rx > 127) rx = 127;
  if(ry > 127) ry = 127;
  gp.setLeftXaxis(lx);
  gp.setRightXaxis(rx);
  gp.setLeftYaxis(ly);
  gp.setRightYaxis(ry);
  
  int UPLEFT, UPRIGHT, UP, DOWN, LEFT, RIGHT, RIGHTBUTTON, LEFTBUTTON, X, Y, A, B;
  UPLEFT = digitalRead(14);
  UPRIGHT = digitalRead(15);
  RIGHTBUTTON = digitalRead(0);
  LEFTBUTTON = digitalRead(1);
  UP = digitalRead(4);
  DOWN = digitalRead(5);
  LEFT = digitalRead(6);
//  RIGHT = digitalRead(7);
  X = digitalRead(8);
  Y = digitalRead(9);
  A = digitalRead(10);
  B = digitalRead(16);
  
  int currentGunState = 0;
  
  if(UPLEFT == LOW)    //
    gp.setButtonState(0, true);
  else
    gp.setButtonState(0, false);

  if(UPRIGHT == LOW)
    gp.setButtonState(1, true);
  else
    gp.setButtonState(1, false);
 
  if(UP == LOW)
    gp.setButtonState(2, true);
  else
    gp.setButtonState(2, false);

  if(DOWN == LOW)
    gp.setButtonState(3, true);
  else
    gp.setButtonState(3, false);    

  if(LEFT == LOW)
    gp.setButtonState(4, true);
  else
    gp.setButtonState(4, false); 

//  if(RIGHT == LOW)
//    gp.setButtonState(5, true);
//  else
//    gp.setButtonState(5, false);     

  if(RIGHTBUTTON == LOW)
    gp.setButtonState(6, true);
  else
    gp.setButtonState(6, false);   

  if(LEFTBUTTON == LOW)
    gp.setButtonState(7, true);
  else
    gp.setButtonState(7, false);     

  if(X == LOW)
    gp.setButtonState(8, true);
  else
    gp.setButtonState(8, false);

  if(Y == LOW)
    gp.setButtonState(9, true);
  else
    gp.setButtonState(9, false);

  if(A == LOW)
    gp.setButtonState(10, true);
  else
    gp.setButtonState(10, false);  

  if(B == LOW)
    gp.setButtonState(11, true);
  else
    gp.setButtonState(11, false);       

  if (Serial.available() > 0) {

        if(Serial.peek() == 'r') {
            Serial.read();
            calDelay = Serial.parseInt();  
            Serial.read();
        } else {
             shooting = Serial.parseInt();  
             Serial.read();  
        }
    }
  unsigned long currentMillis = millis();
  if (abs(currentMillis - gunStateChangeMili) >= calDelay && lastShooting != shooting)  {
    lastShooting = shooting;
    if(shooting == 1) {
      digitalWrite(sol, HIGH);
    } else if (shooting == 0)  {
      digitalWrite(sol, LOW);
    }
    gunStateChangeMili = millis();
    
  }
}


void calibrate()
{
  int lx, ly, rx, ry;
  int i = 0;
  while(i < 13)
  {
    lx = analogRead(A3);
    ly = analogRead(A2);
    rx = analogRead(A1);
    ry = analogRead(A0);
    bool validLX = lx > (leftXcenter - 100) && lx < (leftXcenter + 100);
    bool validLY = ly > (leftYcenter - 100) && ly < (leftYcenter + 100);
    bool validRX = rx > (rightXcenter - 100) && rx < (rightXcenter + 100);
    bool validRY = ry > (rightYcenter - 100) && ry < (rightYcenter + 100);
    if(validLX && validLY && validRX && validRY)
    {
      i++;
      //nothing to do here!
    }
    else i = 0;
    delay(20);
  }
  leftXcenter = lx;
  leftYcenter = ly;
  rightXcenter = rx;
  rightYcenter = ry;
  multiplierLX = (double)127 / (double)lx;
  multiplierLY = (double)127 / (double)ly;
  multiplierRX = (double)127 / (double)rx;
  multiplierRY = (double)127 / (double)ry;
}
