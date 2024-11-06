#include <TaskScheduler.h>

// Define un objeto Scheduler
Scheduler myScheduler;

//callback
bool onEnableFunction();
void onDisableFunction();
void myTaskFunction();

// Crear la tarea
//Task(unsigned long aInterval, long aIterations, void (*aCallback)(), Scheduler* aScheduler, bool aEnable, bool (*aOnEnable)(), void (*aOnDisable)(), bool aSelfdestruct);
Task myTask(1000, 5, &myTaskFunction, &myScheduler, true, &onEnableFunction, &onDisableFunction);       //se ejecuta si onEnableFunction da true

//Funciones de habilitación
bool onEnableFunction() {               //Cuando esta funcion da true, se habilita myTask
    Serial.println("Task enabled");
    return true;                       //mientras de false, la tarea myTask quedara deshabilitada 
}

void onDisableFunction() {              //se ejecuta cuando la tarea se deshabilita, en este caso porque termino las 5 iteraciones
  Serial.println("Task disabled");
  myTask.restartDelayed(1000);
}

// Función de callback de la tarea
void myTaskFunction() {
    Serial.println("Task executed");
}

void setup() {
    Serial.begin(115200);
    delay(3000);
    // Agregar la tarea al Scheduler
    myScheduler.addTask(myTask);
    myTask.enable();
}

void loop() {
    // Ejecutar el Scheduler
    myScheduler.execute();
}


