    #include <Adafruit_GFX.h>
    #include <MCUFRIEND_kbv.h>
    #include <Fonts/FreeSansBold24pt7b.h>


 //keypad
    
    #define LCD_CS A3
    #define LCD_CD A2
    #define LCD_WR A1
    #define LCD_RD A0
    #define LCD_RESET A4
    
    MCUFRIEND_kbv tft;
    
    #define BLACK 0x0000
    #define WHITE 0xFFFF
    #define DARK_RED 0x8000
    
    String pin = "";
    bool pinEntered = false;
    bool confirmPageDisplayed = false;
    
    void setup() {
      Serial.begin(115200);
      uint16_t ID = tft.readID();
      tft.begin(ID);
      tft.setRotation(1);
      tft.fillScreen(WHITE);
      tft.setTextColor(BLACK);
      tft.setFont(&FreeSansBold24pt7b);
      Serial.println("Setup complete.");
    }
    
    void loop() {
      // Code to handle other functionalities (e.g., keypad input) goes here
    }
    
    void displayEnterCandidateID() {
      tft.fillScreen(WHITE);
      String message = "Enter Candidate ID";
      int16_t x1, y1;
      uint16_t w, h;
      tft.setTextWrap(false);
      tft.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
      int16_t x = (tft.width() - w) / 2;
      int16_t y = (tft.height() - h) / 2 - 80;
    
      tft.setCursor(x, y);
      tft.setTextColor(BLACK, WHITE);
      tft.print(message);
    
      updatePINDisplay();
      Serial.println("Enter Candidate ID page displayed");
    }
    
    void updatePINDisplay() {
      Serial.println("Updating PIN display");
      int16_t x = (tft.width() - 3 * 140) / 2;
      int16_t y = (tft.height() / 2) - 60;
    
      for (int i = 0; i < 3; i++) {
        if (i < pin.length()) {
          drawStylishSquare(x + i * 140, y, DARK_RED);
        } else {
          drawEmptySquare(x + i * 140, y);
        }
      }
    
      String clearMessage = "Clear = '*'";
      int16_t x1, y1;
      uint16_t w, h;
      tft.setFont(&FreeSansBold24pt7b);
      tft.getTextBounds(clearMessage, 0, 0, &x1, &y1, &w, &h);
      x = tft.width() - w - 20;
      y = (tft.height() / 2) + 140;
      tft.fillRect(0, y - h, tft.width(), h + 10, WHITE);
      tft.setCursor(x, y);
      tft.print(clearMessage);
      tft.setFont(&FreeSansBold24pt7b);
      Serial.println("PIN display updated");
    }
    
    void drawStylishSquare(int16_t x, int16_t y, uint16_t color) {
      int16_t w = 120;
      int16_t h = 120;
      int16_t padding = 10;
    
      tft.fillRect(x + padding, y + padding, w - padding * 2, h - padding * 2, color);
      tft.drawRect(x, y, w, h, BLACK);
      tft.drawRect(x + padding, y + padding, w - padding * 2, h - padding * 2, WHITE);
    }
    
    void drawEmptySquare(int16_t x, int16_t y) {
      int16_t w = 120;
      int16_t h = 120;
    
      tft.fillRect(x, y, w, h, WHITE);
      tft.drawRect(x, y, w, h, BLACK);
    }
    
    void displayConfirmationPage() {
      tft.fillScreen(WHITE);
      String message1 = "Entered ID:";
      String pinMessage = pin;
      String message2 = "Press * to try again";
      String message3 = "Press # to confirm";
      int16_t x1, y1;
      uint16_t w, h;
    
      tft.setTextWrap(false);
      
      tft.getTextBounds(message1, 0, 0, &x1, &y1, &w, &h);
      int16_t x = (tft.width() - w) / 2;
      int16_t y = (tft.height() / 2) - 120;
      tft.setCursor(x, y);
      tft.setTextColor(BLACK, WHITE);
      tft.print(message1);
    
      tft.getTextBounds(pinMessage, 0, 0, &x1, &y1, &w, &h);
      x = (tft.width() - w) / 2;
      y = (tft.height() / 2) - 60;
      tft.setCursor(x, y);
      tft.setTextColor(DARK_RED, WHITE);
      tft.print(pinMessage);
    
      tft.getTextBounds(message2, 0, 0, &x1, &y1, &w, &h);
      x = (tft.width() - w) / 2;
      y = (tft.height() / 2);
      tft.setCursor(x, y);
      tft.setTextColor(BLACK, WHITE);
      tft.print(message2);
    
      tft.getTextBounds(message3, 0, 0, &x1, &y1, &w, &h);
      x = (tft.width() - w) / 2;
      y = (tft.height() / 2) + 60;
      tft.setCursor(x, y);
      tft.setTextColor(BLACK, WHITE);
      tft.print(message3);
    
      confirmPageDisplayed = true;
      Serial.println("Confirmation page displayed");
    }
    
    void sendPinToESP32() {
      tft.fillScreen(WHITE);
      String message = "Sending Vote";
      int16_t x1, y1;
      uint16_t w, h;
      tft.setTextWrap(false);
      tft.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
      int16_t x = (tft.width() - w) / 2;
      int16_t y = (tft.height() - h) / 2;
    
      tft.setCursor(x, y);
      tft.setTextColor(BLACK, WHITE);
      tft.print(message);
    
      Serial2.println(pin);
    
      delay(1000);
    
      unsigned long startTime = millis();
      while (!Serial2.available()) {
        if (millis() - startTime > 5000) {
          tft.fillScreen(WHITE);
          tft.setCursor(x, y);
          tft.setTextColor(BLACK, WHITE);
          tft.print("Vote Failed");
          delay(2000);
          displayEnterCandidateID();
          return;
        }
      }
    
      String response = Serial2.readStringUntil('\n');
      response.trim();
    
      Serial.print("Response from ESP32: ");
      Serial.println(response);
    
      int responseCode = response.toInt();  // Convert the response to an integer
    
      if (responseCode == 1) {
        tft.fillScreen(WHITE);
        tft.setCursor(x, y);
        tft.setTextColor(BLACK, WHITE);
        tft.print("Vote Accepted");
        delay(2000);
        tft.fillScreen(WHITE);
        String finalMessage = "Have a nice day";
        tft.getTextBounds(finalMessage, 0, 0, &x1, &y1, &w, &h);
        x = (tft.width() - w) / 2;
        y = (tft.height() - h) / 2;
        tft.setCursor(x, y);
        tft.print(finalMessage);
        delay(5000);
        tft.fillScreen(WHITE);
        terminateProcess();
      } else {
        tft.fillScreen(WHITE);
        tft.setCursor(x, y);
        tft.setTextColor(BLACK, WHITE);
        tft.print("Vote Rejected");
        delay(2000);
        displayEnterCandidateID();
      }
    }
    
    void terminateProcess() {
      pinEntered = false;
      confirmPageDisplayed = false;
      pin = "";
      tft.fillScreen(WHITE);
      Serial.println("Process terminated. Ready for new input.");
    }
    

