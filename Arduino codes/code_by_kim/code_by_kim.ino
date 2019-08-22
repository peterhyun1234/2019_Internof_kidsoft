#include <EnableInterrupt.h>

#include <Wire.h>                        // i2C 통신을 위한 라이브러리

#include <LiquidCrystal_I2C.h>          // LCD 2004 I2C용 라이브러리

#include <Time.h>                       //시각설정을 위함 라이브러리

#include <DS1307RTC.h>                  //시간설정을 위한 RTC 라이브러리

#include <SD.h>                         //SD Card 사용을 위한 라이브러리

#include <SPI.h>                        //SPI 통신을 위한 라이브러리

#include <EEPROM.h>                     //EEPROM 라이브러리

#include "ESP8266.h"                    //WiFi 라이브러리

 

// ADC Pin Setting

#define BUSY 3

#define RD 4  //RD+CS tied together

#define RESET 5

#define CONVST 6 //CONVSTA+CONVSTB soldered together on the board

#define RANGE 7

 

#define OS0 8

#define OS1 9

#define OS2 10

#define FIRST 11

 

#define DB0 32   //38 

#define DB1 33 

#define DB2 34

#define DB3 35

#define DB4 36

#define DB5 37

#define DB6 38

#define DB7 39

#define DB8 40

#define DB9 41

#define DB10 42

#define DB11 43

#define DB12 44

#define DB13 45

#define DB14 46

#define DB15 47   //53

 

#define waitRefTime 6000       //단위(ms)= 60초

#define initMaxValue 10000      //최대값 //org:30000

 

#define SSID        "gearS2_2G"

#define PASSWORD    "itsywatch04"

#define HOST_NAME   "192.168.0.13"

#define HOST_PORT   (9002)

 

ESP8266 wifi(Serial3, 115200);

 

//SD Card Setting

const int chipSelect = 53;    //chip select pin

Sd2Card card;

SdVolume volume;

SdFile root;

File myFile;

File dataFile;

 

 

 

// declare rawData as an array of 16 integers

int rawData[16], valueSensor, sensorA1, sensorA2, sensorA3, sensorA4, sensorA5, sensorA6, sensorA7, sensorA8, samplingRate=1000, flagKeyPrinting, flagPrinting, iminTime, isecTime, preMiliRef, preSecRef, diffMilliSec,

    tempArrA[10], tempArrB[10], tempArrC[10], tempArrD[10], tempArrE[10], tempArrF[10], tempArrG[10], tempArrH[10],k, algorithmType,

    sensorPreA=0, sensorCurA=0, sensorPreB=0, sensorCurB=0, sensorPreC=0, sensorCurC=0, sensorPreD=0, sensorCurD=0, thresholdValue,

    sensorPreE=0, sensorCurE=0, sensorPreF=0, sensorCurF=0, sensorPreG=0, sensorCurG=0, sensorPreH=0, sensorCurH=0, workingState=0,

    sensorStateA=0, sensorStateB=0,sensorStateC=0, sensorStateD=0, sensorStateE=0, sensorStateF=0, sensorStateG=0, sensorStateH=0,

    minA, minB, minC, minD, minE, minF, minG, minH, convertA1, convertA2, convertA3, convertA4, convertA5, convertA6, convertA7,convertA8,

    convertMinA1, convertMinA2, convertMinA3, convertMinA4, convertMinA5, convertMinA6, convertMinA7,convertMinA8, secCount, ipreKey3, valueCountA,

    valueCountB, valueCountC, valueCountD, valueCountE, valueCountF, valueCountG, valueCountH, thresholdNum, offsetMode, communicationMode, machineNum, wifiTrCount,

    offsetValueRef, offsetValuePre, offsetValueCur;

unsigned long preMilliSec, curMilliSec, refMilliSec, waitMiliSec, readyMiliSec;

String preKey1, preKey2, preKey3, strInfo, strMin, strTitle, strCount, strLcd;

boolean fileExist,wifiReady=false, isTcpError=true;

 

LiquidCrystal_I2C lcd(0x27,20,4);     // 접근주소: 0x3F or 0x27

 

//------------ 필요기능 ----------------------------------

// 1. Offset 기능: 최초 10를 기준으로 Offset 기능 만들기

// 2. Sampling Rate 조절 하기 (10 Hz, 50Hz, 100 Hz)

// 3. 알고리즘 선택 (최소값, 평균최소값)

// 4. PC가 연결되지 않을시 자동으로 저장, PC연결시는 저장 안함

// 5. WiFi Communication

// 6. 값 Conversion

//--------------------------------------------------------

// URGENT

// 6. 값 Conversion

// 구간의 개수:각 채널별로 카운트 처리

//

 

void EEPROMWrite(int address, long value){

  byte b4= (value & 0xFF);

  byte b3= ((value >> 8) & 0xFF);

  byte b2= ((value >> 16) & 0xFF);

  byte b1= ((value >> 24) & 0xFF);

 

  EEPROM.write(address * 4, b4);

  EEPROM.write(address * 4 + 1, b3);

  EEPROM.write(address * 4 + 2, b2);

  EEPROM.write(address * 4 + 3, b1);

}

 

long EEPROMRead(long address){

  long four = EEPROM.read(address * 4);

  long three = EEPROM.read(address * 4 + 1);

  long two = EEPROM.read(address * 4 + 2);

  long one = EEPROM.read(address * 4 + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);

}

 

int findArrayAvg(int arr[], int size){

  double sumArr=0.0;

  int i, result=0;

 

  for(i=0;i < size;i++){

    sumArr += arr[i];  

  }

 

  result = (int)(sumArr / size) ;

  

  return result;   

}

 

void mySDBegin(){

  if(!SD.begin(chipSelect)){

    //Serial.println("SD R/W initializaton failed!");

    return;

  }

  //Serial.println("SD R/W initialization done!!!");

}

 

void mySDInit(){

  //-------- SD 사용을 위한 SS Pin 정의 -------------

  pinMode(chipSelect, OUTPUT);

 

  //--------- SD 카드 인식 확인 ----------------------

  if(!card.init(SPI_HALF_SPEED, chipSelect)){

    Serial.println("initialization failed, Things to check:");

    return;

  }else{

    Serial.println("SD card is ready");

  }

 

  //--------- 파일 시스템 읽기 실패 시 ----------------

  if(!volume.init(card)){

    Serial.println("Could not find FAT32 partition");

    return;

  }

 

  //--------- SD카드의 파일 시스템 정보 출력 -----------

  uint32_t volumesize;

  //Serial.print("\n Volume type is FAT");

  //Serial.println(volume.fatType(), DEC);

  //Serial.println();

 

  volumesize = volume.blocksPerCluster();

  volumesize *= volume.clusterCount();

  volumesize *= 512;

 

  //Serial.print("Volume size (bytes):  ");

  //Serial.println(volumesize);

 

  //Serial.print("Volume size (Kbytes):  ");

  volumesize /= 1024;

  //Serial.println(volumesize); 

}

 

void mySD_ls()

