//Inclusão das bibliotecas necessárias
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include "EmonLib.h"
#include <TimeLib.h>
#include <TimeAlarms.h>

File myFile;               // Tipo file para salvar os dados coletados
RTC_DS3231 rtc;            // Objeto do tipo RTC_ds3231
const int chipSelect = 10; // Porta de comunicação com o SD Card
bool ativo = true;

// Variaveis usadas pelo SCT - 013
EnergyMonitor SCT013;
#define Tamanho 60  // Periodo de envio dos dados em segundos
int pinSCT = A0;   //Pino analógico conectado ao SCT-013
int tensao = 220;

//Dados para serem enviados
String payload = "";

void setup() {
  // Calibra o leitor de corrente de acordo com o circuito
  SCT013.current(pinSCT, 60.606);

  // Inicia o Serial
  Serial.begin(9600);

  // Inicializa o RTC
  if (! rtc.begin()) { // se não encontrar o rtc:
    Serial.println("DS3231 não encontrado");
    while (1); }//SEMPRE ENTRE NO LOOP
  Serial.println("DS3231 OK!");
  
  // Configura o RTC se necessário
  if (rtc.lostPower()) {
    Serial.print("RTC RESETADO");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  delay(100);

  // Tenta inicializar o cartão SD
  Serial.print("Inicializando Cartão SD...");
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // Pega a data e a hora do RTC
  int dataehora[6];
  getdate(dataehora);

  // Define a hora na biblioteca timeAlarms
  setTime(dataehora[0],dataehora[1],dataehora[2],dataehora[3],dataehora[4],dataehora[5]); 
  
}

void loop () {
  DateTime now = rtc.now(); //CHAMADA DE FUNÇÃO
  // printa a hora
  char date[20] = "DD/MM/YY;hh:mm:ss";
  rtc.now().toString(date);
  Serial.println(date);
  
  Alarm.delay(1000);

  
  //checa se o sistema deve estar ligado
  // Pega a data e a hora do RTC
  int dataehora[6];
  getdate(dataehora);

  ligadesliga(ativo,dataehora);
  if (ativo){
      // Coleta de Dados para a gravação ##############################
      // Reset de variaveis
      float WattsM = 0;
      float SomaM = 0;
      float Media = 0;
      String data = "";
      String hora = "";
      String dados = "";
      
      // Cria os dados para a publicação em uma lista de inteiros
      for (int i = 0; i < Tamanho; i++) {
        double Amp = SCT013.calcIrms(1480);   // Calcula o valor da Corrente
        delay(1000);
        SomaM = SomaM + Amp;
      }
      Media = SomaM / Tamanho; //Calcula média
      WattsM =  Media * tensao;//Calcula em Watts
      
      // Abre o SD
      myFile = SD.open("test.txt", FILE_WRITE);
      // Se abriu tranquilamente, continua o processo:
      if (myFile) {
        Serial.println("Escrevendo para a TXT:");
        //Montar todas informações em uma unica string
          DateTime now = rtc.now(); //CHAMADA DE FUNÇÃO
        // printa a hora
        char date[20] = "DD/MM/YY;hh:mm:ss";
        rtc.now().toString(date);
        dados = String(Media) + "," + String(WattsM);
    
        payload = String(date) + "," + dados;
        //envia os dados para o SD
        myFile.println(payload);
        Serial.println(payload);
        // fecha o arquivo
        myFile.close();
        Serial.println("Feito!!!");
        } else {
        // Se dar erro ao abrir o arquvio:
        Serial.println("erro ao abrir o arquivo");
        }
  }






  //Proxima linha
  Serial.println();
  Alarm.delay(1000);
}


void getdate(int dataehora[6]) {
  //variaveis para conversão de hora para int:
  char H[03] = "hh";
  char M[03] = "mm";
  char S[03] = "ss";

  char Y[05] = "YY";
  char Me[03] = "MM";
  char D[03] = "DD";

  DateTime now = rtc.now();

  rtc.now().toString(H);
  rtc.now().toString(M);
  rtc.now().toString(S);

  rtc.now().toString(Y);
  rtc.now().toString(Me);
  rtc.now().toString(D);

  // transforma data e hora em inteiro e guarda na lista entregue
  dataehora[0] = atol(H);
  dataehora[1] = atol(M);
  dataehora[2] = atol(S);

  dataehora[3] = atol(Me);
  dataehora[4] = atol(D);
  dataehora[5] = atol(Y);

}

void ligadesliga(bool ativo,int dataehora[6]){
  // Converte em segundos o horario atual do dia para ser comparado
  //                    (HH * 60M * 60S)+ MM * 60S  + S
  int  inicioemsegundos = (7  * 36)   + (30 * 60) + 0; // = 27 000
  int  finalemsegundos  = (17 * 36)   + (30 * 60) + 0; // = 63 000
  
  int  segundoatual     = (dataehora[0] * 3600) + (dataehora[1] * 60) + dataehora[2];
  Serial.println(segundoatual);
  Serial.println(inicioemsegundos);
  Serial.println(segundoatual);
  Serial.println(finalemsegundos);
  
  if ((segundoatual > inicioemsegundos) and (segundoatual < finalemsegundos)){
    ativo = true;
  }
  else{
    ativo = false;
  }
  Serial.print("Está ");
  Serial.println(ativo);
}
