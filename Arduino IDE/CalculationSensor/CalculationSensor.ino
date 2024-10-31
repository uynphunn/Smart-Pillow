#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <MQ135.h>
#include "pitches.h"

#define trig1 25
#define echo1 35
#define trig2 33
#define echo2 34
#define trig3 32
#define echo3 39
#define A_MQ  A0
#define A_GP  A19
#define D_GP  27
#define BUZZER_PIN 19

Adafruit_SHT31 sht31 = Adafruit_SHT31();
MQ135 MQ135_sensor = MQ135(A_MQ);

float temp, hum, dis1, dis2, dis3, dust, ppm, correctedPPM;
int melody[] = {
  NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4, 
  NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4, 
  NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
  NOTE_A4
};

int durations[] = {
  8, 8, 4, 4,
  8, 8, 4, 4,
  8, 8, 4, 4,
  2
};

void setup(){
  Serial.begin(9600);
  if (!sht31.begin(0x44)) {  // Địa chỉ I2C của cảm biến (0x44 hoặc 0x45)
    Serial.println("Không tìm thấy SHT31");
    while (1) delay(1);
  }
  pinMode(D_GP, OUTPUT);
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT);
  pinMode(echo3, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void SHT31() {
  temp = sht31.readTemperature();
  hum = sht31.readHumidity();
  delay(100);
}

void MQ(float temperature, float humidity){
  ppm = MQ135_sensor.getPPM();
  correctedPPM = MQ135_sensor.getCorrectedPPM(temperature, humidity);
  delay(100);
} 
 
float GP2Y (int ledPower, int measurePin){
  int samplingTime = 280;
  int deltaTime = 40;
  int sleepTime = 9680;
  float voMeasured = 0;
  float calcVoltage = 0;
  float dustDensity = 0;
  digitalWrite(ledPower,LOW); // Bật IR LED
  delayMicroseconds(samplingTime);  //Delay 0.28ms
  voMeasured = analogRead(measurePin); // Đọc giá trị ADC V0
  delayMicroseconds(deltaTime); //Delay 0.04ms
  digitalWrite(ledPower,HIGH); // Tắt LED
  delayMicroseconds(sleepTime); //Delay 9.68ms
  // Tính điện áp từ giá trị ADC
  calcVoltage = voMeasured * (5.0 / 4096); //Điệp áp Vcc của cảm biến (5.0 hoặc 3.3)
  dustDensity = 0.17 * calcVoltage - 0.1;
  delay(100);
  return dustDensity;
}

float HCSR04 (int trig, int echo) {
  unsigned long duration;
  float distance;
  digitalWrite(trig, LOW);
  delay(2);
  digitalWrite(trig, HIGH);
  delay(5);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration/2./29.412;
  return distance;
}

void loop() {
  // put your main code here, to run repeatedly:
  SHT31();
  dis1 = HCSR04(trig1, echo1);
  dis2 = HCSR04(trig2, echo2);
  dis3 = HCSR04(trig3, echo3);
  dust = GP2Y(D_GP, A_GP);
  MQ(temp, hum);
    int size = sizeof(durations) / sizeof(int);

  // for (int note = 0; note < size; note++) {
  //   //to calculate the note duration, take one second divided by the note type.
  //   //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
  //   int duration = 1000 / durations[note];
  //   tone(BUZZER_PIN, melody[note], duration);

  //   //to distinguish the notes, set a minimum time between them.
  //   //the note's duration + 30% seems to work well:
  //   int pauseBetweenNotes = duration * 1.30;
  //   delay(pauseBetweenNotes);
    
  //   //stop the tone playing:
  //   noTone(BUZZER_PIN);
  // }

  Serial.print("Temp: ");
  Serial.print(temp, 2);
  Serial.print("*C; Hum: ");
  Serial.print(hum, 2);
  Serial.print("%; Dis:");
  Serial.print(dis1, 2);
  Serial.print("cm-");
  Serial.print(dis2, 2);
  Serial.print("cm-");
  Serial.print(dis3, 2);
  Serial.print("cm; PPM: ");
  Serial.print(ppm, 2);
  Serial.print("ppm; Dust: ");
  Serial.print(dust, 2);
  Serial.println("mg/m^3");
  delay(100);
}