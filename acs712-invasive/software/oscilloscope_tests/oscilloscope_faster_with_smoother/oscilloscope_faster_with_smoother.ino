#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// We expect this to take about 160 millis (88 on lowest pollFreq)
#define pollTotal 800 // Can't go much higher than this, as ATMEGA328 only has 2kb of SRAM (2048 bytes, or 1024 integer values)
/* Two ideas for increasing reads:
   1. Change int to byte and lower the range to 256 values
   2. Average out 20 values, store, repeat. Smooths ripple, gives us 20 times the data!
*/
#define smoothFactor 20

int sensorValue[pollTotal];
int startTime;
int endTime;

void setup() {
  
  #if FASTADC
    // set prescale to 16
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    cbi(ADCSRA,ADPS0) ;
  #endif

  int pollIterations = pollTotal * smoothFactor;

  Serial.begin(115200);
  startTime = millis();
  for (int i = 0; i < pollTotal; i++) {
    for (int j = 0; j < smoothFactor; j++) {
      sensorValue[i] += analogRead(A0);
    }
  }
  endTime = millis();
  for (int i = 0; i < pollTotal; i++) {
    Serial.println(sensorValue[i]);
  }
  Serial.println(endTime - startTime);
}

void loop() {

}
