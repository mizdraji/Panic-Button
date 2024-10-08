//  Manejo de memoria EEPROM (Flash)

void initEEPROM() {
  int pos_mem_[SLOTS_CANTIDAD_MAX]={0};
  int pos_=0;
  for (int i = 0; i < SLOTS_CANTIDAD_MAX; ++i) {
     pos_mem_[i]=pos_;
   }
  //Init EEPROM
  if (dbmem) Serial.println("* * * iniciando EEPROM * * *");
  EEPROM.begin(EEPROM_SIZE);

  byte reset_count=EEPROM.read(pos_reset_count);
  if (dbmem) { Serial.print("leer reset_count: ");Serial.println(reset_count); }
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
  if (dbmem) { Serial.print("nodo activado: ");Serial.println(nodo.is_activated); }

  dir_actual=pos_mem_[slot_actual];

  //dir_CMt_actual=leerCMtGuardados();
  //if (dbmem) Serial.printf("Almacenar en direcciones dir_actual: %d dir_CMt_actual: %d \n",dir_actual,dir_CMt_actual);
  //char payloadGuardada[PL_CONT_MONO_SIZE] = {0};
  //readPLFromEEPROM(0,payloadGuardada); //leer la payload que está en el primer slot para armar el paquete START
  //loadPayloadST(payloadGuardada,CMt_corte); //carga la payload START
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

//Esta funcion compara el byte en la posición de memoria con el que se va a escribir, si no es igual lo sobreescribe.
void updateEEPROM(int addr, byte dato_in) {
  byte dato_leido = EEPROM.read(addr);
  if (dato_leido!=dato_in) {
    EEPROM.write(addr, dato_in);
  }
}

void writeResetCountToEEPROM(byte reset_count_) {
  stop_interrupt();
  updateEEPROM(pos_reset_count,reset_count_);
  EEPROM.commit();
  init_interrupt();
}

//Esta funcion lee el bloque de credenciales, devuelve el bit de activacion
byte readCredFromEEPROM() {
  byte bit_act_=EEPROM.read(pos_bit_act);
  if (bit_act_==0x01) {
    nodo.is_activated=1;
    byte length_=EEPROM.read(pos_devID);
    char devID_out_[4]={0};
    for (int i = 0; i < length_-1; i++)  {
      devID_out_[i]=EEPROM.read(pos_devID + 1 + i);
    }
    devID_AA = (unsigned long)devID_out_[0] << 24 | (unsigned long)devID_out_[1] << 16 | (unsigned long)devID_out_[2] << 8 | devID_out_[3];
    Serial.print("devID leido: "); Serial.println(devID_AA);
    length_=EEPROM.read(pos_devAddr);
    //char devAddr_out_[8]={0};
    for (int i = 0; i < length_-1; i++)  {
      devAddr_AA[i]=EEPROM.read(pos_devAddr + 1 + i);
    }
    Serial.print("devAddr leido: "); Serial.println(devAddr_AA);
    //for (int i = 0; i < 8; ++i) Serial.print(devAddr[i]); Serial.println(" ");
  }
  return bit_act_;
}

//Esta funcion lee el bloque CMt escrito en la posición de memoria addrOffset y lo guarda en el array data_out_. 
void readCMtFromEEPROM(int addrOffset, char * data_out_) {
  byte length_=EEPROM.read(addrOffset);
  for (int i = 0; i < length_; i++)  {
    data_out_[i]=EEPROM.read(addrOffset + 1 + i);
  }
}

//Esta funcion sobreescribe los datos en addr, no agrega bytes adicionales.
void overWriteEEPROM(int addr, char data_in_[], byte length_) {
  stop_interrupt();
  for (int i = 0; i < length_; i++)  {
    updateEEPROM(addr + i, data_in_[i]);
  }
  EEPROM.commit();
  init_interrupt();
}
