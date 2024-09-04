/*********
  Complete project details at http://randomnerdtutorials.com  
  https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/system/freertos.html#_CPPv412TaskHandle_t
*********/

TaskHandle_t Task0;


// LED pins
const int led1 = 25;
const int led2 = 4;

void setup() {                          //setup run in core1
  Serial.begin(115200); 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led2, OUTPUT);

  //create a task that will be executed in the loop0() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(loop0, "Task0", 10000, NULL, 1, &Task0, 0);              

}
//xTaskCreatePinnedToCore(TaskFunction, "name_of_task", Stack_size_of_task, parameter_of_the_task, priority_of_the_task, &Task_handle_to_keep_track_of_created_task, pin_task_to_core_0)
/*BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode,           //Pointer to the task entry function. Tasks must be implemented to never return (i.e. continuous loop), or should be terminated using vTaskDelete function.
                                       const char *const pcName,          //A descriptive name for the task. This is mainly used to facilitate debugging. Max length defined by configMAX_TASK_NAME_LEN - default is 16.
                                       const uint32_t usStackDepth,       //The size of the task stack specified as the number of bytes. Note that this differs from vanilla FreeRTOS.  
                                       void *const pvParameters,          //Pointer that will be used as the parameter for the task being created.
                                       UBaseType_t uxPriority,            //The priority at which the task should run. Systems that include MPU support can optionally create tasks in a privileged (system) mode by setting bit portPRIVILEGE_BIT of the priority parameter. For example, to create a privileged task at priority 2 the uxPriority parameter should be set to ( 2 | portPRIVILEGE_BIT ).
                                       TaskHandle_t *const pvCreatedTask, //Used to pass back a handle by which the created task can be referenced.
                                       const BaseType_t xCoreID);         //If the value is tskNO_AFFINITY, the created task is not pinned to any CPU, and the scheduler can run it on any core available. Values 0 or 1 indicate the index number of the CPU which the task should be pinned to. Specifying values larger than (portNUM_PROCESSORS - 1) will cause the function to fail.
                                       */


void loop() {                           //loop run in core1
  digitalWrite(led2, HIGH);
  delay(700);
  digitalWrite(led2, LOW);
  Serial.print("loop() running on core ");
  Serial.println(xPortGetCoreID());
  delay(700);
  
}

void loop0(void *parameter){            //loop run in core0
  while(1==1){
  digitalWrite(led1, HIGH);
  delay(1500);
  digitalWrite(led1, LOW);
  Serial.print("loop0() running on core ");
  Serial.println(xPortGetCoreID());
  delay(1500);

  }
}