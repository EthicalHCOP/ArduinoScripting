#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C
#include <SPI.h>			// incluye libreria bus SPI
#include <MFRC522.h>			// incluye libreria especifica para MFRC522

#define RST_PIN  9			// constante para referenciar pin de reset
#define SS_PIN  10			// constante para referenciar pin de slave select
int led1 = 7; // verde
int led2 = 5; // azul
int led3 = 6; // rojo
String txtld1 = "";
String txtld2 = "";
String txtld3 = "";

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

MFRC522 mfrc522(SS_PIN, RST_PIN);	// crea objeto mfrc522 enviando pines de slave select y reset

byte LecturaUID[4]; 				// crea array para almacenar el UID leido
byte Usuario1[4]= {0x63, 0x63, 0x31, 0xFF} ;    // UID de tarjeta leido en programa 1
byte Usuario2[4]= {0xE6, 0x8B, 0x82, 0x3D} ;    // UID de llavero leido en programa 1

void setup() {

  while (!Serial);
  Serial.begin(9600);			// inicializa comunicacion por monitor serie a 9600 bps
  SPI.begin();				// inicializa bus SPI
  mfrc522.PCD_Init();			// inicializa modulo lector
  Serial.println("Listo");		// Muestra texto Listo

  lcd.setBacklightPin(3,POSITIVE);	// puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH);		// habilita iluminacion posterior de LCD
  lcd.begin(16, 2);			// 16 columnas por 2 lineas para LCD 1602A
  lcd.clear();  // limpiar pantalla
  lcd.setCursor(0, 0);	// ubica cursor en columna 0 y linea 0
  lcd.print("Laboratorio HLM");	// Escribir en pantalla

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  digitalWrite(led2, HIGH);
}


void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent())		// si no hay una tarjeta presente
    return;						// retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial()) 		// si no puede obtener datos de la tarjeta
    return;						// retorna al loop esperando por otra tarjeta
    
  //Serial.print("UID:");				// muestra texto UID:
  for (byte i = 0; i < mfrc522.uid.size; i++) {	// bucle recorre de a un byte por vez el UID
    if (mfrc522.uid.uidByte[i] < 0x10){		// si el byte leido es menor a 0x10
      //Serial.print(" 0");				// imprime espacio en blanco y numero cero
    }
    else{						// sino
      //Serial.print(" ");				// imprime un espacio en blanco
    }
    //Serial.print(mfrc522.uid.uidByte[i], HEX);   	// imprime el byte del UID leido en hexadecimal
    LecturaUID[i]=mfrc522.uid.uidByte[i];   	// almacena en array el byte del UID leido      
  }
          
  //Serial.print("\n");   			// imprime un espacio de tabulacion             
                    
  if(comparaUID(LecturaUID, Usuario1)){
    turnONLed(led1,"Autenticacion","Hola EthicalHCOP");
  }		
  else if(comparaUID(LecturaUID, Usuario2)){
    turnONLed(led1,"Autenticacion","Hola Anguss");
  }	
  else{
    turnONLed(led3,"Autenticacion","Usuario invalido");
  }										         
  mfrc522.PICC_HaltA();  		// detiene comunicacion con tarjeta
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

boolean comparaUID(byte lectura[],byte usuario[]){	// funcion comparaUID
  for (byte i=0; i < mfrc522.uid.size; i++){		// bucle recorre de a un byte por vez el UID
    if(lectura[i] != usuario[i]){
      return(false);
    }			
  }
  return(true);						// si los 4 bytes coinciden retorna verdadero
}