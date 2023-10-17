#include "Arduino.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "DHT.h"

#define deviceID [] = "01"; //ID do dispositivo
#define DHTPIN 13 // Pino onde o sensor DHT11 está conectado
#define DHTTYPE DHT11 // Tipo do sensor DHT
#define REED_SWITCH_PIN 15 // Pino onde o reed switch está conectado
#define arraySize 4 // Definindo o tamanho da Array

char ssid[] = "Galaxy Felipe";
char pass[] = "Felipe100603";
char serverAddress[] = "https://api.tago.io/data";  // TagoIO address
char contentHeader[] = "application/json";
char tokenHeader[]   = "b6649bfb-bca8-4ee5-8cf6-5b8f021f4621"; // TagoIO Token

HTTPClient client; // Iniciar uma nova instância do cliente HTTP
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  init_wifi();
  pinMode(DHTPIN, INPUT);
  dht.begin(); // Inicializar o sensor DHT11
  pinMode(REED_SWITCH_PIN, INPUT_PULLUP); // Configurar o reed switch como entrada com resistor pull-up
}
void init_wifi() {
  Serial.println("Conectando WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Conectado");
  Serial.print("Meu IP eh: ");
  Serial.println(WiFi.localIP());
}

int ReedSwitch = digitalRead(REED_SWITCH_PIN); // Variável do Reed Switch
float Umidade = dht.readHumidity(); // Variável de umidade
float Temperatura = dht.readTemperature(); // Variável de temperatura

float Chuva = 0.0;
float ValueChuva[arraySize] = {0, 0, 0, 0};
float ChuvaHora = 0;
float AddChuva = 1.6;

long Inicio = 0; // Tempo do inicio do aparelho
long Intervalo = 1 * 60 * 1000; // Controle de Minutos
int AnteriorRS = LOW; // Inicialize com um valor conhecido
int contador = 0;

void loop() {
  long Agora = millis();  // Obtém o tempo atual
  ReedSwitch;
  
  char anyData[30];
  char anyData1[30];
  char chuvaData[300];
  char umidadeData[300];
  char tempData[300];
  int statusCode = 0;

  if (ReedSwitch != AnteriorRS) { // Houve uma mudança no estado do reed switch
    if (ReedSwitch == HIGH) {
      // O reed switch mudou para HIGH (contato fechado)
      Chuva += AddChuva;
    } else {
      // O reed switch mudou para LOW (contato aberto)
      Chuva += AddChuva;
    }
    AnteriorRS = ReedSwitch; // Atualize o estado anterior
  }

  if (Agora - Inicio >= Intervalo) { // Passou o intervalo definido

    if (contador >= arraySize) {
            contador = 0;
            ValueChuva[contador] = Chuva;
    } else {
            ValueChuva [contador] = Chuva;
    }
        contador = contador + 1;
        for (int i = 0; i < 4; i++) {
            ChuvaHora += ValueChuva[i];
        } 
        
  // Formatar e enviar dados de Chuva/Hora
  strcpy(chuvaData, "{\n\t\"variable\": \"Chuva\",\n\t\"value\": ");
  dtostrf(ChuvaHora, 6, 2, anyData);
  strncat(chuvaData, anyData, 100);
  strcpy(anyData1, ",\n\t\"unit\": \"mm/h\"\n\t}\n");
  strncat (chuvaData, anyData1, 100);
  Serial.println(chuvaData);
  client.begin(serverAddress);
  client.addHeader("Content-Type", contentHeader);
  client.addHeader("Device-Token", tokenHeader);
  statusCode = client.POST(chuvaData);
  delay (2000);
  Serial.print("Status code (Chuva): ");
  Serial.println(statusCode);
  Serial.println("End of POST to TagoIO");
  Serial.println();

  // Formatar e enviar dados de Temperatura
  strcpy(tempData, "{\n\t\"variable\": \"Temperatura\",\n\t\"value\": ");
  dtostrf(Temperatura, 6, 2, anyData);
  strncat(tempData, anyData, 100);
  strcpy(anyData1, ",\n\t\"unit\": \"ºC\"\n\t}\n");
  strncat (tempData, anyData1, 100);
  Serial.println(tempData);
  client.begin(serverAddress);
  client.addHeader("Content-Type", contentHeader);
  client.addHeader("Device-Token", tokenHeader);
  statusCode = client.POST(tempData);
  delay (2000);
  Serial.print("Status code (Temperatura): ");
  Serial.println(statusCode);
  Serial.println("End of POST to TagoIO");
  Serial.println();
  delay(5000);

  // Formatar e enviar dados de Umidade
  strcpy(umidadeData, "{\n\t\"variable\": \"Umidade\",\n\t\"value\": ");
  dtostrf(Umidade, 6, 2, anyData);
  strncat(umidadeData, anyData, 100);
  strcpy(anyData1, ",\n\t\"unit\": \"%\"\n\t}\n");
  strncat(umidadeData, anyData1, 100);
  Serial.println(umidadeData);
  client.begin(serverAddress);
  client.addHeader("Content-Type", contentHeader);
  client.addHeader("Device-Token", tokenHeader);
  statusCode = client.POST(umidadeData);
   Serial.print("Status code (Umidade): ");
  Serial.println(statusCode);
  Serial.println("End of POST to TagoIO");

  Inicio = Agora;  // Atualiza o tempo da última reinicialização
  
  Serial.print("ValueChuva: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(ValueChuva[i]);
    Serial.print(", ");
  }
  Serial.println();
  Serial.print("Chuva: ");
  Serial.println (Chuva);
  Serial.print("ChuvaHora: ");
  Serial.println (ChuvaHora);
  Serial.print("Umidade: ");
  Serial.println (Umidade);
  Serial.print("Temperatura: ");
  Serial.println (Temperatura);
  }
  
}
