//HID Example with external USB Payloads
// Requires: ATmega32u4 microcontroller (e.g., Arduino Micro/Leonardo), SD module, and HID support

#include <Keyboard.h>    // Library to emulate keyboard input
#include <SPI.h>         // SPI communication for SD card
#include <SD.h>          // SD card file handling

// Custom key definitions for special keyboard keys
#define KEY_CONTEXT 0xED
#define KEY_PAUSE_BREAK 0xD0
#define KEY_NUM_LOCK 0xDB
#define KEY_SNAPSHOT 0xCE
#define KEY_SCROLL_LOCK 0xCF
#define KEY_SPACEBAR 0xB4

// Pin connected to the SD card reader
const int sdPin = 4;

// Configuration and control variables
String command, parameter, executionMode, language = "en";
String scriptName, lastCommand, lastParameter;
char paramChar, lastParamChar, tempChar, stopChar;
int delayMs = 0;
int statusLED = 8;  // LED indicator pin

// File management variables
File rootDir, configFile;
bool errorStatus = false;

// Language mapping arrays (used to map characters for different keyboard layouts)
byte langMapInput[64], langMapMod[64], langMapOutput[64], activeModifier;

void setup() {
  // Initialize LED and keyboard communication
  pinMode(statusLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Keyboard.begin();
  Serial.begin(9600);

  // Initialize SD card
  if (!SD.begin(sdPin)) {
    Serial.println("SD initialization failed");
    return;
  }

  // Open root directory of SD
  rootDir = SD.open("/");

  // Load configuration files from SD
  executionMode = loadFile("mode.cfg");
  scriptName = loadFile("exec.cfg");
  language = loadFile("lang.cfg");

  // Load language layout if not English
  if (language != "en") {
    mapLanguage("lang/" + language + ".bin");
  }

  // Execute behavior based on mode
  if (executionMode == "c") {
    runPayload(scriptName);  // Continuous mode
  } else if (executionMode == "a") {
    runPayload(scriptName);  // Auto-disarm after one execution
    executionMode = "m";
    saveFile("mode.cfg", executionMode);
  }

  // Launch interactive configuration
  handleUserConfig();
  Keyboard.end();
}

// Function to show and update script, language, and mode configuration
void handleUserConfig() {
  digitalWrite(statusLED, HIGH);

  while (!Serial) {
    if (errorStatus) {
      digitalWrite(statusLED, HIGH);
      delay(200);
      digitalWrite(statusLED, LOW);
      delay(200);
    }
  }

  Serial.println("Available scripts:");
  listDirectory(rootDir, 0);
  rootDir.close();

  Serial.println("\nChoose mode: (m = manual, a = auto, c = continuous)");
  saveFile("mode.cfg", getUserInput());

  Serial.println("\nSelect language:");
  saveFile("lang.cfg", getUserInput());

  Serial.println("\nEnter script name:");
  saveFile("exec.cfg", getUserInput());

  Serial.println("\nConfiguration complete. Ready for deployment.");
}

// Recursive function to print directory structure of SD card
void listDirectory(File dir, int depth) {
  while (true) {
    File file = dir.openNextFile();
    if (!file) break;

    for (uint8_t i = 0; i < depth; i++) Serial.print('\t');
    Serial.print(file.name());

    if (file.isDirectory()) {
      Serial.println("/");
      listDirectory(file, depth + 1);
    } else {
      Serial.print("\t\t");
      Serial.println(file.size(), DEC);
    }
    file.close();
  }
}

// Function to collect user input from serial monitor
String getUserInput() {
  while (!Serial.available());
  return Serial.readStringUntil('\n');
}

// Load key mapping for non-English keyboard layouts
void mapLanguage(String fileName) {
  configFile = SD.open(fileName);
  int index = 0;
  if (configFile) {
    while (configFile.available()) {
      langMapInput[index] = configFile.read();
      langMapMod[index] = configFile.read();
      langMapOutput[index] = configFile.read();
      index++;
    }
    configFile.close();
  } else {
    Serial.println("Failed to load language map: " + fileName);
  }
}

// Match a character in the language map and return remapped byte with optional modifier
byte remapChar(byte original) {
  for (int i = 0; i <= 64; i++) {
    if (langMapInput[i]) {
      if (langMapInput[i] == original) {
        activeModifier = langMapMod[i];
        return langMapOutput[i];
      }
    } else {
      return original;
    }
  }
  return original;
}

// Print a character via emulated keyboard, optionally using modifier
void printChar(byte value) {
  if (activeModifier) {
    Keyboard.press(activeModifier);
    delay(5);
    Keyboard.write(value);
    delay(5);
    Keyboard.release(activeModifier);
    activeModifier = 0;
  } else {
    Keyboard.write(value);
    delay(5);
  }
}

// Load content of a text-based config file
String loadFile(String path) {
  String result = "";
  configFile = SD.open(path);
  if (configFile) {
    while (configFile.available()) {
      result += char(configFile.read());
    }
    configFile.close();
    return result;
  } else {
    Serial.println("Error reading " + path);
    return "";
  }
}

// Save content to a config file (overwrite if exists)
void saveFile(String path, String content) {
  SD.remove(path);
  configFile = SD.open(path, FILE_WRITE);
  if (configFile) {
    configFile.print(content);
    configFile.close();
  }
}
