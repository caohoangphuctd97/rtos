

//Include LCD and I2C library

#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX
//Declaring some global variables
int gyro_x, gyro_y, gyro_z;
long acc_x, acc_y, acc_z, acc_total_vector;
int temperature;
long gyro_x_cal, gyro_y_cal, gyro_z_cal,acc_x_cal,acc_y_cal,acc_z_cal;
long loop_timer;
int lcd_loop_counter;
float angle_pitch, angle_roll, angle_yaw,gyro_yaw_input;
int angle_pitch_buffer, angle_roll_buffer;
boolean set_gyro_angles;
float angle_roll_acc, angle_pitch_acc;
float angle_pitch_output, angle_roll_output,Acc_angle_error_x,Acc_angle_error_y;
int data,m[6],so[6],nho;
int count,zone,value,value1;

// đọc các giá trị gia tốc góc
void read_mpu_6050_data(){                                             //Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  acc_x = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_x variable 2byte lấy 1 byte dịch qua 8
  acc_y = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_y variable ví dụ 0000000011101111 <<8 = 1110111100000000|10001000 = 1110111110001000
  acc_z = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_z variable
  temperature = Wire.read()<<8|Wire.read();                            //Add the low and high byte to the temperature variable
  gyro_x = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_x variable
  gyro_y = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_y variable
  gyro_z = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_z variable

}



/// Cài đặt thông số cho mpu6050
void setup_mpu_6050_registers(){
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050 địa chỉ
  Wire.write(0x6B);                                                    //Send the requested starting register  thanh ghi năng lượng
  Wire.write(0x00);                                                    //Set the requested starting register   set chế độ
  Wire.endTransmission();                                              //End the transmission
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    //Send the requested starting register  thanh ghi cấu hình acc
  Wire.write(0x10);                                                    //Set the requested starting register   set 00010000
  Wire.endTransmission();                                              //End the transmission
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    //Send the requested starting register
  Wire.write(0x08);                                                    //Set the requested starting register   set 00001000
  Wire.endTransmission();                                              //End the transmission
}

void setup() {
//  Wire.begin();                                                        //Start I2C as master
  Serial.begin(9600);                                               //Use only for debugging
                                      //Set output 13 (LED) as output
 // mySerial.begin(57600);
  setup_mpu_6050_registers();    
  pinMode(8,INPUT);
  pinMode(9,INPUT);
  zone = 0;  
  for (int cal_int = 0; cal_int < 2000 ; cal_int ++){                  //Run this code 2000 times
                   
    read_mpu_6050_data();                                              //        Read the raw acc and gyro data from the MPU-6050

    gyro_z_cal += gyro_z; 

    delay(3);                                                   
  }

    gyro_z_cal /= 2000;                                                  //Divide the gyro_z_cal variable by 2000 to get the avarage offset

  loop_timer = micros();                                               //Reset the loop timer
}

void loop(){
  // đọc giá trị analog của cảm biến đếm zone
     value = analogRead(A7);
  
   value1 = analogRead(A6); 
//   Serial.print(value1);
//   Serial.print("\t");
//   Serial.println(value);
   if(value>900||value1 >900) count = 1;
   if(count == 1&& value <900 && value1 <900) {
    zone +=1;
    count =0;
   }
   so[5] = zone;
//    while (Serial.available()>0){
//      buffer[count] = Serial.read();
//      
//      count +=1;     
//      
//    }
  
   read_mpu_6050_data(); 
   

  gyro_z -= gyro_z_cal;                                                //Subtract the offset calibration value from the raw gyro_z value
// công thức tính góc
  gyro_yaw_input += gyro_z* 0.0000611;   
    
 // Serial.println(gyro_yaw_input); 
  while(micros() - loop_timer < 4000);                                 //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  loop_timer = micros();                                               //Reset the loop timer
//  PID();                                                 
    data = gyro_yaw_input*100;
 
  if(data<0){
    so[0]= '-';
    data = data*-1;
  }
  else
    so[0] = '+';
  so[1] = data/1000;
  nho = data%1000;
  so[2] = nho/100;
  nho = nho%100;
  so[3] = nho/10;  
  so[4] = nho%10;
  for(int i=0; i<=5;i++){
    m[i]=so[i];
    Serial.write(m[i]);
  }
}
