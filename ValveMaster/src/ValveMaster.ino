/*
 * Project ValveMaster
 * Description:
 * Author:
 * Date:
 */
int valve = D0;
unsigned long lastOn = millis();
bool isOn = false;
const unsigned long kMaxValveDuration = 5 * 60 * 1000;

// setup() runs once, when the device is first turned on.


void setup() {
  // Put initialization like pinMode and begin functions here.
  Particle.subscribe("waterLevel", myHandler, MY_DEVICES);
  pinMode(valve, OUTPUT);
}

void myHandler(const char *event, const char *data)
{
  if (strcmp(data,"high") == 0) {
      Particle.publish("ValveStatus", "Off", 60, PRIVATE);
      digitalWrite(valve, LOW);
      isOn = false;
    } else {
      Particle.publish("ValveStatus", "On", 60, PRIVATE);
      digitalWrite(valve, HIGH);
      isOn = true;
      lastOn = millis();
    }
}

void loop() {
  if (isOn) {
    //check for runaway water
    if ((millis() - lastOn) > kMaxValveDuration) {
      //short circuit this
      Particle.publish("ValveStatus", "OnTooLong", 60, PRIVATE);
      isOn = false;
      digitalWrite(valve, LOW);
    }
  }
  delay(1000);
}
