/* Use mouse to control meArm.
 * The code is modified from Sparkfun USB shield v2 and meArm example
 * Here is the source:
 * USB shield v2:https://www.circuitsathome.com/products-page/arduino-shields/usb-host-shield-2-0-for-arduino
 * meArm :http://mearm.weebly.com/
*/
// modified by YuanChieh August 2015

#include <hidboot.h>
#include <usbhub.h>

#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif
//MeArm
#include "meArm.h"
#include <Servo.h>

//arm pin
int basePin = 9;
int shoulderPin = 6;
int elbowPin = 3;
int gripperPin = 5;
meArm arm;
//arm position
float dx=0,dy=0,dz=0,dg=0;

//mouse activity
class MouseRptParser : public MouseReportParser
{
protected:
	void OnMouseMove	(MOUSEINFO *mi);
	void OnLeftButtonUp	(MOUSEINFO *mi);
	void OnLeftButtonDown	(MOUSEINFO *mi);
	void OnRightButtonUp	(MOUSEINFO *mi);
	void OnRightButtonDown	(MOUSEINFO *mi);
	void OnMiddleButtonUp	(MOUSEINFO *mi);
	void OnMiddleButtonDown	(MOUSEINFO *mi);
};
void MouseRptParser::OnMouseMove(MOUSEINFO *mi)
{
    dx = map(mi->dX, -80, 80, -5.0, 5.0);
    dy = map(mi->dY, -55, 55, 5.0, -5.0);
    if (abs(dx) < 1) dx = 0;
    if (abs(dy) < 1) dy = 0;
    Serial.println(dx);
    delay(50);
    
    arm.goDirectlyTo(arm.getX() + dx, arm.getY() + dy, arm.getZ());
};
void MouseRptParser::OnLeftButtonUp	(MOUSEINFO *mi)
{
    dz = arm.getZ() - 10;
    if(dz<60)
      dz=50;
    delay(50);
    arm.goDirectlyTo(arm.getX(), arm.getY(), dz);
    Serial.println(arm.getZ());
};
void MouseRptParser::OnLeftButtonDown	(MOUSEINFO *mi)
{
    Serial.println("L Butt Dn");
};

void MouseRptParser::OnRightButtonUp	(MOUSEINFO *mi)
{
    dz = arm.getZ() + 10;
    if(dz>160)
      dz=160;
    delay(50);
    arm.goDirectlyTo(arm.getX(), arm.getY(), dz);
    Serial.println(arm.getZ());
};
void MouseRptParser::OnRightButtonDown	(MOUSEINFO *mi)
{
    Serial.println("R Butt Dn");
};
void MouseRptParser::OnMiddleButtonUp	(MOUSEINFO *mi)
{
    arm.openGripper();
};
void MouseRptParser::OnMiddleButtonDown	(MOUSEINFO *mi)
{
    arm.closeGripper();
};

USB     Usb;
USBHub     Hub(&Usb);
HIDBoot<HID_PROTOCOL_MOUSE>    HidMouse(&Usb);

uint32_t next_time;

MouseRptParser  Prs;

void setup()
{
    Serial.begin( 9600 );
#if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
    Serial.println("Start");

    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

    delay( 200 );

    next_time = millis() + 5000;

    HidMouse.SetReportParser(0,(HIDReportParser*)&Prs);
    arm.begin(basePin, shoulderPin, elbowPin, gripperPin);
    arm.goDirectlyTo(0, 150, 136);
    delay(1000);
}

void loop()
{
    Usb.Task();
}

