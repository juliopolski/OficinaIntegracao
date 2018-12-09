#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <stdlib.h>
#include <stdio.h>
#include "SSD1306.h" 
#include "images.h"

#include "../lib/Camada.c"

// Pin definetion of WIFI LoRa 32
// HelTec AutoMation 2017 support@heltec.cn 
#define SCK     5    // GPIO5  -- SX127x's SCK
#define SDA     4    // GPIO4  -- SX127x's SDA
#define SCL     15   // GPIO15 -- SX127X's SCL
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)
#define RST_LED 16   // GPIO16 -- OLED reset pin
#define LED     25   // GPIO25 -- LED Light pin
#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6
#define PABOOST true

#define V2   1

#ifdef V2 //WIFI Kit series V1 not support Vext control
  #define Vext  21
#endif

SSD1306  display(0x3c, SDA, SCL);

String temp;
String lum;

void logo(){
  display.clear();
  display.drawXbm(0,5,logo_width,logo_height,logo_bits);
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
  display.drawString(0, 10, "Aguardando dados...");
  display.display();
  delay(100);
}

void imprimeDados(int posY = 0, String info = ""){
  //display.clear();
  display.drawString(0, 0, ">>>>>>> UTFPR <<<<<<<");
  display.drawString(0, 10, "Julio Opolski Netto");
  display.drawString(0, 20, "### RX - Receptor ###");
  display.drawString(0 , 30 , packSizeRx + " bytes - RSSI: " + rssi); 
  display.drawStringMaxWidth(0 , posY , 128, info);
  display.display();
}

void setup() {
  Serial.begin(9600);
  pinMode(25,OUTPUT);  //Define o pino do led da placa

  configOled();
  configLora();
  
  LoRa.receive();
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    display.clear();
    recebePacote(packetSize);
  }

  delay(50);
}