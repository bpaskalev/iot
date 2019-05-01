#include <PubSubClient.h>
#include <DHTesp.h>
#include <EEPROM.h>
#include <WiFi.h>

//todo init IR transmitter

const char* ssid = "bermuda";
const char* password = "c0ffeec0ff";
const char* mqtt_server = "test.mosquitto.org";
WiFiClient espClient;
PubSubClient client(espClient);
const char led = 12;
long lastMsg = 0;
char msg[20];
long lastChk=0;
int DHTpin=4;
DHTesp dht;
float tempToSet=23; //default value

#define TEMP_TOPIC_CURRENT    "lilian/room1/temp_curr"
#define TEMP_TOPIC_SET        "lilian/room1/temp_set"
#define HUM_TOPIC_CURRENT     "lilian/room1/hum_curr"
#define AC_COMMAND            "lilian/room1/command"
#define POWERON_COMMAND       "poweron"
#define POWEROFF_COMMAND      "poweroff"
#define SETTEMP_COMMAND       "settemp"



int executeCommand(String command){
  Serial.print("Executing command: ");
  Serial.println(command);

  if(command==POWERON_COMMAND){
    Serial.println(command);
  }else if(command==POWEROFF_COMMAND){
    Serial.println(command);
  }else if(command==POWERON_COMMAND){
    Serial.println(command);
  }else if(command==SETTEMP_COMMAND){
    Serial.println(command);
  }else{
    Serial.println("unknown command");
  }
}


void receivedCallback(char* topic, byte* payload, unsigned int length) {
  String strTopic(topic);
  String strPayload="";
  int i;
  for(i=0;i<length;i++){
    strPayload.concat((char)payload[i]); 
  }
  strPayload[length]=0;
  Serial.print("Message received: ");
  Serial.println(topic);
  if(strTopic==AC_COMMAND){
    Serial.println("Executing command  on the ac ");
    Serial.println(strPayload);
    executeCommand(strPayload);
  }else if(strTopic==TEMP_TOPIC_SET){
    Serial.println("Setting value of tempToSet to"+strPayload);
    tempToSet=strPayload.toFloat();
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
void wificonnect(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println();

  /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);
  dht.setup(DHTpin, DHTesp::DHT22);
  Serial.println("temperature and humidity sensor initialized");

}

void loop() {
  
  // put your main code here, to run repeatedly:
  if (WiFi.status() != WL_CONNECTED){
    wificonnect();
  }
  if (!client.connected()) {
    mqttconnect();
  }
  client.loop(); // check for messages periodically
  long now=millis();
  float temp=0;
  float hum=0;
  TempAndHumidity tnh;
  if (now-lastChk>5000){
    lastChk=now;
    temp=0;//read temp todo
    tnh=dht.getTempAndHumidity();
    temp=tnh.temperature;
    hum=tnh.humidity;
    char charTemp[50]="";
    sprintf(charTemp,"%2.2f",temp);
    Serial.println("Temperature is "+String(charTemp));
    client.publish((char *)TEMP_TOPIC_CURRENT,(char *)charTemp);
    char charHum[50]="";
    sprintf(charHum,"%3.2f",hum);
    Serial.println("Humidity is "+String(charHum));
    client.publish((char *)HUM_TOPIC_CURRENT,(char *)charHum);
  }
  delay(1000);
}