//fingerprint

    #include <Adafruit_GFX.h>
        #include <MCUFRIEND_kbv.h>
        #include <Fonts/FreeSansBold24pt7b.h>
        
        #define LCD_CS A3
        #define LCD_CD A2
        #define LCD_WR A1
        #define LCD_RD A0
        #define LCD_RESET A4
        
        MCUFRIEND_kbv tft;
        
        // Color definitions
        #define BLACK 0x0000
        #define WHITE 0xFFFF
        #define GREEN 0x07E0
        #define RED 0xF800
        
        void setup() {
          Serial.begin(115200);
          uint16_t ID = tft.readID();
          Serial.print("TFT ID: 0x");
          Serial.println(ID, HEX);
          tft.begin(ID);
          tft.setRotation(1); // Adjust rotation if needed
          tft.fillScreen(WHITE); // Set background to white
          tft.setFont(&FreeSansBold24pt7b); // Set the font to FreeSansBold24pt7b
        }
        
        void loop() {
          // This loop is empty as TFT related functions are not dependent on loop.
        }
        
        // Function to display multiline text on the TFT screen
        void displayMultilineText(String text, uint16_t color) {
          tft.fillScreen(WHITE); // Clear the screen with white background
          tft.setTextColor(color, WHITE); // Set text color
        
          // Split the text into lines
          int16_t x1, y1;
          uint16_t w, h;
          tft.setTextWrap(false); // Disable text wrap
          int lineHeight = 40; // Approximate height of each line, adjust as needed
        
          // Calculate number of lines in the text
          int numLines = 1;
          for (char c : text) {
            if (c == '\n') {
              numLines++;
            }
          }
        
          int startY = (tft.height() - lineHeight * numLines) / 2; // Calculate start Y position
        
          String line = "";
          int lineIndex = 0;
        
          for (char c : text) {
            if (c == '\n') {
              tft.getTextBounds(line.c_str(), 0, 0, &x1, &y1, &w, &h); // Get text bounds
              tft.setCursor((tft.width() - w) / 2, startY + lineIndex * lineHeight); // Set cursor position
              tft.print(line); // Print line
              line = "";
              lineIndex++;
            } else {
              line += c; // Append character to line
            }
          }
        
          // Print the last line
          tft.getTextBounds(line.c_str(), 0, 0, &x1, &y1, &w, &h);
          tft.setCursor((tft.width() - w) / 2, startY + lineIndex * lineHeight);
          tft.print(line);
        }
        
        // Example functions that use the displayMultilineText function
        void showReadyMessage() {
          displayMultilineText("Ready\nfor\nfingerprint scan", BLACK);
        }
        
        void showRegisterMessage() {
          displayMultilineText("Preparing to register fingerprint.\nPlease wait...", BLACK);
        }
        
        void showRegistrationCancelled() {
          displayMultilineText("Registration\nCancelled", RED);
        }
        
        void showFingerprintRegistered() {
          displayMultilineText("Fingerprint\nstored!", GREEN);
        }
        
        void showFingerprintFailed() {
          displayMultilineText("Failed to capture\nfingerprint", RED);
        }
        
        void showFingerprintMatched() {
          displayMultilineText("Fingerprint\nmatched!", GREEN);
        }
        
        void showNoMatchFound() {
          displayMultilineText("No match\nfound", RED);
        }
        
        void showAllFingerprintsDeleted() {
          displayMultilineText("All fingerprints\ndeleted!", GREEN);
        }
        
        void showDeleteAllFailed() {
          displayMultilineText("Failed to delete\nall fingerprints", RED);
        }
        

