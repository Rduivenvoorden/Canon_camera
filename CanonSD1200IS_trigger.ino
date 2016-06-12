/*****************************************************************************/
/*  Canon SD1200IS triggering                                                */
/*  Reads USB serial port for desired pulse width (1 byte) in milliseconds   */
/*  (0 - 255), and applies a HIGH voltage on the LED pin 13, for the CHDK to */
/*  pick up.                                                                 */
/*                                                                           */
/*  Rikky Duivenvoorden -- Jun 12, 2016                                      */
/*  UTAT UAV                                                                 */
/*                                                                           */
/*****************************************************************************/

const int CanonPin = 13;      // the pin that the LED is attached to

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  // initialize the CanonPin as an output:
  pinMode(CanonPin, OUTPUT);
}

void loop() {
  byte recv;
  int pulse_width;

  // check if data has been sent from the computer:
  if (Serial.available()) {
    // read the most recent byte (which will be from 0 to 255):
    recv = Serial.read();
    // convert byte into integer for delay function
    pulse_width = (int) recv;
    
    // set CanonPin to HIGH 
    digitalWrite(CanonPin, HIGH);

    // keep pin on HIGH for the desired duration in millisec
    delay(pulse_width);

    // set CanonPin to LOW
    digitalWrite(CanonPin, LOW);
  }

}
