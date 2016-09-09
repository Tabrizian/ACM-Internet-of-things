#include <ESP8266WiFi.h>
#include <cstring>
#include <ArduinoJson.h>


const char* ssid = "IP-WDL-RT2T2R";
const char* password = "Tabrizian1";

const char* serverAddress = "192.168.1.6";
const int baudRate = 115200;
const int port = 3000;
const IPAddress ipAddress;


void reportHumidity(double amount);
bool goldoon_exists();
const char* goldoon_create();
void initSerial();
void initWiFi();
String generateRequest(String body, String path);
String getBody(String response);
String goldoon_get();
const char *id;


// Arduino initial entry point #1
void setup() {
    initSerial();
    initWiFi();

    IPAddress ip = WiFi.localIP();
    StaticJsonBuffer<1000> jsonBuffer;
    String json = goldoon_get();
    JsonArray& root = jsonBuffer.parseArray(json.c_str());

    if(!root.success()) {
        Serial.println("JSON parsing failed");
    }

    bool exists = false;
    for (JsonArray::iterator it=root.begin(); it!=root.end(); ++it)
    {
        if(strcmp(it->asObject()["ip"].as<char *>(), ip.toString().c_str()) == 0) {
            Serial.println("IP exists in database.");
            exists = true;
            id = it->asObject()["_id"].as<char *>();
            Serial.println(id);
            break;
        }
    }

    if(!exists) {
        goldoon_create();
    }

}


// Arduino loop point
void loop() {
    delay(5000);
}

void initSerial() {
    Serial.begin(baudRate);
    Serial.println();
}

void initWiFi() {
    // WiFi Connection Initiation
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");

}

const char* goldoon_create() {
    String request = generateRequest("ip=192.168.1.9", "/goldoon");
    Serial.println(request);
    StaticJsonBuffer<1000> jsonBuffer;

    String json =  getBody(request);
    JsonObject& root = jsonBuffer.parseObject(json.c_str());
    if(!root.success()) {
        Serial.println("JSON parsing failed");
    }
    return root["id"].as<char *>();
}

bool goldoon_exists() {
}

String goldoon_get() {
    String request = String("GET /goldoon HTTP/1.1\n ") +
        "Host: " + serverAddress + "\n Cache-Control: no-cache\n\n";
    return getBody(request);
}

void reportHumidity(double amount) {
    String request = generateRequest(String("humidity") + int(amount), String("/goldoon") + "/" + id);
    getBody(request);
}

String getBody(String request) {
    WiFiClient client;
    if(client.connect(serverAddress, port)) {
        client.print(request);
        int contentLength = 0;
        while (client.connected())
        {
            if (client.available())
            {
                String str = client.readStringUntil('\n');
                if(str.startsWith("Content-Length")) {
                    contentLength = str.substring(16).toInt();
                }

                if(str.startsWith("\r") && str.endsWith("\r")) {
                    break;
                }
            }
        }

        String json = "";
        for(int i = 0; i < contentLength; i++) {
            json += (char) client.read();
        }


        client.stop();
        return json;
    }
}

String generateRequest(String body, String path) {
    String request = String("POST") + path + " HTTP/1.1\n" + "HOST: " + serverAddress +
        "content-type: application/x-www-form-urlencoded\n" +
        "content-length: "+ body.length() + "\n\n" + body +
        "\n";

    return request;
}