//bmp
        #include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BMP085_U.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>

// Pin definitions for TFT
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

MCUFRIEND_kbv tft;

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define BLUE 0x001F

Adafruit_BMP280 bmp280; // BMP280 sensor
Adafruit_BMP085_Unified bmp180 = Adafruit_BMP085_Unified(10085); // BMP180 sensor

const float pressureThresholdDigitBMP280 = 60.0; // Pressure change threshold for detecting a puff on digit entry page
const float pressureThresholdDigitBMP180 = 100.0; // Pressure change threshold for detecting a puff on digit entry page
const float pressureThresholdConfirmBMP280 = 100.0; // Pressure change threshold for detecting a puff on confirmation page
const float pressureThresholdConfirmBMP180 = 60.0; // Pressure change threshold for detecting a puff on confirmation page

float lastPressureBMP280 = 0.0;
float lastPressureBMP180 = 0.0;
bool bmp280Initialized = false;
bool bmp180Initialized = false;
bool secondPageDisplayed = false;
bool fifthPageDisplayed = false;
bool confirmationPageDisplayed = false;
bool thirdPageDisplayed = false;
int digitCount = 0; // Tracks the current digit being selected
int selectedDigit = 0;  // The first cell is selected
int threeDigitNumber[3] = {0, 0, 0}; // Array to store the three-digit number

bool processStarted = false; // Flag to track if the process has started
unsigned long lastUpdateTime = 0;

void displayMessage(const char* message, const char* countdownMessage = nullptr) {
    tft.fillScreen(WHITE);
    tft.setFont(&FreeSansBold24pt7b);
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
    int16_t x = (tft.width() - w) / 2;
    int16_t y = (tft.height() - h) / 2 - 20;  // Adjusted for better centering
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message);

    if (countdownMessage != nullptr) {
        tft.setFont(&FreeSansBold24pt7b);
        tft.getTextBounds(countdownMessage, 0, 0, &x1, &y1, &w, &h);
        x = (tft.width() - w) / 2;
        y = (tft.height() - h) / 2 + 60;  // Adjusted for better centering
        tft.setCursor(x, y);
        tft.setTextColor(RED);
        tft.print(countdownMessage);
    }
}

void displayCountdown(int seconds) {
    char countdownMessage[3];
    for (int i = seconds; i > 0; i--) {
        snprintf(countdownMessage, 3, "%d", i);
        displayMessage("Voting Starting", countdownMessage);
        delay(1000);
    }
    displaySecondPage();
}

