// ----------------------------------------
// The following "defines" control library functionality at compile time,
// and should be used in the main sketch depending on the functionality required
//
// #define _TASK_TIMECRITICAL          // Enable monitoring scheduling overruns
// #define _TASK_SLEEP_ON_IDLE_RUN     // Enable 1 ms SLEEP_IDLE powerdowns between runs if no callback methods were invoked during the pass
// #define _TASK_STATUS_REQUEST        // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
// #define _TASK_WDT_IDS               // Compile with support for wdt control points and task ids
// #define _TASK_LTS_POINTER           // Compile with support for local task storage pointer
// #define _TASK_PRIORITY              // Support for layered scheduling priority
// #define _TASK_MICRO_RES             // Support for microsecond resolution
// #define _TASK_STD_FUNCTION          // Support for std::function (ESP8266 ONLY)
// #define _TASK_DEBUG                 // Make all methods and variables public for debug purposes
// #define _TASK_INLINE                // Make all methods "inline" - needed to support some multi-tab, multi-file implementations
// #define _TASK_TIMEOUT               // Support for overall task timeout
// #define _TASK_OO_CALLBACKS          // Support for callbacks via inheritance
// #define _TASK_EXPOSE_CHAIN          // Methods to access tasks in the task chain
#define _TASK_SCHEDULING_OPTIONS  // Support for multiple scheduling options
// #define _TASK_DEFINE_MILLIS         // Force forward declaration of millis() and micros() "C" style
// #define _TASK_EXTERNAL_TIME         // Custom millis() and micros() methods
// #define _TASK_THREAD_SAFE           // Enable additional checking for thread safety
// #define _TASK_SELF_DESTRUCT         // Enable tasks to "self-destruct" after disable

#include <TaskScheduler.h>



// Scheduler
Scheduler ts;

/*
   Approach 1: LED is driven by the boolean variable; false = OFF, true = ON
*/

void blink1CB();
Task tBlink1(500, TASK_FOREVER, &blink1CB, &ts, true);

#define buttom 37


void IRAM_ATTR panic_show(void) {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 25) {
    Serial.println("<----------------------------------------------");
  }
  last_interrupt_time = interrupt_time;
}

//unsigned long currentMillis = 0;
unsigned long previousMillis = 0;       // Variable para almacenar el tiempo anterior
const unsigned long ledOnTime = 500;    // Tiempo de encendido en milisegundos (0.5 segundos)
const unsigned long ledOffTime = 5000;  // Tiempo de apagado en milisegundos (5 segundos)
bool LED_state = LOW;                   // Estado actual del LED

void setup() {

  Serial.begin(115200);
  delay(TASK_SECOND);
  delay(2 * TASK_SECOND);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttom, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttom), panic_show, FALLING);

  //currentMillis = millis();
}

void loop() {
  ts.execute();
}

inline void LEDOn() {
  digitalWrite(LED_BUILTIN, HIGH);
}

inline void LEDOff() {
  digitalWrite(LED_BUILTIN, LOW);
}


// === 1 =======================================

void blink1CB() {
  unsigned long currentMillis = millis();

  if (LED_state == HIGH && (currentMillis - previousMillis >= ledOnTime)) {
    // Apagar el LED después de 0.5 segundos
    LED_state = LOW;                  // Cambiar estado del LED
    previousMillis = currentMillis;   // Actualizar tiempo anterior
    digitalWrite(LED_BUILTIN, LED_state);  // Apagar el LED
  } else if (LED_state == LOW && (currentMillis - previousMillis >= ledOffTime)) {
    // Encender el LED después de 5 segundos
    LED_state = HIGH;                 // Cambiar estado del LED
    previousMillis = currentMillis;   // Actualizar tiempo anterior
    digitalWrite(LED_BUILTIN, LED_state);  // Encender el LED
  }
}
