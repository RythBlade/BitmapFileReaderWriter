#include <stdio.h>
#include <stdlib.h>

#include "ImageFile.h"
#include "ImageCanvas.h"
#include "Colour.h"

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

    myCanvas.resize(800, 800);
    
    for (int i = 0; i < 800; ++i)
    {
        for (int j = 0; j < 800; ++j)
        {
            if (i == j)
            {
                myCanvas.setPixel(i, j, Bitmap::Colour(255u, 0u, 0u));
            }
            else if (i == 800 - j)
            {
                myCanvas.setPixel(i, j, Bitmap::Colour(0u, 255u, 0u));
            }
        }
    }

    myBitmapFile.write("test\\test5.bmp", myCanvas);


    myBitmapFile.load("test\\imageToLoad.bmp", myCanvas);


    for (int i = 0; i < myCanvas.getHeight(); ++i)
    {
        for (int j = 0; j < myCanvas.getWidth(); ++j)
        {
            // y = mx
            int blueLineIndex = i;

            // y = -mx + width - 1;
            int greenLineIndex = -i + myCanvas.getWidth() - 1;

            if (j == blueLineIndex)
            {
                myCanvas.setPixel(i, j, Bitmap::Colour(0u, 0u, 255u));
            }
            else if (j == greenLineIndex)
            {
                myCanvas.setPixel(i, j, Bitmap::Colour(0u, 255u, 0u));
            }
        }
    }

    myBitmapFile.write("test\\alteredSource.bmp", myCanvas);

    myBitmapFile.load("test\\imageToLoad2.bmp", myCanvas);


    for (int y = 0; y < myCanvas.getHeight(); ++y)
    {
        for (int x = 0; x < myCanvas.getWidth(); ++x)
        {
            float gradient = static_cast<float>(myCanvas.getHeight()) / static_cast<float>(myCanvas.getWidth());

            // y = mx
            int blueLineIndex = gradient * x;

            // y = -mx + width - 1;
            int greenLineIndex = -gradient * x + myCanvas.getHeight() - 1;

            if (y == blueLineIndex)
            {
                myCanvas.setPixel(y, x, Bitmap::Colour(0u, 0u, 255u));
            }
            else if (y == greenLineIndex)
            {
                myCanvas.setPixel(y, x, Bitmap::Colour(0u, 255u, 0u));
            }
        }
    }

    myBitmapFile.write("test\\alteredSource2.bmp", myCanvas);

    system("mspaint.exe test\\alteredSource2.bmp");

    return 0;
}