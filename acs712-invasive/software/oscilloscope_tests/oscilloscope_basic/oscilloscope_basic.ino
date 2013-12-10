int sensorValue[500];
int startTime;
int endTime;

void setup() {
  Serial.begin(115200);
  startTime = millis();
  for (int i = 0; i < 500; i++) {
    sensorValue[i] = analogRead(A0);
  }
  endTime = millis();
  for (int i = 0; i < 500; i++) {
    Serial.println(sensorValue[i]);
  }
  Serial.println(endTime - startTime);
}

void loop() {
  
}
