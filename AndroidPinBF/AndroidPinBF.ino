#include <Keyboard.h>

int led = 2;

void typeKey(int key){
  Keyboard.press(key);
  delay(50);
  Keyboard.release(key);
}

void setup(){
  pinMode(led, OUTPUT);
}

void loop() {
  Keyboard.begin();
  delay(3000);
  Keyboard.releaseAll();
  int temp;
  for(int i = 0; i < 5; i++){
    digitalWrite(led, HIGH);
    temp = random(0000, 9999);
    if(String(temp).length() == 3){
      String val = "0";
      Keyboard.print(val+""+String(temp)); 
    }else{
      Keyboard.print(temp); 
    }
    delay(200);
    typeKey(KEY_RETURN);
    digitalWrite(led, LOW);
    delay(300);
  }
  Keyboard.releaseAll();
  Keyboard.end();
  delay(32000);
}

