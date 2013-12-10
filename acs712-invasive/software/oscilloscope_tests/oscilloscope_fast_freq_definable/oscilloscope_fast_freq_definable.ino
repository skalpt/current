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
#define pollFreq 30 // This can be as low as 30us, but this will decrease total timespan of reading

int sensorValue[pollTotal];
int startTime;
int endTime;
long pollTime = -pollFreq;
int pollCount = 0;

void setup() {
  
  #if FASTADC
    // set prescale to 16
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    cbi(ADCSRA,ADPS0) ;
  #endif

  Serial.begin(115200);
  startTime = millis();
}

void loop() {
  if (pollCount < pollTotal) {
    if (micros() - pollTime >= pollFreq) {
      pollTime = micros();
      sensorValue[pollCount] = analogRead(A0);
      pollCount++;
      if (pollCount == pollTotal) {
        endTime = millis();
        for (int i=0; i<pollTotal; i++) {
          Serial.println(sensorValue[i]);
        }
        Serial.println(endTime-startTime);
      }
    }
  }
}
