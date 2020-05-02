#include <stdio.h>
#include <stdlib.h>

#include "ImageFile.h"
#include "ImageCanvas.h"

int main()
{
    printf("Hello world!\n");

    Bitmap::ImageFile myBitmapFile;

    Bitmap::ImageCanvas myCanvas(200, 20);
    myCanvas.setCanvasToTestImage();
    myBitmapFile.write("test\\test.bmp", myCanvas);

    myCanvas.resize(201, 21);
    myCanvas.setCanvasToTestImage();
    myBitmapFile.write("test\\test1.bmp", myCanvas);

    myCanvas.resize(800, 800);
    myCanvas.setCanvasToTestImage();
    myBitmapFile.write("test\\test2.bmp", myCanvas);

    myCanvas.resize(803, 803);
    myCanvas.setCanvasToTestImage();
    myBitmapFile.write("test\\test3.bmp", myCanvas);

    system("mspaint.exe test\\test3.bmp");

    return 0;
}