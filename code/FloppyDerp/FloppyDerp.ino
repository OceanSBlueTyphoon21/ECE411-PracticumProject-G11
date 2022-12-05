#include <SPI.h>

// Pin number macro definitions
#define DISPLAY_LITE 9

#define BUTTON_LEFT A5
#define BUTTON_UP A4
#define BUTTON_RIGHT A3
#define BUTTON_DOWN A2
#define BUTTON_A A1
#define BUTTON_B A0

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// From the PDQ_GFX config file found in the library's repo
#define    ILI9341_CS_PIN               6            
#define    ILI9341_DC_PIN              11        
#define    ILI9341_RST_PIN             10            
#define    ILI9341_SAVE_SPI_SETTINGS    0

// Initialize display object

#include <PDQ_GFX.h>
#include <PDQ_ILI9341.h>

PDQ_ILI9341 tft;

// Sprite bit map
const PROGMEM uint8_t ballBitMap[21 * 21] = {
    0x01, 0xfc, 0x00, 0x07, 0xff, 0x00, 0x0f, 0xff, 0x80, 0x1f, 0xff, 0xc0, 0x3f, 0xff, 0xe0, 0x7f, 
	0xff, 0xf0, 0x7f, 0xff, 0xf0, 0xf9, 0xfc, 0xf8, 0xf9, 0xfc, 0xf8, 0xff, 0xff, 0xf8, 0xff, 0xff, 
	0xf8, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xf8, 0xf9, 0xfc, 0xf8, 0x78, 0xf8, 0xf0, 0x7c, 0x01, 0xf0, 
	0x3e, 0x03, 0xe0, 0x1f, 0xff, 0xc0, 0x0f, 0xff, 0x80, 0x07, 0xff, 0x00, 0x01, 0xfc, 0x00};

// Initialize global vars
int Screen=0;
int screenWidth=320;
int screenHeight=240;

// Ball vars
int ballX=screenWidth/5;
int ballY=screenHeight/4;
int ballSize = 21;
int ballColor = GREEN;
int oldY=ballY;

// Gravity vars
float gravity = 0.5;
float ballSpeedVert = 0;

void setup() {
  Serial.begin(9600);
  delay(300);
  tft.begin();
  tft.setRotation(3);
  //canvas.setTextWrap(false);
  
  // Initialize user button inputs
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);

}

// Storing button states for debouncing
byte upPress = LOW;
byte rightPress = LOW;
byte downPress = LOW;
byte leftPress = LOW;
byte aPress = LOW;
byte bPress = LOW;

// Storing time when buttons pressed for debouncing
unsigned long upTime = 0;
unsigned long rightTime = 0;
unsigned long downTime = 0;
unsigned long leftTime = 0;
unsigned long aTime = 0;
unsigned long bTime = 0;

unsigned long timeSince = 0;

byte quitSelect = 0;

void loop() {
  FloppyDerp();
}

void FloppyDerp() {

/********* SETUP BLOCK *********/

  drawBackground();
  initScreen();
  while (true) {
    // Display the contents of the current screen
    if(millis() - timeSince > 35){
      timeSince = millis();
      if (Screen == 0) {
        initScreen(); // Initial Screen
      }
      else if (Screen == 1) {
        gameScreen(); // Game Play Screen
      }
      else if (Screen == 2) {
        gameOverScreen(); // Game Over Screen
      }
      else if (Screen == 3) {
        pauseScreen(); // Pause Screen
      }
    }
      
    if (checkButton(BUTTON_A, &aPress, &aTime)) { // Press A, play game
      if (Screen == 0) { // Initial screen
        Screen=1; // Game screen
      }
      else if (Screen == 1) { // Game screen
        // Jump
      }
      else if (Screen == 3) { // Pause screen
        if (quitSelect) {
          // Return to main menu here once game is combined with UI
          drawBackground();
          Screen = 0; // For now, just restart game
        }
        else {
          drawBackground();
          Screen = 1; // Unpause game
        }
      }
    }

    if (checkButton(BUTTON_B, &bPress, &bTime)) { // Press B to pause
      if (Screen == 1) { // If in game screen
        tft.fillRect(screenWidth/4, screenHeight/4, screenWidth/2, screenHeight/2, BLACK); // Draw pause menu background
        quitSelect = 0;
        Screen = 3; // Pause the game
      }
      else if (Screen == 3) { // If on pause screen
        drawBackground();
        Screen = 1; // Unpause the game
      }
    }

    if (checkButton(BUTTON_LEFT, &leftPress, &leftTime)) {
      if (Screen == 3) { // If on pause screen
        quitSelect = 0; // Select "Don't return to main menu"
      }
    }

    if (checkButton(BUTTON_RIGHT, &rightPress, &rightTime)) {
      if (Screen == 3) { // If on pause screen
        quitSelect = 1; // Select "Return to main menu"
      }
    }
  }
}


/********* SCREEN CONTENTS *********/
void drawBackground() {
  tft.fillScreen(BLUE);
}

void initScreen() {
  // codes of initial screen graphics
}
void gameScreen() {
  // codes of game screen
  drawBall();
  applyGravity();
  keepInScreen();
}
void gameOverScreen() {
  // codes for game over screen
}

void pauseScreen() {
  tft.setCursor(94, 75);
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  tft.print("GAME PAUSED");

  tft.setTextSize(1);
  tft.setCursor(99, 105);
  tft.print("Return to main menu?");

  if (!quitSelect) {
    tft.setTextColor(BLUE, YELLOW);
    tft.setCursor(120, 135);
    tft.print("No");
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(180, 135);
    tft.print("Yes");
  }
  else {
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(120, 135);
    tft.print("No");
    tft.setTextColor(BLUE, YELLOW);
    tft.setCursor(180, 135);
    tft.print("Yes");
  }

}

void drawBall() {
  tft.drawBitmap(ballX, ballY, ballBitMap, ballSize, ballSize, GREEN, BLUE);
  clearBall();
}

void clearBall() {
  int edge = oldY;
  if ((ballY - oldY) < 0) {
    edge = ballY + ballSize;
  }
    tft.fillRect(ballX, edge, ballSize, abs(ballY - oldY), BLUE);
    


}

void applyGravity() {
  ballSpeedVert += gravity;
  oldY = ballY;
  ballY += ballSpeedVert;
}

void makeBounceBottom(float surface) {
  oldY = ballY;
  ballY = surface-ballSize;
  ballSpeedVert*=-1;
}

void makeBounceTop(float surface) {
  oldY = ballY;
  ballY = surface;
  ballSpeedVert*=-1;
}
// keep ball in the screen

void keepInScreen() {
  // ball hits floor
  if (ballY+ballSize > screenHeight) { 
    makeBounceBottom(screenHeight);
  }
  // ball hits ceiling
  if (ballY < 0) {
    makeBounceTop(0);
  }
}

byte checkButton(int pinNumber, byte *buttonPress, unsigned long *buttonTime) {
  // Reads button press with debounce timer, based on debounce routine from arduino docs
  // buttonPress is the current state of the input, buttonTime is the time when the button state last changed from millis()
  // Pass pointers to variables containing button state and time

  byte buttonInput = digitalRead(pinNumber);

  if ((millis() - *buttonTime) > 50) {
    if (*buttonPress != buttonInput) {
      *buttonTime = millis(); // Record last time button pressed/released
      *buttonPress = buttonInput; // Change button state

      if (*buttonPress == LOW) {
        return 1; // Return true if button state was changed to low
      }   
    }
      
    return 0;
  }

  // Returns false if button state was not changed to LOW
  return 0;
}