{

  // =============== SD카드의 루트 디렉토리의 파일 목록 출력 =============== //

  //Serial.println("\nFiles found on the card (name, date and size in bytes): ");

  root.openRoot(volume);

  root.ls(LS_R | LS_DATE | LS_SIZE);

 

  //Serial.println("\nEnd of List\n");

}

 

void myFileRWTest()

{

  //Serial.println("File R/W Test");

 

  myFile = SD.open("test.txt", FILE_WRITE);

 

  // if the file opened okay, write to it:

  if (myFile)

  {

    //Serial.print("Writing to test.txt...");

    myFile.println("testing 1, 2, 3.");

    // close the file:

    myFile.close();

    Serial.println("done.");

  }

  else

  {

    // if the file didn't open, print an error:

    Serial.println("error opening test.txt");

  }

 

  // re-open the file for reading:

  myFile = SD.open("test.txt");

  if (myFile)

  {

    Serial.println("test.txt:");

 

    // read from the file until there's nothing else in it:

    while (myFile.available())

    {

      Serial.write(myFile.read());

    }

    // close the file:

    myFile.close();

  }

  else

  {

    // if the file didn't open, print an error:

    Serial.println("error opening test.txt");

  } 

}

 

void wifiInit(){

 

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

      wifiReady = true;     

  } else {

      wifiReady = false;     

      Serial.print("Join AP failure\r\n");

  } 

   

  Serial.print("WiFi setup end\r\n");

}

 

 

