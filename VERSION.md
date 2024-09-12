# Boton de Panico - VERSION

* 1.0
* * Envia paquetes lora
* * Se crean tareas de prueba.

* 1.1
* * Se agrega procesamiento lora.update y lora.readData en core0 mientras que el resto se ejecuta en core1.

* 1.2
* * Se cambia config_pines.h por pinout.h.
* * Se crea función config_pines para setear entradas y salidas en setup.
* * Se crea Hardware.h y Hardware.ino.
* * Se crea función pdr_function() para prueba inicial de red LORA pero aun no está funcional.

* V1.3: 
* * Se configuran los 3 pulsadores como tareas que se activan a travez de interrupciones. (pendiente mejorar rebote)
* * Se crea el objeto Scheduler interrupt para establecer como alta prioridad.
* * Se cambian los nombres de las tareas por t1-t7.
* * Se crean las tareas t5, t6 y t7 para interrupciones.
* * Se crea estructura de mensajes para continuar con SMS en proxima version.

* V1.4: 
* * Se agregan funciones Serialcom y ReceiveMode en Hardware.ino para enviar/recibir SMS.
* * Se envia uncero al iniciar el setup por LORA para establecer la conexión.
* * Pulsadores ahora envian mensajes y prende led para confirmar que se envio el mensaje. Pendiente solucionar rebote!!!
* * Se bajan delays en funcion Enviar_msj para mejorar rendimiento.
* * Se agregan fotos en documentación.
* * Pendiente circuito esquematico para primer prototipo en placa.

* V1.5
* * * V1.5: 
* * Se agregan los pinout faltantes de leds.
* * Se agrega mensajes recibidos de confirmación. Pendiente agregar todos los mensajes recibidos sms, aun falta todo LORA.
* * Se agregan funciones prender y apagar led para trabajar con tareas programadas.
* * Se prende el led_recibido cuando se recibe una respuesta correcta durante 50 segundos.
* * Se agrega capacitor de 1000uF 10V entre VCC y GND del modulo GSM para evitar errores de lectura y reseteos del ESP32
por baja tension que genera el modulo SIM800.