//  Manejo de memoria EEPROM (Flash)

void initEEPROM() {
  // int pos_mem_[SLOTS_CANTIDAD_MAX]={0};
  // int pos_=0;
  // for (int i = 0; i < SLOTS_CANTIDAD_MAX; ++i) {
  //   pos_mem_[i]=pos_;
  // }
  //Init EEPROM
  if (dbmem) Serial.println("* * * iniciando EEPROM * * *");
  EEPROM.begin(EEPROM_SIZE);

  byte reset_count=EEPROM.read(pos_reset_count);
  if (dbmem) Serial.print("leer reset_count: ");Serial.println(reset_count);
  if (reset_count==255) { //Si reset_count es "FF" limpiar memoria y resetear
    clearEEPROM();
    Serial.print("Reiniciando... ");
    delay(2000);
    ESP.restart();
    delay(2000);
  }
  else {
    writeResetCountToEEPROM(reset_count+1);
  }
  
  if (activacion_manual == 1) {
    nodo.is_activated=1;
    if (dbmem) Serial.println("se usaran las credenciales predefinidas");
  }
  else if (readCredFromEEPROM()==1) {
    nodo.is_activated=1;
  }
  else nodo.is_activated=0;
  if (dbmem) Serial.print("nodo activado: ");Serial.println(nodo.is_activated);

  slot_actual=leerPLGuardadas();
  dir_actual=pos_mem_[slot_actual];
  checkDir();
  dir_CMt_actual=leerCMtGuardados();
  if (dbmem) Serial.printf("Almacenar en direcciones dir_actual: %d dir_CMt_actual: %d \n",dir_actual,dir_CMt_actual);
  char payloadGuardada[PL_CONT_MONO_SIZE] = {0};
  readPLFromEEPROM(0,payloadGuardada); //leer la payload que está en el primer slot para armar el paquete START
  loadPayloadST(payloadGuardada,CMt_corte); //carga la payload START
}

void clearEEPROM() {
  Serial.println("Limpiando memoria EEPROM");
  // apagar el led integrado
  digitalWrite(LED_BUILTIN, HIGH);
  stop_interrupt();
  EEPROM.begin(EEPROM_SIZE);
  // write a 0 to all bytes of the EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  // encender y apagar el led integrado 2 veces
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  EEPROM.end();
  init_interrupt();
  cambio_bloque_PL=1;
}


