#include "MeOrion.h"
#include "SoftwareSerial.h"
//#define sv_mode true
#define pump_mode true
#ifdef sv_mode
int sv_pin = mePort[PORT_4].s2;
Servo sv;
MeUSBHost usbhost(PORT_5);
#else
MeUSBHost usbhost(PORT_6);
#endif
MeDCMotor dcLeft(M1);
MeDCMotor dcRight(M2);
MeDCMotor dcRClaw(PORT_1);
MeDCMotor dcClaw(PORT_2);
void parseJoystick(unsigned char *buf)   //Analytic function, print 8 bytes from USB Host
{
  int i = 0;
  for(i = 0; i < 7; i++)
  {
    //Serial.print(buf[i]);  //It won't work if you connect to the Makeblock Orion.
    //Serial.print('-');
  }
  switch((buf[4]>>4)){
    case 0:
      stopClaw();
      break;
    case 1:
      openClaw();
      break;
    case 4:
      closeClaw();
      break;
    case 2:
      rotateClaw(1);
      break;
    case 8:
      rotateClaw(-1);
      break;
  }
  if((buf[4]&15)!=15){
    switch(buf[4]&7){
      case 0:
        move(1.5,1.5);
        break;
      case 1:
        move(2,0.5);
        break;
      case 2:
        move(1,-1);
        break;
      case 3:
        move(-2,-0.5);
        break;
      case 4:
        move(-1.5,-1.5);
        break;
      case 5:
        move(-0.5,-2);
        break;
      case 6:
        move(-1,1);
        break;
      case 7:
        move(0.5,2);
        break;
    }
  }else{
    move(0,0);
  }
  delay(10);
}
void move(float left,float right){
  dcLeft.run(-left*100);
  dcRight.run(-right*100);
}
void stopClaw(){
  dcClaw.run(0);
  
  #ifdef pump_mode
  #else
  dcRClaw.run(0);
  #endif
}
void closeClaw(){
  dcClaw.run(-200);
  #ifdef sv_mode
  sv.write(110);
  #endif
}
void openClaw(){
  dcClaw.run(200);
  #ifdef sv_mode
  sv.write(60);
  #endif
}
void rotateClaw(int dir){
  #ifdef pump_mode
  if(dir>0){
    dcRClaw.run(250*dir);
  }else{
    dcRClaw.run(0);
  }
  #else
  dcRClaw.run(100*dir);
  #endif
}
void setup()
{
  Serial.begin(9600);
  usbhost.init(USB1_0);  //USB1_0 or USB2_0
  #ifdef sv_mode
  sv.attach(sv_pin);
  #else
  
  #endif
}

void loop()
{
  if(!usbhost.device_online)
  {
    usbhost.probeDevice();
    delay(1000);
  }
  else
  {
    int len = usbhost.host_recv();
    parseJoystick(usbhost.RECV_BUFFER);
    delay(5);
  }
}
