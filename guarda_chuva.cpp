#define deviceID = 1    //ID do dispositivo
#define DHTPIN 15       // Pino onde o sensor DHT11 está conectado
#define DHTTYPE DHT11   // Tipo do sensor DHT
#define REED_SWITCH 13  // Pino onde o reed switch está conectado
#define arraySize 10    // Definindo o tamanho da Array

/* Definindo regra de status return */
#define TIPO_CHUVA 1
#define TIPO_TEMPERATURA 2
#define TIPO_UMIDADE 3

/* Incluindo bibliotecas */
#include "WiFi.h"
#include "HTTPClient.h"
#include "DHT.h"

char ssid[] = "ZETROC";                      // Nome da rede WiFi
char pass[] = "501508110";                         // Senha da rede WiFi
char serverAddress[] = "https://api.tago.io/data";  // TagoIO address
char contentHeader[] = "application/json";
char tokenHeader[] = "ca8e83ae-5007-446e-8a27-a2d455500580";  // TagoIO Token

HTTPClient client;  // Iniciar uma nova instância do cliente HTTP
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  init_wifi();
  pinMode(DHTPIN, INPUT);
  dht.begin();  // Inicializar o sensor DHT11
  pinMode(REED_SWITCH, INPUT);
  attachInterrupt(digitalPinToInterrupt(REED_SWITCH), count, FALLING);  //Debouncing Reed Switch
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

char anyData[30];
char chuvaData[300];
char umidadeData[300];
char tempData[300];
int statusCode = 0;

float temperatura = 0;
float umidade = 0;
float chuva = 0;
float addChuva = 0.4;
float chuvaH = 0;
bool reedStats = false;

float lastChuva[arraySize] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int arrayCounter = 0;

long controle = 0;           // Tempo do inicio do aparelho
long intervalo = 20 * 1000;  // Controle de Minutos

// Função para obter o status com base no valor
void getStatus(float value, int variableName, char* retorno) {
  switch (variableName) {

    case TIPO_CHUVA:
      if (value >= 15.0) {
        strcpy(retorno, ";ChuvaForte");
      } else if (value >= 5.0) {
        strcpy(retorno, ";ChuvaMedia");
      } else if (value > 0) {
        strcpy(retorno, ";ChuvaFraca");
      } else {
        strcpy(retorno, ";ChuvaNull");
      }
      break;

    case TIPO_TEMPERATURA:
      if (value >= 30.0) {
        strcpy(retorno, ";TemperaturaAlta");
      } else if (value >= 17.0) {
        strcpy(retorno, ";TemperaturaBoa");
      } else if (value > 0) {
        strcpy(retorno, ";TemperaturaBaixa");
      }
      break;

    case TIPO_UMIDADE:
      if (value >= 80.0) {
        strcpy(retorno, ";UmidadeAlta");
      } else if (value >= 50.0) {
        strcpy(retorno, ";UmidadeBoa");
      } else if (value > 0) {
        strcpy(retorno, ";UmidadeBaixa");
      }
      break;
  }
}

void count(void) {
  if (!reedStats) {
    reedStats = true;
    chuva += addChuva;
  }
}

void loop() {
  long agora = millis();                      // Obtém o tempo atual
  float umidade = dht.readHumidity();         // Variável de umidade
  float temperatura = dht.readTemperature();  // Variável de temperatura

  if (reedStats) {
    delay(300);
    reedStats = false;
    Serial.println("chuva: ", chuva, "mm");
    Serial.println("umidade: ", umidade, "%");
    Serial.println("temperatura: ", temperatura, "ºC");
    Serial.println("-------------------------------------------------------------------------------");
  }
  if (agora - controle >= intervalo) {
    if (arrayCounter >= arraySize) {
      arrayCounter = 0;
    }
    Serial.print("Chuva Última Hora = ", ChuvaH, "mm/h");

    lastChuva[arrayCounter] = chuva;
    arrayCounter = arrayCounter + 1;

    for (int i = 0; i < arraySize; i++) {
      chuvaH = chuvaH + lastChuva[i];
    }

    Serial.print("lastChuva: [");
    for (int i = 0; i < arraySize; i++) {
      Serial.print(lastChuva[i]);
      if (i < arraySize - 1) {
        Serial.print(", ");
      }
    }
    Serial.println("]");
    Serial.println("-------------------------------------------------------------------------------");

    /* Mandando dados para TAGO */
    char varStatus[100];

    /* Informações de Chuva*/
    strcpy(chuvaData, "{\n\t\"variable\": \"Chuva\",\n\t\"value\": \"");
    dtostrf(chuvaH, 6, 2, anyData);
    strncat(chuvaData, anyData, 100);
    getStatus(chuvaH, TIPO_CHUVA, varStatus);
    strncat(chuvaData, varStatus, 100);
    strcat(chuvaData, "\"\n\t}\n");
    client.begin(serverAddress);
    client.addHeader("Content-Type", contentHeader);
    client.addHeader("Device-Token", tokenHeader);
    statusCode = client.POST(chuvaData);
    Serial.print("Informações de Chuva: ");
    Serial.println(chuvaData);
    Serial.println(statusCode);
    delay(400);

    /* Informações de Umidade*/
    strcpy(umidadeData, "{\n\t\"variable\": \"Umidade\",\n\t\"value\": \"");
    dtostrf(umidade, 6, 2, anyData);
    strncat(umidadeData, anyData, 100);
    getStatus(umidade, TIPO_UMIDADE, varStatus);
    strncat(umidadeData, varStatus, 100);
    strcat(umidadeData, "\"\n\t}\n");
    statusCode = client.POST(umidadeData);
    Serial.print("Informações de umidade: ");
    Serial.println(umidadeData);
    Serial.println(statusCode);
    delay(400);

    /* Informações de Temperatura*/
    strcpy(tempData, "{\n\t\"variable\": \"Temperatura\",\n\t\"value\": \"");
    dtostrf(temperatura, 6, 2, anyData);
    strncat(tempData, anyData, 100);
    getStatus(temperatura, TIPO_TEMPERATURA, varStatus);
    strncat(tempData, varStatus, 100);
    strcat(tempData, "\"\n\t}\n");
    statusCode = client.POST(tempData);
    Serial.print("Informações de temperatura: ");
    Serial.println(tempData);
    Serial.println(statusCode);
    delay(400);

    chuvaH = 0;
    chuva = 0;

    controle = millis();
  }
}
