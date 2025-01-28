/*
 * Like all Arduino code - copied from somewhere else :)
 * So don't claim it as your own
 * ---------------------------------
 * Basedbased on Pierre Massat's code -> http://guitarextended.wordpress.com/
 * Custom Character Generator: https://omerk.github.io/lcdchargen/
 * The game needs the " LiquidCrystal I2C" library (by Frank de Brabander) installed.
 * ---------------------------------
 Modified version - 25/01 by skuydi
* changed the LCD to a LCD i2c
* added 2 buttons as digital input instead of the LCD Keypad Shield
* added a feature that increases the speed during the game
* added a feature to show high score
* add more scenery divided into several circuits
* minor modifications to the code
* ---------------------------------
*/

#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include "graphics.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

int buttonLeft = 9;  
int buttonRight = 8;

// Various variables used later on...
int pos=0;
char up[16] = "               ";
int roadPos;
int oldRoadPos=5;
int actuDir=1;
unsigned long getTime;
unsigned long getGameTime;
unsigned long getCarTime;
int collision=0;
unsigned long initTime;
int roadWidth; // Road width
int limitButtonRight; // Limit to the left of the road to avoid overtaking on the right.
int moveCar=1;
int carPos;
int posConc1;
int obsConc;
int compteurConc;
int gameSpeed=500;    // Speed of the game
int gameSpeedDec=20;  // Speed step
int gameTime=5000;    // Interval between speed increases
int highScore = 0;
int startGame = 0;
int raceCircuit = 1;

// Read hiscore value from EEPROM
void readHighScore() {
  highScore = (EEPROM.read(0) << 8) + EEPROM.read(1);
  
  // EEPROM initial value is FFFF
  if(highScore == 0xffff)
    highScore = 0;
}

// Write hiscore value to EEPROM
void writeHighScore() {
  EEPROM.write(0, highScore >> 8);
  EEPROM.write(1, highScore & 0xff);
}

// Log function
void logMessage(String message) {
  Serial.println("[LOG] " + message); 
}

void setup() {
  Serial.begin(9600);
  int i;
  lcd.init(); // Initialize the LCD
  lcd.backlight();
  lcd.createChar(1,backslash); // Loading of custom characters into LCD memory
  lcd.createChar(2,car); 
  lcd.createChar(3,carLeft); 
  lcd.createChar(4,carRight); 
  
  roadWidth = 8; // Road width
  limitButtonRight = 6;

  randomSeed(analogRead(A2));

  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);

  // reset hiscore
  if (digitalRead(buttonLeft) == LOW && digitalRead(buttonRight) == LOW) {
    highScore = 0;
    writeHighScore();
    logMessage("Highscore reinitialized!");
  }

  initial();
}

void initial() {
  // Splash screen before the game starts
  long i = 0;
  getTime = millis();
  logMessage("Ready!");

  // Loop until buttons his pressed
  while ((digitalRead(buttonLeft) == HIGH && digitalRead(buttonRight) == HIGH)) {

    if ((millis() - getTime) > 1000) {
      if (i % 2 == 0) {
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("Vroum!");
        for (i = 0; i < 8; i++) {
          lcd.setCursor(i * 2, 1);
          lcd.write(4 - (i % 3));
        }
      } else {
        lcd.clear();
        logMessage("Press both keys ... ");
      } 
      getTime = millis();
    }
  }

  // Initialize variables
  for (i = 0; i < 16; i++) {
    up[i] = ' ';
  }
  getTime = millis();
  getCarTime = getTime;
  initTime = getTime;
  roadPos = 5;
  carPos = roadPos + 3;
  posConc1 = 0;
  obsConc = 0;
  compteurConc = 7;

  // Select a random race circuit (city, desert, forest, dino)
  raceCircuit=random(1,4);
  if (raceCircuit==1) {
    lcd.createChar(5,house2);
    lcd.createChar(6,house2);
    lcd.createChar(7,house1);
    lcd.createChar(8,house2);
  }
  else if (raceCircuit==2) {
    lcd.createChar(5,cactus2);
    lcd.createChar(6,cactus1);
    lcd.createChar(7,cactus2);
    lcd.createChar(8,house1);
  }
  else if (raceCircuit==3) {
    lcd.createChar(5,tree1);
    lcd.createChar(6,tree2);
    lcd.createChar(7,tree3);
    lcd.createChar(8,pinTree);
  }
  else {
    lcd.createChar(5,cow1);
    lcd.createChar(6,dino2);
    lcd.createChar(7,dino2);
    lcd.createChar(8,dino1);
  }
}

