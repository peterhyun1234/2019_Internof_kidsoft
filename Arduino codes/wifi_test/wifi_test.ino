#include "ESP8266.h"


#define SSID        "gearS2_2G"
#define PASSWORD    "itsywatch04"
#define HOST_NAME   "192.168.0.13"
#define HOST_PORT   (9002)

ESP8266 wifi(Serial3, 115200);

//SD Card Setting


void setup(void)
{
    Serial.begin(115200);
    Serial.print("setup begin\r\n");


  wifi.restart();
    if(wifi.kick ())
      Serial.print("esp8266 is alive\r\n");
    else
      Serial.print("esp8266 is not alive\r\n");
      
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
 
    if (wifi.setOprToStation ()) {
        Serial.print("to station mode\r\n");
    } else {
        Serial.print("to station mode error\r\n");
    }
 
     if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
  
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());       
    } else {
        Serial.print("Join AP failure\r\n");
    }  
    
    
    Serial.print("setup end\r\n");
}




void loop(void)
{
delay(5000);
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
      Serial.print("create tcp ok\r\n");
    } else {
      Serial.print("create tcp err\r\n");
    }


 
  char t1[150] = "T:45805,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t2[150] = "T:45810,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t3[150] = "T:45815,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t4[150] = "T:45820,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t5[150] = "T:45825,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t6[150] = "T:45830,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t7[150] = "T:45835,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t8[150] = "T:45840,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t9[150] = "T:45845,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t10[150] = "T:45850,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t11[150] = "T:45855,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t12[150] = "T:45860,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t13[150] = "T:45865,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t14[150] = "T:45870,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t15[150] = "T:45875,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t16[150] = "T:45876,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char t17[150] = "T:45808,1,A1:7064,A2:7063,A3:7065,A4:7064,A5:7063,A6:7067,A7:7063,A8:7064";
  char maximum[150] = "A1:8013,A2:8013,A3:8014,A4:8012,A5:8009,A6:8015,A7:8011,A8:8014";
  char count[150] = "Count A1:196,A2:196,A3:196,A4:196,A5:196,A6:196,A7:196,A8:196";
  
  char sndbuf[255];
  char cTemp;
  int cnt = 0;

  Serial.println(t1);
   wifi.send((const uint8_t*)t1, strlen(t1));
  Serial.println(t2);
   wifi.send((const uint8_t*)t2, strlen(t2));
  Serial.println(t3);
   wifi.send((const uint8_t*)t3, strlen(t3));
  Serial.println(t4);
   wifi.send((const uint8_t*)t4, strlen(t4));
  Serial.println(t5);
   wifi.send((const uint8_t*)t5, strlen(t5));
  Serial.println(t6);
   wifi.send((const uint8_t*)t6, strlen(t6));
  Serial.println(t7);
   wifi.send((const uint8_t*)t7, strlen(t7));
  Serial.println(t8);
   wifi.send((const uint8_t*)t8, strlen(t8));
  Serial.println(t9);
   wifi.send((const uint8_t*)t9, strlen(t9));
  Serial.println(t10);
   wifi.send((const uint8_t*)t10, strlen(t10));
  Serial.println(t11);
   wifi.send((const uint8_t*)t11, strlen(t11));
  Serial.println(t12);
   wifi.send((const uint8_t*)t12, strlen(t12));
  Serial.println(t13);
   wifi.send((const uint8_t*)t3, strlen(t13));
  Serial.println(t14);
   wifi.send((const uint8_t*)t14, strlen(t14));
  Serial.println(t15);  
   wifi.send((const uint8_t*)t15, strlen(t15));
  Serial.println(t16);
   wifi.send((const uint8_t*)t16, strlen(t16));
  Serial.println(t17);
   wifi.send((const uint8_t*)t17, strlen(t17));
  
  Serial.println(maximum);
   wifi.send((const uint8_t*)maximum, strlen(maximum));
  Serial.println(count);
   wifi.send((const uint8_t*)count, strlen(count));



  
     
    if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }

    delay(5000);
   
}
