  #include <WiFi.h>
#include <SPI.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid     = "rpise";
const char* password = "rpise2024";


IPAddress server_host(10, 42, 0, 1);
const uint16_t  server_port = 5000;

// Set web server port number to 80
WiFiServer server(80);

int status = WL_IDLE_STATUS;

WiFiClient client;

void setup() {
  Serial.begin(9600);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin(); 

  if (client.connect(server_host, server_port)) {
    Serial.println("Connected to server");

    makeRequest("GET /get_bowls_list HTTP/1.1");
  }
}

void loop(){
  String payload = "";
  while(client.available()) {
    char c = client.read();
    payload = c;
    Serial.print(payload);
  }
}

void makeRequest(String cmd) {
  client.println(cmd);        
  client.print("Host: ");
  client.println(server_host);
  client.println("User-Agent: TriMini Buttons");        
  client.println();
}
