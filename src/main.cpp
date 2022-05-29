#include <WiFi.h>                       // either this or the pubsub one has the serial monitor libary
#include <PubSubClient.h>
#include <Wire.h>                       //somehow doesnt have the serial monitor library
#include "MAX30100_PulseOximeter.h"     //doesnt have the serial monitor library

#define REPORTING_PERIOD_MS     1000


#define USERMQTT "2sa34dd5" // Put your Username
#define PASSMQTT "2sa34dd5" // Put your Password
#define MQTT_CLIENT_NAME "" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
#define ArrayMQTT
// #define NormMQTT
// #define TenMsg

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

// Replace the next variables with your SSID/Password combination
const char* ssid = "2.4";
const char* password = "296JBD82kK";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
long lastMsg2 =0;
char msg[50];
int count=0;

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

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
    Serial.begin(115200);

    //MQTT section
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    pinMode(LED_BUILTIN, OUTPUT);

    //MAX30100 section
    Serial.print("Initializing pulse oximeter..");

    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);

    // The default current for the IR LED is 50mA and it could be changed by uncommenting the following line.
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

}
// kmkmkknknknkknkn
char JSONString[450]= "testing Temp";
int temp   =0;
// int indexCount=0;
// int indexSPO2 =0;

float HRAr[10]={};
int SPO2Ar[10]={};
int CountAr[10]={};

void loop()
{
    // Make sure to call update as fast as possible
    pox.update();
    
    // long now2 = millis();
    float HR= pox.getHeartRate();
    int SPO2= pox.getSpO2();

    // if (now2 - lastMsg > 1000) {
    // Serial.print("######Heart rate:");
    // Serial.print(HR);

    // Serial.print("#######bpm / SpO2:");
    // Serial.print(SPO2);
    // Serial.println("%");
    // } 


    //   if (!client.connected()) {
    //     reconnect();
    //     client.publish("esp32/temp", JSONString);
    //   }
    client.loop();

    long now = millis();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    
    if (now - lastMsg > REPORTING_PERIOD_MS) {
        lastMsg = now;
        Serial.print("Heart rate:");
        Serial.print(HR);


        Serial.print("bpm / SpO2:");
        Serial.print(SPO2);
        Serial.println("%");

        // add to HR array
        HRAr[temp]=HR;

        // add to SPO2 array
        SPO2Ar[temp]=SPO2;

        // add to count array
        CountAr[temp]=count;
        temp++;
        
        int ind=4;
        // send MQTT after temp reacehes 10 units (synonymous to after 10seconds)
        #ifdef TenMsg
        ind=10;
        #endif
        if(temp==ind){

            // Prepare JSON
            #ifdef ArrayMQTT
            sprintf(JSONString, 
"["
    #ifdef TenMsg
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "},"
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "},"
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "},"
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "},"
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "},"
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "},"
    #endif
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "},"
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "},"
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "},"
    "{"
        "\"count\": %d,"
        "\"SPO2\": %d,"
        "\"HeartRate\": %.2f"
    "}"
"]",CountAr[0],SPO2Ar[0],HRAr[0],CountAr[1],SPO2Ar[1],HRAr[1],
CountAr[2],SPO2Ar[2],HRAr[2],CountAr[3],SPO2Ar[3],HRAr[3]
#ifdef TenMsg
,CountAr[4],SPO2Ar[4],HRAr[4],CountAr[5],SPO2Ar[5],HRAr[5],
CountAr[6],SPO2Ar[6],HRAr[6],CountAr[7],SPO2Ar[7],HRAr[7],
CountAr[8],SPO2Ar[8],HRAr[8],CountAr[9],SPO2Ar[9],HRAr[9]
#endif
);
            #endif

            #ifdef NormMQTT
            sprintf(JSONString,"{\"count\":[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d], \"SPO2\":[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d], \"HeartRate\":[%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]}", CountAr[0],CountAr[1],CountAr[2],CountAr[3],CountAr[4],CountAr[5],CountAr[6],CountAr[7],CountAr[8],CountAr[9],SPO2Ar[0],SPO2Ar[1],SPO2Ar[2], SPO2Ar[3],SPO2Ar[4],SPO2Ar[5],SPO2Ar[6],SPO2Ar[7],SPO2Ar[8],SPO2Ar[9], HRAr[0],HRAr[1],HRAr[2],HRAr[3],HRAr[4],HRAr[5],HRAr[6],HRAr[7],HRAr[8],HRAr[9]);
            #endif
            

            Serial.println(JSONString);


            while(!client.connected()){
                reconnect();
            } 

            //sending measurement via MQTT
            sendMQTT(JSONString, "esp32/humid");
            
            //clear the variabels
            temp=0;
            HRAr[10]={};
            SPO2Ar[10]={};
            CountAr[10]={};

            if (!pox.begin()) {
                Serial.println("FAILED OX");
                for(;;);
            } else {
                Serial.println("SUCCESS OX");
            }
        }
        
        count++;

    }


}


