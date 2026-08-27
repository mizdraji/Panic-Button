#include "Arduino.h"

extern HardwareSerial SIM800L;

#define MAX_RANDOM_LARGO       50
#define MAX_REINTENTOS         6
#define MAX_PAUSAS_LARGAS      3
#define LONG_TIME_TO_WAIT      180
#define MIN_RANDOM             10
#define MAX_RANDOM             40
#define UN_DIA                 84600
#define PDR_SETUP_TIMEOUT_MS   120000UL
#define SMS_LINE_BUF_SIZE      160
#define SIM800_RX_BUF_SIZE     512
#define SIM800_READ_CHUNK      64
#define BTN_DEBOUNCE_MS        200
#define LORA_IRQ_DEBOUNCE_MS   30

struct str {
  uint8_t pdr_ok                = 0;
  int32_t t_wait                = 0;
  uint8_t pausa_larga           = 0;
  uint8_t cont_pausas_largas    = 0;
  uint8_t cont_reintento_corto  = 0;
};
str nodo;

void IRAM_ATTR buttonInterrupt1();
void IRAM_ATTR buttonInterrupt2();
void IRAM_ATTR buttonInterrupt3();
void IRAM_ATTR onReceive();

volatile bool lora_irq_pending = false;
volatile bool btn1_pending = false;
volatile bool btn2_pending = false;
volatile bool btn3_pending = false;

uint16_t random_time(unsigned int MIN_, unsigned int MAX_);
uint32_t idempotencia_random();
uint32_t extraer_numero(const char* mensaje_completo);
void pdr_function();
void config_pines();
void config_inicial();
void Enviar_msj(const char* msj);
void ReceiveMode();
void processPendingButtons();
bool processPendingLora();
void poll_sim800_messages();
void report_heap(bool force);
#if DEBUG_GSM
void Serialcom();
#endif
