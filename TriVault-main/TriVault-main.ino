#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <SPI.h>
#include <RFID.h>
#include <EEPROM.h>
#define SS_PIN 53
#define RST_PIN 2
RFID rfid(SS_PIN, RST_PIN);

String rfidCard;

Servo servo;
String password = "5747";
int pass_count = 4;
int pos = 0;
int false_attempt = 0;
int temp=0;
int temp1=0;
String cmd;
String entered_password = "";
String eeprompassword;

const byte rows = 4;
const byte cols = 4;

char keyMap [rows] [cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins [rows] = {36,34,32,30};
byte colPins [cols] = {28,26,24,22};

Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, rows, cols);

const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

void(* resetFunc)(void) = 0;
unsigned long currtime;
unsigned long runtime;

void setup() {

  password = readStringFromEEPROM(0);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  lcd.begin(16, 2);
  Serial.begin(9600);
  Serial2.begin(38400);
  Serial3.begin(9600);
  SPI.begin();
  rfid.init();

  servo.attach(3);
  servo.write(104);

  delay(1000);
}

void loop() {

  Rc522_rfid();

}

void Rc522_rfid(){
  if(temp1 == 0){
    temp1++;
    lcd.clear();
    lcd.print("Scan 1st Card :");

  }
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      // Serial.println(rfidCard);
      if (rfidCard == "67 224 4 25") {
        currtime = runtime = millis();
        while((runtime - currtime)/1000 <= 15){ // 15 secs
          runtime = millis();
          char whichKey = myKeypad.getKey();
          if(whichKey == '#'){
            temp1=0;
            break;
          }

          if(temp1 == 1){
            temp1++;
            lcd.clear();
            lcd.print("1st card Verifired");
            delay(1500);
            for(int positionCount = 0; positionCount < 2; positionCount++){
              lcd.scrollDisplayLeft();
              delay(500);
            }
            delay(2000);
            lcd.clear();
            lcd.print("Scan 2nd Card :");

          }
          if (rfid.isCard()) {
            if (rfid.readCardSerial()) {
              rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
              Serial.println(rfidCard);
              if (rfidCard == "156 3 32 74") {
                temp1=0;
                lcd.clear();
                lcd.print("2nd Card Verified");
                delay(1500);
                for(int positionCount = 0; positionCount < 2; positionCount++){
                  lcd.scrollDisplayLeft();
                  delay(500);
                }
                delay(2000);
                currtime = runtime = millis();
                while((runtime - currtime)/1000 <= 30){
                  runtime = millis();
                  passwordCheck();
                }
                temp=0;
              } else {
                lcd.clear();
                lcd.print("2nd card InValid");
                delay(2000);
                temp1=0;
                break;
              }
            }
            rfid.halt();
          }
          temp1=0;
        }
      } else {
        lcd.clear();
        lcd.print("1st card InValid");
        delay(2000);
        temp1=0;
      }
    }
    rfid.halt();
  }
}

void resetPassword(){
  pass_count=0;
  password = "";
  lcd.clear();
  lcd.print("Enter New PassKey");
  lcd.setCursor(0,1);
  for(;;){
    char whichKey = myKeypad.getKey();
    if(whichKey){
      if(whichKey == 'D'){
        break;
      }
      password+=whichKey;

      lcd.print(whichKey);
      pass_count++;
      delay(100);
      
    }
  }
  writeStringToEEPROM(0,password);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Password Reset");
  lcd.setCursor(0,1);
  lcd.print("SucessFully");
  Serial3.println("AT+CMGF=1");    
  delay(1000);  
  Serial3.println("AT+CMGS=\"+919156545747\"\r"); 
  delay(1000);
  String str4 = "Password Reset SucessFully.";
  Serial3.println(str4);
  delay(1000);
  Serial3.println((char)26);
  delay(1000);
  false_attempt=0;
  temp=0;
  delay(2000);
  lcd.clear();
}

void opendoor(int k) {
  if (k) {
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Opening Door");
    Serial3.println("AT+CMGF=1");    
    delay(1000);  
    Serial3.println("AT+CMGS=\"+919156545747\"\r"); 
    delay(1000);
    String str4 = "Door opened sucessfully.";
    Serial3.println(str4);
    delay(1000);
    Serial3.println((char)26);
    delay(1000);
    servo.write(14);
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Door Opened");
    digitalWrite(13, HIGH);
    delay(2000);
  }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press * to close");
    for(;;){
        char whichKey = myKeypad.getKey(); 
        
        if(whichKey){
          if(whichKey == '*'){
            lcd.clear();
            digitalWrite(13, LOW);
            lcd.setCursor(0, 0);
            lcd.print("Closing Door");
            Serial3.println("AT+CMGF=1");    
            delay(1000);  
            Serial3.println("AT+CMGS=\"+919156545747\"\r"); 
            delay(1000);
            String str4 = "Door closed sucessfully.";
            Serial3.println(str4);
            delay(1000);
            Serial3.println((char)26);
            delay(1000);
            servo.write(104);
            delay(1000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Door Closed");
            delay(2000);
            temp=0;
            false_attempt=0;
            entered_password="";
            temp1=0;
            for (;;)
            {
              loop();
            }
            break;
          }
        }
    }
    
  }
}

