/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define USERMQTT "2sa34dd5" // Put your Username
#define PASSMQTT "2sa34dd5" // Put your Password
#define MQTT_CLIENT_NAME "" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 

// Replace the next variables with your SSID/Password combination
const char* ssid = "2.4";
const char* password = "296JBD82kK";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

//send MQTT function
void sendMQTT(String MQTTMessage, String MQTTtopic){
  char MQTTpayload[500];
  char arraytopic[300];

  //Making the syntax for json as per Ameen's equest:
  // MQTTMessage= String("{\"") + String(ID) + String("\":[{\"values\":{\"") +String(MQTTMessage) +String("\": ") + String(state) + String("}}]}");
  /*no need as will do this befoe hand*/

  // converting Strings to char array
      // sprintf(MQTTpayload, "%s", MQTTMessage); // This doesnt work as we have String class
      // sprintf(arraytopic, "%s", MQTTtopic);
    MQTTMessage.toCharArray(MQTTpayload, 500);
    MQTTtopic.toCharArray(arraytopic, 300);

  if(client.publish(arraytopic, MQTTpayload)){
    Serial.print("\nSENDMQTT: MQTT Topic: ");
    Serial.println(arraytopic);
    Serial.print("          MQTT Message: ");
    Serial.println(MQTTpayload);
  }
  else{
    Serial.println("INFO: MQTT failed to send");
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // sendMQTT(String("Attempting MQTT connection..."),debugTopic);
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, USERMQTT, PASSMQTT)) {
      Serial.println("Connected");
      // sendMQTT(String("Connected"),debugTopic);
      // client.subscribe(ID_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 0.5 seconds");
      // Wait 2 seconds before retrying
      delay(500);
    }
  }
}


void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(LED_BUILTIN, OUTPUT);
}

int count=0;

void loop() {
  char tempString[]= "testing Temp";
//   if (!client.connected()) {
//     reconnect();
//     client.publish("esp32/temp", tempString);
//   }
  client.loop();

  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    
//Temperature measurement
    // Convert the value to a char array
    // const char tempString[]= "testing Temp";
    Serial.print("Temperature: ");
    sprintf(tempString, "%d", count);
    Serial.println(tempString);


    while(!client.connected()){
        reconnect();
    } 
    // client.publish("esp32/temp", tempString);
    // sendMQTT(tempString, "esp32/temp");

//Humidity measurement
    char humString[]= "Humidity Testing";
    Serial.print("Humidity: ");
    sprintf(humString, "%d", count);
    Serial.println(humString);

    while (!client.connected()) {
    reconnect();
    }
    // client.publish("esp32/humid", humString);
    sendMQTT(humString, "esp32/humid");

    count++;
  }
}