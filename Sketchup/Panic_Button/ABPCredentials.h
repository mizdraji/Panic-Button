//ABP Credentials para comunicación LoRa

unsigned int devID_AA = 0;   //devID para Activacion Automatica
char devAddr_AA[9] = "0118e299"; //devAddr para Activacion Automatica

//Activación
#define activacion_manual 1 // 1: manual, 0: automático

#if activacion_manual == 0
  const char *nwkSKey = "d8e5a8e01bddfaf48e059992119be745";
  const char *appSKey = "3f19e1ef0da5286e178c5b84fc9e23ae";

//////////////////////////////////////////////////////////

#elif activacion_manual == 1

#define devID 1

  #if devID == 1        //Dispositivo PB00001
  const char *devAddr = "0149cf9b";
  const char *nwkSKey = "d8e5a8e01bddfaf48e059992119be745";
  const char *appSKey = "3f19e1ef0da5286e178c5b84fc9e23ae";
  #endif

#endif