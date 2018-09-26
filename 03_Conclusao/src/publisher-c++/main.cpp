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

