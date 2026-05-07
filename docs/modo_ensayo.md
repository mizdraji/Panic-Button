# Modo Ensayo (SMS + LoRa)

## Objetivo

El modo ensayo envia un mensaje periodico por **SMS y LoRa** al mismo tiempo, para medir:

- tasa de entrega,
- latencia de cada canal,
- diferencias de recepcion por horario.

## Parametros configurables

En `Sketchup/Panic_Button/configuracion.h`:

- `MODO_ENSAYO`
  - `0`: modo normal
  - `1`: habilita modo ensayo
- `ENSAYO_INTERVALO_MS`
  - intervalo entre envios en ms
  - valor actual: `300000` (5 minutos)
- `ENSAYO_DURACION_HS`
  - duracion total del ensayo
  - valor actual: `48` horas
- `ENSAYO_TOTAL_MENSAJES`
  - calculado automaticamente
  - con 48h y 5 min: `576` mensajes

Calculo:

- `48 horas * 60 min / 5 min = 576`

## Payload de ensayo

Cada envio incluye:

- contador incremental (`1..576`)
- timestamp relativo en milisegundos (`millis()`)

Formatos actuales:

- SMS: `Ens,<contador>,<timestamp_ms>`
- LoRa: `Le,<contador>,<timestamp_ms>`

Ejemplo:

- `Ens,34,10234567`
- `Le,34,10234567`

## Comportamiento

- El contador arranca en `1`.
- Se envia cada 5 minutos.
- Al llegar a `576`, la tarea se deshabilita automaticamente.
- Durante el ensayo se resetea `timer` para evitar sleep.

## Activar / desactivar

1. Editar `MODO_ENSAYO` en `configuracion.h`.
2. Compilar y cargar firmware.
3. Para volver a operacion normal, dejar `MODO_ENSAYO 0`.

## Nota para Node-RED

En servidor, almacenar:

- payload recibido,
- timestamp de recepcion,
- metricas de red (RSSI/SNR u otras).

Luego calcular latencia aproximada:

- `timestamp_recepcion_servidor - timestamp_ms_del_payload`

El `timestamp_ms` es relativo al uptime del nodo (no hora absoluta RTC).

