#ifndef CAMADA_H                                        //guardas de cabeçalho, impedem inclusões cíclicas
#define CAMADA_H

/* Funções auxiliares */
    void string2Char(String);
    void imprimeDados(int, String);

/* TX */
    unsigned int packSize;                              //para controlar o tamanho do pacote a ser enviado
    char packTx[255];                                   //Pacote a ser enviado
    char sizePackTx[255];                               //Tamanho do Pacote

    void sendData(double);
    void sendData(String);

/* RX */
    String bytesRx = "";                                //variável responsável por armazenar o pacote recebido
    String rssi = "RSSI --";                            //variável responsável por armazenar a potência do sinal  
    String packSizeRx = "--";                           //variável responsável por armazenar o tamanho do pacote 

    String dado;
    char dadoArray[255];
    int posYDisplay;                                     //posição do display que será impresso o dado

    void recebePacote(int);
#endif