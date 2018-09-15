#include <MqttPublisher.h>
#include <WiFi.h>
#include <ArduinoJson.h>


#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

WiFiClient espClient;
//struct MqttConfiguration config = {"Prosaico01", "semfiopros@ico01", "001", "152.92.155.5", 1883};
struct MqttConfiguration config = {"LUFER", "21061992", "001", "192.168.15.7", 1883};
MqttPublisher publisher = MqttPublisher(espClient, config);

void callback( char* topic, uint8_t* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println((char*) payload);
}

class const_stream : public data_stream
{
  public:
    const char* constant = "";
    const_stream(const char* consta)
    {
      this->name = "constant";
      this->payload = "Hallo!";
      this-> constant = consta;
      Serial.println(this->constant);
    }

     void onMessage(char* topic, const char* payload,unsigned int length)
     {
            StaticJsonBuffer<200> jsonBuffer;
            this->payload = (char*) payload;
            JsonObject& params = jsonBuffer.parseObject(this->payload);
            this->constant = params["constant"];
    }

    void process()
    {
      Serial.println(this->constant);
    }
};

const_stream* cs;

void setup()
{
  Serial.begin(115200);
  delay(3000);
  publisher.onMessage(
  [=](char* topic, uint8_t* payload, unsigned int length)
  {
    publisher.middlewares(topic, payload, length);
    callback(topic, payload, length);
  });
  cs = new const_stream("initial");
  Serial.println(cs->constant);  
  publisher.add_stream(cs);

  publisher.check_network(
    [=]() -> bool
    {
      if(WiFi.status() == WL_CONNECTED)
        return true;
      else 
        return false;
  });
  
  publisher.init_network(
    [=]() -> bool
    {
      delay(10);
      // We start by connecting to a WiFi network
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(config.ssid);

      WiFi.begin(config.ssid, config.password);

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("");
      Serial.println("[Network] : Connected");
      return true;
  });
  
}

  
//temporarily holds data from vals
char charVal[10];   
String json;             
bool lock = true;
void loop()
{
   
   publisher.reconnect(
      [=]()
      {
        if(lock){
          lock = false;
          publisher.publish_stream("/001/configure/stream:periodic", "continous","{\"millis\":1000}");
        }
        Serial.println("Publisher state: " + String(publisher.Publisher_state()));
      });
   

  //4 is mininum width, 3 is precision; float value is copied onto buff
  dtostrf((temprature_sens_read() - 32) / 1.8, 4, 3, charVal);
  json = "{\"device\":\"esp32\",\"temperature\":" + String(charVal) + "}";
  Serial.println(json);
  publisher.publish_stream("/001/stream:periodic", "periodic",json.c_str());
}


// #ifdef __cplusplus
// extern "C" {
// #endif
// uint8_t temprature_sens_read();
// #ifdef __cplusplus
// }
// #endif
// uint8_t temprature_sens_read();

// void setup() {
//   Serial.begin(115200);
// }

// void loop() {
//   Serial.print("Temperature: ");
  
//   // Convert raw temperature in F to Celsius degrees
//   Serial.print((temprature_sens_read() - 32) / 1.8);
//   Serial.println(" C");
//   delay(5000);
// }




// #include <ESP8266WiFi.h>
// #include <PubSubClient.h>

// // Update these with values suitable for your network.

// const char* ssid = "FOL";
// const char* password = "21061992";
// const char* mqtt_server = "192.168.15.5";

// WiFiClient espClient;
// PubSubClient client(espClient);
// long lastMsg = 0;
// char msg[50];
// int value = 0;

// void setup_wifi() {

//   delay(10);
//   // We start by connecting to a WiFi network
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);

//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   randomSeed(micros());

//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }

// void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.print("Message arrived [");
//   Serial.print(topic);
//   Serial.print("] ");
//   for (int i = 0; i < length; i++) {
//     Serial.print((char)payload[i]);
//   }
//   Serial.println();

//   // Switch on the LED if an 1 was received as first character
//   if ((char)payload[0] == '1') {
//     digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
//     // but actually the LED is on; this is because
//     // it is acive low on the ESP-01)
//   } else {
//     digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
//   }

// }

// void reconnect() {
//   // Loop until we're reconnected
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     // Create a random client ID
//     String clientId = "ESP8266Client-";
//     clientId += String(random(0xffff), HEX);
//     // Attempt to connect
//     if (client.connect(clientId.c_str())) {
//       Serial.println("connected");
//       // Once connected, publish an announcement...
//       client.publish("outTopic", "hello world");
//       // ... and resubscribe
//       client.subscribe("inTopic");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       // Wait 5 seconds before retrying
//       delay(5000);
//     }
//   }
// }

// void setup() {
//   pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
//   Serial.begin(115200);
//   setup_wifi();
//   client.setServer(mqtt_server, 1883);
//   client.setCallback(callback);
// }

// void loop() {

//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();

//   long now = millis();
//   if (now - lastMsg > 2000) {
//     lastMsg = now;
//     ++value;
//     snprintf (msg, 75, "hello world #%ld", value);
//     Serial.print("Publish message: ");
//     Serial.println(msg);
//     client.publish("outTopic", msg);
//   }
// }