void serialcom(){

  char data;

  String str;

  int c=0;

 

  while(Serial.available()){

    data = Serial.read();

    str = data;

    c = data - '0';

    //Serial.println(data);

    //Serial.print(preKey1); 

    //Serial.print(str);

    //clockDsiplay();

 

    if(str.compareTo("0")==0){

      Serial.println("--------------- Menu ------------------------");

      Serial.println("// RS 232 Printing Meue");

      Serial.println("// Powered by KIDSOFT 2019/07/25");

      Serial.println("---------------------------------------------");

      Serial.println("1. Enter the Title");

      Serial.println("2. Start Printing");

      Serial.println("3. Stop Printing");

      Serial.println("4. Sampling Rate(default: 100 Hz)");

      Serial.println("5. Alogorithm: MinValue(Sensor: by pass - default)");

      Serial.println("6. Threshold(default: 80%)");

      Serial.println("7. Offset Mode");

      Serial.println("8. UART/WiFi Mode(default: UART)");

      Serial.println("9. Machine Number(default: 00)");

      Serial.println("---------------------------------------------");               

      flagKeyPrinting = 0;

      //strcpy(preKey1, str);

      preKey2 = "";

      preKey3 = "";

      strTitle = "";

      c = "";

      preKey1 = str;

    }

 

    //if(c == -35){

    //  Serial.print(preKey1);     

    //  Serial.print(preKey2);     

    //  Serial.print(preKey3);     

    //  Serial.println("--> Press the -- Enter Key: ");     

    //}

 

    if(preKey1.compareTo("0")==0 && preKey2.compareTo("")==0 && str.compareTo("1")==0){

      Serial.println("--> Enter the Tile: ");     

      preKey2 = str;

     

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("1")==0 && ipreKey3 == -35 && str.compareTo("1")==0){    

      ipreKey3 = 0;

      strTitle = "";

      Serial.println("--> Retry the Tile: ");     

      

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("1")==0 && ipreKey3 == -35 && str.compareTo("2")==0){     

      Serial.print("---> Start Printing Title: ");

      Serial.println(strTitle); 

      

      flagKeyPrinting = 1;

 

      /*

      if(fileExist){

        myFile.println(strTitle);

        Serial.println("File is already opened");

      }else{       

        myFile = SD.open("test.txt", FILE_WRITE);

        myFile.println(strTitle);        

        Serial.println("File is opening");

        fileExist = true;

      }

      */

     

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("1")==0 && ipreKey3 == -35 && str.compareTo("3")==0){     

      flagKeyPrinting = 0;   

      Serial.print("---> Stop Printing Title: ");

      Serial.println(strTitle);

        

      if(fileExist){

        myFile.println(strTitle);

        myFile.println("----------------- Stop File Writing ----------------");

        myFile.close();               

        Serial.println("File is closing");

        fileExist = false;

      }

     

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("1")==0 && c == -35){     

      Serial.print("Final Title: "); 

      Serial.println(strTitle);

      Serial.println("Retry title setting: 1, Start capture: 2 ");       

      ipreKey3 = -35; 

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("1")==0){

      strTitle.concat(str); 

      Serial.println(strTitle);     

    

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("")==0 && str.compareTo("2")==0){

      Serial.println("--------------- Start Printing ---------------");

      flagKeyPrinting = 1;

 

      /*

      //file

      if(fileExist){

        Serial.println("File is not open");

      }else{       

        myFile = SD.open("test.txt", FILE_WRITE);    

        Serial.println("File is opening");

        fileExist = true;

      }

      */

     

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("")==0 && str.compareTo("3")==0){

      Serial.println("--------------- Stop Printing ---------------");

      flagKeyPrinting = 0;

      //file

      //delay(2000);

      if(fileExist){

        myFile.println("----------------- Stop File Writing ----------------");

        myFile.close();       

        fileExist = false;

        Serial.println("File is closing");

      }

    //--------------------------------------------------------------------------------------------             

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("")==0 && str.compareTo("4")==0){

      Serial.println("-------- Sampling Rate(default: 100 Hz)--------");

      Serial.println("1. Sampling Rate( 150 Hz)");

      Serial.println("2. Sampling Rate( 100 Hz)");

      Serial.println("3. Sampling Rate( 50 Hz)");

      Serial.println("4. Sampling Rate( 10 Hz)");

      Serial.println("------------------------------------------------");

      preKey2 = str;

     

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("4")==0 && str.compareTo("1")==0){

      Serial.println("----- Sampling Rate 150 Hz ---------------");

      samplingRate = 7;  

      EEPROM.write(0,samplingRate);     

      samplingRate = EEPROM.read(0);

      Serial.println(samplingRate);

      //-------------------------------------

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("4")==0 && str.compareTo("2")==0){

      Serial.println("----- Sampling Rate 100 Hz ---------------");

      samplingRate = 9;  

      EEPROM.write(0,samplingRate);     

      samplingRate = EEPROM.read(0);

      Serial.println(samplingRate);

      //-------------------------------------  

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("4")==0 && str.compareTo("3")==0){

      Serial.println("----- Sampling Rate 50 Hz ---------------");

      samplingRate = 18;  

      EEPROM.write(0,samplingRate);     

      samplingRate = EEPROM.read(0);

      Serial.println(samplingRate);

      //------------------------------------- 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("4")==0 && str.compareTo("4")==0){

      Serial.println("----- Sampling Rate 10 Hz ---------------");

      samplingRate = 95;        

      EEPROM.write(0,samplingRate);     

      samplingRate = EEPROM.read(0);

      Serial.println(samplingRate);

    //--------------------------------------------------------------------------------------------

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("")==0 && str.compareTo("5")==0){

      Serial.println("-------- Alogorithm: MinValue(Sensor: by pass - default)--------");

      Serial.println("1. Alogorithm: MinValue(Sensor: by pass)");

      Serial.println("2. Alogorithm: AvgMinValue(Sensor: 3 Avg.)");     

      Serial.println("------------------------------------------------");

      preKey2 = str;

     

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("5")==0 && str.compareTo("1")==0){

      Serial.println("----- Alogorithm: MinValue(Sensor: by pass)---------------");

      algorithmType = 1;  

      EEPROM.write(1,algorithmType);     

      algorithmType = EEPROM.read(1);

      Serial.println(algorithmType);

     

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("5")==0 && str.compareTo("2")==0){

      Serial.println("----- Alogorithm: AvgMinValue(Sensor: 3 Avg.)---------------");

      algorithmType = 2;  

      EEPROM.write(1,algorithmType);     

      algorithmType = EEPROM.read(1);

      Serial.println(algorithmType); 

 

    //--------------------------------------------------------------------------------------------             

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("")==0 && str.compareTo("6")==0){

      Serial.println("-------- Threshold(default: 80%)--------");

      Serial.println("1. Threshold: 90% ");

      Serial.println("2. Threshold: 80% ");

      Serial.println("3. Threshold: 70% ");

      Serial.println("4. Threshold: 60% ");

      Serial.println("5. Threshold: 50% ");

      Serial.println("------------------------------------------------");

      preKey2 = str; 

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("6")==0 && str.compareTo("1")==0){

      Serial.println("----- Threshold: 90%---------------");

      thresholdNum = 90;  

      EEPROM.write(2,thresholdNum);     

      thresholdNum = EEPROM.read(2);

      Serial.println(thresholdNum);

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("6")==0 && str.compareTo("2")==0){

      Serial.println("----- Threshold: 80%---------------");

      thresholdNum = 80;  

      EEPROM.write(2,thresholdNum);     

      thresholdNum = EEPROM.read(2);

      Serial.println(thresholdNum); 

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("6")==0 && str.compareTo("3")==0){

      Serial.println("----- Threshold: 70%---------------");

      thresholdNum = 70;  

      EEPROM.write(2,thresholdNum);     

      thresholdNum = EEPROM.read(2);

      Serial.println(thresholdNum);   

 

   }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("6")==0 && str.compareTo("4")==0){

      Serial.println("----- Threshold: 70%---------------");

      thresholdNum = 60;  

      EEPROM.write(2,thresholdNum);     

      thresholdNum = EEPROM.read(2);

      Serial.println(thresholdNum);    

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("6")==0 && str.compareTo("5")==0){

      Serial.println("----- Threshold: 50%---------------");

      thresholdNum = 50;  

      EEPROM.write(2,thresholdNum);     

      thresholdNum = EEPROM.read(2);

      Serial.println(thresholdNum);

      

    //--------------------------------------------------------------------------------------------             

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("")==0 && str.compareTo("7")==0){

      Serial.println("-------- Offset Mode--------");

      Serial.println("1. Measuring Mode(w/o Offset:Org) ");

      Serial.println("2. Offset Test Mode ");           

      Serial.println("3. Measuring Mode(w/ Offset)  ");     

      Serial.println("4. Offset Value Reset Mode ");     

      Serial.println("------------------------------------------------");

      preKey2 = str; 

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("7")==0 && str.compareTo("1")==0){

      Serial.println("----- Measuring Mode(w/o Offset:Org)---------------");

      offsetMode = 1;  

      EEPROM.write(3,offsetMode);     

      offsetMode = EEPROM.read(3);

      Serial.println(offsetMode);         

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("7")==0 && str.compareTo("2")==0){

      Serial.println("----- Offset Test Mode---------------");

      offsetMode = 2;  

      EEPROM.write(3,offsetMode);     

      offsetMode = EEPROM.read(3);

      Serial.println(offsetMode);   

      /////////////////////////////////

      //EEPROM.write(4,offsetValue);   

      /////////////////////////////////

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("7")==0 && str.compareTo("3")==0){

      Serial.println("----- Measuring Mode(w/ Offset)---------------");

      offsetMode = 3;  

      EEPROM.write(3,offsetMode);     

      offsetMode = EEPROM.read(3);

      Serial.println(offsetMode); 

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("7")==0 && str.compareTo("4")==0){

      Serial.println("----- Offset Value Reset Mode ---------------");

      EEPROMWrite(7, initMaxValue);     //Offset value 초기화

 

    //--------------------------------------------------------------------------------------------             

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("")==0 && str.compareTo("8")==0){

      Serial.println("-------- UART/WiFi Mode(default: UART)--------");

      Serial.println("1. UART Mode ");

      Serial.println("2. WiFi Mode ");                       

      Serial.println("------------------------------------------------");

      preKey2 = str;   

      

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("8")==0 && preKey3.compareTo("")==0  && str.compareTo("1")==0){

      Serial.println("----- UART Mode---------------");

      communicationMode = 1;  

      EEPROM.write(5,communicationMode);     

      communicationMode = EEPROM.read(5);

      Serial.println(communicationMode);                      

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("8")==0 && preKey3.compareTo("")==0 && str.compareTo("2")==0){

      Serial.println("----- WiFi Mode---------------");

      communicationMode = 2;  

      EEPROM.write(5,communicationMode);     

      communicationMode = EEPROM.read(5);

      Serial.println(communicationMode);       

 

    //--------------------------------------------------------------------------------------------             

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("")==0 && str.compareTo("9")==0){

      Serial.println("-------- Machine Number(default: 00) --------");

      Serial.println("1. 00 ");

      Serial.println("2. 01 ");                       

      Serial.println("3. 10 ");                       

      Serial.println("4. 11 ");                        

      Serial.println("------------------------------------------------");

      preKey2 = str;   

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("9")==0 && preKey3.compareTo("")==0  && str.compareTo("1")==0){

      Serial.println("-------- Machine Number(default: 00) -------");

      machineNum = 1;  

      EEPROM.write(6,machineNum);     

      machineNum = EEPROM.read(6);

      Serial.println(machineNum);                    

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("9")==0 && preKey3.compareTo("")==0  && str.compareTo("2")==0){

      Serial.println("-------- Machine Number(default: 01) -------");

      machineNum = 2;  

      EEPROM.write(6,machineNum);     

      machineNum = EEPROM.read(6);

      Serial.println(machineNum);                      

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("9")==0 && preKey3.compareTo("")==0  && str.compareTo("3")==0){

      Serial.println("-------- Machine Number(default: 10) -------");

      machineNum = 3;  

      EEPROM.write(6,machineNum);     

      machineNum = EEPROM.read(6);

      Serial.println(machineNum);            

 

    }else if(preKey1.compareTo("0")==0 && preKey2.compareTo("9")==0 && preKey3.compareTo("")==0  && str.compareTo("4")==0){

      Serial.println("-------- Machine Number(default: 11) -------");

      machineNum = 4;  

      EEPROM.write(6,machineNum);      

      machineNum = EEPROM.read(6);

      Serial.println(machineNum);                      

    //--------------------------------------------------------------------------------------------

    }

  }   

}

 

