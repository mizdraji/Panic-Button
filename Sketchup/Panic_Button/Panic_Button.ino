/* Detalle de versiones
 * V2.0:
 *   RX Class C fijo SF7/500 (LoraTec). PDR en SF7; botones/ensayo con SF de prueba.
 *   ISR seguras, SMS linea a linea, destino fijo, heap sin String, serial limpio.
 * V1.9.5:
 *   Heap: sin String globales, UART SIM800 512, reporte periodico.
 * V1.9.4:
 *   SMS diferido 1.5s tras LoRa; botones en taskManager (no interrupt); solo borra core dump si corrupto.
 * V1.9.3:
 *   Borra core dump corrupto al arrancar; yield() tras LoRa; WDT cores deshabilitado en demo.
 * V1.9.2:
 *   MODO_DEMO: disableLoopWDT() + yield() en SMS/AT para evitar Reset reason: WDT.
 * V1.9.1:
 *   Reset reason al arranque, filtro eco SMS, dedupe LoRa.
 * V1.9.0:
 *   Paquete robustez demo: ISR seguras, SMS linea a linea, Enviar_msj char[],
 *   PDR timeout, MODO_DEMO sin sleep, sim800_send_at, LoRa RX seguro.
 * V1.8.10:
 *   ReceiveMode con sim800_send_at (OK/timeout); Serialcom solo si DEBUG_GSM=1.
 */

#define _TASK_PRIORITY
#include <TaskScheduler.h>
#include "Task.h"
#include <lorawan.h>
#include "LoraTec.h"
#include "ABPcredentials.h"
#include "pinout.h"
#include "Hardware.h"
#include "configuracion.h"
#include "esp_system.h"
#if MODO_DEMO
#include "esp_core_dump.h"
#endif

HardwareSerial SIM800L(2);

static const char* resetReasonStr(void) {
  switch (esp_reset_reason()) {
    case ESP_RST_POWERON:  return "POWERON";
    case ESP_RST_EXT:      return "EXT";
    case ESP_RST_SW:       return "SW";
    case ESP_RST_PANIC:    return "PANIC";
    case ESP_RST_INT_WDT:  return "INT_WDT";
    case ESP_RST_TASK_WDT: return "TASK_WDT";
    case ESP_RST_WDT:      return "WDT";
    case ESP_RST_DEEPSLEEP:return "DEEPSLEEP";
    case ESP_RST_BROWNOUT: return "BROWNOUT";
    default:               return "UNKNOWN";
  }
}

void setup() {
#if MODO_DEMO
  if (esp_core_dump_image_check() != ESP_OK) {
    esp_core_dump_image_erase();
  }
#endif
  SIM800L.setRxBufferSize(SIM800_RX_BUF_SIZE);
  SIM800L.begin(SERIAL_SIM, SERIAL_8N1, RX, TX);
  Serial.begin(SERIAL_SPEED);
  delay(500);
  Serial.print("Reset reason: ");
  Serial.println(resetReasonStr());

  delay(2500);
  config_pines();
  config_inicial();

  Serial.print("Firmware ");
  Serial.println(VERSION);
#if MODO_DEMO
  disableLoopWDT();
#endif

  if (!lora.init()) {
    Serial.println("RFM95 not detected");
  }

  if (initLoraTec()) {
    config_task();
    t_pdr.enable();
    uint32_t pdr_deadline = millis() + PDR_SETUP_TIMEOUT_MS;
    while (nodo.pdr_ok == 0 && (int32_t)(millis() - pdr_deadline) < 0) {
      taskManager.execute();
      yield();
    }
    if (nodo.pdr_ok == 0) {
      Serial.println("PDR timeout: continuar sin red LoRa");
    }
    t_pdr.disable();
  }

  ReceiveMode();

  delay(1000);
  memset(datoEntrante, 0, sizeof(datoEntrante));

#if MODO_ENSAYO
  ensayo_counter = 0;
  t_ensayo.enable();
#endif

  attachInterrupt(digitalPinToInterrupt(button1), buttonInterrupt1, RISING);
  attachInterrupt(digitalPinToInterrupt(button2), buttonInterrupt2, RISING);
  attachInterrupt(digitalPinToInterrupt(button3), buttonInterrupt3, RISING);
  attachInterrupt(digitalPinToInterrupt(RFM_pins.DIO0), onReceive, RISING);

  uint64_t mask = (1ULL << GPIO_NUM_39) | (1ULL << GPIO_NUM_38) | (1ULL << GPIO_NUM_36);
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_HIGH);
  print_wakeup_pins();
  delay(2000);
  slp = false;
  report_heap(true);
}

