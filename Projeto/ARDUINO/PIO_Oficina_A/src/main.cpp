#include <Arduino.h>
#include <SPI.h>      //Resposável pela comunicação Serial do LoRa
#include <LoRa.h>     //Resposável pela comunicação com o WiFi LoRa
#include <Wire.h>     //Resposável pela comunicação i2c
#include "SSD1306.h"  //Resposável pela comunicação com o display
#include "images.h"   //Arquivo com a logo da HelTec
#include "../lib/Camada.c"

// Define os pinos do OLED
#define SDA    4
#define SCL   15
#define Oled_RST   16 //RST must be set by software

// Define os pinos do WiFi LoRa
#define SDA     4    // GPIO4  -- SX127x's SDA
#define SCK     5    // GPIO5  -- SX127x's SCK
#define SCL     15   // GPIO15 -- SX127X's SCL
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define RST_LED 16   // GPIO16 -- OLED reset pin
#define LED     25   // GPIO25 -- LED Light pin
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)

#define BAND    433E6  //Define a frequência do LoRa. 433E6 é a mesma coisa de 433000000MHz. Você também pode usar 868E6 e 915E6.
#define PABOOST true  //Sem conhecimento dessa variavel mas ela deve aparecer para funcionar

SSD1306  display(0x3c, SDA, SCL);  //Define o endereço do i2c do Oled(0x3c) e os pinos SDA(4) e SCL(15) do ESP32

unsigned int cont = 0;        //contador utilizado para enviar os dados por LoRa

//LM35 -> 5V
const int analogIn = 36; // GPIO36  -- Leitura Analógica para o sensor LM35
int RawValue= 0;
double Voltage = 0;
double tempC = 0;
double mediaTemp = 0;

//LDR -> 5V
const int analogLDR = 37; // GPIO37  -- Leitura Analógica VP para o sensor LDR
double luminosidadeTemp = 0;
double luminosidade = 0;

void displayData(){
  display.clear();
  display.drawString(0, 0, ">>>>>>> UTFPR <<<<<<<");
  display.drawString(0, 10, "Julio Opolski Netto");
  display.drawString(0, 20, "### TX - Transmissor ###");
  display.drawString(0, 40, "TEMP: ");
  display.drawString(40, 40, String(tempC));  //Imprime o valor da variável 'counter' em forma de String
  display.drawString(0, 50, "LUM: ");
  display.drawString(40, 50, String(luminosidade));  //Imprime o valor da variável 'counter' em forma de String
  display.display();
}

void logo(){
  display.clear();
  display.drawXbm(0,5,logo_width,logo_height,logo_bits);  //Seta o X e Y de onde irá imprimir. Seta a largura e altura que será utilizado. Imprime uma imagem em hexa.
  display.display();
}

void configOled(){
    pinMode(16,OUTPUT);  //Define o pino Reset do Oled
    
    digitalWrite(16, LOW);  //Coloca o pino em Low para dar um Reset no Oled
    delay(50);
    digitalWrite(16, HIGH); //Coloca em High para o Oled voltar a funcionar

    display.init();  //Inicializa o Oled
    display.flipScreenVertically();  //Vira o Display para a vertical
    display.setTextAlignment(TEXT_ALIGN_LEFT);  //Alinha o texto a Esquerda
    display.setFont(ArialMT_Plain_10);  //Define o tipo e tamanho da fonte
    logo();  //Chama a função de mostrar a Logo
    delay(200);
    display.clear();  //Limpa a tela
}

void configLora(){
    SPI.begin(SCK,MISO,MOSI,SS);  //Inicializa a comunicação Serial com o LoRa
    LoRa.setPins(SS,RST,DI00);  //Define os pinos que serão utilizados pelo LoRa

    if (!LoRa.begin(BAND))
    {
      display.drawString(0, 0, "Problema ao Inic. LoRa!");
      display.display();
      while (1);
    }
    
    display.drawString(0, 0, ">>> LoRa Inicializado! <<<");
    display.display();
    delay(100);
}

void setup(){
    Serial.begin(9600);
    pinMode(25,OUTPUT);  //Define o pino do led da placa

    configOled();
    configLora();

    //display.clear();
    display.drawString(0, 10, "Lendo sensor...!");
    display.display();
    delay(100);
}

void loop(){
    digitalWrite(25, HIGH);                                 //Liga o led da placa
    
    RawValue = analogRead(analogIn);
    luminosidade = analogRead(analogLDR);

    Voltage = (double)((RawValue / 2048.0) * 3300);         //5000 to get millivots.
    tempC = (double)(Voltage * 0.1);

    switch(cont){
      case 0:
        packSize = 0;
        mediaTemp = tempC;
        luminosidadeTemp = luminosidade;
        cont++;
        break;
      case 10:
        cont = 0;
        tempC = (mediaTemp + tempC)/2;
        luminosidade = (luminosidadeTemp + luminosidade)/2;
        
        //Envia dados
        LoRa.beginPacket();
          sendData("TEMP:" + (String)tempC);
          sendData("LUM:" + (String)luminosidade);
        LoRa.endPacket();

        displayData();

        mediaTemp = 0;
        luminosidadeTemp = 0;

        break;
      default:
        mediaTemp = (mediaTemp + tempC)/2;
        luminosidadeTemp = (luminosidadeTemp + luminosidade)/2;
        cont++;
        
        break;
    }

    digitalWrite(25, LOW);  //Desliga o led da placa
    
    delay(500);
}