void clockDsiplay(){

  Serial.print(year());

  Serial.print(":");

  Serial.print(month());

  Serial.print(":");

  Serial.print(day());

  Serial.print(":");

  Serial.print(hour()); 

  printDigits(minute());

  printDigits(second());

  Serial.print(":");

  Serial.print(diffMilliSec);

  Serial.print(",");

  //Serial.println();

}

 

void printDigits(int digits){

  Serial.print(":");

  if(digits < 0) Serial.print('0');

  Serial.print(digits);

}

 

void setup() {

 

  enableInterrupt(BUSY, ISR_, FALLING);

 

  pinMode(RESET, OUTPUT);

  pinMode(CONVST, OUTPUT);

  pinMode(RD, OUTPUT);

  pinMode(RANGE, OUTPUT);

  pinMode(BUSY, INPUT);

 

  pinMode(OS0, OUTPUT);

  pinMode(OS1, OUTPUT);

  pinMode(OS2, OUTPUT);

  //pinMode(FIRST, OUTPUT);

 

  Serial.begin(115200);

  Serial3.begin(115200);

 

  setSyncProvider(RTC.get);

  setTime(12,0,0,23,7,19);

  refMilliSec = millis();

 

  //oversampling mode

  digitalWrite(OS0, LOW);

  digitalWrite(OS1, LOW);

  digitalWrite(OS2, LOW);

 

  //reset ADC to begin conversion  

  digitalWrite(RESET, HIGH);

  delayMicroseconds(10);

  digitalWrite(RESET, LOW);

 

  digitalWrite(CONVST, LOW); 

  digitalWrite(RD, HIGH);

  digitalWrite(RANGE, LOW);

  digitalWrite(BUSY, LOW);

 

  delayMicroseconds(1000);

 

  lcd.init();                      // LCD 초기화

  lcd.backlight();                // 백라이트 켜기

 

  //data Array 초기화

  for(k=0;k < 10;k++){

    tempArrA[k] = initMaxValue;

    tempArrB[k] = initMaxValue;

    tempArrC[k] = initMaxValue;

    tempArrD[k] = initMaxValue;

    

    tempArrE[k] = initMaxValue;

    tempArrF[k] = initMaxValue;

    tempArrG[k] = initMaxValue;

  }

 

  //init value  

  samplingRate = EEPROM.read(0);

  if(samplingRate < 1 || samplingRate > 100 ){

    samplingRate = 9;  //org 9 for 100Hz

  }

 

  algorithmType = EEPROM.read(1);

  if(algorithmType < 1 || algorithmType > 2 ){

    algorithmType = 1;  //org 1

  }

 

  thresholdNum = EEPROM.read(2);

  if(thresholdNum < 1 || thresholdNum > 100 ){

    thresholdNum = 80;  //org 80

  }

 

  offsetMode = EEPROM.read(3);

  if(offsetMode < 1 || offsetMode> 100 ){

    offsetMode = 1;  //org 1

  }

 

  communicationMode = EEPROM.read(5);

  if(communicationMode < 1 || communicationMode> 100 ){

    communicationMode = 1;  //org 1

  }

 

  machineNum = EEPROM.read(6);

  if(machineNum < 1 || machineNum> 100 ){

    machineNum = 1;  //org 1

  }

   

  Serial.println("---------------- Configuration -------------------------- ");

  Serial.print("SamplingRate:");

  Serial.println(samplingRate);

  Serial.print("algorithmType:");

  Serial.println(algorithmType);

  Serial.print("thresholdNum:");

  Serial.println(thresholdNum);

  Serial.print("offsetMode:");

  Serial.println(offsetMode);

  Serial.print("communicationMode:");

  Serial.println(communicationMode);

  Serial.print("machineNum:");

  Serial.println(machineNum);

  Serial.println("----------------------------------------------------------- ");

 

  curMilliSec = millis();

  preMilliSec = curMilliSec;

 

  //--- initial value --------------------------

  thresholdValue = (initMaxValue * thresholdNum / 100) ; //org:0.8 //삼성전기에서 원하는 threshold를 정하는 값

 

  sensorPreA=initMaxValue;

  sensorCurA=initMaxValue;

  sensorPreB=initMaxValue;

  sensorCurB=initMaxValue;

  sensorPreC=initMaxValue;

  sensorCurC=initMaxValue;

  sensorPreD=initMaxValue;

  sensorCurD=initMaxValue; 

  sensorPreE=initMaxValue;

  sensorCurE=initMaxValue;

  sensorPreF=initMaxValue;

  sensorCurF=initMaxValue;

  sensorPreG=initMaxValue;

  sensorCurG=initMaxValue;

  sensorPreH=initMaxValue;

  sensorCurH=initMaxValue;

 

  minA = initMaxValue;

  minB = initMaxValue;

  minC = initMaxValue;

  minD = initMaxValue;

  minE = initMaxValue;

  minF = initMaxValue;

  minG = initMaxValue;

  minH = initMaxValue;

 

  valueCountA = 0;

  valueCountB = 0;

  valueCountC = 0;

  valueCountD = 0;

  valueCountE = 0;

  valueCountF = 0;

  valueCountG = 0;

  valueCountH = 0;

 

  lcd.setCursor(0,0);

  lcd.print("-> Ready(1)..");

 

  // SD 카드 사용 준비

  mySDBegin();

 

  // SD카드 준비

  mySDInit();

 

  fileExist = true;

 

  // 카드의 루트 디렉터리 출력

  //mySD_ls();

 

  // 파일 쓰고 읽기 테스트

  //myFileRWTest(); 

 

  // 카드의 루트 디렉터리 출력

  //mySD_ls();

 

  //WiFi 사용 준비

  wifiInit();

 

}

 

