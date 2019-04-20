#include <EEPROM.h>
#include <WiFi.h>
#include <PubSubClient.h>


//todo init termomether
//todo init IR transmitter

const char* ssid = "bermuda";
const char* password = "mypasswd";
const char* mqtt_server = "192.168.88.103";
WiFiClient espClient;
PubSubClient client(espClient);
const char led = 12;
long lastMsg = 0;
char msg[20];

#define TEMP_TOPIC_CURRENT    "myschool/room1/temp_curr"
#define LED_TOPIC     "smarthome/room1/led" /* 1=on, 0=off */
#define TEMP_TOPIC_SET        "myschool/room1/temp_set"
#define AC_COMMAND            "myschool/room1/command"
#define POWERON_COMMAND       "poweron"
#define POWEROFF_COMMAND      "poweroff"




int executeCommand(byte* command){
  Serial.print("Executing command: ");
  Serial.println((char*)command);
  switch((char*)command){
    case POWERON_COMMAND:
      //send poweron with temp todo
      break;
    case POWEROFF_COMMAND:
      //send poweroff todo
      break;
    default:
      Serial.print("Unknown command: ");
      Serial.println((char*)command);
      break;
  }
}


void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
  String payloadStr =new String(payload,0);
  Serial.print("payload: ");
  Serial.println(payloadStr);
  switch(topic){
    case TEMP_TOPIC_SET:
      // todo set the temp variable
      break;
    case AC_COMMAND:
      //todo execute command
      executeCommand(payloadStr);
      break;
    default:
      Serial.print("unknown topic: ");
      Serial.println(topic);
      break;
  }

  
  /* we got '1' -> on */
  if ((char)payload[0] == '1') {
    digitalWrite(led, HIGH); 
  } else {
    /* we got '0' -> on */
    digitalWrite(led, LOW);
  }

}


void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "Lilian";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe(TEMP_TOPIC_SET);
      client.subscribe(AC_COMMAND);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}



void setup() {
  // put your setup code here, to run once:
  //todo setup WIFI
  //todo connecto to mqtt
  Serial.begin(115200);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  /* set led as output to control led on-off */
  pinMode(led, OUTPUT);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);
  //todo init temp sensor

}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    mqttconnect();
  }
  client.loop(); // check for messages periodically
  long now=millis();
  if (now-lastChk>5000){
    lastChk=now;
    temp=0;//read temp todo
    sprintf(msg,20,"%lf",temp);
    client.publish(TEMP_TOPPIC_CURRENT,msg);
    
    
  }
  delay(1000);
}
