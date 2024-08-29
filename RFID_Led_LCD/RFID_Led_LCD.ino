#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C
#include <SoftwareSerial.h>
#include <SPI.h>			// incluye libreria bus SPI


const int BUFFER_SIZE = 14; // RFID DATA FRAME FORMAT: 1byte head (value: 2), 10byte data (2byte version + 8byte tag), 2byte checksum, 1byte tail (value: 3)
const int DATA_SIZE = 10; // 10byte data (2byte version + 8byte tag)
const int DATA_VERSION_SIZE = 2; // 2byte version (actual meaning of these two bytes may vary)
const int DATA_TAG_SIZE = 8; // 8byte tag
const int CHECKSUM_SIZE = 2; // 2byte checksum

int led1 = 7; // verde
int led2 = 5; // azul
int led3 = 6; // rojo
String txtld1 = "";
String txtld2 = "";
String txtld3 = "";
int user = 15672711;
int cnt = 0;

long lasttag;

SoftwareSerial ssrfid = SoftwareSerial(10,11); 
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

uint8_t buffer[BUFFER_SIZE]; // used to store an incoming data frame 
int buffer_index = 0;
int buzzer = 13;

void setup() {
  Serial.begin(9600); 
  ssrfid.begin(9600);
  ssrfid.listen(); 

  lcd.setBacklightPin(3,POSITIVE);	// puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH);		// habilita iluminacion posterior de LCD
  lcd.begin(16, 2);			// 16 columnas por 2 lineas para LCD 1602A
  lcd.clear();  // limpiar pantalla
  lcd.setCursor(0, 0);	// ubica cursor en columna 0 y linea 0
  lcd.print("Laboratorio HLM");	// Escribir en pantalla

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
}

void loop() {
  if (ssrfid.available() > 0){
    bool call_extract_tag = false;
    
    int ssvalue = ssrfid.read(); // read 
    if (ssvalue == -1) { // no data was read
      return;
    }

    if (ssvalue == 2) { // RDM630/RDM6300 found a tag => tag incoming 
      buffer_index = 0;
    } else if (ssvalue == 3) { // tag has been fully transmitted       
      call_extract_tag = true; // extract tag at the end of the function call
    }

    if (buffer_index >= BUFFER_SIZE) { // checking for a buffer overflow (It's very unlikely that an buffer overflow comes up!)
      Serial.println("Error: Buffer overflow detected! ");
      return;
    }
    
    buffer[buffer_index++] = ssvalue; // everything is alright => copy current value to buffer

    if (call_extract_tag == true) {
      if (buffer_index == BUFFER_SIZE) {
        unsigned tag = extract_tag();
      } else { // something is wrong... start again looking for preamble (value: 2)
        buffer_index = 0;
        cnt= 0;
        Serial.println(cnt);
        return;
      }
    }    
  }    
}

unsigned extract_tag() {
    uint8_t msg_head = buffer[0];
    uint8_t *msg_data = buffer + 1; // 10 byte => data contains 2byte version + 8byte tag
    uint8_t *msg_data_version = msg_data;
    uint8_t *msg_data_tag = msg_data + 2;
    uint8_t *msg_checksum = buffer + 11; // 2 byte
    uint8_t msg_tail = buffer[13];

    long tag = hexstr_to_value(msg_data_tag, DATA_TAG_SIZE);
    if(tag == lasttag){

    }
    else{
      Serial.print("Extracted Tag: ");
      Serial.println(tag);
      if(comparaUID(tag)){
        turnONLed(led1,"Autenticacion","Hola EthicalHCOP");
      }else{
        turnONLed(led3,"Autenticacion","Usuario invalido");
      }	
    }
    
    
    lasttag = tag;
    
    long checksum = 0;
    for (int i = 0; i < DATA_SIZE; i+= CHECKSUM_SIZE) {
      long val = hexstr_to_value(msg_data + i, CHECKSUM_SIZE);
      checksum ^= val;
    }
       
    return tag;

}

void turnONLed(int ld, String txt1, String txt2){
  digitalWrite(led2, LOW);
  digitalWrite(ld, HIGH);
  printScreen(txt1,txt2);
  delay(1500);
  digitalWrite(ld, LOW);
  digitalWrite(led2, HIGH);
  printScreen("Laboratorio HLM"," ");
}

void printScreen(String txt1, String txt2){
  lcd.clear();
  lcd.setCursor(0, 0);	// ubica cursor en columna 0 y linea 0
  lcd.print(txt1+":");	// Escribir en pantalla
  lcd.setCursor(0, 1);	
  lcd.print(txt2);
}

boolean comparaUID(int tag){	// funcion comparaID
  if(tag != user){
    return(false);
  }
  return(true);	
}

long hexstr_to_value(char *str, unsigned int length) { // converts a hexadecimal value (encoded as ASCII string) to a numeric value
  char* copy = malloc((sizeof(char) * length) + 1); 
  memcpy(copy, str, sizeof(char) * length);
  copy[length] = '\0'; 
  // the variable "copy" is a copy of the parameter "str". "copy" has an additional '\0' element to make sure that "str" is null-terminated.
  long value = strtol(copy, NULL, 16);  // strtol converts a null-terminated string to a long value
  free(copy); // clean up 
  return value;
}
