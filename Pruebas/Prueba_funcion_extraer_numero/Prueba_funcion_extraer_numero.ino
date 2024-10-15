//funcion para extraer el numero de un mensaje del tipo String: "mensaje, numero"

// Mensaje recibido
String mensaje_recibido = "policia, 12334412";

void setup() {
    // Inicializar la comunicación serie
    Serial.begin(115200);
    delay(3000);
Serial.print("la funcion extraer_numero da: ");Serial.println(extraer_numero(mensaje_recibido));
}

void loop() {
    // No se necesita hacer nada en el loop
}

uint32_t extraer_numero(String mensaje_completo){
    int coma_pos = mensaje_completo.indexOf(',');        // Encontrar la posición de la coma
    uint32_t numero = 0;

    // Verificar si la coma fue encontrada
    if (coma_pos != -1) {
        String parte_numerica = mensaje_completo.substring(coma_pos + 2);     // Extraer la parte numérica (después de la coma y espacio)
        numero = parte_numerica.toInt();
    } else return 0;                                                          //devuelve un cero si no encuentra la coma
  return numero;
}