#include <Stepper.h>
#include <WiFi.h>
#include <SPI.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

const char* ssid     = "nome_da_net";
const char* password = "passe_da_net";

// Configurações do servidor
//char server[] = "192.168.43.42";
char server[] = "172.20.10.3";
int port = 5000;

WiFiClient client;

const int stepsPerRevolution = 1000;

bool motorRunning = false;
bool sendWeight = false;
bool resetScale = false;

Stepper myStepper(stepsPerRevolution, 5, 4, 3, 2);

int stepCount = 0;

unsigned long lastTime = 0;

unsigned long timerDelay = 2000;

int sensor = A0;

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(200);

  // Conecta-se à rede WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  int status = WiFi.begin(ssid, password);

  if (status != WL_CONNECTED) {
    Serial.println("Falha ao conectar à rede WiFi");
    while (true);
  }

  Serial.println("Conectado à rede WiFi");

  // Conecta-se ao servidor
  if (client.connect(server, port)) {
    Serial.println("Conectado ao servidor");
  } else {
    Serial.println("Falha ao conectar ao servidor");
    while (true);
  }
}

void loop() {
  // Processa pedidos de motor e de dados do sensor paralelamente
  checkServerCommands();
  updateMotorState();
  handleSensorData();
  getBowlWeight();
}

void checkServerCommands() {
  while (client.available()) {
    String response = client.readStringUntil('\n');
    if (response == "activate_motor") {
      Serial.println("Vou ligar o motor.");
      motorRunning = true;
      sendWeight = true;
    } else if (response == "deactivate_motor") {
      Serial.println("Vou desligar o motor.");
      motorRunning = false;
      sendWeight = false;
    } else if (response == "reset_scale") {
      resetScale = true;
    } 
  }
}

void updateMotorState() {
  if(motorRunning) {
      myStepper.step(5);
      Serial.print("steps:");
      Serial.println(stepCount);
      stepCount+=1;
      delay(0);
  } else {
    stepCount = 0;
  }
}

void handleSensorData() {
  static unsigned long lastSensorSendTime = 0;
  static const unsigned long sensorSendInterval = 650; // Tempo de espera em milissegundos entre cada envio de amostra
  
  if(sendWeight && (millis() - lastSensorSendTime >= sensorSendInterval)) {
    int sensorValue = analogRead(sensor);
    String sensorData = String(sensorValue);
    Serial.println(sensorData);
    client.println(sensorData);
    
    lastSensorSendTime = millis(); // Atualiza o tempo do último envio da amostra
  }
}


void getBowlWeight() {
  if(resetScale) {
    const unsigned long stabilityTime = 5000; // 5 seconds for stability
    const unsigned long maxTime = 15000; // 15 seconds total time

    unsigned long startTime = millis();
    unsigned long stableStartTime = millis();
    int lastSensorValue = analogRead(sensor);

    while (millis() - startTime < maxTime) {
      int currentSensorValue = analogRead(sensor);
      String sensorData = String(currentSensorValue);

      if (abs(currentSensorValue - lastSensorValue) < 0.5) {
        if (millis() - stableStartTime >= stabilityTime) {
          // Dados estáveis por 5 segundos, envia para o servidor
          Serial.println(sensorData);
          client.println(sensorData);
          resetScale = false;
          return;
        }
      } else {
        // Valor instável, resetar o tempo de estabilidade
        stableStartTime = millis();
      }

      lastSensorValue = currentSensorValue;

      delay(1000);
    }

    // Se o tempo total de tentativa exceder 15 segundos, envia mensagem de erro
    String errorMessage = "error: unable to get stable weight";
    resetScale = false;
    Serial.println(errorMessage);
    client.println(errorMessage);
  }
}
