#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1 // number of LED matrices
// Defining pins for LED matrix
#define CS_PIN 21
#define DIN_PIN 18
#define CLK_PIN 5

// Create the display object
MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Define the pins that the touch sensors are connected to
const int touchPins[4] = {15, 19, 22, 16};
int score = 0;

// Function to light a specific quadrant (4x4 area) on the 8x8 matrix.
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

// Function to light every LED on the board.
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

// Function to flash the full display red three times.
void roundFailEnd()
{
  for (int i = 0; i < 3; i++)
  {
    fullDisplay(true);
    delay(250);
    fullDisplay(false);
    delay(250);
  }
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("\n--Game Begin--");

  // Initialize the LED matrix and set light intensity.
  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, 15);
  matrix.clear();

  // Setup touch sensor pins using INPUT_PULLUP, sensors read HIGH when untouched.
  for (int i = 0; i < 4; i++)
  {
    pinMode(touchPins[i], INPUT_PULLUP);
  }
}

void loop() 
{
  // Flash full display for 1 second at round start.
  fullDisplay(true);
  Serial.println("-- Full display ON for round start --");
  delay(1000);
  matrix.clear();

  // Choose a random quadrant and light it.
  int quadrant = random(0, 4);
  lightQuads(quadrant, true);
  Serial.println("-- Quadrant lit. Awaiting correct touch... --");

  // Wait for user input with timeout (2 seconds).
  // First, wait until the correct sensor is released (HIGH).
  while (digitalRead(touchPins[quadrant]) == LOW)
  {
    delay(10);
  }

  bool correctTouch = false;
  long startMillis = millis();
  while (millis() - startMillis < 2000)
  {
    // Check if the correct sensor is pressed (should read LOW when touched).
      if (digitalRead(touchPins[quadrant]) == LOW)
      {
        correctTouch = true;
        break;
      }
    delay(10);
  }

  // Handle the outcome.
  if (correctTouch == true)
  {
    Serial.println("!! CORRECT sensor touched !!");
    lightQuads(quadrant, false); // Turn off the quadrant
    score++;
    Serial.print("Score: ");
    Serial.println(score);
  }

  else 
  {
    Serial.println("!! TIME OUT / WRONG SENSOR !!");
    roundFailEnd();  // Flash full display red three times
    delay(3000);     // Wait 3 seconds before a new round
  }

  delay(500); // Short pause before next round
}
