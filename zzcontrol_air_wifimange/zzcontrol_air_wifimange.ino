#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Mitsubishi.h>
#include <ir_Fujitsu.h>
#include <ir_Daikin.h>
#include <ir_Toshiba.h>
#include <ir_Carrier.h>
#include <ir_Gree.h>
#include <ir_Samsung.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

WiFiClient espClient;
PubSubClient client(espClient);

WiFiManager wifiManager;


// MQTT
const char *mqtt_server = ".";
const char *topic = "/INPUT/AIRCON/53";
const char *mqtt_username = ".";
const char *mqtt_password = ".";
const int mqtt_port = 1883;
String msg = " ";
String powers = " ";
int mode_ac = 0;
int temp_ac = 25;
int fan_lv = 0;
int swingv = 0; // id6 = 0-5,7
int swingh = 8; // id7 = 1-8
char* brand;
long lastMsg = 0;
//char msg[50];
int value = 0;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // message received
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
  //    Serial.print((char)payload[i]);
      msg += (char) payload[i];
    }
    Serial.println(msg);
    String str = msg;
  int str_len = str.length() + 1; 
  char tChar[str_len];
  char delimiters[] = "|";
  char* valPosition;
  str.toCharArray(tChar, str_len);
  valPosition = strtok(tChar, delimiters);
  for (int ii=0;ii<sizeof(str);ii++) {
    
    Serial.print(valPosition);
    
    String strVal = String(valPosition);
//    brand = valPosition;
    if (strVal == "1=1") {
        brand = "1";
        Serial.println("Brand : Mitsubishi");
    } else if (strVal == "1=2") {
        brand = "2";
        Serial.println("Brand : Fujisu");
    } else if (strVal == "1=3") {
        brand = "3";
        Serial.println("Brand : Daikin");
    } else if (strVal == "1=4") {
        brand = "4";
        Serial.println("Brand : Toshiba");
    } else if (strVal == "1=5") {
        brand = "5";
        Serial.println("Brand : Carrier");
    } else if (strVal == "1=6") {
        brand = "6";
        Serial.println("Brand : Trane(GreeAC)");
    } else if (strVal == "1=7") {
        brand = "7";
        Serial.println("Brand : Samsung");
    }
    //    id 2 ON/OFF Air
    if (strVal == "2=1") {
      powers = "on";
      Serial.println("AIR : ON");
      digitalWrite(BUILTIN_LED, HIGH);
    } else if (strVal == "2=0") {
      powers = "off";
      Serial.println("AIR : OFF");
      digitalWrite(BUILTIN_LED, LOW);
    }
//    id 3 Mode Air
    if (strVal == "3=1") {
      mode_ac = 1;
      Serial.println("Mode : 1(Auto)");
    } else if (strVal == "3=2") {
      mode_ac = 2;
      Serial.println("Mode : 2(Cool)");
    } else if (strVal == "3=3") {
      mode_ac = 3;
      Serial.println("Mode : 3(Dry)");
    } else if (strVal == "3=4") {
      mode_ac = 4;
      Serial.println("Mode : 4(Heat)");
    } else if (strVal == "3=5") {
      mode_ac = 5;
      Serial.println("Mode : 5(Fan)");
    }
//    id 4 Temperature 
    if (strVal == "4=20") {
      temp_ac = 20;
      Serial.println("Temp : 20");
    } else if(strVal == "4=21") {
      temp_ac = 21;
      Serial.println("Temp : 21");
    } else if(strVal == "4=22") {
      temp_ac = 22;
      Serial.println("Temp : 22");
    } else if(strVal == "4=23") {
      temp_ac = 23;
      Serial.println("Temp : 23");
    } else if(strVal == "4=24") {
      temp_ac = 24;
      Serial.println("Temp : 24");
    } else if(strVal == "4=25") {
      temp_ac = 25;
      Serial.println("Temp : 25");
    } else if(strVal == "4=26") {
      temp_ac = 26;
      Serial.println("Temp : 26");
    } else if(strVal == "4=27") {
      temp_ac = 27;
      Serial.println("Temp : 27");
    } else if(strVal == "4=28") {
      temp_ac = 28;
      Serial.println("Temp : 28");
    } else if(strVal == "4=29") {
      temp_ac = 29;
      Serial.println("Temp : 29");
    } else if(strVal == "4=30") {
      temp_ac = 30;
      Serial.println("Temp : 30");
    }
//    id 5 Fan Level
    if (strVal == "5=1") {
      fan_lv = 1;
      Serial.println("Fan Level : 1");
    } else if(strVal == "5=2") {
      fan_lv = 2;
      Serial.println("Fan Level : 2");
    } else if(strVal == "5=3") {
      fan_lv = 3;
      Serial.println("Fan Level : 3");
    } else if(strVal == "5=0") {
      fan_lv = 0;
      Serial.println("Fan Level : 0");
    }
    
//    id 6 Swing(V)
    if (strVal == "6=0") {
      swingv = 0;
      Serial.println("Swing(V) : 0(Auto)");
    } else if (strVal == "6=1") {
      swingv = 1;
      Serial.println("Swing(V) : 1(Highest)");
    } else if (strVal == "6=2") {
      swingv = 2;
      Serial.println("Swing(V) : 2(High)");
    } else if (strVal == "6=3") {
      swingv = 3;
      Serial.println("Swing(V) : 3(Middle)");
    } else if (strVal == "6=4") {
      swingv = 4;
      Serial.println("Swing(V) : 4(Low)");
    } else if (strVal == "6=5") {
      swingv = 5;
      Serial.println("Swing(V) : 5(Lowest)");
    } else if (strVal == "6=6") {
      swingv = 6;
      Serial.println("Swing(V) : 6(Swing)");
    } 

//    id 7 Swing(H)
    if (strVal == "7=1") {
      swingh = 1;
      Serial.println("Swing(H) : 1(LeftMax)");
    } else if (strVal == "7=2") {
      swingh = 2;
      Serial.println("Swing(H) : 2(Left)");
    } else if (strVal == "7=3") {
      swingh = 3;
      Serial.println("Swing(H) : 3(Middle)");
    } else if (strVal == "7=4") {
      swingh = 4;
      Serial.println("Swing(H) : 4(Right)");
    } else if (strVal == "7=5") {
      swingh = 5;
      Serial.println("Swing(H) : 5(RightMax)");
    } else if (strVal == "7=6") {
      swingh = 6;
      Serial.println("Swing(H) : 6(Wide)");
    } else if (strVal == "7=0") {
      swingh = 0;
      Serial.println("Swing(H) : 0(Auto)");
    }

    
//    Cut Data by delimiters
    valPosition = strtok(NULL, delimiters);
    
  }
