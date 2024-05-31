#include <Arduino.h>
#include <unity.h>
/*
  ASCII table

  Prints out byte values in all possible formats:
  - as raw binary values
  - as ASCII-encoded decimal, hex, octal, and binary values

  For more on ASCII, see http://www.asciitable.com and http://en.wikipedia.org/wiki/ASCII

  The circuit: No external hardware needed.

  created 2006
  by Nicholas Zambetti <http://www.zambetti.com>
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/ASCIITable
*/

void testcom(){
  // ASCII printable characters
  // from 33 to 126
  // (note that 127 is the DEL, and is often skipped)
  for (int thisByte = 33; thisByte < 127; thisByte++) {
    // you can also write ASCII characters in single quotes.
    // for example. '!' is the same as 33, so you could also use this:
    // Serial.write(thisByte);
    Serial.write(thisByte);
    //test byte
    TEST_ASSERT_EQUAL(thisByte, Serial.read());
  }
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // prints title with ending line break
  Serial.println("ASCII Table ~ Character Map");
  UNITY_BEGIN();
  RUN_TEST(testcom);
}
void loop() {

}
void tearDown(void) {
    // Cette fonction est appelée après chaque test
    // Vous pouvez y libérer les ressources si nécessaire
}

