#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT

#include "M5StickC.h"
#include "bugC.h"

#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>

namespace
{
	char auth[] = "YOUR_KEY";
	TFT_eSprite gTftSprite = TFT_eSprite(&M5.Lcd);
	int gX0=0, gY0=0, gX1=0, gY1=0;
	int gSwitch=0;
	bool gConnectedFlg=false;
}

/*------------------------------------------------------------
 * Show BLE Connection
 *------------------------------------------------------------*/
void showConnected()
{
	if(gConnectedFlg){
		gTftSprite.fillSprite(M5.Lcd.color565(0,128,32));
		gTftSprite.printf("Connected\n");
	}
	else
	{
		gTftSprite.fillSprite(M5.Lcd.color565(128,0,0));
		gTftSprite.printf("Disconnected\n");
	}
}

/*------------------------------------------------------------
 * move Bugc
 *------------------------------------------------------------*/
void moveBugC()
{
	int j0 = (int)(100*gX0/512.0);
	int j1 = (int)(100*gY0/512.0);
	int j2 = (int)(100*gX1/512.0);
	int j3 = (int)(100*gY1/512.0);
	int v0=0, v1=0, v2=0, v3=0; //Front_L, Front_R, Rear_L, Rear_R

	//Left
	if(abs(j0)<abs(j1))
	{
		int v = j1;
		v0=-v;
		v2= v;
	}
	else
	{
		int v = j0;
		v0= v;
		v2= v;
	}

	//Right
	if(abs(j2)<abs(j3))//Left-Right
	{
		int v = j3;
		v1=-v;
		v3= v;
	}
	else//Forward-Back
	{
		int v = j2;
		v1=-v;
		v3=-v;
	}
	BugCSetAllSpeed(v0, v1, v2, v3);

	//Show values
	char spd[512];
	gTftSprite.setTextColor(TFT_BLACK);

	sprintf(spd, "% 3d", v0);
	gTftSprite.setCursor(2,80);
	gTftSprite.print(spd);

	sprintf(spd, "% 3d", v1);
	gTftSprite.setCursor(42,80);
	gTftSprite.print(spd);

	sprintf(spd, "% 3d", v2);
	gTftSprite.setCursor(2,100);
	gTftSprite.print(spd);
	
	sprintf(spd, "% 3d", v3);
	gTftSprite.setCursor(42,100);
	gTftSprite.print(spd);
}

/*------------------------------------------------------------
 * Draw A joystick on LCD
 *------------------------------------------------------------*/
void drawJoyStick(int idx)
{
	int cx, x, y;

	if(0==idx)//Left
	{
		cx=60;
		x =-gY1;
		y = gX1;
	}
	else
	{
		cx=20;
		x =-gY0;
		y = gX0;
	}
	int cy   = 30;
	int r0   = 18;//Outer radius
	int rjoy =  9;//Joystick radius
	int px   = cx+(int)(((r0-rjoy)*   x)/512.0);
	int py   = cy+(int)(((r0-rjoy)*(-y))/512.0);

	gTftSprite.drawCircle(cx, cy, r0, TFT_LIGHTGREY);
	gTftSprite.fillCircle(px, py, rjoy, TFT_LIGHTGREY);
	gTftSprite.drawCircle(px, py, rjoy, TFT_BLACK);
}

/*------------------------------------------------------------
 * Draw Joysticks on LCD (with Double buffer)
 *------------------------------------------------------------*/
void refreshAll()
{
	gTftSprite.setCursor(2,2);

	showConnected();
	drawJoyStick(0);
	drawJoyStick(1);
	moveBugC();

	gTftSprite.pushSprite(0,0);
}

/*------------------------------------------------------------
 * Receive Joystick status
 *------------------------------------------------------------*/
BLYNK_WRITE(V0)
{
	gX0 = (int)(param[0].asInt());
	gY0 = (int)(param[1].asInt());
	refreshAll();
}
BLYNK_WRITE(V1)
{
	gX1 = (int)(param[0].asInt());
	gY1 = (int)(param[1].asInt());
	refreshAll();
}

/*------------------------------------------------------------
 * Receive Button status
 *------------------------------------------------------------*/
BLYNK_WRITE(V2)
{
	gSwitch=param.asInt();
	if(0<gSwitch)
	{
		BugCSetColor(0x100000, 0x001000);
	}
	else
	{
		BugCSetColor(0x000000, 0x000000);
	}
}

/*------------------------------------------------------------
 * Setup
 *------------------------------------------------------------*/
void setup() 
{
	bool result[0x80];
	M5.begin();
	Wire.begin(0, 26, 400000);
	//bool isI2c = M5.I2C.scanID(&result[0]);
	//if(isI2c){M5.Lcd.setTextColor(TFT_GREEN);}
	//else{M5.Lcd.setTextColor(TFT_RED);}

	// if add battery, need increase charge current
	M5.Axp.SetChargeCurrent(CURRENT_360MA);

	Serial.begin(9600);
	Serial.println("Waiting for connections...");
	Blynk.setDeviceName("Blynk");

	M5.Lcd.setRotation(0);
	M5.Lcd.setSwapBytes(false);

	gTftSprite.createSprite(80, 160);
	gTftSprite.setSwapBytes(true);
	gTftSprite.setTextSize(1);
	gTftSprite.setCursor(5,30);
	gTftSprite.fillSprite(TFT_GREEN);
	gTftSprite.setTextColor(TFT_BLACK);
	gTftSprite.print("Connect BLE");
	gTftSprite.pushSprite(0,0);

	Blynk.begin(auth);
	delay(2000);
	refreshAll();
}

/*------------------------------------------------------------
 * Loop
 *------------------------------------------------------------*/
void loop() 
{
	Blynk.run();

	//Check BLE connection
	static int count=0;
	if(20<count++)
	{
		gConnectedFlg=Blynk.connected();
		refreshAll();
		count=0;
	}

	delay(30);
}