void loop() {

 

  //-------- serial communication --------------- 

  serialcom();

 

 

(BUSY);

 

  //delayMicroseconds(40000);

  delay(samplingRate);

 

  digitalWrite(CONVST, LOW);

  delayMicroseconds(10);

  digitalWrite(CONVST, HIGH);

  enableInterrupt(BUSY, ISR_, FALLING);

 

  //------Time Diff------------------------------

  curMilliSec = millis();

  preMilliSec = curMilliSec - refMilliSec;

  diffMilliSec = (int)(preMilliSec%1000);

 

  //Serial.println("------------------ loop--------------------------- ");

 

  //---------------------------------------------

  int avgA, avgB, avgC, avgD, avgE, avgF, avgG, avgH;

 

  if(algorithmType == 2){

    //최소 평균값 사용시

    //새로운 값을 채울준비를 하자

    for(k=9;k > 0;k--){

      tempArrA[k] = tempArrA[k-1]; 

      tempArrB[k] = tempArrB[k-1]; 

      tempArrC[k] = tempArrC[k-1]; 

      tempArrD[k] = tempArrD[k-1];     

      tempArrE[k] = tempArrE[k-1]; 

      tempArrF[k] = tempArrF[k-1]; 

      tempArrG[k] = tempArrG[k-1]; 

    }

 

    tempArrA[0] = sensorA1;

    tempArrB[0] = sensorA2;

    tempArrC[0] = sensorA3;

    tempArrD[0] = sensorA4;

    tempArrE[0] = sensorA5;

    tempArrF[0] = sensorA6;

    tempArrG[0] = sensorA7;

    tempArrH[0] = sensorA8;

 

    avgA = findArrayAvg(tempArrA,3);

    avgB = findArrayAvg(tempArrB,3);

    avgC = findArrayAvg(tempArrC,3);

    avgD = findArrayAvg(tempArrD,3);

    avgE = findArrayAvg(tempArrE,3);

    avgF = findArrayAvg(tempArrF,3);

    avgG = findArrayAvg(tempArrG,3);

    avgH = findArrayAvg(tempArrH,3);

 

    sensorCurA = avgA;

    sensorCurB = avgB;

    sensorCurC = avgC;

    sensorCurD = avgD;

    sensorCurE = avgE;

    sensorCurF = avgF;

    sensorCurG = avgG;

    sensorCurH = avgH;

 

  }else if(algorithmType == 1){

    //센서값 그대로 사용

    sensorCurA = sensorA1;

    sensorCurB = sensorA2;

    sensorCurC = sensorA3;

    sensorCurD = sensorA4;

    sensorCurE = sensorA5;

    sensorCurF = sensorA6;

    sensorCurG = sensorA7;

    sensorCurH = sensorA8;

 

  }//if(algorithmType == 0){

 

  if(sensorCurA < minA) minA = sensorCurA;

  if(sensorCurB < minB) minB = sensorCurB;

  if(sensorCurC < minC) minC = sensorCurC;

  if(sensorCurD < minD) minD = sensorCurD;

  if(sensorCurE < minE) minE = sensorCurE;

  if(sensorCurF < minF) minF = sensorCurF;

  if(sensorCurG < minG) minG = sensorCurG;

  if(sensorCurH < minG) minH = sensorCurH;

 

  convertMinA1 = initMaxValue -  minA;

  convertMinA2 = initMaxValue -  minB;

  convertMinA3 = initMaxValue -  minC;

  convertMinA4 = initMaxValue -  minD;

  convertMinA5 = initMaxValue -  minE;

  convertMinA6 = initMaxValue -  minF;

  convertMinA7 = initMaxValue -  minG;

  convertMinA8 = initMaxValue -  minH;

 

  //-------------- offset test mode ---------------------------------------

  if(offsetMode == 2 && workingState == 1){

    if(sensorCurA > offsetValueCur) offsetValueCur = sensorCurA; 

    if(sensorCurB > offsetValueCur) offsetValueCur = sensorCurB; 

    if(sensorCurC > offsetValueCur) offsetValueCur = sensorCurC; 

    if(sensorCurD > offsetValueCur) offsetValueCur = sensorCurD; 

    if(sensorCurE > offsetValueCur) offsetValueCur = sensorCurE; 

    if(sensorCurF > offsetValueCur) offsetValueCur = sensorCurF; 

    if(sensorCurG > offsetValueCur) offsetValueCur = sensorCurG; 

    if(sensorCurH > offsetValueCur) offsetValueCur = sensorCurH;    

  }//if(offsetMode==2){

 

  

 

  //--------------- 센서 상태 점검---------------------------------------------

  if(sensorCurA < thresholdValue && sensorPreA >= thresholdValue){

    sensorStateA = 1;

  }else if(sensorCurA >= thresholdValue && sensorPreA < thresholdValue){

    sensorStateA = -1; 

  }

 

  if(sensorCurB < thresholdValue && sensorPreB >= thresholdValue){

    sensorStateB = 1;

  }else if(sensorCurB >= thresholdValue && sensorPreB < thresholdValue){

    sensorStateB = -1; 

  }

   

  if(sensorCurC < thresholdValue && sensorPreC >= thresholdValue){

    sensorStateC = 1;

  }if(sensorCurC >= thresholdValue && sensorPreC < thresholdValue){

    sensorStateC = -1;

  }

   

  if(sensorCurD < thresholdValue && sensorPreD >= thresholdValue){

    sensorStateD = 1;

  }else if(sensorCurD >= thresholdValue && sensorPreD < thresholdValue){

    sensorStateD = -1;

  }

 

  if(sensorCurE < thresholdValue && sensorPreE >= thresholdValue){

    sensorStateE = 1;

  }else if(sensorCurE >= thresholdValue && sensorPreE < thresholdValue){

    sensorStateE = -1;

  }

 

  if(sensorCurF < thresholdValue && sensorPreF >= thresholdValue){

    sensorStateF = 1;

  }else if(sensorCurF >= thresholdValue && sensorPreF < thresholdValue){

    sensorStateF = -1;

  }

   

  if(sensorCurG < thresholdValue && sensorPreG >= thresholdValue){

    sensorStateG = 1;

  }else if(sensorCurG >= thresholdValue && sensorPreG < thresholdValue){

    sensorStateG = -1;

  }

 

  if(sensorCurH < thresholdValue && sensorPreH >= thresholdValue){

    sensorStateH = 1;

  }else if(sensorCurG >= thresholdValue && sensorPreH < thresholdValue){

    sensorStateH = -1;

  }

 

  //---------- Working State 점검 ---------------------------------------

  readyMiliSec = abs(curMilliSec - waitMiliSec);

  //if((sensorStateA > 0 || sensorStateB > 0 || sensorStateC > 0 || sensorStateD > 0 || sensorStateE > 0 || sensorStateF > 0 || sensorStateG > 0) && workingState == 0 ){

  if(sensorStateA > 0 && sensorStateB > 0 && sensorStateC > 0 && sensorStateD > 0 && sensorStateE > 0 && sensorStateF > 0 && sensorStateG > 0 && workingState == 0 ){    

    //Measure

    workingState = 1;

    secCount = 0;   

    isTcpError=true;

    offsetValueCur = 0;

    Serial.println("---------------- Measure -------------------------- ");

 

    lcd.clear();

   

    if(fileExist){

      myFile = SD.open("test.txt", FILE_WRITE);     

      delayMicroseconds(10);

      if (myFile){     

        myFile.println("---------------- Measure -------------------------- ");      

        Serial.println("--> File is ready  ");

        myFile.close();

      }     

      lcd.setCursor(0,0);             // 1번째, 1라인

      lcd.print("-> Measuring(F:O)");

    }else{

      lcd.setCursor(0,0);             // 1번째, 1라인

      lcd.print("-> Measuring(F:X)");

    }

 

  }else if(sensorStateA < 0 && sensorStateB < 0 && sensorStateC < 0 && sensorStateD < 0 && sensorStateE < 0 && sensorStateF < 0 && sensorStateG < 0 && workingState == 1){  

  //}else if((sensorStateA < 0 || sensorStateB  < 0 || sensorStateC < 0 || sensorStateD < 0 || sensorStateE < 0 || sensorStateF < 0 || sensorStateG < 0) && workingState == 1){  

    //wait

    workingState = 2; 

    //---------------------------------   

    myFile = SD.open("test.txt", FILE_WRITE);

    delayMicroseconds(10);    

    // if the file opened okay, write to it:

    if (myFile){       

     myFile.println("--------------- Result value ------------------------- ");      

      // close the file:

      myFile.close();

      Serial.println("--------------- Result value ------------------------- ");

    }else{

      // if the file didn't open, print an error:

      Serial.println("error opening test.txt");

      Serial.println("--------------- Result value ------------------------- ");   

    }         

    //---------------------------------      

 

    //------------ offset value write -----------------------------------------

    offsetValuePre = (int)EEPROMRead(7);

    Serial.println("--------------- Offset Value First (EEPROM) ------------------------- ");    

    Serial.println(offsetValueRef);

    

    if(offsetValuePre < 3000){

      offsetValuePre = initMaxValue;

    }

    offsetValueRef = (offsetValuePre + offsetValueCur)/2; 

    Serial.println("--------------- Offset Value Before (EEPROM) ------------------------- ");

    Serial.println(offsetValuePre);

    Serial.println(offsetValueCur);

    Serial.println(offsetValueRef);

  

    EEPROMWrite(7, offsetValueRef);

       

    Serial.println("--------------- Offset Value After (EEPROM) ------------------------- ");

    offsetValueRef = (int)EEPROMRead(7);

    Serial.println(offsetValueRef);

 

   

    //-----------------------------------------------------------------------------

    waitMiliSec = millis();

 

    lcd.clear();

 

    strMin = "A1:";

    strMin.concat(convertMinA1);

    strMin.concat(",A2:");

    strMin.concat(convertMinA2);

    strMin.concat(",A3:");

    strMin.concat(convertMinA3);

    strMin.concat(",A4:");

    strMin.concat(convertMinA4);

    strMin.concat(",A5:");

    strMin.concat(convertMinA5);

    strMin.concat(",A6:");

    strMin.concat(convertMinA6);

    strMin.concat(",A7:");

    strMin.concat(convertMinA7);

    strMin.concat(",A8:");

    strMin.concat(convertMinA8);

 

    strCount = "Count A1:";

    strCount.concat(valueCountA);

    strCount.concat(",A2:");

    strCount.concat(valueCountB);

    strCount.concat(",A3:");

    strCount.concat(valueCountC);

    strCount.concat(",A4:");

    strCount.concat(valueCountD);

    strCount.concat(",A5:");

    strCount.concat(valueCountE);

    strCount.concat(",A6:");

    strCount.concat(valueCountF);

    strCount.concat(",A7:");

    strCount.concat(valueCountG);

    strCount.concat(",A8:");

    strCount.concat(valueCountH);

 

    lcd.setCursor(0,0);

    lcd.print("-> Waiting.......");   

 

    strLcd = "A:";

    strLcd.concat(valueCountA);

    strLcd.concat(",B:");

    strLcd.concat(valueCountA);

    strLcd.concat(",C:");

    strLcd.concat(valueCountC);   

    lcd.setCursor(0,1);

    lcd.print(strLcd);   

 

    strLcd = "D:";

    strLcd.concat(valueCountD);

    strLcd.concat(",E:");

    strLcd.concat(valueCountE);

    strLcd.concat(",F:");

    strLcd.concat(valueCountF);   

    lcd.setCursor(0,2);

    lcd.print(strLcd);

 

    strLcd = "G:";

    strLcd.concat(valueCountG);

    strLcd.concat(",H:");

    strLcd.concat(valueCountH);   

    lcd.setCursor(0,3);

    lcd.print(strLcd);

 

    //----------------------------------   

    /*

    if(fileExist){

      myFile.println(strMin);

      myFile.println(strCount);             

    }   

    Serial.println(strMin);   

    Serial.println(strCount);   

    */

    //----------------------------------

 

    //---------------------------------

    myFile = SD.open("test.txt", FILE_WRITE);     

    delayMicroseconds(10);

    // if the file opened okay, write to it:

    if (myFile){       

      myFile.println(strMin); 

      myFile.println(strCount);             

      // close the file:

      myFile.close();

      Serial.println(strMin);   

      Serial.println(strCount);

    }else{

      // if the file didn't open, print an error:

      Serial.println("error opening test.txt");

    }  

    //---------------------------------

 

    //WiFi 전송 모드

    delayMicroseconds(10);

 

    int i;

    if(wifiReady && communicationMode == 2){

      if(isTcpError){

        for(i=0;i < 3;i++){

          //////////////////////////////////////

          if (wifi.createTCP(HOST_NAME, HOST_PORT)){

            //Serial.print("create tcp ok\r\n");

            myFile = SD.open("test.txt");

            char sndbuf[255];

            char cTemp;

            int cnt = 0;

   

            if (myFile){     

              while (myFile.available()){         

                //strcpy(sndbuf, myFile.read());

                cTemp = myFile.read();

                sndbuf[cnt] = cTemp;

                cnt++;

               

                if(cTemp =='\n')

                {

                   sndbuf[cnt] = 0;

                   wifi.send((const uint8_t*)sndbuf, cnt*sizeof(cnt));

                    //Serial.print(cnt);

                   cnt = 0;              

                }       

              }           

              myFile.close();     

            }else{     

              Serial.println("error opening test.txt");

            }

    

            if (wifi.releaseTCP()) {

              //Serial.print("release tcp ok\r\n");

            } else {

               Serial.print("release tcp err\r\n");

            }

   

            SD.remove("test.txt");

            if (SD.exists("test.txt")) { //If the file still exist display message exist

              //Serial.println("The test.txt exists.");

            } else { //If the file was successfully deleted display message that the file was already deleted.

              //Serial.println("The test.txt doesn't exist.");

            }

            isTcpError=false;

            break;

           

          } else {

            Serial.print("create tcp err\r\n");

            isTcpError=true;

          }

          ///////////////////////////////////////

        }//for(i=0;i < 3;i++){             

      }//if(isTcpError){    

    }else{

      Serial.print("WiFi is not ready\r\n");

    }

 

   

 

    minA = initMaxValue;

    minB = initMaxValue;

    minC = initMaxValue;

    minD = initMaxValue;

    minE = initMaxValue;

    minF = initMaxValue;

    minG = initMaxValue;

    minH = initMaxValue; 

 

    valueCountA = 0;

    valueCountB = 0;

    valueCountC = 0;

    valueCountD = 0;

    valueCountE = 0;

    valueCountF = 0;

    valueCountG = 0;

    valueCountH = 0;

 

  }else if(workingState == 2 && readyMiliSec >= waitRefTime ){

    //ready

    workingState = 0; 

 

    //---------------------------------   

    myFile = SD.open("test.txt", FILE_WRITE);     

    // if the file opened okay, write to it:

    delayMicroseconds(10);

    if (myFile){       

     myFile.println("------------------ Ready(2) ---------------------------  ");      

      // close the file:

      myFile.close();

      Serial.println("------------------ Ready(2) --------------------------- ");

    }else{

      // if the file didn't open, print an error:

      Serial.println("error opening test.txt");

      Serial.println("------------------ Ready(2) ---------------------------  ");   

    }         

    //---------------------------------      

 

    lcd.clear();

    lcd.setCursor(0,0);

    lcd.print("-> Ready(2)........");

 

    minA = initMaxValue;

    minB = initMaxValue;

    minC = initMaxValue;

    minD = initMaxValue;

    minE = initMaxValue;

    minF = initMaxValue;

    minG = initMaxValue;

    minH = initMaxValue; 

 

    valueCountA = 0;

    valueCountB = 0;

    valueCountC = 0;

    valueCountD = 0;

    valueCountE = 0;

    valueCountF = 0;

    valueCountG = 0;

    valueCountH = 0;

   

  }else if(workingState == 2 && readyMiliSec < waitRefTime ){

    //ready

    lcd.setCursor(13,0);

    if(readyMiliSec%100 == 0){

      secCount++;

      lcd.print(secCount);

    }

    //Serial.println(secCount);   

  }

 

 

  if(workingState==1){

    if(sensorStateA > 0) valueCountA++;     

    if(sensorStateB > 0) valueCountB++;

    if(sensorStateC > 0) valueCountC++;

    if(sensorStateD > 0) valueCountD++;

    if(sensorStateE > 0) valueCountE++;

    if(sensorStateF > 0) valueCountF++;

    if(sensorStateG > 0) valueCountG++;

    if(sensorStateH > 0) valueCountH++;    

  }

 

 

 

  // com port Printing 

  if(workingState== 1  && flagKeyPrinting > 0){

  //if(flagPrintingReady > 0){

    flagPrinting =1;

   /*

    strInfo = year();

    strInfo.concat(":");

    strInfo.concat(month());

    strInfo.concat(":");

    strInfo.concat(day());

    strInfo.concat(":");

    strInfo.concat(hour());

    strInfo.concat(":");

    strInfo.concat(minute());

    strInfo.concat(":");

    //strInfo.concat(second());

    isecTime = second();

    if((preMiliRef < diffMilliSec) && (preSecRef != isecTime)){

      isecTime = preSecRef;     

    }else if((preMiliRef > diffMilliSec) && (preSecRef == isecTime)){

      //Serial.println(preMiliRef);

      //Serial.println(diffMilliSec);

      //Serial.println(preSecRef);

      //Serial.println(isecTime);

      if(isecTime == 59){

        isecTime = 0;

      }else{

        isecTime = isecTime + 1;       

      }     

    }       

    strInfo.concat(isecTime);

    strInfo.concat(":");

    strInfo.concat(diffMilliSec);

 

    Serial.println(strInfo); 

    */

   

  }else if(flagKeyPrinting == 0 || workingState == 0 || workingState == 2  ){

  //}else{

    flagPrinting =0;

  }

 

 

  /*

  if(flagKeyPrinting > 0){

     flagPrinting =1;

  }else{

     flagPrinting =0;

  }

  */

 

 

 

 

 

/*

  lcd.setCursor(0,0);             // 1번째, 1라인

  //lcd.print("Hello, world!");

  //lcd.clear();

  lcd.print("A1:");

  lcd.print(sensorA1);

 

  lcd.setCursor(10,0);             // 10번째, 1라인

  lcd.print("A2:");

  lcd.print(sensorA2);

 

  lcd.setCursor(0,1);             // 1번째, 2라인

  lcd.print("A3:");

  lcd.print(sensorA3);

 

  lcd.setCursor(10,1);             // 10번째, 2라인

  lcd.print("A4:");

  lcd.print(sensorA4);

 

  lcd.setCursor(0,2);             // 1번째, 3라인

  lcd.print("A5:");

  lcd.print(sensorA5);

 

  lcd.setCursor(10,2);             // 10번째, 3라인

  lcd.print("A6:");

  lcd.print(sensorA6);

 

  lcd.setCursor(0,3);             // 1번째, 4라인

  lcd.print("A7:");

  lcd.print(sensorA7);

 

  lcd.setCursor(10,3);             // 10번째, 4라인

  lcd.print("A8:");

  lcd.print(sensorA8);

  */

 

  //-- WiFi Check ---------------------------

// if(Serial3.available()){

//   Serial.write(Serial3.read());

// } 

 // if(Serial.available()){

//   Serial3.write(Serial.read());

// }

 

//------ update -------------

  sensorPreA = sensorCurA; 

  sensorPreB = sensorCurB; 

  sensorPreC = sensorCurC; 

  sensorPreD = sensorCurD; 

  sensorPreE = sensorCurE; 

  sensorPreF = sensorCurF; 

  sensorPreG = sensorCurG; 

  sensorPreH = sensorCurH; 

 

}

 

