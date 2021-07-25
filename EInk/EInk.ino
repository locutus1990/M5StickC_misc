/*
  Eink_factory_code_213.ino
  Driver for MCP9600
  Copyright (c) 2018 Seeed Technology Co., Ltd.
  Website  : www.seeed.cc
  Author   : Jenkin
  Create Time: October 2018
  Change Log :
  The MIT License (MIT)
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#include <M5Atom.h>
#include "images/kururun.h"
HardwareSerial SerialX(1);

//------------------------------
//Send data to e-link board.
//------------------------------
void serial_send_data(const uint8_t* data, uint32_t data_len) {
  for (int i = 0; i < data_len; i++) {
    SerialX.write(pgm_read_byte(&data[i]));
  }
}

//------------------------------
//Send image array
//------------------------------
void write_image_picture(void) {
  for (int i = 0; i < 38; i++) {
    serial_send_data(&IMAGE_BLACK[i * 76], 76);
    delay(70);
  }
  delay(70);
  for (int i = 0; i < 38; i++) {
    serial_send_data(&IMAGE_RED[i * 76], 76);
    delay(70);
  }
}

//------------------------------
//Show LED
//------------------------------
void refreshLed(char *str)
{
  static uint8_t FSM=0;
  int colarr[]={
    0xff0000,//Green
    0x00ff00,//Red
    0x0000ff,//Blue
    0x888800,//Yellow
    0x880088,//Cyan
    0x008888,//Magenta
    0x444444,//White
    0x000000,//Off
  };
  int arrn = int(sizeof(colarr)/sizeof(int));


  Serial.println(str);
  for(int i=0;i<5;i++)
  {
    M5.dis.drawpix(0, colarr[FSM]);
    delay(50);
    M5.dis.drawpix(0, 0x000000);
    delay(50);
  }
  FSM=(FSM+1)%arrn;

}

//------------------------------
//Send the start transfer command
//------------------------------
void send_begin() {
  char str = 'a';
  int alt=SerialX.write(str);
  char mymsg[512];
  sprintf(mymsg, "Enter send_begin:%d, %c", alt, str);
  refreshLed(mymsg);
  while (1) {
    if (0<SerialX.available()) {
      delay(10);
      char str1 = SerialX.read();
      Serial.println(str1);
      if (str1 == 'b') {
        break;
      }
    }
  }
}


//------------------------------
//------------------------------
void setup() {
  bool SerialEnable=true;
  bool I2CEnable=false;
  bool DisplayEnable=true;
  M5.begin( SerialEnable, I2CEnable, DisplayEnable);
  M5.dis.drawpix(0, 0x000000);

  delay(1000);
  Serial.println("");
  Serial.println("Hello Serial");

  Serial.begin(115200);
  SerialX.begin(230400, SERIAL_8N1, 32, 26);// Grove(boudrate, config, RX, TX)

  delay(10);
  refreshLed("send_begin");
  send_begin();

  delay(2000);
  refreshLed("write_begin");
  write_image_picture();
}

//------------------------------
//------------------------------
void loop() {
  /*
  if(M5.Btn.wasPressed())
  {
    refreshLed("send_begin");
    send_begin();

    delay(2000);
    refreshLed("write_begin");
    write_image_picture();
  }
  */
  delay(10);
  M5.update();
}
