# Arduino LCD Racing Game

This project is a simple racing game for Arduino, played on a 16x2 I2C LCD display.  
It is adapted from [Pierre Massat's original code](http://guitarextended.wordpress.com/),  
with several improvements and new features for a more engaging experience!

## Features

- **Car Racing Simulation:** Dodge obstacles and stay on the road as long as you can.
- **LCD Animation:** The game uses custom LCD characters for road, cars, and scenery.
- **Button Controls:** Two digital buttons let you steer left and right.
- **Increasing Difficulty:** The game speed increases the longer you survive.
- **High Score Memory:** Your best score is saved in EEPROM, even after power off.
- **Random Circuits:** Different backgrounds (city, desert, forest, dino) each time you play.
- **High Score Reset:** Hold both buttons on startup to clear the high score.

## Hardware Requirements

- **Arduino UNO, Mega, or compatible board**
- **16x2 I2C LCD display** (tested with address 0x27, modify if yours differs)
- **2 Push Buttons**
- **Resistors** (for buttons, if needed)
- **Jumper wires, breadboard or PCB**

## Libraries Needed

- [LiquidCrystal I2C](https://github.com/johnrickman/LiquidCrystal_I2C) by Frank de Brabander
- EEPROM (standard with Arduino)
- Your project also requires the `graphics.h` file with custom LCD characters (provided in the repo).

## Pinout

- **Button Left:** Pin 9
- **Button Right:** Pin 8
- **LCD:** SDA/SCL to Arduino's I2C pins (A4/A5 on UNO, 20/21 on Mega)

## How to Play

1. **Start:** Press both buttons to begin. The splash screen will welcome you.
2. **Steer:** Use the left (pin 9) and right (pin 8) buttons to move your car and avoid obstacles.
3. **Dodge Obstacles:** Stay on the road and avoid other cars. Crashing ends the game.
4. **Survive:** The longer you stay on the road, the higher your score (measured in seconds).
5. **High Score:** Try to beat your high score!  
   Hold both buttons on power-up to reset the high score.

## Getting Started

1. **Wire your hardware** according to the pinout above.
2. **Install required libraries** using the Arduino Library Manager.
3. **Upload the code** (`car_lcd_game.ino`) and the `graphics.h` file to your Arduino.
4. **Play and enjoy!**

## Custom Characters

Custom characters for the car, road, and scenery are defined in `graphics.h`.  
You can use [Omer.K's LCD Custom Character Generator](https://omerk.github.io/lcdchargen/) to make your own.

## Acknowledgments

- Original idea and inspiration: [Pierre Massat](http://guitarextended.wordpress.com/)
- LCD custom characters tool: [Omer.K's LCD CharGen](https://omerk.github.io/lcdchargen/)

## License

This project is open source. See the `LICENSE` file for details.
