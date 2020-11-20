#include <SoftwareSerial.h> // Arduino IDE <1.6.6
#include <PZEM004T.h>

PZEM004T pzem(10,11); 
#include <LiquidCrystal.h>

IPAddress ip(192,168,1,1);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7    );
char command;
String ssid="NETGEAR";
String pswd="schematic";
byte  done=0;
String payload = ""; // a string to hold incoming data
boolean stringComplete = false;
 boolean StringReady=false;
int lngth=0;
byte nocon=0;
byte join;
//byte snd=0;
//char ipd=0;
byte snd=0;
byte snd1=0;
void setup() {
    Serial.begin(115200);
    Serial1.begin(9600);
    Serial2.begin(115200);
 #define TIMEOUT 5000 // mS
  payload.reserve(100);
   lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("SMART METER");
   Serial2.println("AT");
  ////////////////////////////////////config wifi

 SendCommand("AT+RST", "Ready");
 delay(5000);
 SendCommand("AT+CWMODE=1","OK");
 SendCommand("AT+CIFSR", "OK");
 SendCommand("AT+CIPMUX=1","OK");
 SendCommand("AT+CIPSERVER=1,80","OK");
   //////////////////////////////////////
  
  delay(3000);
   pzem.setAddress(ip);
  lcd.clear();
  join=0;
}

void loop() {
  //////////////////////////JOIN TO ROUTER
 
  if(join==0)
  {
 SendCommand1("AT+CWJAP=","WIFI CONNECTED");
  }

  if (StringReady){
    Serial.println("Received String: " +  payload);
   //  lcd.setCursor(0, 1);
    //lcd.print(payload);
    if (payload.indexOf("0,CONNECT") != -1) {
   snd=1;
   Serial.println("CONNECTED");
   }
    if (payload.indexOf("0,CLOSED") != -1) {
   snd=0;
    Serial.println("CONNECTION CLOSED");
   }
   if (payload.indexOf("WIFI DISCONNECT") != -1) {
   snd=0;
    Serial.println("WIFI DISCONNECTED");
   }
    if (payload.indexOf(">") != -1) {
   snd1=1;
   }
    payload="";
 StringReady = false;
 
  }
  
  /////////////////////////
  float v = pzem.voltage(ip);
  if (v < 0.0) v = 0.0;
  //Serial.print(v);Serial.print("V; ");
   lcd.setCursor(0, 0);
   if(v==0.0&&done==0)
   {
    done=1;
    lcd.clear();
   }
   if(v>0.0)
   done=0;
   if(v==0.0&&snd==1)
   {
    Serial2.println("AT+CIPSEND=0,11");
   delay(1000);
   snd1=1;
   }
   if(v>0.0&&snd==1)
   {
    Serial2.println("AT+CIPSEND=0,51");
   delay(1000);
   snd1=1;
   }
   if(snd1==1)
   {
     Serial2.print("voltage:");
    Serial2.print(v);
    delay(1000);
   }
lcd.print(v);lcd.print("V ");
//////////////////////////////////////////////////////send data to telnet

  float i = pzem.current(ip);
  if(i >= 0.0)
  { 
  //  Serial.print(i);Serial.print("A; ");
     lcd.setCursor(8, 0);
    lcd.print(i);lcd.print("A   ");
     if(snd1==1)
   {
     Serial2.print("current:");
    Serial2.print(i);
    delay(1000);
   }
  }
  
  float p = pzem.power(ip);
  if(p >= 0.0){
  //  Serial.print(p);Serial.print("W; ");
     lcd.setCursor(0, 1);
    lcd.print(p);lcd.print("W  "); 
     if(snd1==1)
   {
     Serial2.print("watts:");
    Serial2.print(p);
    delay(1000);
   }
    }
  
  float e = pzem.energy(ip);
  if(e >= 0.0){ 
  //  Serial.print(e);Serial.print("Wh; ");
     lcd.setCursor(8,1);
    lcd.print(e);lcd.print("Wh "); 
     if(snd1==1)
   {
     Serial2.print("Watt hour:");
    Serial2.print(e);
    delay(1000);
   }
    }

//  Serial.println();

//  delay(1000);
}

boolean SendCommand(String cmd, String ack){
  Serial2.println(cmd); // Send "AT+" command to module
  if (!echoFind(ack)) // timed out waiting for ack string
    return true; // ack blank or ack found
}
 boolean SendCommand1(String cmd, String ack){
  Serial2.print(cmd); // Send "AT+" command to module
  Serial2.print('"');
  Serial2.print(ssid);
  Serial2.print('"');
    Serial2.print(',');
    Serial2.print('"');
  Serial2.print(pswd);
  Serial2.println('"');
  if (!echoFind(ack)) // timed out waiting for ack string
    return true; // ack blank or ack found
}
boolean echoFind(String keyword){
 byte current_char = 0;
 byte keyword_length = keyword.length();
 long deadline = millis() + TIMEOUT;
 while(millis() < deadline){
  if (Serial2.available()){
    char ch = Serial2.read();
    Serial.write(ch);
    if (ch == keyword[current_char])
      if (++current_char == keyword_length){
       Serial.println();
       join=1;
       return true;
    }
   }
  }
 return false; // Timed out
}
////////////////////////
void serialEvent2() {
  while (Serial2.available()) {
    // get the new byte:
     char ch=Serial2.read(); 
   //  Serial.print(inChar); 
    // add it to the inputString:
   payload += ch;
   lngth++;
  if(lngth>6)
   StringReady= true;
    } 
  }
//
///////////////////////////checkok
//void ok()
//{
//  while(1)
//  {
//  Serial2.println(command);
//  //enter();
//  delay(1000);
//    Serial.println( payload); 
//  if (stringComplete) {
//   Serial.println( payload); 
//   for(int l=0;l<40;l++)
//   {
//    if(payload[l]=='O'&&payload[l+1]=='K')
//    {
//      stringComplete=false;
//      payload="";
//      break;
//    }
//    
//   }
//   ///////////////////////////////////
//   
//  }
//   }
//}
//////////////////////////////////
//void ok1()
//{
// 
//  Serial2.print(command);
//  Serial2.print(ssid);
//  Serial2.print(',');
//  Serial2.print(pswd);
//  enter();
//  delay(2000);
//  if (stringComplete) {
//   Serial.println( payload); 
//   for(int l=0;l<40;l++)
//   {
//    if(payload[l]=='F'&&payload[l+1]=='A'&&payload[l+2]=='I'&&payload[l+3]=='L')
//    {
//      stringComplete=false;
//      payload="";
//      join=0;
//    }
//    
//    
//   }
//     for(int l=0;l<40;l++)
//   {
//    if(payload[l]=='W'&&payload[l+1]=='I'&&payload[l+2]=='F'&&payload[l+3]=='I'&&payload[l+5]=='C'&&payload[l+6]=='O'&&payload[l+7]=='N')
//    {
//      stringComplete=false;
//      payload="";
//      join=1;
//    }
//    
//    
//   }
//   ///////////////////////////////////
//   
//  }
//   }
//
//

