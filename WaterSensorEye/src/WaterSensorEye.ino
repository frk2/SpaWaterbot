/*
 * Project WaterSensorEye
 * Description:
 * Author:
 * Date:
 */

 #include "SparkFunMAX17043/SparkFunMAX17043.h"
 extern char* itoa(int a, char* buffer, unsigned char radix);

int sensor = A0;
int trigger = D0;
double voltage = 0;
double soc = 0;
int waterLevel = 1;
unsigned long wakeupTime = 0;
char meow[50];
bool debug_mode = false;
double lowmark = 260;
double highmark = 230;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Particle.variable("voltage", voltage);
  Particle.variable("soc", soc);
  Particle.variable("waterLevel", waterLevel);
  pinMode(trigger, OUTPUT);
  lipo.begin();
  lipo.quickStart();
  Serial.begin(9600);
  waterLevel = 1;
}

// loop() runs over and over again, as quickly as it can execute.
void publish(const char *eventName, const char *data)
{
  if (!debug_mode) {
    Particle.publish(eventName, data, 60, PRIVATE);
  } else {
    Serial.println("---");
    Serial.println(eventName);
    Serial.println(data);
    Serial.println("---");
  }

}
void loop() {
  if (WiFi.ready() && Particle.connected()) {
    if (wakeupTime == 0) {
      wakeupTime = millis();
    }
    /*Serial.println("Have no fear, WiFi is here");*/

    digitalWrite(trigger, HIGH);
    delay(200);
    double water = analogRead(sensor);
    digitalWrite(trigger, LOW);
    if (debug_mode) {
      Serial.println(water);
      delay(1000);
    }

    publish("analogReading",itoa(water, meow, 10));
    if (water > 2000) {
      publish("SensorError","1");
      publish("waterLevel","high");
    } else if (water > lowmark) {
      publish("waterLevel","low");
      waterLevel = 0;
    } else if (water < highmark) {
      publish("waterLevel","high");
      waterLevel = 1;
    } else {
      //keep water level to whatever it was!
      if (debug_mode) {
        Serial.println("maintaining last water command");
      }
      if (waterLevel) {
        publish("waterLevel", "high");
      } else {
        publish("waterLevel", "low");
      }
    }
    voltage = lipo.getVoltage();
    soc = lipo.getSOC();
    if (debug_mode) {
      delay(1000);
    } else {
      delay(10000);
    }
    if (waterLevel == 1) {
      /*Serial.println("Water level is high, worry not - sleep! ");*/
      int sleep_hours = 2;
      if (soc < 40) {
        sleep_hours = 12;
      }
      if (!debug_mode) {
        System.sleep(SLEEP_MODE_DEEP, 60 * 60 * sleep_hours);
      } else {
        Serial.println("Would have deep slept");
      }
    } else {
      /*Serial.println("Low level, control that valve");*/
      if (debug_mode){
        Serial.println("Valve wait would happen");
      } else {
        delay(30000);
      }
    }

  } else {
    wakeupTime = 0;
    waterLevel = 1;
    delay(100); // just loop some till you get connected.
  }
  // The core of your code will likely live here.

}