void displaySecondPage() {
    if (secondPageDisplayed) return;

    secondPageDisplayed = true;
    Serial.println("Displaying second page...");
    tft.fillScreen(WHITE);
    tft.setFont(&FreeSansBold24pt7b);  // Using the same font size
    const char* message1 = "Puff X to Switch";
    const char* message2 = "Puff Y to Select";

    // Display message1 centered at the top
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(message1, 0, 0, &x1, &y1, &w, &h);
    int16_t x = (tft.width() - w) / 2;
    int16_t y = (tft.height() / 2) - 60;  // Adjust position to center vertically
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message1);

    // Display message2 centered below message1
    tft.getTextBounds(message2, 0, 0, &x1, &y1, &w, &h);
    x = (tft.width() - w) / 2;
    y = (tft.height() / 2);  // Adjust position to center vertically
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message2);

    Serial.println("Displayed second page. Starting 10-second countdown...");

    // Display loading bar
    int barWidth = tft.width() - 40;
    int barHeight = 30;  // Increased height
    int barX = 20;
    int barY = (tft.height() / 2) + 40;
    tft.drawRect(barX, barY, barWidth, barHeight, BLACK);  // Draw the border for the loading bar

    for (int i = 0; i <= barWidth; i += 2) {  // Increment by 2 for smoother animation
        tft.fillRect(barX + 1, barY + 1, i, barHeight - 2, RED);  // Fill the loading bar
        delay(50);  // Smooth animation for 10 seconds
    }

    displayThirdPage();
}

void displayThirdPage() {
    if (thirdPageDisplayed) return;

    thirdPageDisplayed = true;
    Serial.println("Displaying third page...");
    tft.fillScreen(WHITE);
    tft.setFont(&FreeSansBold18pt7b);  // Slightly smaller font size
    const char* message1 = "Go to previous page: Puff X";
    const char* message2 = "Proceed to next page";
    const char* message3 = "in 15 seconds";

    // Display message1 centered at the top
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(message1, 0, 0, &x1, &y1, &w, &h);
    int16_t x = (tft.width() - w) / 2;
    int16_t y = (tft.height() / 2) - 80;  // Adjust position to center vertically
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message1);

    // Display message2 centered below message1
    tft.getTextBounds(message2, 0, 0, &x1, &y1, &w, &h);
    x = (tft.width() - w) / 2;
    y = (tft.height() / 2) - 40;  // Adjust position to center vertically
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message2);

    // Display message3 centered below message2
    tft.getTextBounds(message3, 0, 0, &x1, &y1, &w, &h);
    x = (tft.width() - w) / 2;
    y = (tft.height() / 2);  // Adjust position to center vertically
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message3);

    Serial.println("Displayed third page. Starting 15-second loading bar...");

    // Display loading bar
    int barWidth = tft.width() - 40;
    int barHeight = 30;  // Increased height
    int barX = 20;
    int barY = (tft.height() / 2) + 40;
    tft.drawRect(barX, barY, barWidth, barHeight, BLACK);  // Draw the border for the loading bar

    unsigned long startTime = millis();
    while (millis() - startTime < 15000) {  // 15 seconds
        int elapsed = millis() - startTime;
        int progress = map(elapsed, 0, 15000, 0, barWidth);  // Calculate progress
        tft.fillRect(barX + 1, barY + 1, progress, barHeight - 2, RED);  // Fill the loading bar

        // Print BMP280 pressure value to serial monitor every second
        if (bmp280Initialized && (elapsed % 1000 == 0)) {
            float currentPressure = bmp280.readPressure();
            Serial.print("BMP280 Pressure: ");
            Serial.print(currentPressure);
            Serial.println(" Pa");

            float pressureChange = currentPressure - lastPressureBMP280;
            Serial.print("Pressure Change: ");
            Serial.print(pressureChange);
            Serial.println(" Pa");

            if (pressureChange >= pressureThresholdDigitBMP280) {
                Serial.print("Puff detected by BMP280! Pressure change: ");
                Serial.print(pressureChange);
                Serial.println(" Pa");
                displaySecondPage(); // Go back to the second page
                return;
            }

            lastPressureBMP280 = currentPressure;
        }

        delay(50);  // Smooth animation
    }

    Serial.println("Loading bar completed.");
    displayFourthPage("Next page: First digit");
}

