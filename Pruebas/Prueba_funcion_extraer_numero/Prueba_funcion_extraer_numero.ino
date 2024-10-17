//funcion extraer_numero para extraer el numero de un mensaje del tipo String: "mensaje, numero"
//funcion extraer_numero_char para extraer el numero de un mensaje del tipo char: "mensaje, numero"

// Mensaje recibido como String
String mensaje_recibido_string = "policia, 12334412";

// Mensaje recibido como char[]
char mensaje_recibido_char[] = "policia, 12334412";

//mensaje completo como String
String mensaje_completo = "+CMT: \"+5493795572356\",\"\",\"24/10/16,09:45:06-12\" \nque me importa lo que ponga aca, 6079999";

void setup() {
    // Inicializar la comunicación serie
    Serial.begin(115200);
    delay(3000);
Serial.print("la funcion extraer_numero de un string da: "); Serial.println(extraer_numero(mensaje_recibido_string));
Serial.print("la funcion extraer_numero de un char da: "); Serial.println(extraer_numero(mensaje_recibido_char));
Serial.print("la funcion extraer_numero_completo de un String da: "); Serial.println(extraer_numero_completo(mensaje_completo));
}

void loop() {
    // No se necesita hacer nada en el loop
}

//extrae cualquier numero despues de la ultima coma de un mensaje tipo string
uint32_t extraer_numero_completo(String mensaje_completo) {
    // Buscar la última coma en el mensaje
    int posicion_ultima_coma = mensaje_completo.lastIndexOf(',');

    // Verificar si se encontró la coma
    if (posicion_ultima_coma != -1) {
        // Extraer la parte numérica después de la última coma y espacio
        String parte_numerica = mensaje_completo.substring(posicion_ultima_coma + 2);  // +2 para saltar la coma y el espacio
        return parte_numerica.toInt();  // Convertir a uint32_t y retornar
    }

    // Si no se encontró la coma, devolver 0
    return 0;
}
//funcion extraer_numero para extraer el numero de un mensaje del tipo String: "mensaje, numero"
uint32_t extraer_numero(String mensaje_completo){
    int coma_pos = mensaje_completo.indexOf(',');        // Encontrar la posición de la coma
    uint32_t numero = 0;

    // Verificar si la coma fue encontrada
    if (coma_pos != -1) {
        String parte_numerica = mensaje_completo.substring(coma_pos + 2);     // Extraer la parte numérica (después de la coma y espacio)
        numero = parte_numerica.toInt();
        return numero;
    } else return 0;                                                          // Si no se encontró la coma, devolver 0
}
//funcion extraer_numero para extraer el numero de un mensaje del tipo char[]: "mensaje, numero"
uint32_t extraer_numero(char mensaje_completo[]) {
    char *coma_pos = strchr(mensaje_completo, ',');                   // Encontrar la posición de la coma en el mensaje

    // Verificar si la coma fue encontrada
    if (coma_pos != NULL) {
        char *parte_numerica = coma_pos + 2;                          // Saltar la coma y el espacio para obtener la parte numérica
        uint32_t numero = strtol(parte_numerica, NULL, 10);           // Convertir la parte numérica en un uint32_t usando strtol
        return numero;
    } else return 0;  // Si no se encontró la coma, devolver 0
}