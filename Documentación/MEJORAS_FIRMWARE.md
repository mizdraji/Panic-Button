# Documentación de Mejoras - Firmware Panic Button

## Índice
1. [Mejoras de Seguridad y Confiabilidad](#mejoras-de-seguridad-y-confiabilidad)
2. [Mejoras de Funcionalidad](#mejoras-de-funcionalidad)
3. [Mejoras de Rendimiento y Optimización](#mejoras-de-rendimiento-y-optimización)
4. [Mejoras de Gestión de Energía](#mejoras-de-gestión-de-energía)
5. [Mejoras de Comunicación](#mejoras-de-comunicación)
6. [Mejoras de Interfaz de Usuario](#mejoras-de-interfaz-de-usuario)
7. [Mejoras de Mantenibilidad y Código](#mejoras-de-mantenibilidad-y-código)
8. [Mejoras de Diagnóstico y Monitoreo](#mejoras-de-diagnóstico-y-monitoreo)

---

## Mejoras de Seguridad y Confiabilidad

### 1. Sistema de Reintentos Inteligente para Mensajes
**Problema actual:** Si un mensaje no se envía correctamente o no se recibe confirmación, no hay reintentos automáticos.

**Mejora propuesta:**
- Implementar un sistema de reintentos con backoff exponencial
- Máximo 3-5 reintentos con intervalos crecientes (1s, 2s, 4s, 8s)
- Diferentes estrategias para SMS y LoRaWAN
- Marcar mensajes como "fallidos" después de agotar reintentos
- Almacenar mensajes fallidos en EEPROM para reenvío posterior

**Beneficios:**
- Mayor confiabilidad en la entrega de alertas críticas
- Mejor manejo de condiciones de red intermitentes
- Trazabilidad de mensajes no entregados

### 2. Validación de Confirmaciones de Envío
**Problema actual:** Los LEDs se encienden al intentar enviar, pero no verifican si el SIM800 respondió "OK" o "ERROR".

**Mejora propuesta:**
- Parsear respuesta del SIM800 después de cada comando AT
- Encender LED sólido solo cuando se recibe "OK"
- Parpadear LED cuando se recibe "ERROR"
- Reintentar envío automáticamente si hay error
- Log de errores en EEPROM para diagnóstico

**Implementación sugerida:**
```cpp
bool Enviar_msj_validado(String numero, String msj) {
  String respuesta = "";
  // ... código de envío ...
  // Esperar respuesta y validar
  unsigned long timeout = millis() + 10000; // 10 segundos timeout
  while (millis() < timeout && !respuesta.endsWith("OK")) {
    if (SIM800L.available()) {
      respuesta += SIM800L.readString();
      if (respuesta.indexOf("ERROR") != -1) {
        return false; // Error detectado
      }
    }
    delay(50);
  }
  return respuesta.endsWith("OK");
}
```

### 3. Sistema de Watchdog Timer (WDT)
**Problema actual:** Si el sistema se bloquea, no hay recuperación automática.

**Mejora propuesta:**
- Implementar watchdog timer del ESP32
- Reset automático si no se alimenta el watchdog en 30-60 segundos
- Almacenar estado crítico en EEPROM antes de reset
- Recuperar estado después del reset

**Beneficios:**
- Recuperación automática de bloqueos
- Mayor disponibilidad del sistema
- Diagnóstico de problemas recurrentes

### 4. Validación de Integridad de Mensajes
**Problema actual:** No hay verificación de integridad de mensajes recibidos.

**Mejora propuesta:**
- Agregar checksum CRC a mensajes LoRaWAN
- Validar formato de mensajes antes de procesar
- Ignorar mensajes malformados con log
- Implementar timeout para mensajes incompletos

---

## Mejoras de Funcionalidad

### 5. Implementación de Timestamp Epoch
**Problema actual:** Según `Solicitud de cambios para firmware.txt`, se requiere incluir timestamp epoch en los mensajes.

**Mejora propuesta:**
- Obtener timestamp del servidor en mensajes de confirmación
- Sincronizar reloj interno con timestamp del servidor
- Incluir timestamp en todos los mensajes salientes
- Formato: `"PB, policia, 1726703523, PB00001, fv:1.8.7, hv:1.0.0"`

**Implementación:**
```cpp
// Sincronizar tiempo desde servidor
void sincronizar_tiempo(uint32_t timestamp_servidor) {
  // Almacenar offset entre tiempo local y servidor
  tiempo_offset = timestamp_servidor - (millis() / 1000);
}

uint32_t obtener_timestamp() {
  return (millis() / 1000) + tiempo_offset;
}
```

### 6. Sistema de Almacenamiento en EEPROM
**Problema actual:** Pendiente según `Pendientes.md` - se empezó en versión 1.9 pero no se completó.

**Mejora propuesta:**
- Almacenar configuración del dispositivo
- Guardar número de serie del dispositivo
- Almacenar mensajes pendientes de envío
- Guardar estadísticas de uso (contadores)
- Backup de estado antes de deep sleep

**Estructura sugerida:**
```cpp
struct EEPROM_Data {
  uint32_t numero_serie;
  char version_firmware[10];
  char version_hardware[10];
  uint32_t contador_policia;
  uint32_t contador_bomberos;
  uint32_t contador_medica;
  uint32_t ultimo_timestamp;
  // ... más campos
};
```

### 7. Sistema de Activación y Configuración Remota
**Problema actual:** La configuración requiere modificar código y recompilar.

**Mejora propuesta:**
- Comandos AT personalizados para configuración
- Configuración vía SMS con PIN de seguridad
- Cambio de números de teléfono remoto
- Ajuste de parámetros LoRaWAN
- Modo de configuración con secuencia de botones

**Comandos sugeridos:**
- `CONFIG, PIN, NUMERO, +5491234567890` - Cambiar número
- `CONFIG, LORA, SF, 7` - Cambiar Spreading Factor
- `STATUS` - Obtener estado del dispositivo

### 8. Sistema de Monitoreo de Batería
**Problema actual:** No hay monitoreo de nivel de batería.

**Mejora propuesta:**
- Leer voltaje de batería mediante ADC
- Calcular nivel de batería (0-100%)
- Enviar alerta cuando batería < 20%
- Secuencia de LED para indicar batería baja
- Ajustar comportamiento según nivel de batería (reducir frecuencia de envío)

**Implementación:**
```cpp
uint8_t leer_nivel_bateria() {
  int adc_value = analogRead(ADC_BATTERY);
  float voltage = (adc_value / 4095.0) * 3.3 * 2; // Asumiendo divisor 1:1
  // Mapear voltaje a porcentaje según características de batería
  if (voltage < 3.0) return 0;
  if (voltage > 4.2) return 100;
  return map(voltage * 100, 300, 420, 0, 100);
}
```

### 9. Indicador de Cobertura de Red
**Problema actual:** No hay forma de saber si hay cobertura GSM o LoRaWAN.

**Mejora propuesta:**
- Consultar calidad de señal GSM (AT+CSQ)
- Verificar estado de red LoRaWAN
- Secuencia de LED para indicar cobertura
- Blink rápido = buena cobertura, lento = mala cobertura
- Enviar alerta si no hay cobertura por X minutos

---

## Mejoras de Rendimiento y Optimización

### 10. Optimización del Manejo de Interrupciones
**Problema actual:** Las interrupciones pueden generar múltiples llamadas.

**Mejora propuesta:**
- Mejorar filtrado de rebotes en hardware (si es posible)
- Usar cola de eventos para interrupciones
- Procesar interrupciones en tareas de baja prioridad
- Deshabilitar interrupciones durante operaciones críticas

**Implementación:**
```cpp
QueueHandle_t interruptQueue;

void IRAM_ATTR buttonInterrupt1() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200) {
    int button = 1;
    xQueueSendFromISR(interruptQueue, &button, NULL);
  }
  last_interrupt_time = interrupt_time;
}
```

### 11. Optimización del Buffer de Serial
**Problema actual:** Buffer de SIM800 puede ser insuficiente para mensajes largos.

**Mejora propuesta:**
- Aumentar tamaño de buffer de SoftwareSerial
- Implementar buffer circular para datos entrantes
- Procesar mensajes en chunks si son muy largos
- Limpiar buffer periódicamente

### 12. Gestión Mejorada de Memoria
**Problema actual:** Uso de Strings puede causar fragmentación de memoria.

**Mejora propuesta:**
- Reemplazar Strings por arrays de char donde sea posible
- Usar PROGMEM para strings constantes
- Implementar pool de memoria para mensajes
- Monitorear memoria libre disponible

### 13. Optimización del Loop Principal
**Problema actual:** El loop puede tener bloqueos con `delay()` implícitos.

**Mejora propuesta:**
- Eliminar todos los `delay()` del loop
- Usar máquina de estados para operaciones asíncronas
- Procesar SIM800 de forma no bloqueante
- Timeouts basados en millis() en lugar de delay()

---

## Mejoras de Gestión de Energía

### 14. Modo Sleep Mejorado
**Problema actual:** El deep sleep funciona pero podría optimizarse más.

**Mejora propuesta:**
- Diferentes niveles de sleep según situación
- Light sleep para operaciones rápidas
- Deep sleep con wakeup por múltiples fuentes
- Calcular tiempo de sleep dinámicamente
- Despertar periódicamente para verificar estado

**Implementación:**
```cpp
void entrar_modo_sleep_optimizado() {
  // Apagar periféricos no esenciales
  WiFi.mode(WIFI_OFF);
  btStop();
  
  // Configurar wakeup sources
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_sleep_enable_timer_wakeup(3600 * 1000000ULL); // 1 hora
  
  // Guardar estado crítico
  guardar_estado_EEPROM();
  
  esp_deep_sleep_start();
}
```

### 15. Gestión Inteligente del SIM800
**Problema actual:** El SIM800 consume mucha energía incluso en sleep.

**Mejora propuesta:**
- Usar modo mínimo (AT+CFUN=0) cuando no se necesita
- Desactivar completamente cuando no hay cobertura
- Encender solo cuando se necesita enviar
- Monitorear consumo y ajustar estrategia

### 16. Ajuste Dinámico de Potencia LoRa
**Problema actual:** Potencia de transmisión LoRa es fija.

**Mejora propuesta:**
- Ajustar potencia según calidad de señal
- Reducir potencia si hay buena señal (ahorra energía)
- Aumentar potencia solo cuando sea necesario
- Balance entre alcance y consumo

---

## Mejoras de Comunicación

### 17. Protocolo de Mensajes Estructurado
**Problema actual:** Mensajes son strings simples sin estructura clara.

**Mejora propuesta:**
- Definir protocolo JSON o binario estructurado
- Campos: tipo, timestamp, idempotencia, datos
- Validación de formato
- Versión de protocolo para compatibilidad

**Ejemplo:**
```json
{
  "app": "PB",
  "cmd": "policia",
  "timestamp": 1726703523,
  "serial": "PB00001",
  "fw": "1.8.7",
  "hw": "1.0.0"
}
```

### 18. Compresión de Mensajes LoRaWAN
**Problema actual:** Mensajes pueden ser largos y consumir muchos créditos de aire.

**Mejora propuesta:**
- Codificar comandos como bytes únicos
- Usar códigos cortos para mensajes comunes
- Reducir tamaño de idempotencia si es posible
- Maximizar uso del payload disponible

### 19. Sistema de Priorización de Mensajes
**Problema actual:** Todos los mensajes tienen la misma prioridad.

**Mejora propuesta:**
- Prioridad alta: alertas de emergencia
- Prioridad media: confirmaciones
- Prioridad baja: estadísticas, heartbeat
- Cola de mensajes con prioridad
- Enviar mensajes prioritarios primero

### 20. Heartbeat y Estado del Dispositivo
**Problema actual:** No hay forma de saber si el dispositivo está funcionando.

**Mejora propuesta:**
- Enviar heartbeat periódico (cada 24 horas)
- Incluir estado de batería, cobertura, contadores
- Servidor puede detectar dispositivos inactivos
- Alertar si no se recibe heartbeat en X tiempo

---

## Mejoras de Interfaz de Usuario

### 21. Secuencias de LED Mejoradas
**Problema actual:** Las secuencias de LED podrían ser más informativas.

**Mejora propuesta:**
- Diferentes patrones para cada estado
- Código de colores estándar (rojo=error, verde=ok, amarillo=advertencia)
- Secuencia de diagnóstico al iniciar
- Indicador de modo (normal/configuración/sleep)

**Patrones sugeridos:**
- 1 blink = mensaje enviado
- 2 blinks = confirmación recibida
- 3 blinks = alerta atendida
- Blink rápido = error
- Blink lento = batería baja

### 22. Feedback Táctil (si hay espacio)
**Problema actual:** Solo feedback visual.

**Mejora propuesta:**
- Agregar buzzer piezoeléctrico si es posible
- Diferentes tonos para diferentes eventos
- Vibración si hay motor disponible
- Confirmación audible de envío

### 23. Modo de Diagnóstico
**Problema actual:** Difícil diagnosticar problemas sin Serial.

**Mejora propuesta:**
- Modo diagnóstico con secuencia de botones
- Enviar reporte de diagnóstico por SMS
- LED muestra códigos de error
- Información de estado en EEPROM

---

## Mejoras de Mantenibilidad y Código

### 24. Modularización del Código
**Problema actual:** Código está distribuido pero podría estar mejor organizado.

**Mejora propuesta:**
- Separar lógica de negocio de hardware
- Clases para manejo de SIM800
- Clases para manejo de LoRaWAN
- Clases para gestión de LEDs
- Facilita testing y mantenimiento

**Estructura sugerida:**
```
Panic_Button/
├── src/
│   ├── Communication/
│   │   ├── SIM800.h/cpp
│   │   └── LoRaWAN.h/cpp
│   ├── Hardware/
│   │   ├── Buttons.h/cpp
│   │   ├── LEDs.h/cpp
│   │   └── Battery.h/cpp
│   ├── System/
│   │   ├── Config.h/cpp
│   │   ├── EEPROM_Manager.h/cpp
│   │   └── Watchdog.h/cpp
│   └── Utils/
│       ├── Timestamp.h/cpp
│       └── MessageParser.h/cpp
```

### 25. Sistema de Logging
**Problema actual:** Solo Serial.println para debugging.

**Mejora propuesta:**
- Sistema de logging con niveles (DEBUG, INFO, WARN, ERROR)
- Logging a EEPROM para diagnóstico offline
- Envío de logs críticos al servidor
- Rotación de logs para no llenar memoria

**Implementación:**
```cpp
enum LogLevel { DEBUG, INFO, WARN, ERROR };

void log(LogLevel level, const char* message) {
  #ifdef DEBUG_MODE
    Serial.print("[");
    Serial.print(level);
    Serial.print("] ");
    Serial.println(message);
  #endif
  
  if (level >= WARN) {
    // Guardar en EEPROM o enviar al servidor
  }
}
```

### 26. Configuración Centralizada
**Problema actual:** Configuración dispersa en múltiples archivos.

**Mejora propuesta:**
- Archivo único de configuración
- Valores por defecto claros
- Documentación de cada parámetro
- Validación de configuración al iniciar

### 27. Manejo de Errores Robusto
**Problema actual:** Algunos errores no se manejan adecuadamente.

**Mejora propuesta:**
- Try-catch donde sea posible
- Códigos de error estándar
- Recuperación automática cuando sea posible
- Fallback a modo seguro en errores críticos

---

## Mejoras de Diagnóstico y Monitoreo

### 28. Estadísticas de Uso
**Problema actual:** No se registran estadísticas de uso.

**Mejora propuesta:**
- Contadores de mensajes enviados/recibidos
- Tasa de éxito de envíos
- Tiempo promedio de respuesta
- Historial de eventos en EEPROM

### 29. Telemetría del Dispositivo
**Problema actual:** No se envía información del estado del dispositivo.

**Mejora propuesta:**
- Incluir en heartbeat: temperatura, voltaje, uptime
- Estadísticas de red (calidad señal, reintentos)
- Estado de componentes (SIM800, LoRa)
- Alertas proactivas de problemas

### 30. Sistema de Actualización OTA (Over-The-Air)
**Problema actual:** Actualizar firmware requiere acceso físico.

**Mejora propuesta:**
- Actualización vía LoRaWAN o WiFi (si disponible)
- Verificación de integridad del firmware
- Rollback automático si falla
- Notificación de nueva versión disponible

---

## Priorización de Mejoras

### Alta Prioridad (Crítico para funcionamiento)
1. ✅ Validación de confirmaciones de envío (#2)
2. ✅ Sistema de reintentos (#1)
3. ✅ Watchdog Timer (#3)
4. ✅ Timestamp Epoch (#5)
5. ✅ Monitoreo de batería (#8)

### Media Prioridad (Mejora significativa)
6. ✅ Almacenamiento EEPROM (#6)
7. ✅ Indicador de cobertura (#9)
8. ✅ Optimización de interrupciones (#10)
9. ✅ Modo sleep mejorado (#14)
10. ✅ Secuencias de LED mejoradas (#21)

### Baja Prioridad (Nice to have)
11. ✅ Configuración remota (#7)
12. ✅ Heartbeat (#20)
13. ✅ Logging (#25)
14. ✅ Estadísticas (#28)
15. ✅ OTA (#30)

---

## Consideraciones de Implementación

### Testing
- Probar cada mejora de forma aislada
- Testing en condiciones reales (baja señal, batería baja)
- Pruebas de estrés (múltiples pulsaciones rápidas)
- Validar consumo de energía después de cada cambio

### Compatibilidad
- Mantener compatibilidad con versiones anteriores cuando sea posible
- Versionar protocolo de mensajes
- Documentar cambios breaking

### Documentación
- Actualizar README con nuevas funcionalidades
- Documentar comandos de configuración
- Guía de troubleshooting
- Diagramas de flujo de operación

---

## Notas Finales

Esta documentación proporciona un roadmap completo para mejorar el firmware del Panic Button. Se recomienda implementar las mejoras de forma incremental, probando cada una antes de continuar con la siguiente.

Para cada mejora, se debe:
1. Analizar el impacto en el código existente
2. Estimar el esfuerzo de implementación
3. Probar en entorno de desarrollo
4. Validar en condiciones reales
5. Documentar cambios

**Versión del documento:** 1.0  
**Fecha:** 2024  
**Firmware base:** V1.8.7

