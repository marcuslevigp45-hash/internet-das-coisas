#include "WiFi.h"
#include <WebServer.h>
#include "DHT.h"
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "senha.ino"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

const int LDR_PIN = 1;
const int LED_PIN = 14;
const int DHT_PIN = 15;
const int LIMIAR_LUZ = 5000;
int ldr;
int temperatura;
int umidade;
float estadoLuz = LOW;
IPAddress ip;
bool automatico = true;

void mudar_luz(float estado);

WebServer server(80);
WiFiClient wifi;

DHT dht(DHT_PIN, DHT11);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  dht.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE, BLACK);
  display.display();
  display.clearDisplay();
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro Display");
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Conectando...");
  display.display();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  ip = WiFi.localIP();
  
  server.on("/", handle_index);
  server.on("/ligar", handle_ligar);
  server.on("/desligar", handle_desligar);
  server.on("/automatico", handle_automatico);
  server.on("/monitoramento", handle_monitoramento); 
  server.on("/LED", handle_led);
  server.onNotFound(handle_not_found);
  server.begin();
}

void loop() {
  server.handleClient();
  int t = dht.readTemperature();
  int u = dht.readHumidity();
  if (!isnan(t)) temperatura = t;
  if (!isnan(u)) umidade = u;
  
  ldr = analogRead(LDR_PIN);
  if (automatico) {
    if (ldr < LIMIAR_LUZ) {
      int luz = 255 - map(ldr, 1000, LIMIAR_LUZ, 0, 255);
      mudar_luz(luz);
    } else {
      mudar_luz(LOW);
    }
  }
  mostra_tela();
  delay(5);
}

void mostra_tela() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("IP: " + ip.toString());
  display.println("Luz: " + String(ldr));
  display.println("Temp: " + String(temperatura) + "C");
  display.println("Umid: " + String(umidade) + "%");
  display.display();
}

void mudar_luz(float estado) {
  estadoLuz = estado;
  analogWrite(LED_PIN, estadoLuz);
}

void handle_index() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Ativo");
}

void handle_ligar() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  automatico = false;
  mudar_luz(255);
  server.send(200, "text/plain", "Ligado");
}

void handle_desligar() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  automatico = false;
  mudar_luz(0);
  server.send(200, "text/plain", "Desligado");
}

void handle_automatico() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  automatico = true;
  server.send(200, "text/plain", "Automatico");
}

void handle_monitoramento() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String json = "{\"luz\":" + String(ldr) + ",\"temperatura\":" + String(temperatura) + ",\"umidade\":" + String(umidade) + ",\"ventilador\":0.4,\"refletor\":0.4,\"exaustor\":0}";
  server.send(200, "application/json", json);
}

void handle_led() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("velocidade")) {
    int v = server.arg("velocidade").toInt();
    automatico = false;
    mudar_luz(v);
    server.send(200, "text/plain", "Velocidade alterada");
  } else {
    server.send(400, "text/plain", "Erro ...");
  }
}

void handle_not_found() {
  server.send(404, "text/plain", "Erro");
}
