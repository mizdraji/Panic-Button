//Armar un string con otros strings

// Al enviar mensajes por SMS o LoRaWAN incluir: (No necesario para la demo):
// 	* identificador de la aplicación. Ej: "PB" (Panic Button)
// 	* comando. Ej: "policia".
// 	* Epoch timestamp. Ej: 1726703523 segundos = 18/09/2024 20:54:08
// 	* numero_serie del dispositivo. Ej: "PB00001" (Puede ser generado por el servidor y enviado en la respuesta del proceso de activación de LoRaWAN).
// 	* versión del firmware.
// 	* versión del hardware.
// Ejemplo de mensaje: "PB, policia, 1726703523, PB00001, fv:1.0.0, hv:1.0.0".


//PB00001
#define VERSION 1.9
#define VERSION_HARDWARE 1.0
String devID  =   "PB00001";
#define timestamp 1726703523
String nombre = "German";
String apellido = "Mizdraji";
String Policia_send = "policia";
int edad = 33;

void setup(){
  Serial.begin(115200);
}
void loop() {
    // Crear una cadena formateada usando String y luego imprimirla
    String mensaje = devID + ", " + Policia_send + ", "+ timestamp + ", fv: " + VERSION + ", hv: " + VERSION_HARDWARE;
    Serial.println(mensaje);

    delay(3000);
}