// This is the main loop. It controls the input from the buttons, 
// the animation of the road and the car, and the score if the car crashes out of the road.
void loop() {

/*
Serial.print("Road pos : ");
Serial.print(roadPos);
Serial.print(" Car pos : ");
Serial.print(carPos);
Serial.print(" Conc pos : ");
Serial.print(posConc1);
Serial.print(" Obs pos : ");
Serial.println(obsConc);
*/

  int i=0;

  // Controls the car with the 2 buttons
  if (digitalRead(buttonLeft) == LOW) {
    moveCar=0;
    startGame=1;
  }
  else if (digitalRead(buttonRight) == LOW) {
    moveCar=2;
  }
  else {
    moveCar=1;
  }

  // Increase the speed during the game 
  if ((millis()-getGameTime)>5000 && gameSpeed!=gameSpeedDec) { 
    gameSpeed = gameSpeed - gameSpeedDec;
    //Serial.print("Game speed : ");
    //Serial.println(gameSpeed);
    getGameTime=millis();
  }

  // The "showAnimation()" function runs every 500 ms (car and road animation).
  // To increase the speed, replace 500 by a lower value.
  if ((millis()-getTime)>gameSpeed) { 
    showAnimation();
    getTime=millis();
  }
  if (collision!=1) {
    if ((millis()-getCarTime)>125) {
      showCarAnimation();
      getCarTime=millis();
    }
  }
  else score();
}

// This function prints some kind of a "game over" splash screen. 
// It displays the time the player managed to drive the car without
// bumping into another one or crashing out of the road. After a few seconds it
// calls the "initial()" function to display the firt splash screen again.
void score() {
  gameSpeed = 500;
  int time;

  time = (millis() - initTime)/1000;
    
  if (highScore<time){
    highScore = time;
    writeHighScore();
  }
  
  delay(800);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("GAME OVER!");
  delay(1500);

  String message = "Score : ";
  message = message + time;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(message);
  message = "High score : ";
  message = message + highScore;
  lcd.setCursor(0,1);
  lcd.print(message);
  delay(3000);
  
  collision=0;
  initial();
}

