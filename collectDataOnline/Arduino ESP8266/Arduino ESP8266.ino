#include <EmonLib.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"

// Credenciais da internet:
const char* ssid     = "IFRO_VISITANTES";
const char* password = "";

// Google Script ID:
const char *GScriptId = "AKfycby8VQfjo5otGx1K2Z7Q8zqYouhkqe7fhXc40UCfK3Cy4joV1XVS9_Tb28czpt1ZIT5s";

// Comando enviado para a planilha, bem como o nome da mesma:
String payload_base =  "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload = "";

// Google Sheets setup
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;
// Variaveis usadas pelo SCT - 013 
EnergyMonitor SCT013;
#define Tamanho 30  // Periodo de envio dos dados em segundos
int pinSCT = A0;   //Pino analógico conectado ao SCT-013
int tensao = 220;
int potencia;

// Variaveis usadas para calcular os dados a serem enviados
int j;
int cont[Tamanho] = {};
float conta, moda,Media;
double Amplist[Tamanho] = {};


void setup() {
  SCT013.current(pinSCT, 6.0606);
  Serial.begin(115200);        
  delay(10);
  Serial.println('\n');

  conectar();
}


void loop() {
   float SomaM = 0;
   // Cria os dados para a publicação na planilha em uma lista de inteiros
   for (int i=0; i<Tamanho; i++){ 
    double Amp = SCT013.calcIrms(1480);   // Calcula o valor da Corrente
    Amplist[i] = Amp;
    delay(1000);
    SomaM = SomaM + Amp;
   }
   Media = SomaM / Tamanho; //calcula média
   //calcula a moda da lista
  for(int i=0;i<Tamanho;i++){
        for(j=i+1;j<Tamanho;j++){
          
      if(Amplist[i]==Amplist[j]){
        cont[i]++;
          if(cont[i]>conta){
            conta=cont[i];
            moda=Amplist[i];
          }
      }

        }
        cont[i]=0;
    }
    if(conta == 0){
      moda = Media;
      }
//Prepara o redirecionamento https 
  static bool flag = false;
  if (!flag){
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
    }
  }
  else{
    Serial.println("Erro ao criar objeto cliente !!!");
  }
  
  // Cria o objeto no formato json para enviar à planilha Sheets1
  payload = payload_base + "\"" + Media + "," + moda + "\"}";
  
  // Envia os dados
  Serial.println("Enviando dados...");
  Serial.println(payload);
  if(client->POST(url, host, payload)){ 
    // Se a publicação funcionar...
  }
  else{
    Serial.println("Erro ao conectar");
    // se não funcionar...
    conectar();  
  }

}

void conectar(){
  WiFi.begin(ssid, password);             
  Serial.print("Connectando ao ");
  Serial.print(ssid); Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.println("Conecção estabelecida!");  
  Serial.print("endereço IP:\t");
  Serial.println(WiFi.localIP());

  // Usa HTTPSRedirect class para criar uma nova conecção TLS
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Conectando ao ");
  Serial.println(host);

  // Tenta conectar 5x 
  bool flag = false;
  for (int i=0; i<5; i++){ 
    int retval = client->connect(host, httpsPort);
    if (retval == 1){
       flag = true;
       Serial.println("Conectado");
       break;
    }
    else
      Serial.println("Conecção falhou, tentando novamente...");
  }
  if (!flag){
    Serial.print("Não conseguimos conectar ao server: ");
    Serial.println(host);
    return;
  }
  delete client;    // deleta o objeto HTTPS
  client = nullptr; // deleta o objeto HTTPS
}
