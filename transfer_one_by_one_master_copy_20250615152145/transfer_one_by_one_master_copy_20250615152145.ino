#include <SPI.h>
#include "helper.h"

SPIClass master(HSPI);

#ifdef CONFIG_IDF_TARGET_ESP32
static constexpr uint8_t PIN_SS = 5;
#else
static constexpr uint8_t PIN_SS = SS;
#endif

static constexpr size_t BUFFER_SIZE = 8;
uint8_t tx_buf[BUFFER_SIZE] {0};
uint8_t rx_buf[BUFFER_SIZE] {0};

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

    pinMode(PIN_SS, OUTPUT);
    digitalWrite(PIN_SS, HIGH);

    master.begin(SCK, MISO, MOSI, PIN_SS);

    delay(2000);

    Serial.println("Start SPI master");
    Serial.println("Invia una parola (max 8 caratteri) e premi invio:");
}

void loop()
{
    if (Serial.available()) {
        String msgToSend = Serial.readStringUntil('\n');
        msgToSend.trim(); // rimuove spazi e newline residui

        // Pulisci tx_buf e copia i caratteri della stringa (max BUFFER_SIZE)
        memset(tx_buf, 0, BUFFER_SIZE);
        size_t len = msgToSend.length();
        if (len > BUFFER_SIZE) len = BUFFER_SIZE;
        for (size_t i = 0; i < len; i++) {
            tx_buf[i] = (uint8_t)msgToSend.charAt(i);
        }

        // Inizializza rx_buf a zero
        memset(rx_buf, 0, BUFFER_SIZE);

        // Invia e ricevi via SPI
        master.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
        digitalWrite(PIN_SS, LOW);
        master.transferBytes(tx_buf, rx_buf, BUFFER_SIZE);
        digitalWrite(PIN_SS, HIGH);
        master.endTransaction();

        // Stampa dati inviati e ricevuti
        Serial.print("Messaggio inviato: ");
        Serial.println(msgToSend);
        printBuffer("Dati inviati (master -> slave)", tx_buf, BUFFER_SIZE);
        printBuffer("Dati ricevuti (slave -> master)", rx_buf, BUFFER_SIZE);

        // Verifica differenze
        if (verifyAndDumpDifference("master", tx_buf, BUFFER_SIZE, "slave", rx_buf, BUFFER_SIZE)) {
            Serial.println("Ricevuti dati attesi dallo slave");
        } else {
            Serial.println("Differenze inattese tra dati master e slave");
        }

        Serial.println();
        Serial.println("Invia una parola (max 8 caratteri) e premi invio:");
    }
}
