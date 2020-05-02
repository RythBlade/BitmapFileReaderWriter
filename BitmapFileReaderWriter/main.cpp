#include <stdio.h>
#include <stdlib.h>

#include "ImageFile.h"

int main()
{
    printf("Hello world!\n");

    Bitmap::ImageFile myBitmap;
    myBitmap.writeTestBitmap("test\\test.bmp", 200, 20);
    myBitmap.writeTestBitmap("test\\test1.bmp", 201, 21);
    myBitmap.writeTestBitmap("test\\test2.bmp", 800, 800);
    myBitmap.writeTestBitmap("test\\test3.bmp", 803, 803);

    system("mspaint.exe test\\test3.bmp");

    return 0;
}