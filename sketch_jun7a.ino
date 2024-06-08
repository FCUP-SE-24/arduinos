#include <WiFi.h>
#include <HttpClient.h>
#include <ArduinoHttpClient.h>

const char* ssid = "Tua_Net";
const char* password = "password";

char serverAddress[] = "192.168.1.95";  // server address
int port = 5001;
const char* serverPath = "/test_send"; 

WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, serverAddress, port);

void setup() {
  Serial.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
  
    String jsonPayload = "{\"sensor\": \"temperature\", \"value\": 24.7}";

    Serial.println("Sending POST request");    
    
    client.beginRequest();
    client.post(serverPath);

    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", jsonPayload.length());
    client.sendHeader("X-Custom-Header", "custom-header-value");
    client.sendHeader("Connection", "close");
    client.beginBody();
    client.print(jsonPayload);
    client.endRequest();

    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

    client.stop();

    Serial.print("Status code: ");
    Serial.println(statusCode);

    delay(500); // Send a POST request every 5 seconds
  } else {
    Serial.println("WiFi not connected");
  }
}
