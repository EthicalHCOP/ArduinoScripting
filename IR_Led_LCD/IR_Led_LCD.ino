#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C
#include "IRLremote.h"

#define pinIR 2
int led1 = 7; // verde
int led2 = 5; // azul
int led3 = 6; // rojo
String txtld1 = "";
String txtld2 = "";
String txtld3 = "";

CNec IRLremote;
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

void setup()
{
  while (!Serial);
  Serial.begin(9600);
  Serial.println(F("Startup"));

  lcd.setBacklightPin(3,POSITIVE);	// puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH);		// habilita iluminacion posterior de LCD
  lcd.begin(16, 2);			// 16 columnas por 2 lineas para LCD 1602A
  lcd.clear();  // limpiar pantalla
  lcd.setCursor(0, 0);	// ubica cursor en columna 0 y linea 0
  lcd.print("Laboratorio HLM");	// Escribir en pantalla

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  if (!IRLremote.begin(pinIR))
    Serial.println(F("You did not choose a valid pin."));

}

void loop(){
  if (IRLremote.available())
  {
    auto data = IRLremote.read();
    if(data.command == 69){
      txtld1 = validateLed(led1, "V");
      printScreen("Colores",txtld1,0);
    }else if(data.command == 70){
      txtld2 = validateLed(led2, "A");
      printScreen("Colores",txtld2,1);
    }else if(data.command == 71){
      txtld3 = validateLed(led3, "R");
      printScreen("Colores",txtld3,2);
    } 
  }
}

String validateLed(int led , String ltr){
  String txtl = "";
  if(digitalRead(led) == LOW){
    digitalWrite(led, HIGH);
    txtl = ltr;
    Serial.println("Color "+ltr+" encendido.");
  }else{
    digitalWrite(led, LOW);
    txtl = " ";
    Serial.println("Color "+ltr+" apagado.");
  }
  return txtl;
}

void printScreen(String txt1, String txt2,int pos){
  lcd.setCursor(0, 0);	// ubica cursor en columna 0 y linea 0
  lcd.print(txt1+":        ");	// Escribir en pantalla
  lcd.setCursor(pos, 1);	
  lcd.print(txt2);
}