void ISR_ ()  {

 

//flagPrinting =1;

 

  //Serial.println("------------------ ADC--------------------------- ");

 

  //Serial.print(millis());

  //Serial.print(",");

   //---------- Time Printing ----------------------

  if(flagPrinting > 0 ){

    /*

    strInfo = year();

    strInfo.concat(":");

    strInfo.concat(month());

    strInfo.concat(":");

    strInfo.concat(day());

    strInfo.concat(":");

    strInfo.concat(hour());

    strInfo.concat(":");

    strInfo.concat(minute());

    strInfo.concat(":");

    //strInfo.concat(second());

    isecTime = second();

 

    if((preMiliRef < diffMilliSec) && (preSecRef != isecTime)){

      isecTime = preSecRef;     

    }else if((preMiliRef > diffMilliSec) && (preSecRef == isecTime)){

      //Serial.println(preMiliRef);

      //Serial.println(diffMilliSec);

      //Serial.println(preSecRef);

      //Serial.println(isecTime);

      if(isecTime == 59){

        isecTime = 0;

      }else{

        isecTime = isecTime + 1;       

      }     

    }

       

    strInfo.concat(isecTime);

    strInfo.concat(":");

    strInfo.concat(diffMilliSec);

    */   

    strInfo = millis();

  } 

  //---------------------------

  preMiliRef = diffMilliSec;

  preSecRef = isecTime;

  

  //--------------------------

  //Sensor A1

  delayMicroseconds(1);

  digitalWrite(RD, LOW);

  delayMicroseconds(1);

  readDBpins(); // do read

  sensorA1 = valueSensor;

  convertA1 = initMaxValue -  sensorA1;  

  digitalWrite(RD, HIGH); 

  if(flagPrinting > 0 ){

    strInfo.concat(",A1:");

    //strInfo.concat(convertA1);

    strInfo.concat(sensorA1);

  } 

 

  //--------------------------

  //Sensor A2

  delayMicroseconds(1);

  digitalWrite(RD, LOW);

  delayMicroseconds(1);

  readDBpins(); // do read

  sensorA2 = valueSensor;

  convertA2 = initMaxValue -  sensorA2;  

  digitalWrite(RD, HIGH); 

  if(flagPrinting > 0 ){

      strInfo.concat(",A2:");

      //strInfo.concat(convertA2);   

      strInfo.concat(sensorA2);

  }

 

  //--------------------------

  //Sensor A3

  delayMicroseconds(1);

  digitalWrite(RD, LOW);

  delayMicroseconds(1);

  readDBpins(); // do read

  sensorA3 = valueSensor;

  convertA3 = initMaxValue -  sensorA3;  

  digitalWrite(RD, HIGH);

  if(flagPrinting > 0 ){

      strInfo.concat(",A3:");

      //strInfo.concat(convertA3);  

      strInfo.concat(sensorA3);

  }

 

  //--------------------------

  //Sensor A4

  delayMicroseconds(1);

  digitalWrite(RD, LOW);

  delayMicroseconds(1);

  readDBpins(); // do read

  sensorA4 = valueSensor;

  convertA4 = initMaxValue -  sensorA4; 

  digitalWrite(RD, HIGH);

  if(flagPrinting > 0 ){ 

      strInfo.concat(",A4:");

      //strInfo.concat(convertA4);

      strInfo.concat(sensorA4);  

  }

 

  //--------------------------

  //Sensor A5

  delayMicroseconds(1);

  digitalWrite(RD, LOW);

  delayMicroseconds(1);

  readDBpins(); // do read

  sensorA5 = valueSensor;

  convertA5 = initMaxValue -  sensorA5; 

  digitalWrite(RD, HIGH);

  if(flagPrinting > 0 ){

      strInfo.concat(",A5:");

      //strInfo.concat(convertA5);

      strInfo.concat(sensorA5);    

  }

 

  //--------------------------

  //Sensor A6

  delayMicroseconds(1);

  digitalWrite(RD, LOW);

  delayMicroseconds(1);

  readDBpins(); // do read

  sensorA6 = valueSensor;

  convertA6 = initMaxValue -  sensorA6; 

  digitalWrite(RD, HIGH);

  if(flagPrinting > 0 ){

      strInfo.concat(",A6:");

      //strInfo.concat(convertA6);

      strInfo.concat(sensorA6);     

  }

 

  //--------------------------

  //Sensor A7

  delayMicroseconds(1);

  digitalWrite(RD, LOW);

  delayMicroseconds(1);

  readDBpins(); // do read

  sensorA7 = valueSensor;

  convertA7 = initMaxValue -  sensorA7; 

  digitalWrite(RD, HIGH);

  if(flagPrinting > 0 ){

      strInfo.concat(",A7:");

      //strInfo.concat(convertA7);

      strInfo.concat(sensorA7);     

  }

 

  //--------------------------

  //Sensor A8

  delayMicroseconds(1);

  digitalWrite(RD, LOW);

  delayMicroseconds(1);

  readDBpins(); // do read

  sensorA8 = valueSensor;

  convertA8 = initMaxValue -  sensorA8; 

  digitalWrite(RD, HIGH);

  

  if(flagPrinting > 0 ){

      strInfo.concat(",A8:");

      //strInfo.concat(convertA8);  

      strInfo.concat(sensorA8);     

      //---------------------------------     

      myFile = SD.open("test.txt", FILE_WRITE); 

      delayMicroseconds(10);   

      // if the file opened okay, write to it:

      if (myFile){       

        myFile.println(strInfo); 

        // close the file:

        myFile.close();

        Serial.println(strInfo);

      }else{

        // if the file didn't open, print an error:

        Serial.println("error opening test.txt");

        Serial.println(strInfo);

      }           

      //---------------------------------              

  }

}

 

