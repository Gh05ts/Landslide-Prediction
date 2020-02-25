#include <MPU6050_tockn.h>
#include <Wire.h>
#include <SoftwareSerial.h>
SoftwareSerial MySerial(9,10);  //Rx,Tx

MPU6050 mpu6050(Wire);

long timer = 0;

void setup() {
  Serial.begin(115200);
  MySerial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  
  MySerial.println("AT");
  delay(200);

  MySerial.println("AT+CPIN?");
  delay(200);

  MySerial.println("AT+CREG?");
  delay(200);

  MySerial.println("AT+CGATT?");
  delay(200);

  MySerial.println("AT+CIPSHUT");
  delay(200);

  MySerial.println("AT+CIPSTATUS");
  delay(1000);

  MySerial.println("AT+CIPMUX=0");
  delay(1000);

  ShowSerialData();

  MySerial.println("AT+CSTT=\"airtelgprs.com\",\"\",\"\"");  //start task and setting the APN,
  delay(200);

  ShowSerialData();
   
  MySerial.println("AT+CIICR");  //bring up wireless connection
  delay(6000);  //Increase if necessary 

  ShowSerialData();

  MySerial.println("AT+CIFSR");  //get local IP adress
  delay(2000);

  ShowSerialData();

  for(int i = 0; i < 20; i++) {
    mpu6050.update();
    Serial.println("=======================================================");
    Serial.print("accX : ");Serial.print(mpu6050.getAccX());
    Serial.print("\taccY : ");Serial.print(mpu6050.getAccY());
    Serial.print("\taccZ : ");Serial.println(mpu6050.getAccZ()+0.05);  //calibrated for given unit
    
    Serial.print("angleX : ");Serial.print(mpu6050.getAngleX()+1.7);  //calibrated for given unit
    Serial.print("\tangleY : ");Serial.print(mpu6050.getAngleY()-0.8);  //calibrated for given unit
    Serial.print("\tangleZ : ");Serial.println(mpu6050.getAngleZ()+0.6);  //calibrated for given unit
    Serial.println("=======================================================\n");
    int val;
    val = analogRead(0);  //connect Soil Moisture sensor to Analog 0
    Serial.print("Moisture Value= ");Serial.println(val);
    delay(1000);
  }
}

void loop() {
  
  mpu6050.update();
  
    if(millis() - timer > 25000) {  
    Serial.println("=======================================================");
    Serial.print("temp : ");Serial.println(mpu6050.getTemp());
    Serial.print("accX : ");Serial.print(mpu6050.getAccX());
    Serial.print("\taccY : ");Serial.print(mpu6050.getAccY());
    Serial.print("\taccZ : ");Serial.println(mpu6050.getAccZ()+0.05);  //calibrated for given unit
    
    Serial.print("angleX : ");Serial.print(mpu6050.getAngleX()+1.7);  //calibrated for given unit
    Serial.print("\tangleY : ");Serial.print(mpu6050.getAngleY()-0.8);  //calibrated for given unit
    Serial.print("\tangleZ : ");Serial.println(mpu6050.getAngleZ()+0.6);  //calibrated for given unit
    Serial.println("=======================================================\n");
    int val;
    val = analogRead(0); //connect Soil Moisture sensor to Analog 0
    Serial.print("Moisture Value= ");Serial.println(val);

    Connect2Server();  // Establishing connection
    Send(mpu6050.getAccX(), mpu6050.getAccY(), mpu6050.getAccZ()+0.05, mpu6050.getAngleX()+1.7, 
         mpu6050.getAngleY()-0.8, mpu6050.getAngleZ()+0.6, val);  // Uploading data to Thingspeak
    EndConnection();  // Terminating connection
    delay(1000);
    
    timer = millis();
  }
}

void Connect2Server() {
  MySerial.println("AT+CIPSPRT=0");
  delay(1000);

  ShowSerialData();

  MySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");  //start up the connection
  delay(2000);

  ShowSerialData();

  MySerial.println("AT+CIPSEND");  //begin send data to remote server
  delay(10000);  //updated, still giving errors randomly
  ShowSerialData();
}

void EndConnection() {
  MySerial.println("AT+CIPSHUT");  //close the connection
  delay(100);
  ShowSerialData();
}

void ShowSerialData() {
  while(MySerial.available()!=0)
    Serial.write(MySerial.read());
}

void Send(float data1, float data2, float data3, float data4, float data5, float data6, int data7) {
  String str = "GET http://api.thingspeak.com/update?api_key=VSM4ILHBYBUOZSS6&field1="+String(data1)
               +"&field2="+String(data2)+"&field3="+String(data3)+"&field4="+String(data4)+"&field5="
               +String(data5)+"&field6="+String(data6)+"&field7="+String(data7);

  MySerial.println(str);//begin send data to remote server

  delay(2000);
  ShowSerialData();
  MySerial.println((char)26);  //sending
  delay(5000);  //waitting for reply, important! the time is base on the condition of internet
  MySerial.println();

  ShowSerialData();
}
