#include "Camada.h"

void string2Char(String dado){
  strcpy(dadoArray, dado.c_str());
}

/* TX */
    void sendData(double data){
      packSize = sprintf(packTx, "%5.2f", data);      //retorna o tamanho da informação com 2 casas decimais.
      //sprintf(sizePackTx, "%03d", packSize);        //converte o tamanho da informação para char
      LoRa.print((char)packSize);
      LoRa.print(packTx);
      
      Serial.print("\nvar conv char:");
      Serial.print(packTx);
      Serial.print("\nvalor: ");
      Serial.print(data);
      Serial.print("\ntamanho: ");
      Serial.print(sizePackTx);
      Serial.print("\n-------\n");
    }

    void sendData(String data){
      packSize = data.length();                      //retorna o tamanho da informação
      // sprintf(sizePackTx, "%03d", packSize);      //converte o tamanho da informação para char
      LoRa.print((char)packSize);
      LoRa.print(data);
      
      Serial.print("\nvalor: ");
      Serial.print(data);
      Serial.print("\ntamanho: ");
      Serial.print(sizePackTx);
      Serial.print("\n-------\n");
    }

/* RX */
    void recebePacote(int packetSize) {
        bytesRx = "";                                 //variável responsável por armazenar o pacote recebido
        packSizeRx = String(packetSize,DEC);          //variável responsável por armazenar o tamanho do pacote

        dado = "";
        
        posYDisplay = 40;                             //Devido aos dados que já possuem no display, inserido para começar os dados na pos 40 de y
        int contNovoDado = 0;                         //Contador para ir decrementando conforme lê o dado atual sendo lido
        int tamDadoRx = 0;                            //Tamanho do dado atual sendo lido

        //AONDE PAREI: CONSEGUI COLOCAR O TAMANHO DO DADO EM UM HEXADECIMAL E PASSAR O DADO EM SOMENTE 1 BYTE AO INVÉS DE 3 BYTES. a partir daqui, montar novamente a camada de comunicação, passando sempre o primeiro byte p/ o tamanho do dado que vem a seguir.
        for (int i = 0; i < packetSize; i++) {
          if(contNovoDado == 0){                      //Só entra aqui se for p/ ler o Byte responsável por armazenar a quantidade de bytes da informação
              tamDadoRx = (int)LoRa.read();
              bytesRx += tamDadoRx;
              contNovoDado = tamDadoRx;
          }else{
            dado += (char)LoRa.read();

            if(contNovoDado == 1){                     //Se finalizar a leitura da informação atual
                bytesRx += dado;
                string2Char(dado);                     //Converte a string dado para um array de char (dadoArray)
                imprimeDados(posYDisplay, (String) dado);
                
                dado = "";
                posYDisplay += 10;
            }
            contNovoDado--; 
          }
        }

        rssi = String(LoRa.packetRssi(), DEC);
    }