void displayFourthPage(const char* message) {
    Serial.println("Displaying fourth page...");
    tft.fillScreen(WHITE);
    tft.setFont(&FreeSansBold18pt7b);  // Slightly smaller font size

    // Display message centered
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
    int16_t x = (tft.width() - w) / 2;
    int16_t y = (tft.height() - h) / 2;
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message);

    Serial.println("Displayed fourth page. Starting 5-second loading bar...");

    // Display loading bar
    int barWidth = tft.width() - 40;
    int barHeight = 30;  // Increased height
    int barX = 20;
    int barY = (tft.height() / 2) + 40;
    tft.drawRect(barX, barY, barWidth, barHeight, BLACK);  // Draw the border for the loading bar

    for (int i = 0; i <= barWidth; i += 2) {  // Increment by 2 for smoother animation
        tft.fillRect(barX + 1, barY + 1, i, barHeight - 2, RED);  // Fill the loading bar
        delay(50);  // Smooth animation for 5 seconds
    }

    // Transition to the fifth page after the loading bar
    displayFifthPage();
}

void displayFifthPage() {
    Serial.println("Displaying fifth page with digit cells...");
    tft.fillScreen(WHITE);
    tft.setFont(&FreeSansBold24pt7b);  // Slightly larger font size

    const int cellWidth = tft.width() / 5;
    const int cellHeight = tft.height() / 2;

    const char* digits[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    const int numDigits = 10;

    for (int i = 0; i < numDigits; i++) {
        int x = (i % 5) * cellWidth;
        int y = (i / 5) * cellHeight;

        if (i == selectedDigit) {
            tft.fillRect(x, y, cellWidth, cellHeight, BLUE);
            tft.setTextColor(WHITE);
        } else {
            tft.drawRect(x, y, cellWidth, cellHeight, BLACK);
            tft.setTextColor(BLACK);
        }

        int16_t x1, y1;
        uint16_t w, h;
        tft.getTextBounds(digits[i], 0, 0, &x1, &y1, &w, &h);
        int16_t textX = x + (cellWidth - w) / 2;
        int16_t textY = y + (cellHeight + h) / 2;

        tft.setCursor(textX, textY);
        tft.print(digits[i]);
    }

    fifthPageDisplayed = true;
    Serial.println("Fifth page displayed with digit cells.");
}

void updateSelectedDigit() {
    selectedDigit = (selectedDigit + 1) % 10;
    displayFifthPage();
}

void updatePressureValues() {
    if (bmp280Initialized) {
        float currentPressureBMP280 = bmp280.readPressure();
        float pressureChangeBMP280 = currentPressureBMP280 - lastPressureBMP280;

        Serial.print("BMP280 Pressure: ");
        Serial.print(currentPressureBMP280);
        Serial.print(" Pa (Change: ");
        Serial.print(pressureChangeBMP280);
        Serial.println(" Pa)");

        lastPressureBMP280 = currentPressureBMP280;
    }

    if (bmp180Initialized) {
        sensors_event_t event;
        bmp180.getEvent(&event);
        float currentPressureBMP180 = event.pressure * 100;  // Convert hPa to Pa
        float pressureChangeBMP180 = currentPressureBMP180 - lastPressureBMP180;

        Serial.print("BMP180 Pressure: ");
        Serial.print(currentPressureBMP180);
        Serial.print(" Pa (Change: ");
        Serial.print(pressureChangeBMP180);
        Serial.println(" Pa)");

        lastPressureBMP180 = currentPressureBMP180;
    }
}

void sendThreeDigitNumber() {
    String threeDigitString = String(threeDigitNumber[0]) + String(threeDigitNumber[1]) + String(threeDigitNumber[2]);
    Serial2.println(threeDigitString); // Send the three-digit number to the ESP32
    Serial.print("Sent three-digit number: ");
    Serial.println(threeDigitString);
}

    void displayWaitPage() {
    sendThreeDigitNumber(); // Send the number before displaying the wait page
    
    Serial.println("Displaying wait page...");
    tft.fillScreen(WHITE);
    tft.setFont(&FreeSansBold24pt7b);  // Use larger font size for the main message

    const char* message1 = "Please wait,";
    const char* message2 = "checking the";
    const char* message3 = "candidate ID";

    // Display message1 centered
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(message1, 0, 0, &x1, &y1, &w, &h);
    int x = (tft.width() - w) / 2;
    int y = (tft.height() - h) / 2 - 40; // Adjust position for better centering
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message1);

    // Display message2 centered below message1
    tft.getTextBounds(message2, 0, 0, &x1, &y1, &w, &h);
    x = (tft.width() - w) / 2;
    y += h + 20;  // Add some space between the two lines
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message2);

    // Display message3 centered below message2
    tft.getTextBounds(message3, 0, 0, &x1, &y1, &w, &h);
    x = (tft.width() - w) / 2;
    y += h + 20;  // Add some space between the two lines
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message3);

    Serial.println("Wait page displayed.");
}

   void handleResponse(String response) {
    Serial.print("Handling response: ");
    Serial.println(response);

    if (response == "SUCCESS") {
        displayMessage("Vote Successful");
        delay(3000); // Display the success message for 3 seconds
        displayMessage("Have a nice day");
        delay(3000); // Display "Have a nice day" for 3 seconds
        tft.fillScreen(WHITE); // Clear the display
        processStarted = false; // Reset the processStarted flag after successful vote
    } else if (response == "INVALID") {
        displayInvalidIdPage();
    }
}

   void displayInvalidIdPage() {
    Serial.println("Displaying invalid ID page...");
    tft.fillScreen(WHITE);
    tft.setFont(&FreeSansBold24pt7b);  // Use larger font size for the main message

    const char* message1 = "Invalid ID";
    const char* message2 = "Returning to";
    const char* message3 = "digit entry";

    // Display message1 centered
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(message1, 0, 0, &x1, &y1, &w, &h);
    int x = (tft.width() - w) / 2;
    int y = (tft.height() - h) / 2 - 40; // Adjust position for better centering
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message1);

    // Display message2 centered below message1
    tft.getTextBounds(message2, 0, 0, &x1, &y1, &w, &h);
    x = (tft.width() - w) / 2;
    y += h + 20;  // Add some space between the two lines
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message2);

    // Display message3 centered below message2
    tft.getTextBounds(message3, 0, 0, &x1, &y1, &w, &h);
    x = (tft.width() - w) / 2;
    y += h + 20;  // Add some space between the two lines
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message3);

    delay(2000); // Wait for 2 seconds before returning to the digit entry page
    digitCount = 0; // Reset digit count
    selectedDigit = 0; // Reset selected digit
    displayFifthPage(); // Go back to the digit entry page
  }

   void storeDigit(int digit) {
    if (digitCount < 3) {
        threeDigitNumber[digitCount] = digit;
        digitCount++;
        Serial.print("Stored digit: ");
        Serial.println(digit);
        Serial.print("Current number: ");
        for (int i = 0; i < digitCount; i++) {
            Serial.print(threeDigitNumber[i]);
        }
        Serial.println();
    }
    }

    void displayConfirmationPage() {
    Serial.println("Displaying confirmation page...");
    tft.fillScreen(WHITE);
    tft.setFont(&FreeSansBold24pt7b);  // Use larger font size for the main message

    const char* message1 = "Confirm your";
    const char* message2 = "selection";

    // Display message1 centered at the top with increased top margin
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(message1, 0, 0, &x1, &y1, &w, &h);
    int x = (tft.width() - w) / 2;
    int y = 50;  // Adjust top margin to move it closer to the middle
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message1);

    // Display message2 centered below message1
    tft.getTextBounds(message2, 0, 0, &x1, &y1, &w, &h);
    x = (tft.width() - w) / 2;
    y += h + 10;  // Add some space between the two lines
    tft.setCursor(x, y);
    tft.setTextColor(BLACK);
    tft.print(message2);

    // Draw a black box for "Yes" response
    tft.fillRect((tft.width() / 4) - 60, (tft.height() / 2) - 30, 120, 60, BLACK);
    tft.drawRect((tft.width() / 4) - 60, (tft.height() / 2) - 30, 120, 60, BLACK);
    tft.setTextColor(WHITE);
    tft.setCursor((tft.width() / 4) - 30, (tft.height() / 2) + 15); // Adjusted position for "Yes"
    tft.print("Yes");

    // Draw a black box for "No" response
    tft.fillRect((3 * tft.width() / 4) - 60, (tft.height() / 2) - 30, 120, 60, BLACK);
    tft.drawRect((3 * tft.width() / 4) - 60, (tft.height() / 2) - 30, 120, 60, BLACK);
    tft.setTextColor(WHITE);
    tft.setCursor((3 * tft.width() / 4) - 30, (tft.height() / 2) + 15); // Adjusted position for "No"
    tft.print("No");

    // Display instructions below the buttons with more margin between lines
    tft.setFont(&FreeSansBold18pt7b);  // Reduced font size to fit instructions on two rows
    tft.setTextColor(BLACK);
    tft.setCursor((tft.width() / 4) - 50, (tft.height() / 2) + 90); // Adjusted position for "Puff X"
    tft.print("Puff X");
    tft.setCursor((tft.width() / 4) - 50, (tft.height() / 2) + 120); // Increased margin for "for Yes"
    tft.print("for Yes");
    tft.setCursor((3 * tft.width() / 4) - 50, (tft.height() / 2) + 90); // Adjusted position for "Puff Y"
    tft.print("Puff Y");
    tft.setCursor((3 * tft.width() / 4) - 50, (tft.height() / 2) + 120); // Increased margin for "for No"
    tft.print("for No");

    confirmationPageDisplayed = true;
    Serial.println("Confirmation page displayed.");
  }

   void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);

    // Initialize TFT display
    tft.begin(tft.readID());
    tft.setRotation(1);
    tft.fillScreen(WHITE);
    tft.setTextColor(BLACK);
    tft.setFont(&FreeSansBold24pt7b);

    // Add a simple message to confirm TFT is working
    tft.setCursor(0, 0);
    tft.print("TFT Initialized");

    // Initialize BMP280
    Serial.println("Initializing BMP280 sensor...");
    if (!bmp280.begin(0x76)) {  // Initialize at address 0x76
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    } else {
        Serial.println("BMP280 sensor initialized at address 0x76");
        bmp280Initialized = true;
        lastPressureBMP280 = bmp280.readPressure();
        Serial.print("Initial BMP280 Pressure: ");
        Serial.print(lastPressureBMP280);
        Serial.println(" Pa");
    }

    // Initialize BMP180
    Serial.println("Initializing BMP180 sensor...");
    if (!bmp180.begin()) {
        Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    } else {
        Serial.println("BMP180 sensor initialized");
        bmp180Initialized = true;
        sensors_event_t event;
        bmp180.getEvent(&event);
        lastPressureBMP180 = event.pressure * 100;  // Convert hPa to Pa
        Serial.print("Initial BMP180 Pressure: ");
        Serial.print(lastPressureBMP180);
        Serial.println(" Pa");
    }

    Serial.println("Setup complete.");
    }

    void loop() {
    if (Serial2.available()) {
        String command = Serial2.readStringUntil('\n');
        command.trim();
        Serial.print("Received command: ");
        Serial.println(command);

        if (command == "Disable_Voting") {
            Serial.println("Disable voting signal received");
            if (!processStarted) {
                processStarted = true; // Set processStarted flag to true
                secondPageDisplayed = false; // Reset flag to allow second page display
                thirdPageDisplayed = false; // Reset flag to allow third page display
                displayCountdown(5);
            }
        }
    }

    // Check BMP280 pressure change to detect puff on confirmation page
    if (bmp280Initialized && confirmationPageDisplayed) {
        float currentPressure = bmp280.readPressure();
        float pressureChange = currentPressure - lastPressureBMP280;

        if (pressureChange >= pressureThresholdConfirmBMP280) {
            Serial.print("Puff detected by BMP280! Pressure change: ");
            Serial.print(pressureChange);
            Serial.println(" Pa");
            confirmationPageDisplayed = false;
            storeDigit(selectedDigit);  // Store the selected digit
            if (digitCount == 1) {
                displayFourthPage("Next page: Second digit");
            } else if (digitCount == 2) {
                displayFourthPage("Next page: Third digit");
            } else if (digitCount == 3) {
                displayWaitPage();  // Display wait page after third digit confirmation
            }
        }

        lastPressureBMP280 = currentPressure;
    }

    // Check BMP180 pressure change to detect puff on confirmation page
    if (bmp180Initialized && confirmationPageDisplayed) {
        sensors_event_t event;
        bmp180.getEvent(&event);
        float currentPressure = event.pressure * 100;  // Convert hPa to Pa
        float pressureChange = currentPressure - lastPressureBMP180;

        if (pressureChange >= pressureThresholdConfirmBMP180) {
            Serial.print("Puff detected by BMP180! Pressure change: ");
            Serial.print(pressureChange);
            Serial.println(" Pa");
            confirmationPageDisplayed = false;
            storeDigit(selectedDigit);  // Store the selected digit
            if (digitCount == 1) {
                displayFourthPage("Next page: Second digit");
            } else if (digitCount == 2) {
                displayFourthPage("Next page: Third digit");
            } else if (digitCount == 3) {
                displayWaitPage();  // Display wait page after third digit confirmation
            }
        }

        lastPressureBMP180 = currentPressure;
    }

    // Update pressure values on fifth page every second
    if (fifthPageDisplayed && millis() - lastUpdateTime >= 1000) {
        updatePressureValues();
        lastUpdateTime = millis();
    }

    // Check BMP280 pressure change to detect puff on fifth page
    if (bmp280Initialized && fifthPageDisplayed) {
        float currentPressure = bmp280.readPressure();
        float pressureChange = currentPressure - lastPressureBMP280;

        if (pressureChange >= pressureThresholdDigitBMP280) {
            Serial.print("Puff detected by BMP280! Pressure change: ");
            Serial.print(pressureChange);
            Serial.println(" Pa");
            updateSelectedDigit();
        }

        lastPressureBMP280 = currentPressure;
    }

    // Check BMP180 pressure change to detect puff on fifth page
    if (bmp180Initialized && fifthPageDisplayed) {
        sensors_event_t event;
        bmp180.getEvent(&event);
        float currentPressure = event.pressure * 100;  // Convert hPa to Pa
        float pressureChange = currentPressure - lastPressureBMP180;

        if (pressureChange >= pressureThresholdDigitBMP180) {
            Serial.print("Puff detected by BMP180! Pressure change: ");
            Serial.print(pressureChange);
            Serial.println(" Pa");
            fifthPageDisplayed = false;  // Prevent multiple confirmation page displays
            displayConfirmationPage();  // Go to confirmation page
        }

        lastPressureBMP180 = currentPressure;
    }

    // Check for responses from ESP32
    if (Serial2.available()) {
        String response = Serial2.readStringUntil('\n');
        response.trim();
        Serial.print("Response from ESP32: ");
        Serial.println(response);
        handleResponse(response);
    }
//sketch2.ino 

    #include <SPI.h>
    #include <Adafruit_GFX.h>
    #include <MCUFRIEND_kbv.h>
    #include <Fonts/FreeSansBold24pt7b.h>
    
    #define LCD_CS A3
    #define LCD_CD A2
    #define LCD_WR A1
    #define LCD_RD A0
    #define LCD_RESET A4
    
    MCUFRIEND_kbv tft;
    
    // Color definitions
    #define BLACK 0x0000
    #define WHITE 0xFFFF
    #define GREEN 0x07E0 // The green value that worked in your test code
    #define RED 0xF800  // Using a standard red value
    
    void setup() {
      Serial.begin(115200); // For serial monitor
      Serial2.begin(115200); // For communication with ESP32 (RX3, TX3)
    
      // Initialize the display
      uint16_t ID = tft.readID();
      tft.begin(ID);
      tft.setRotation(1); // Adjust rotation if needed
      tft.fillScreen(WHITE); // Set background to white
      tft.setFont(&FreeSansBold24pt7b); // Set the font to FreeSansBold24pt7b
    }
    
    void loop() {
      if (Serial2.available()) {
        String command = Serial2.readStringUntil('\n');
        command.trim(); // Remove any leading or trailing whitespace
        Serial.print("Received command: '");
        Serial.print(command);
        Serial.println("'");
    
        if (command == "Clear display" || command == "Clear Face ID") {
          tft.fillScreen(WHITE);
        } else {
          tft.fillScreen(WHITE);
    
          uint16_t textColor = BLACK;
          if (command.equals("Face ID Matched")) {
            Serial.println("Setting text color to GREEN");
            textColor = GREEN;
          } else if (command.equals("Face ID Not Matched")) {
            Serial.println("Setting text color to RED");
            textColor = RED;
          } else {
            Serial.println("Setting text color to BLACK");
          }
    
          int16_t x1, y1;
          uint16_t w, h;
          tft.setTextWrap(false);
          tft.getTextBounds(command, 0, 0, &x1, &y1, &w, &h);
          int16_t x = (tft.width() - w) / 2;
          int16_t y = (tft.height() - h) / 2;
    
          tft.setCursor(x, y);
          tft.setTextColor(textColor, WHITE);
          tft.print(command);
        }
      }
    }
    
    