# Boton de Panico - VERSION

* 1.0
* * Envia paquetes lora
* * Se crean tareas de prueba

* 1.1
* * Se agrega procesamiento lora.update y lora.readData en core0 mientras que el resto se ejecuta en core1.

* 1.2
* * Se cambia config_pines.h por pinout.h.
* * Se crea función config_pines para setear entradas y salidas en setup.
* * Se crea Hardware.h y Hardware.ino.
* * Se crea función pdr_function() para prueba inicial de red LORA pero aun no está funcional.

* V1.3: 
* * Se configuran los 3 pulsadores como tareas que se activan a travez de interrupciones. (pendiente mejorar rebote)
* * Se crea el objeto Scheduler interrupt para establecer como alta prioridad
* * Se cambian los nombres de las tareas por t1-t7
* * Se crean las tareas t5, t6 y t7 para interrupciones
* * Se crea estructura de mensajes para continuar con SMS en proxima version.
