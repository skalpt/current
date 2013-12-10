#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define pollTotal 800 // Can't go much higher than 800, as ATMEGA328 only has 2kb of SRAM (2048 bytes, or 1024 integer values)
#define pollFreq 30 // This can be as low as 30us, but this will decrease total timespan of reading
#define smoothFactor 20 // If this is > 30 then sensorValue should be stored as long

int sensorValue[pollTotal];
long startTime;
long endTime;
long pollTime = -pollFreq;
int pollCount = 0;
int valueCount = 0;
long loopTime = 1L * pollTotal * pollFreq * smoothFactor / 1000;

void setup() {
  
  #if FASTADC
    // set prescale to 16
    sbi(ADCSRA,ADPS2) ;
    cbi(ADCSRA,ADPS1) ;
    cbi(ADCSRA,ADPS0) ;
  #endif

  Serial.begin(115200);
  
  // Add an initialise routine here to define average reading @ zero load
  
  startTime = millis();
}

void loop() {
  if (micros() - pollTime >= pollFreq) {
    pollTime = micros();
    // Maybe we should add a loop here to take [smoothFactor] x readings all at once
    // In that case, min(pollFreq) would be 30 x smoothFactor
    // Only problem is values might not be as evenly spread for RMS calc
    sensorValue[valueCount] += analogRead(A0);
    pollCount++;
    if (pollCount == smoothFactor) {
      valueCount++;
      pollCount = 0;
    }
    if (valueCount == pollTotal) {
      endTime = millis();

      // Change values to variance
      long zeroLine = 0;
      for (int i = 0; i < pollTotal; i++) {
        zeroLine += sensorValue[i];
      }
      zeroLine /= pollTotal;
      for (int i = 0; i < pollTotal; i++) {
        sensorValue[i] -= zeroLine;
      }

      // Get the sum of the squared values
      long sumSq = 0;
      for (int i = 0; i < pollTotal; i++) {
        sumSq += sq(sensorValue[i] + 0L);
      }
      
      // Find the RMS value
      float RMS;
      RMS = sqrt(sumSq/pollTotal);
      
      // Convert to mA
      RMS = RMS/smoothFactor/1023*5/0.185*1000;
      
      Serial.print(RMS);
      Serial.print("mA  /  Sample time: ");
      Serial.print(loopTime);
      Serial.print("ms (expected), ");
      Serial.print(endTime - startTime);
      Serial.print("ms (actual)  /  Conversion time: ");
      Serial.print(millis() - endTime);
      Serial.println("ms");

      memset(sensorValue, 0, sizeof(sensorValue));
      startTime = millis();
      valueCount = 0;
    }
  }
}