void passwordCheck(){
  if(temp==0){
    temp++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome Enter");
    lcd.setCursor(0, 1);
    lcd.print("Password- ");
    pass_count = password.length();
    Serial.println(pass_count);
    Serial.println(password);
    pos=0;
  }
  char whichKey = myKeypad.getKey();
  
  Serial.println(whichKey);
  if (whichKey == '*') {
    temp=0;
    for(;;){
      loop();
    }
  }
  if (whichKey == '#') {
    bluetooth();
  }
  else if(whichKey == 'A'){
    opendoor(1);
  }
  else if(whichKey == 'B'){
    opendoor(0);
  }
  else if(whichKey == 'D'){
    resetFunc();
  }
  else if(whichKey == 'C'){
    resetPassword();
  }
  else{
    if(whichKey){
      entered_password+=whichKey;
      pos++;
      lcd.print(whichKey);
    }
    Serial.println(entered_password);
    
    if(pos==pass_count){
      if(entered_password==password){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("*** Verified ***");
        
        delay(2000);
        lcd.clear();
        lcd.print("Waiting for Face");
        pos = 0;
        currtime = runtime = millis();
        while((runtime - currtime)/1000 <= 20){
          runtime = millis();
          face_rec();
        }
        temp=0;
        false_attempt=0;
        entered_password="";
        temp1=0;
        for (;;)
        {
          loop();
        }

      }
      else{
        temp=0;
        false_attempt++;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Invalid Password");
        lcd.setCursor(0,1);
        lcd.print("Attempt- ");
        lcd.print(false_attempt);
        entered_password="";
        send_sms(false_attempt);
        delay(3000);
        if(false_attempt == 3){
          send_sms_resetarduino();;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Maximum Attempts");
          lcd.setCursor(0,1);
          lcd.print("Reset Arduino");
          for(;;){
            
          }
        }
      }
    }
  }
  delay(100);
}

void bluetooth() {
  bool flag = false;
  int count=0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Trying BT :");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("Press # to back");
  
  for(;;){
    char whichKey = myKeypad.getKey();
    if(whichKey == '#'){
      temp=0;
      for(;;){
        passwordCheck();
      }
    }
    while (Serial2.available()) {
      delay(10);
      char c = Serial2.read();
      Serial.println(c);
      if (c == '#') {
        break;
      }
      cmd += c;
    }
    if (cmd.length() > 0) {
      Serial.println(cmd);
      int i;
      String pass;
      for (i = 0; i < pass_count; i++) {
        pass += password[i];
      }
      if(pass == cmd){
        flag=true;
      }
      if(i == pass_count && flag == true){
        opendoor(1);
        delay(1000);
        opendoor(0);
        cmd="";
        false_attempt=0;
        temp=0;
        for(;;){
          loop();
        }
        break;
      }
      else if(i==pass_count && flag != true){
        lcd.clear();
        false_attempt++;
        lcd.setCursor(0,0);
        lcd.print("Invalid PassKey");
        lcd.setCursor(0,1);
        lcd.print("Attempt- ");
        lcd.print(false_attempt);
        send_sms(false_attempt);
        delay(3000);
        cmd="";
        
        if(false_attempt == 3){
          send_sms_resetarduino();
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Maximum Attempts");
          lcd.setCursor(0,1);
          lcd.print("Reset Arduino");
          int i=0;
          for(;;){
            delay(1000);
            i++;
            lcd.print(i);
          } 
        }
        delay(2000);
        bluetooth();
      }
  
      cmd = "";
    }
  }
}

void send_sms(int attempt){
  Serial3.println("AT+CMGF=1");    
  delay(1000);  
  Serial3.println("AT+CMGS=\"+919156545747\"\r"); 
  delay(1000);
  delay(100);
  String str1 = " InValid attempt has been made on locker while entering password.";
  String str2 = "Number of attempts Left : ";
  String str3 = "Alert";
  String str4 = "Maximum 3 Unsuccessfull attempts were made by user, Reset Arduino to try again";
  Serial3.println(attempt + str1);
  delay(100);
  Serial3.println(str2 + (3-attempt));
  delay(100);
  Serial3.println((char)26);
  delay(1000);
}

void send_sms_resetarduino(){
  Serial3.println("AT+CMGF=1");    
  delay(1000);  
  Serial3.println("AT+CMGS=\"+919156545747\"\r"); 
  delay(1000);
  delay(100);
  String str3 = "Alert :-";
  String str4 = "Maximum 3 Unsuccessfull attempts were made by user, Reset Arduino to try again.";
  Serial3.println(str3);
  delay(100);
  Serial3.println(str4);
  delay(100);
  Serial3.println((char)26);
  delay(1000);
}

void writeStringToEEPROM(int addrOffset, const String &strToWrite){
  byte len = strToWrite.length();
  Serial.println(len);
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, int(strToWrite[i]));
    int ch = strToWrite[i];
    Serial.println(ch);
  }
}

String readStringFromEEPROM(int addrOffset){
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  String readpass = "";
  for (int i = 0; i < newStrLen; i++)
  {
    // data[i] = EEPROM.read(addrOffset + 1 + i);
    readpass += char(EEPROM.read(addrOffset + 1 + i));
  }
  data[newStrLen] = '\0';
  Serial.println(readpass);
  return readpass;
}

void face_rec(){
  if(Serial.available() > 0){
    int access = Serial.read();
    lcd.setCursor(0,1);
    lcd.print("Scanning Face ");
    if(access == '1'){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.println("Facial Rec Authorised");
      delay(1000);
      for(int positionCount = 0; positionCount < 5; positionCount++){
        lcd.scrollDisplayLeft();
        delay(500);
      }
      delay(500);
      opendoor(1);
      delay(1000);
      opendoor(0);
    }
  }
  delay(10);
}
