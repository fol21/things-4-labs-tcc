#ifndef DATA_STREAM_H
#define DATA_STREAM_H

#include <Arduino.h>
#include <string.h>
#include <ArduinoJson.h>

#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0])) 

#define CONTINOUS_STREAM "continous"
#define PERIODIC_STREAM "periodic"
#define CONTINOUS_STREAM_STRING String("continous")
#define PERIODIC_STREAM_STRING String("periodic")


class data_stream
{
public:

    int Threshold(){ return this->threshold; }
    char* Name(){ return this ->name; };


    //overridable stream process
    virtual void process() = 0;

    virtual void onMessage(char*, const char*, unsigned int) = 0;

    const char* send(const char*); // send message after process is done

    bool operator==(const data_stream& o )
    {
      return (strcmp(this->name, o.name) == 0) ? true:false;
    };

protected:
    
    char* name;
    int  threshold = 0;
    char* payload = NULL;
    bool lock = false;
};

class continous_stream : public data_stream
{
    public:
        continous_stream() {
            this->name = CONTINOUS_STREAM;

        };
        continous_stream(int size)
        {
            this->name = CONTINOUS_STREAM;
            this->threshold = size;
        };

        void process(){};

        void onMessage(char* topic, const char* payload,unsigned int length){};
};

class periodic_stream : public data_stream
{
    public:
        periodic_stream()
        {
            this->name = PERIODIC_STREAM;
        }
        periodic_stream(int size)
        {
            this->name = PERIODIC_STREAM;
            this->threshold = size;
        }

        void onMessage(char* topic, const char* payload,unsigned int length)
        {
            StaticJsonBuffer<200> jsonBuffer;
            this->payload = (char*) payload;
            JsonObject& params = jsonBuffer.parseObject(this->payload);
            this->millis = params["millis"];
        }

        void process()
        {
            delay(this->millis);
        }
    protected:
        int millis = 0;

};

#endif // DATA_STREAM_H