// ------End substring-------------------------------------------------
//  Control AIR
  if (brand == "1") { // #brand1
    IRMitsubishiAC ac(kIrLed);
    ac.begin();
    delay(200);
    if (powers == "on") {
      ac.on();
    } else if (powers == "off") {
      ac.off();
    }
    ac.setFan(fan_lv);
    // #mode_ac
    if (mode_ac == 0) {
      ac.setMode(kMitsubishiAcAuto); // 0b100
    } else if (mode_ac == 1) {
      ac.setMode(kMitsubishiAcCool); // 0b011
    } else if (mode_ac == 2) {
      ac.setMode(kMitsubishiAcDry); // 0b010
    } else if (mode_ac == 3) {
      ac.setMode(kMitsubishiAcHeat); // 0b001
    } else if (mode_ac == 4) {
      ac.setMode(kMitsubishiAcFan); // 0b111
    }
    
    ac.setTemp(temp_ac);
    
    // Swing (V) ------------------------
    if (swingv == 0) {
      ac.setVane(kMitsubishiAcVaneAuto);
    } else if (swingv == 1) {
      ac.setVane(kMitsubishiAcVaneHighest);
    } else if (swingv == 2) {
      ac.setVane(kMitsubishiAcVaneHigh);
    } else if (swingv == 3) {
      ac.setVane(kMitsubishiAcVaneMiddle);
    } else if (swingv == 4) {
      ac.setVane(kMitsubishiAcVaneLow);
    } else if (swingv == 5) {
      ac.setVane(kMitsubishiAcVaneLowest);
    } else if (swingv == 6) {
      ac.setVane(kMitsubishiAcVaneSwing);
    }
    // Swing (V)
    if (swingh == 1) {
      ac.setWideVane(kMitsubishiAcWideVaneLeftMax);
    } else if (swingh == 2) {
      ac.setWideVane(kMitsubishiAcWideVaneLeft);
    } else if (swingh == 3) {
      ac.setWideVane(kMitsubishiAcWideVaneMiddle);
    } else if (swingh == 4) {
      ac.setWideVane(kMitsubishiAcWideVaneRight);
    } else if (swingh == 5) {
      ac.setWideVane(kMitsubishiAcWideVaneRightMax);
    } else if (swingh == 6) {
      ac.setWideVane(kMitsubishiAcWideVaneWide);
    } else if (swingh == 0) {
      ac.setWideVane(kMitsubishiAcWideVaneAuto);
    } 
    // -------------------------------------
    
    if (powers == "on" || powers == "off") {
      Serial.println("Send A/C.");
      ac.send();
      Serial.println(ac.toString());
    }
  } else if (brand == "2") { // brand2
    IRFujitsuAC ac(kIrLed);
    ac.begin();
    delay(200);
    
    ac.setModel(ARRAH2E);
    ac.setSwing(kFujitsuAcSwingOff);
    
    ac.setMode(mode_ac);
//    ac.setMode(kFujitsuAcModeAuto);
    
//    ac.setFanSpeed(kFujitsuAcFanHigh);
    ac.setFanSpeed(fan_lv);
    ac.setTemp(temp_ac);
    if (powers == "on") {
      ac.setCmd(kFujitsuAcCmdTurnOn);
    } else if (powers == "off") {
      ac.setCmd(kFujitsuAcCmdTurnOff);
    }
    if (powers == "on" || powers == "off") {
      Serial.println("Send A/C.");      
      ac.send();
      Serial.println(ac.toString());
    } 
  } else if (brand == "3") { // brand3
    IRDaikinESP ac(kIrLed);
    ac.begin();
    delay(200);
    if (powers == "on") {
      ac.on();
    } else if (powers == "off") {
      ac.off();
    }
    // fan_daikin
    if (fan_lv == 0) {
      ac.setFan(kDaikinFanAuto);
    } else if (fan_lv == 1) {
      ac.setFan(kDaikinFanMin);
    } else if (fan_lv == 2) {
      ac.setFan(kDaikinFanMed);
    } else if (fan_lv == 3) {
      ac.setFan(kDaikinFanMax);
    }
    
    // mode_daikin
    if (mode_ac == 0) {
      ac.setMode(kDaikinAuto);
    } else if (mode_ac == 1) {
      ac.setMode(kDaikinCool);
    } else if (mode_ac == 2) {
      ac.setMode(kDaikinDry);
    } else if (mode_ac == 3) {
      ac.setMode(kDaikinHeat);
    } else if (mode_ac == 4) {
      ac.setMode(kDaikinFan);
    }
    
    ac.setTemp(temp_ac);
    
    ac.setSwingVertical(false);
    ac.setSwingHorizontal(false);
    
    Serial.println(ac.toString());
    
    if (powers == "on" || powers == "off") {
      Serial.println("Send A/C.");
      ac.send();
      Serial.println(ac.toString());
    } 
  } else if (brand == "4") { // brand4
    
    IRToshibaAC ac(kIrLed);
    
    ac.begin();
    
    delay(200);
    if (powers == "on") {
      ac.on();
    }
    // toshiba_fan
    if (fan_lv == 0) {
        ac.setFan(kToshibaAcFanAuto);
    } else if (fan_lv == 1) {
        ac.setFan(kToshibaAcFanMin);
    } else if (fan_lv == 2) {
        ac.setFan(kToshibaAcFanMed);
    } else if (fan_lv == 3) {
        ac.setFan(kToshibaAcFanMax);
    }
    // toshiba_mode
    if (mode_ac == 0) {
      ac.setMode(kToshibaAcAuto);
    } else if (mode_ac == 1) {
      ac.setMode(kToshibaAcCool);
    } else if (mode_ac == 2) {
      ac.setMode(kToshibaAcDry);
    } else if (mode_ac == 3) {
      ac.setMode(kToshibaAcHeat);
    } else if (mode_ac == 4) {
      ac.setMode(kToshibaAcFan);
    }
    
    ac.setTemp(temp_ac);
    
    Serial.println(ac.toString());
    
    if (powers == "on") {
      Serial.println("Send A/C.");
      ac.send();
    } else if (powers == "off") {
      IRsend irsend(kIrLed);
      irsend.begin();
      Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
    
      uint16_t rawData[295] = {4592, 4204,  734, 1434,  734, 1430,  732, 1434,  732, 1432,  734, 348,  734, 352,  734, 1430,  732, 350,  732, 350,  732, 350,  732, 350,  732, 350,  732, 1436,  732, 1430,  732, 352,  732, 1432,  732, 350,  732, 350,  734, 348,  734, 348,  734, 348,  734, 350,  734, 1432,  732, 1434,  732, 1434,  734, 1430,  734, 1434,  732, 1432,  732, 1436,  732, 1432,  734, 348,  734, 350,  734, 350,  732, 350,  732, 350,  732, 352,  732, 350,  732, 350,  732, 350,  732, 1432,  734, 348,  734, 1434,  730, 1434,  732, 350,  732, 352,  734, 348,  734, 348,  734, 350,  734, 348,  732, 350,  732, 350,  732, 350,  732, 350,  732, 1434,  734, 1430,  732, 1434,  732, 352,  732, 350,  732, 350,  732, 350,  732, 350,  732, 350,  732, 352,  734, 348,  732, 350,  732, 1436,  732, 1434,  732, 352,  732, 348,  734, 1432,  732, 1434,  732, 350,  732, 6520,  4592, 4204,  734, 1434,  732, 1432,  734, 1432,  736, 1430,  736, 346,  736, 350,  736, 1428,  736, 348,  734, 346,  736, 346,  736, 346,  736, 346,  736, 1430,  736, 1430,  734, 350,  734, 1432,  734, 348,  734, 346,  736, 346,  736, 346,  734, 348,  734, 350,  734, 1432,  732, 1432,  734, 1432,  734, 1430,  732, 1434,  732, 1432,  734, 1434,  734, 1430,  734, 348,  734, 350,  732, 350,  732, 350,  732, 350,  734, 352,  734, 348,  736, 346,  734, 348,  734, 1432,  732, 350,  732, 1434,  732, 1432,  734, 348,  734, 350,  734, 348,  734, 348,  734, 350,  732, 350,  732, 348,  732, 350,  734, 348,  734, 348,  734, 1434,  732, 1430,  732, 1434,  732, 352,  734, 348,  734, 348,  734, 348,  734, 348,  734, 348,  734, 350,  734, 346,  732, 350,  732, 1434,  736, 1428,  736, 348,  736, 344,  736, 1430,  734, 1432,  736, 346,  736};  // TOSHIBA_AC
      uint8_t state[9] = {0xF2, 0x0D, 0x03, 0xFC, 0x01, 0x60, 0x07, 0x00, 0x66};
      
      Serial.println("a rawData capture from IRrecvDumpV2");
      irsend.sendRaw(rawData, 295, 38);  // Send a raw data capture at 38kHz.
    }
  } else if (brand == "5") { // brand5
    IRCarrierAc64 ac(kIrLed);
    ac.begin();
    delay(200);
    if (powers == "on") {
      ac.on();
    } else if (powers == "off") {
      ac.off();
    }
    ac.setFan(fan_lv);

    // carrier_mode
    if (mode_ac == 0) {
      ac.setMode(kCarrierAc64Fan);
    } else if (mode_ac == 1) {
      ac.setMode(kCarrierAc64Cool);
    } else if (mode_ac == 2) {
      ac.setMode(kCarrierAc64Fan);
    } else if (mode_ac == 3) {
      ac.setMode(kCarrierAc64Heat);
    } else if (mode_ac == 4) {
      ac.setMode(kCarrierAc64Fan);
    }
    
    ac.setTemp(temp_ac);
    
    Serial.println(ac.toString());
    
    if (powers == "on" || powers == "off") {
      Serial.println("Send A/C.");
      ac.send();
      Serial.println(ac.toString());
    } 
  } else if (brand == "6") { // brand6
    IRGreeAC ac(kIrLed);
    ac.begin();
    delay(200);
    if (powers == "on") {
      ac.on();
    } else if (powers == "off") {
      ac.off();
    }
    ac.setFan(fan_lv);
    // gree_mode
    if (mode_ac == 0) {
      ac.setMode(kGreeAuto);
    } else if (mode_ac == 1) {
      ac.setMode(kGreeCool);
    } else if (mode_ac == 2) {
      ac.setMode(kGreeDry);
    } else if (mode_ac == 3) {
      ac.setMode(kGreeHeat);
    } else if (mode_ac == 4) {
      ac.setMode(kGreeFan);
    }
    
    ac.setTemp(temp_ac);
    // Swing (V)
    if (swingh == 0) {
      ac.setSwingVertical(true, kGreeSwingAuto);
    } else if (swingh == 1) {
      ac.setSwingVertical(true, kGreeSwingUp);
    } else if (swingh == 2) {
      ac.setSwingVertical(true, kGreeSwingMiddleUp);
    } else if (swingh == 3) {
      ac.setSwingVertical(true, kGreeSwingMiddle);
    } else if (swingh == 4) {
      ac.setSwingVertical(true, kGreeSwingMiddleDown);
    } else if (swingh == 5) {
      ac.setSwingVertical(true, kGreeSwingDown);
    } else if (swingh == 6) {
      ac.setSwingVertical(true, kGreeSwingDownAuto);
    } else if (swingh == 7) {
      ac.setSwingVertical(true, kGreeSwingMiddleAuto);
    } else if (swingh == 8) {
      ac.setSwingVertical(true, kGreeSwingUpAuto);
    }
    
    ac.setXFan(false);
    ac.setLight(false);
    ac.setSleep(false);
    ac.setTurbo(false);
    
    Serial.println(ac.toString());
    
    if (powers == "on" || powers == "off") {
      Serial.println("Send A/C.");
      ac.send();
    } 
  } else if (brand == "7") { // brand7
    IRSamsungAc ac(kIrLed);
    ac.begin();
    delay(200);
    if (powers == "on") {
      ac.on();
    } else if (powers == "off") {
      ac.off();
    }
    // fan_samsung
    if (fan_lv == 0) {
      ac.setFan(kSamsungAcFanAuto);
    } else if (fan_lv == 1) {
      ac.setFan(kSamsungAcFanLow);
    } else if (fan_lv == 2) {
      ac.setFan(kSamsungAcFanMed);
    } else if (fan_lv == 3) {
      ac.setFan(kSamsungAcFanHigh);
    }
    ac.setSwing(false);
    // mode_samsung
    if (mode_ac == 0) {
      ac.setMode(kSamsungAcAuto);
    } else if (mode_ac == 1) {
      ac.setMode(kSamsungAcCool);
    } else if (mode_ac == 2) {
      ac.setMode(kSamsungAcDry);
    } else if (mode_ac == 3) {
      ac.setMode(kSamsungAcHeat);
    } else if (mode_ac == 4) {
      ac.setMode(kSamsungAcFan);
    }
    
    ac.setTemp(temp_ac);
    
    Serial.println(ac.toString());
    
    if (powers == "on" || powers == "off") {
      Serial.println("Send A/C.");
      ac.send();
    } 
  }
  
  msg = "";
  Serial.println();
  Serial.println("-----------------------");
    
}

void mqttReconnect() {
    // reconnect code from PubSubClient example
    while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
      client.publish(topic, "AirCon Ready!!");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      ESP.reset();
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  wifiManager.setTimeout(180);
  if(!wifiManager.autoConnect("CCT-Air-AP2")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  } 

  Serial.println("connected...yeey :)");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("MQTT connected");
          client.publish(topic, "AirCon Ready!!");
          client.subscribe(topic);
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          ESP.reset();
          delay(2000);
      }
  }
//  client.publish(topic, "AirCon Ready!!");
//  client.subscribe(topic);
}

void loop() {
//  if(!wifiManager.autoConnect("AutoConnectAP")) {
//    Serial.println("failed to connect and hit timeout");
//    delay(3000);
//    ESP.reset();
//    delay(5000);
//  } 
  if (!client.connected()) {
    mqttReconnect();
  }
  
  client.loop();
  yield();
}