// Actual animation (including random generation of obstacles and scenery).
void showAnimation() {
  int i;
  int increment=0;
  int rand;
  int typeConc;
  int oldDir;
  int posDec1=1000;
  int posDec2=1000;
  int posDec3=1000;
  int posDec4=1000;
  int typeDec1;
  int typeDec2;
  int typeDec3;
  int typeDec4;
  oldDir = actuDir;
  oldRoadPos = roadPos;

  // Generation of random scenery
  if (random(0,100)>60) { // Decor 1
    typeDec1=random(0,4);
    if (random(0,2)>0) {
      posDec1=roadPos-random(2,5);
    }
    else posDec1=roadPos+roadWidth+random(0,4)+2;
  }
  else posDec1=1000;

  if (random(0,100)>50) { // Decor 2
    typeDec2=random(0,4);
    if (random(0,2)>0) {
      posDec2=roadPos-random(2,5);
    }
    else posDec2=roadPos+roadWidth+random(0,4)+2;
  }
  else posDec2=1000;

  if (random(0,100)>50) { // Decor 3
    typeDec3=random(0,4);
    if (random(0,2)>0) {
      posDec3=roadPos-random(2,5);
    }
    else posDec3=roadPos+roadWidth+random(0,4)+2;
  }
  else posDec3=1000;

  if (random(0,100)>80) { // Decor 4
    typeDec4=random(0,4);
    if (random(0,2)>0) {
      posDec4=roadPos-random(2,5);
    }
    else posDec4=roadPos+roadWidth+random(0,4)+2;
  }
  else posDec4=1000;

  // Random generation of new road direction (depending on constraints)
  if (roadPos==0) rand = random(1,3); // only staight or to the right
  else if (roadPos==limitButtonRight) rand = random(0,2); // Only straight or to the left
  else rand = random(0,3); // no constraints
  // Moves the road sideways when necessary

  if (oldDir==0) {
    if (rand!=2 && roadPos!=0) roadPos = roadPos-1;
  }
  else if (oldDir==2) {
    if (rand!=0 && roadPos!=limitButtonRight) roadPos = roadPos+1;
  }
  actuDir = rand;
  // Copy top line to bottom line.
  lcd.clear();
  lcd.setCursor(0,1);
  for (i=0;i<16;i++) { 
    lcd.setCursor(i,1);
    if (up[i]=='*') lcd.write(1);
    else if (up[i]=='C') lcd.write(5);
    else if (up[i]=='T') lcd.write(6);
    else if (up[i]=='P') lcd.write(7);
    else if (up[i]=='H') lcd.write(8);
    else lcd.write(up[i]);
    //Serial.println(up[i]);
  }
  // Create and print the top line based on the direction and the position of the road.
  for (i=0;i<16;i++) { // (we write the top line)
    lcd.setCursor(i,0);
    if (i==roadPos || i==roadPos+roadWidth) { // If left or right border of the road
      if (actuDir==0) {
        lcd.write(1); // We use a custom character for the backlash when turning left
        up[i]='*';
      }
      else if (actuDir==1) {
        lcd.write('|');
        up[i]='|';
      }
      else if (actuDir==2) {
        lcd.write('/');
        up[i]='/';
      }
    }
    else if (i==posDec1) {
      lcd.write(typeDec1+5);
      if (typeDec1==0) up[i]='C';
      else if (typeDec1==1) up[i]='T';
      else if (typeDec1==2) up[i]='P';
      else if (typeDec1==3) up[i]='H';
    }
    else if (i==posDec2) {
      lcd.write(typeDec2+5);
      if (typeDec2==0) up[i]='C';
      else if (typeDec2==1) up[i]='T';
      else if (typeDec2==2) up[i]='P';
      else if (typeDec2==3) up[i]='H';
    }
    else if (i==posDec3) {
      lcd.write(typeDec3+5);
      if (typeDec3==0) up[i]='C';
      else if (typeDec3==1) up[i]='T';
      else if (typeDec3==2) up[i]='P';
      else if (typeDec3==3) up[i]='H';
    }
    else if (i==posDec4) {
      lcd.write(typeDec4+5);
      if (typeDec4==0) up[i]='C';
      else if (typeDec4==1) up[i]='T';
      else if (typeDec4==2) up[i]='P';
      else if (typeDec4==3) up[i]='H';
    }
    else { // Nothing special to write
      lcd.write(' ');
      up[i]=' ';
    }
  }
  // Other cars
  if (compteurConc>6) { // Create new car when the last one has disapeared
    obsConc=1000;
    if (random(0,100)>70) {
      posConc1 = random(1,roadWidth); // Random position on top line
      compteurConc = 0;  
    }
  }
  else { // If the car is still there, update screen
    if (compteurConc<3) { // The car stays a little in the top line...
      lcd.setCursor(posConc1 + roadPos,0);
      // We adapt the direction of the car to that of the road
      if (actuDir==0) typeConc=3;
      else if (actuDir==1) typeConc = 2;
      else typeConc = 4;
      lcd.write(typeConc);  
      compteurConc = compteurConc + 1;  
    }
    else { // ... then it moves to the bottom line
      lcd.setCursor(posConc1 + oldRoadPos,1);
      obsConc = posConc1 + oldRoadPos;
      // We adapt the direction of the car to that of the road
      if (oldDir==0) typeConc=3;
      else if (oldDir==1) typeConc = 2;
      else typeConc = 4;
      lcd.write(typeConc);
      compteurConc = compteurConc + 1;       
    }
  }
}

// This function controls the animation of the car itself.
void showCarAnimation() {
  int typeCar;  
  lcd.setCursor(carPos,1);
  lcd.print(' ');
  // Computes new position and direction of the car.
  if (moveCar==0) {
    if (carPos!=0) {
      carPos = carPos - 1; // Move the car to the left
      typeCar = 3; // Custom character for the car turning to the left
    }
    else typeCar = 2;
  }
  else if (moveCar==2) {
    if (carPos!=15) {
      carPos = carPos + 1; // Move the car to the right
      typeCar = 4;  // Custom character for the car turning to the right
    }
    else typeCar = 2;
  }
  else {
    typeCar = 2;  // Custom character for the car moving straight ahead
  }
  // If no collision, print the car on the screen...
  if (carPos!=roadPos && carPos!=(roadPos + roadWidth) && carPos!=obsConc) { 
    lcd.setCursor(carPos,1);
    lcd.write(typeCar);
  }

  else {  // ... else : boom!
    lcd.setCursor(carPos-2,1);
    lcd.print("Boom!");
    collision = 1;
  }  
}
