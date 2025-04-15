#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1 // number of led matrixes 
// Defining pins for LED matrix
#define CS_PIN 21
#define DIN_PIN 18
#define CLK_PIN 5

// Create the display object
MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Define the pin that the touch sensor is connected to
const int touchPins[4] = {15, 19, 22, 16};
int score = 0;
long fullDeisplayTime = 1000;
long debounceDealy = 500;

void lightQuads (int quadrant, bool quadState)
{
  int rowStart;
  int rowEnd;
  int colStart;
  int colEnd;

  if (quadrant == 0) // Top-Left
  {          
    rowStart = 0; 
    rowEnd = 3; 
    colStart = 0;
    colEnd = 3;
  } 
  else if (quadrant == 1) // Top-Right
  {   
    rowStart = 0; 
    rowEnd = 3; 
    colStart = 4; 
    colEnd = 7;
  } 
  else if (quadrant == 2)  // Bottom-Left
  {  
    rowStart = 4; 
    rowEnd = 7; 
    colStart = 0; 
    colEnd = 3;
  } 
  else if (quadrant == 3) // Bottom-Right
  {   
    rowStart = 4; 
    rowEnd = 7; 
    colStart = 4; 
    colEnd = 7;
  } 
  else 
  {
    return; // Invalid quadrant
  }

  for (int r = rowStart; r <= rowEnd; r++)
  {
    for (int c = colStart; c <= colEnd; c++)
    {
      matrix.setPoint(r, c, quadState);
    }
  }
}

// Function for testing every LED on board works (turns on and off)
void fullDisplay (bool state)
{
  for (int r = 0; r < 8; r++)
  {
    for (int c = 0; c < 8; c++)
    {
      matrix.setPoint(r, c, state);
    }
  }
}

void setup() 
{
  Serial.begin(115200);

  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, 5);
  matrix.clear();

  for (int i = 0; i < 4; i++)
  {
    pinMode(touchPins[i], INPUT_PULLUP);
  }
}

void loop() 
{
  // Flash display for a second
  fullDisplay(true);
  delay(1000);
  matrix.clear();

  //Choose random quadrant and light it up
  int quadrant = random(0,4);
  lightQuads(quadrant, true);

  // For now wait forever for correct sensor touch, can be adjusted.
  // Uses INPUT_PULLUP so set sensors default state to high, reads low then touched.
  while (digitalRead(touchPins[quadrant]) == HIGH)
  {
    delay(10); // check every 10ms for a touch.
  }

  delay(500);

  // Checking again, if sensor is still touch, then its correct.
  if (digitalRead(touchPins[quadrant]) == LOW)
  {
    lightQuads (quadrant, false);
    score++;
  }

  delay(500);
}

