# -*- coding: utf-8 -*-
"""
* file     imageConversion.py
* brief    フォルダ内の画像をArduino形式(rgb565)に変換する
"""
import os
import sys
import struct
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

#size of monitor
LCD_W=160
LCD_H=80

#------------------------------
#------------------------------
def rgb2hex(rgb888):
    """
    rgb888をrgb565に変換する
    Attributes:
        rgb888 : rgb888の画素値[r, g, b]（１画素３バイト）
        rgb565 : Arduino用の画素値（１画素２バイト）
    """
    r = rgb888[0]
    g = rgb888[1]
    b = rgb888[2]
    rh = int(round((2**5-1)*(r/255.0)))<<11
    gh = int(round((2**6-1)*(g/255.0)))<<5
    bh = int(round((2**5-1)*(b/255.0)))
    rgb565 = (rh+gh+bh)
    return rgb565

#------------------------------
#------------------------------
def resizeImage(srcImg):
    """
    画像ファイルをスクリーンに収まるように縮小する
    Attributes:
        srcImg : PIL.Image 形式の画像データ（サイズは任意）
        dstImg : np.array 形式の画像データ(縮小後画像。rgb888)
    """
    src_w = srcImg.width
    src_h = srcImg.height
    #print(src_w, src_h)

    if(src_w<src_w):
        image = srcImg.rotate(90)
        src_w = srcImg.width
        src_h = srcImg.height

    kw = LCD_W/src_w
    kh = LCD_H/src_h
    if(kw<kh):
        dst_w = int(kw*src_w)
        dst_h = int(kw*src_h)
    else:
        dst_w = int(kh*src_w)
        dst_h = int(kh*src_h)

    img = srcImg.resize((dst_w, dst_h))

    dx = int( (LCD_W-dst_w)/2 )
    wimg = Image.new('RGB', (LCD_W, LCD_H), (0xff, 0xff, 0xff))
    wimg.paste(img, (dx, 0))
    #print(src_w, src_h, dst_w, dst_h)
    dstImg = np.array(wimg)

    return dstImg


#------------------------------
#------------------------------
def writeBinary(fout, img):
    """
    画像データをCのヘッダファイル形式でファイルに書き込む
    Attributes:
        fout : ファイルハンドラ
        img : numpy.array形式の縮小後画像(rgb888)
    """
    ww = img.shape[1]
    hh = img.shape[0]

    cnt=0
    fout.write( "{\n" )
    for j in range(hh):
        for i in range(ww):
            rgb = img[j][i]
            val = rgb2hex(rgb)
            fout.write( hex(val)+"," );
            cnt = cnt+1
            if(7<cnt):
                fout.write( "\n" )
                cnt=0
    fout.write( "},\n" )
                
#------------------------------
# main routine
#------------------------------
if __name__=="__main__":
    """
    メイン関数
    * フォルダ名を引数として渡すと、フォルダ内の画像を全てをバイナリ化して
        ヘッダファイルに書き込む。
        （M5StickCの場合、フォルダ内の画像数は30枚程度が上限）
    * フォルダ内の画像以外のファイルは無視される
    """
    if 1<len(sys.argv):
        dname = sys.argv[1]

    dnameArr = os.listdir(dname)
    with open("images.h", mode="w") as fout:
        fout.write("const uint16_t images[]["+str(LCD_W*LCD_H)+"]  = {\n")
        imgCnt=0

        for fname in dnameArr:
            fname = dname + "/" + fname
            print(fname)
            try:
                image = Image.open(fname)
                img = resizeImage(image)
                writeBinary(fout, img);
                imgCnt=imgCnt+1
            except IOError:
                print("Read failed : "+str(fname));

        fout.write("};\n")
        fout.write("int img_width  = "+ str(LCD_W)+";\n")
        fout.write("int img_height = "+ str(LCD_H)+";\n")
        fout.write("int imgCount  = "+ str(imgCnt)+";\n")

