#include <Bridge.h>
#include <BridgeClient.h>
#include <MQTT.h>

BridgeClient net;
MQTTClient client;

//heart rate
unsigned long lastMillis = 0;
long currentInterrupt = millis();
long lastInterrupt = millis();
long currentInterval = 0;
int prevCount = 0;
int counter = 0;
long pulsetimer = millis();
int currentRate = 0;

unsigned long temp[10];
int index = 0;

//gsr
const int GSR = A0;
int gsrSensorValue = 0;
int gsrAverage = 0;
long gsrtimer = millis();



void connect() {
  while (!client.connect("arduino-yun-vizinviz", "ecf929fe", "f65bde19d8e46d67")) {

  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Bridge.begin();

  client.begin("broker.shiftr.io", net);
  connect();

  arrayInit();
  attachInterrupt(digitalPinToInterrupt(2), interrupt, RISING);//set interrupt 0,digital port 3
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  //calc new rate
  if (prevCount != counter) {
    prevCount = counter;

    float sum = 0;
    for (int i = 0; i < 10; i++) {
      sum += temp[i];
    }
    float avg = sum / 10.0;
    currentRate = round(60000 / avg);
    //client.publish("/hello", "world");
    //client.publish("/velo", "moto");
  }

  if (millis() - pulsetimer > 1000) {
    pulsetimer = millis();
    String topic = "/pulse";
    String payload = "";
    //    payload += rate;
    //    //digitalWrite(LED_BUILTIN, HIGH);
    //    client.publish(topic, payload);

    //publish pulse and interval
    topic = "/pulse/interval";
    payload = "";
    payload += currentRate;
    payload += ",";
    payload += currentInterval;
    client.publish(topic, payload);

    //publish interval only
    //    topic = "/interval";
    //    payload = "";
    //    payload += currentInterval;
    //    client.publish(topic, payload);


    //blink
    digitalWrite(LED_BUILTIN, HIGH);
    delay(10);
    digitalWrite(LED_BUILTIN, LOW);

  }


  //send gsr every two secods
  if (millis() - gsrtimer > 1000) {
    gsrtimer = millis();
    //gsr specific code
    long gsrSum = 0;
    for (int i = 0; i < 10; i++)    //Average the 10 measurements to remove the glitch
    {
      gsrSensorValue = analogRead(GSR);
      gsrSum += gsrSensorValue;
      delay(5);
    }
    gsrAverage = gsrSum / 10;
    String gsrTopic = "/gsr";
    String gsrPayload = "";
    gsrPayload += gsrAverage;
    client.publish(gsrTopic, gsrPayload);

    //blink
    digitalWrite(LED_BUILTIN, HIGH);
    delay(10);
    digitalWrite(LED_BUILTIN, LOW);
  }


}

/*Function: Interrupt service routine.Get the sigal from the external interrupt*/
void interrupt()
{

  lastInterrupt = currentInterrupt;
  currentInterrupt = millis();
  long diff = currentInterrupt - lastInterrupt;
  temp[index] = diff;
  currentInterval = diff;
  index++;
  if (index >= 10) {
    index = 0;
  }
  counter++;
}

/*Function: Initialization for the array(temp)*/
void arrayInit()
{
  for (int i = 0; i < 10; i++)
  {
    temp[i] = 0;
  }

}
