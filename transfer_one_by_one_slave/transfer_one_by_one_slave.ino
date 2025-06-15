#include <ESP32SPISlave.h>
#include "helper.h"

ESP32SPISlave slave;

static constexpr size_t BUFFER_SIZE = 8;
static constexpr size_t QUEUE_SIZE = 1;
uint8_t tx_buf[BUFFER_SIZE] {0};  // buffer per dati da inviare
uint8_t rx_buf[BUFFER_SIZE] {0};  // buffer per dati ricevuti

#define MOSI 23
#define MISO 19
#define SCK  18

void printBuffer(const char* label, uint8_t* buf, size_t len) {
    Serial.print(label);
    Serial.print(": ");
    for (size_t i = 0; i < len; i++) {
        Serial.print(buf[i]);
        if (i < len - 1) Serial.print(", ");
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    delay(2000);

    slave.setDataMode(SPI_MODE0);
    slave.setQueueSize(QUEUE_SIZE);
    slave.begin();

    Serial.println("Start SPI slave");
}

void loop()
{
    // inizializza i buffer
    initializeBuffers(tx_buf, rx_buf, BUFFER_SIZE);

    // esegui la transazione SPI (bloccante)
    const size_t received_bytes = slave.transfer(tx_buf, rx_buf, BUFFER_SIZE);

    // interpreta i dati ricevuti come stringa (aggiungendo terminatore)
    char receivedStr[BUFFER_SIZE + 1] = {0};
    memcpy(receivedStr, rx_buf, received_bytes);
    receivedStr[received_bytes] = '\0';  // terminatore stringa

    Serial.print("Stringa ricevuta dal master: ");
    Serial.println(receivedStr);

    // Prepara la risposta da inviare al master
    // Esempio: rispondi con la stessa stringa ricevuta (echo)
    memset(tx_buf, 0, BUFFER_SIZE);
    size_t len = strlen(receivedStr);
    if (len > BUFFER_SIZE) len = BUFFER_SIZE;
    memcpy(tx_buf, receivedStr, len);

    // Stampa i buffer per debug
    printBuffer("Dati inviati (slave -> master)", tx_buf, BUFFER_SIZE);
    printBuffer("Dati ricevuti (master -> slave)", rx_buf, received_bytes);

    Serial.println();  // riga vuota per leggibilità
}
