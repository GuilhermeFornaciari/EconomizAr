
#include <Wire.h> //INCLUSÃO DA BIBLIOTECA
#include <RTClib.h> //INCLUSÃO DA BIBLIOTECA
#include <TimeLib.h>//INCLUSÃO DA BIBLIOTECA
#include <TimeAlarms.h>//INCLUSÃO DA BIBLIOTECA
#include <IRremote.h> //INCLUSÃO DE BIBLIOTECA

int RECV_PIN = 11; //PINO DIGITAL UTILIZADO PELO FOTORRECEPTOR KY-022
RTC_DS3231 rtc; //OBJETO DO TIPO RTC_DS3231

//sistema do sensor/emissor infravermelho
IRrecv irrecv(RECV_PIN); //PASSA O PARÂMETRO PARA A FUNÇÃO irrecv
IRsend emissorIR;
decode_results results; //VARIÁVEL QUE ARMAZENA OS RESULTADOS (SINAL IR RECEBIDO)
unsigned int desliga[] = {8950,4400, 450,600, 550,600, 500,1700, 500,650, 500,600, 500,650, 500,600, 500,650,
500,600, 500,1700, 550,600, 500,1700, 500,650, 500,1700, 500,1700, 500,650, 500,600, 500,650, 500,600, 500,650,
500,600, 500,650, 500,600, 500,650, 500,600, 500,650, 500,600, 500,650, 500,600, 500,650, 500,600, 500,650, 450,
650, 500,650, 500,600, 500,650, 500,600, 500,600, 550,600, 500,650, 500,600, 500,650, 450,1750, 500,600, 500};;

unsigned int liga[] = {8950,4350, 500,650, 500,600, 500,1700, 500,600, 550,1700, 500,600, 550,600, 500,650, 500,
600, 500,1700, 550,600, 500,1700, 500,650, 500,1650, 550,1700, 500,650, 500,600, 500,650, 500,600, 500,650, 500,
600, 500,600, 550,550, 550,650, 500,600, 500,650, 500,600, 500,600, 550,600, 500,600, 500,650, 500,650, 500,600,
500,600, 550,600, 500,600, 500,600, 550,600, 500,650, 500,600, 550,1700, 500,600, 500,1700, 500,650, 500};
#define frequencia 38







void setup() {
    // Define o horário do RTC se necessário
    Serial.begin(9600);
    rtc.begin();
    if (rtc.lostPower()){
      Serial.print("RTC RESETADO");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
    }
    // Pega a data e a hora do RTC
    int dataehora[6];
    getdate(dataehora);
    
    // Define a hora na biblioteca timeAlarms
    setTime(dataehora[0],dataehora[1],dataehora[2],dataehora[3],dataehora[4],dataehora[5]); // Define o horario de acordo com o RTC 
    

    SetAlarms();
    delay(100); //INTERVALO DE 100 MILISSEGUNDOS

  // TESTES ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for(int i = 0; i <= sizeof(dataehora); i++){ 
    Serial.println(dataehora[i]);
    }


    
}

void loop() {
  Alarm.delay(1000); // wait one second between clock display
  char date[19] = "YY:MM:DD  hh:mm:ss";
  rtc.now().toString(date);
  Serial.println(date);   
}
void ligar(){
     emissorIR.sendRaw(liga, sizeof(liga) / sizeof(liga[0]), frequencia);
     Serial.println("liga acionado");
     delay(100); 
}
void desligar(){
     emissorIR.sendRaw(desliga, sizeof(desliga) / sizeof(desliga[0]), frequencia);
     Serial.println("desliga acionado");
     delay(100);   
}

void getdate(int dataehora[6]){
      //variaveis para conversão de hora para int:
    char H[03] = "hh";
    char M[03] = "mm";
    char S[03] = "ss";

    char Y[05] = "YY";
    char Me[03]= "MM";
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

void SetAlarms(){
    // HORARIOS QUE O SISTEMA VAI LIGAR/DESLIGAR
    Alarm.alarmRepeat(7,30,00, ligar);  
    Alarm.alarmRepeat(10,0,00, desligar);
    
    Alarm.alarmRepeat(10,20,00, ligar);  
    Alarm.alarmRepeat(12,0,00, desligar);

    Alarm.alarmRepeat(13,50,00, ligar);  
    Alarm.alarmRepeat(15,30,00, desligar);
}
