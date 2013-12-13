// Author : Rahul Rajeev
// medScan @ Developer Weekend 2013

#include <Stepper.h>
#define STEPS 100

boolean bt= true;	//using the bluetooth module(true) or the included UART(false)

int stepper_1 = 7;
int stepper_2 = 6;
int stepper_3 = 5;
int stepper_4 = 4;

int UV_led = 8;

const int start_button = 2;
int tray_motor = 3;

int analog_pin = 5;

int buttonState = 0; 
int sum =0;
int sum_high =0;
int sum_low =0;

Stepper stepper(STEPS, stepper_1, stepper_2, stepper_3, stepper_4);

void setup()
{
  // set the speed of the motor to 60 RPMs
  stepper.setSpeed(60);
  pinMode(tray_motor, OUTPUT);
  pinMode(start_button, INPUT);
  pinMode(UV_led,OUTPUT);
  Serial1.begin(115200);
  Serial.begin(115200);
  analogWrite(tray_motor, 255);
  
  //home the stepper motor
  stepper.step(250);
}

void loop()
{

  buttonState = digitalRead(start_button);
  // move a number of steps equal to the change in the
  // sensor reading
  
  
  
  if(buttonState == HIGH){
    //pull tray in
    analogWrite(tray_motor, 0);
    delay(3000);
    analogWrite(tray_motor, 255);
    delay(1000);
	
	
	//packet format : 
	//FF | FE | 01 - begin sequence
	//FF | FE | 02 - end read
	//FF | FE | 03 | val_high | val_low  - sending 2 byte int (val_high*255+val_low)
	//data sent twice since there is no other CDC (error checking mechanism)
	
    //begin read
    if(bt)
    {
      Serial1.write(0xFF);
      Serial1.write(0xFE);
      Serial1.write(0x01);
      Serial1.write(0xFF);
      Serial1.write(0xFE);
      Serial1.write(0x01);
    }else{
      Serial.write(0xFF);
      Serial.write(0xFE);
      Serial.write(0x01);
      Serial.write(0xFF);
      Serial.write(0xFE);
      Serial.write(0x01);
    }
	
	//switch on LED
    digitalWrite(UV_led,HIGH);
	
	//start reading
    for(int i=0;i<200;i++)
    {
	  //move one step along the -ve x-axis
      stepper.step(-1);
	  
	  //wait for the reading head to stabilize
      delay(50);
      sum=0;
	  
	  // take average of 10 readings for increased accuracy
      for(int j=0;j<10;j++)
      {
        sum+=analogRead(analog_pin);
        delay(5);
      }
      sum = sum/10;
      sum_high = sum/255;
      sum_low = sum - (sum_high*255);
	  
      //data transfer
      if(bt)
      {
        Serial1.write(0xFF);
        Serial1.write(0xFE);
        Serial1.write(0x03);
        Serial1.write(sum_high);
        Serial1.write(sum_low);
      }else{
        Serial.write(0xFF);
        Serial.write(0xFE);
        Serial.write(0x03);
        Serial.write(sum_high);
        Serial.write(sum_low);
      }
    }
    digitalWrite(UV_led,LOW);
    //end
    
    if(bt)
    {
      Serial1.write(0xFF);
      Serial1.write(0xFE);
      Serial1.write(0x02);
      Serial1.write(0xFF);
      Serial1.write(0xFE);
      Serial1.write(0x02);
    }else{
      Serial.write(0xFF);
      Serial.write(0xFE);
      Serial.write(0x02);
      Serial.write(0xFF);
      Serial.write(0xFE);
      Serial.write(0x02);
    }
    stepper.step(220);
  }
  delay(100); 
}