void readDBpins()

{

  valueSensor = 0;    //센서값 초기화

 

  rawData[0] = digitalRead(DB15);

  //Serial.print (rawData[0]); 

  

  if (rawData[0] > 0) {rawData[0] = 32768;}

  //Serial.print ("rawData[0]:"+rawData[0]);

 

  rawData[1] = digitalRead(DB14); 

  //Serial.print (rawData[1]);

  if (rawData[1] > 0) {rawData[1] = 16384;}

  //Serial.print ("rawData[1]:"+rawData[1]);

 

  rawData[2] = digitalRead(DB13);

  //Serial.print (rawData[2]);

  if (rawData[2] > 0) {rawData[2] = 8192;}

  //Serial.print ("rawData[2]:"+rawData[2]);

 

  rawData[3] = digitalRead(DB12);

  //Serial.print (rawData[3]);

  if (rawData[3] > 0) {rawData[3] = 4096;}

  //Serial.print ("rawData[3]:"+rawData[3]);

 

  rawData[4] = digitalRead(DB11);

  //Serial.print (rawData[4]);

  if (rawData[4] > 0) {rawData[4] = 2048;}

 

  rawData[5] = digitalRead(DB10);

  //Serial.print (rawData[5]);

  if (rawData[5] > 0) {rawData[5] = 1024;}

 

  rawData[6] = digitalRead(DB9);

  //Serial.print (rawData[6]);

  if (rawData[6] > 0) {rawData[6] = 512;}

 

  rawData[7] = digitalRead(DB8);

  //Serial.print (rawData[7]);

  if (rawData[7] > 0) {rawData[7] = 256;}

 

  rawData[8] = digitalRead(DB7);

  //Serial.print (rawData[8]);

  if (rawData[8] > 0) {rawData[8] = 128;}

 

  rawData[9] = digitalRead(DB6);

  //Serial.print (rawData[9]);

  if (rawData[9] > 0) {rawData[9] = 64;}

 

  rawData[10] = digitalRead(DB5);

  //Serial.print (rawData[10]);

  if (rawData[10] > 0) {rawData[10] = 32;}

 

  rawData[11] = digitalRead(DB4);

  //Serial.print (rawData[11]);

  if (rawData[11] > 0) {rawData[11] = 16;}

 

  rawData[12] = digitalRead(DB3);

  //Serial.print (rawData[12]);

  if (rawData[12] > 0) {rawData[12] = 8;}

 

  rawData[13] = digitalRead(DB2);

  //Serial.print (rawData[13]);

  if (rawData[13] > 0) {rawData[13] = 4;}

 

  rawData[14] = digitalRead(DB1);

  //Serial.print (rawData[14]);

  if (rawData[14] > 0) {rawData[14] = 2;}


  rawData[15] = digitalRead(DB0);

  //Serial.print (rawData[15]);

  if (rawData[15] > 0) {rawData[15] = 1;}

 

  valueSensor = rawData[0]+rawData[1]+rawData[2]+rawData[3]+rawData[4]+rawData[5]+rawData[6]+rawData[7]+rawData[8]+rawData[9]+rawData[10]+rawData[11]+rawData[12]+rawData[13]+rawData[14]+rawData[15];

  //Serial.println(valueSensor);

}
