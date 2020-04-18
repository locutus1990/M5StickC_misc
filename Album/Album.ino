/*
 * @file		Album.ino
 * @brief		M5StickCに複数の画像を表示する(Button-A)
 */
#include <M5StickC.h>
#include "images.h"

namespace{
	TFT_eSprite gTftSprite = TFT_eSprite(&M5.Lcd);
}
 
/*------------------------------------------------------------
 * @fn			Slide image
 * @brief		画像のトランジション表示
 *------------------------------------------------------------*/
void slideImage()
{
	static int idx=0;
	int nextIdx = (idx+1)%imgCount;

#if 1
	//Slide
	for(int x=0;x<8;x++)
	{
		double th = M_PI*(x/7.0)/2.0;
		int xx = (int)(160*sin(th));
		gTftSprite.pushImage(0,0,img_width,img_height,images[nextIdx]);
		gTftSprite.pushImage(xx,0,img_width,img_height,images[idx]);
		gTftSprite.pushSprite(0,0);
		delay(30);
	}
#else
	//Simple refresh
	gTftSprite.pushImage(0,0,img_width,img_height,images[nextIdx]);
	gTftSprite.pushSprite(0,0);
#endif

	idx = (idx+1)%imgCount;
}

/*------------------------------------------------------------
 * @fn		setup
 * @brief	セットアップ。スプラッシュ画面（緑色）と最初の画像を表示する
 *------------------------------------------------------------*/
void setup(void)
{
	M5.begin();

	M5.Lcd.setRotation(1);
	M5.Lcd.setSwapBytes(false);

	gTftSprite.createSprite(img_width, img_height);
	gTftSprite.setSwapBytes(false);

	gTftSprite.setTextSize(2);
	gTftSprite.setTextColor(TFT_BLACK);
	gTftSprite.fillSprite( M5.Lcd.color565(32,128,32));
	gTftSprite.setCursor(2, 40);
	gTftSprite.print("Album start");  
	gTftSprite.pushSprite(0,0);
	delay(1000);

	gTftSprite.pushImage(0,0,img_width,img_height,images[0]);
	gTftSprite.pushSprite(0,0);
}

/*------------------------------------------------------------
 * @fn		loop
 * @brief	ループ処理。Aボタンが押されたら画像を切り替える
 *------------------------------------------------------------*/
void loop()
{
	M5.update();
	if(M5.BtnA.isPressed())
	{
		slideImage();
	}
	delay(100); 
}