void loop() {
  static uint32_t last_lora_poll_ms = 0;
  static char last_lora_payload[INPUTBUFF];
  static uint32_t last_lora_payload_ms = 0;

#if DEBUG_GSM
  Serialcom();
#endif

  processPendingButtons();
  poll_sim800_messages();
  yield();

  taskManager.execute();
  yield();
  interrupt.execute();
  yield();

  if (SIM800L.available() ||
      digitalRead(button1) == HIGH ||
      digitalRead(button2) == HIGH ||
      digitalRead(button3) == HIGH) {
    timer = 0;
  }

  lora.update();
  yield();
  bool do_lora_read = false;

  if (processPendingLora()) {
    do_lora_read = true;
  } else if ((millis() - last_lora_poll_ms) >= 100) {
    do_lora_read = true;
  }

  if (do_lora_read) {
    last_lora_poll_ms = millis();
    recvStatus = lora.readData(datoEntrante);
    datoEntrante[INPUTBUFF - 1] = '\0';
    yield();
  }

  if (recvStatus > 1) {
    if (!isValidLoraPayload(datoEntrante)) {
      memset(datoEntrante, 0, sizeof(datoEntrante));
      recvStatus = 0;
    } else {
      uint32_t now = millis();
      if (strcmp(datoEntrante, last_lora_payload) == 0 &&
          last_lora_payload_ms != 0 &&
          (uint32_t)(now - last_lora_payload_ms) < 10000UL) {
        memset(datoEntrante, 0, sizeof(datoEntrante));
        recvStatus = 0;
      } else {
        strncpy(last_lora_payload, datoEntrante, INPUTBUFF - 1);
        last_lora_payload[INPUTBUFF - 1] = '\0';
        last_lora_payload_ms = now;

        Serial.print("LORA RX -> ");
        Serial.println(datoEntrante);

        timer = 0;
        uint32_t numrcv = extraer_numero(datoEntrante);
        if (strstr(datoEntrante, atendidorcv_lora) != NULL && numrcv == numsnt) {
          t_atendido.enable();
        }
        if ((strstr(datoEntrante, policiarcv_lora)  != NULL ||
             strstr(datoEntrante, bomberosrcv_lora) != NULL ||
             strstr(datoEntrante, medicarcv_lora)   != NULL) &&
            numrcv == numsnt && !checknum) {
          checknum = true;
          t_recibido.enable();
        }
        if (strstr(datoEntrante, informadorcv_lora) != NULL && numrcv == numsnt) {
          Tinformadorcv_Led.enable();
        }
        memset(datoEntrante, 0, sizeof(datoEntrante));
      }
    }
  }
  recvStatus = 0;

  report_heap(false);
  yield();
}

void print_wakeup_pins() {
  uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
  if (wakeup_pin_mask == 0) {
    return;
  }
  despertarSIM800L();
  delay(100);
  for (int i = 0; i < GPIO_NUM_MAX; i++) {
    if ((wakeup_pin_mask & (1ULL << i)) != 0) {
      switch (i) {
        case 36:
          btn1_pending = true;
          break;
        case 38:
          btn2_pending = true;
          break;
        case 39:
          btn3_pending = true;
          break;
        default:
          break;
      }
    }
  }
}
