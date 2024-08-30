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
#define _TASK_SCHEDULING_OPTIONS       // Support for multiple scheduling options
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
#define PERIOD1 500
#define DURATION 10000
void blink1CB();
Task tBlink1 ( PERIOD1 * TASK_MILLISECOND, DURATION / PERIOD1, &blink1CB, &ts, true );



void setup() {

  Serial.begin(115200);
  delay(TASK_SECOND);
  delay(2 * TASK_SECOND);
#endif
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  ts.execute();

}


// === 1 =======================================
bool LED_state = false;
void blink1CB() {
  if ( tBlink1.isFirstIteration() ) {
    _PP(millis());
    _PL(": Blink1 - simple flag driven");
    LED_state = false;
  }

  if ( LED_state ) {
    LEDOff();
    LED_state = false;
  }
  else {
    LEDOn();
    LED_state = true;
  }

  if ( tBlink1.isLastIteration() ) {
    tBlink2.restartDelayed( 2 * TASK_SECOND );
    LEDOff();
  }
}
