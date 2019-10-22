#include <Bridge.h>
#include <BridgeClient.h>
#include <MQTT.h>

BridgeClient net;
MQTTClient client;

unsigned long lastMillis = 0;
long currentInterrupt = millis();
long lastInterrupt = millis();
int prevCount = 0;
int counter = 0;

unsigned long temp[5];
int index = 0;


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

  // publish a message roughly every second.
  if (prevCount!=counter) {
    prevCount = counter;

    float sum = 0;
    for(int i=0; i<5; i++){
      sum+=temp[i];
    }
    float avg = sum/5.0;
    float rate = 60000/avg;
    //client.publish("/hello", "world");
    //client.publish("/velo", "moto");
    String topic = "/rate";
    String payload = "";
    payload += rate;
    digitalWrite(LED_BUILTIN, HIGH);
    client.publish(topic, payload);

    delay(100);
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
  index++;
  if(index>=5){
    index = 0;
  }
  counter++;
}

/*Function: Initialization for the array(temp)*/
void arrayInit()
{
  for (int i = 0; i < 5; i++)
  {
    temp[i] = 0;
  }

}
