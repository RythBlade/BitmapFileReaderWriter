#include <stdio.h>
#include <stdlib.h>

#include "ImageFile.h"
#include "ImageCanvas.h"
#include "Colour.h"

void printLoadSuccess(char const* const filename, Bitmap::FileHandlingErrors errorCode)
{
    if (errorCode == Bitmap::FileHandlingErrors::OK)
    {
        printf("Successfully wrote \"%s\"\n", filename);
    }
    else
    { 
        printf("Failed to write a file with error code, %d\n", errorCode);
    }
}

void printWriteSuccess(char const* const filename, Bitmap::FileHandlingErrors errorCode)
{
    if (errorCode == Bitmap::FileHandlingErrors::OK)
    {
        printf("Successfully wrote \"%s\"\n", filename);
    }
    else
    {
        printf("Failed to write a file \"%s\" with error code, %d\n", filename, errorCode);
    }
}

int main()
{
    printf("Starting bitmap test sequence!\n");

    //////////////////////////////////////////////////////////////////////////

    Bitmap::ImageFile myBitmapFile;

    Bitmap::ImageCanvas myCanvas(200, 20);
    myCanvas.setCanvasToTestImage();
    char const* fileName = "test\\test.bmp";
    Bitmap::FileHandlingErrors returnedError = myBitmapFile.write(fileName, myCanvas);
    printWriteSuccess(fileName, returnedError);

    //////////////////////////////////////////////////////////////////////////

    myCanvas.resize(201, 21);
    myCanvas.setCanvasToTestImage();
    fileName = "test\\test1.bmp";
    returnedError = myBitmapFile.write(fileName, myCanvas);
    printWriteSuccess(fileName, returnedError);

    //////////////////////////////////////////////////////////////////////////

    myCanvas.resize(800, 800);
    myCanvas.setCanvasToTestImage();
    fileName = "test\\test2.bmp";
    returnedError = myBitmapFile.write(fileName, myCanvas);
    printWriteSuccess(fileName, returnedError);

    //////////////////////////////////////////////////////////////////////////

    myCanvas.resize(803, 803);
    myCanvas.setCanvasToTestImage();
    fileName = "test\\test3.bmp";
    returnedError = myBitmapFile.write(fileName, myCanvas);
    printWriteSuccess(fileName, returnedError);

    //////////////////////////////////////////////////////////////////////////

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

    fileName = "test\\test5.bmp";
    returnedError = myBitmapFile.write(fileName, myCanvas);
    printWriteSuccess(fileName, returnedError);

    //////////////////////////////////////////////////////////////////////////

    fileName = "test\\imageToLoad.bmp";
    returnedError = myBitmapFile.load(fileName, myCanvas);
    printLoadSuccess(fileName, returnedError);

    if (returnedError == Bitmap::FileHandlingErrors::OK)
    {
        for (unsigned int i = 0; i < myCanvas.getHeight(); ++i)
        {
            for (unsigned int j = 0; j < myCanvas.getWidth(); ++j)
            {
                // y = mx
                int blueLineIndex = i;

                // y = -mx + width - 1;
                int greenLineIndex = -static_cast<int>(i) + static_cast<int>(myCanvas.getWidth()) - 1;

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
    }

    fileName = "test\\alteredSource.bmp";
    returnedError = myBitmapFile.write(fileName, myCanvas);
    printWriteSuccess(fileName, returnedError);

    //////////////////////////////////////////////////////////////////////////

    fileName = "test\\imageToLoad2.bmp";
    returnedError = myBitmapFile.load(fileName, myCanvas);
    printLoadSuccess(fileName, returnedError);

    if (returnedError == Bitmap::FileHandlingErrors::OK)
    {
        for (unsigned int y = 0; y < myCanvas.getHeight(); ++y)
        {
            for (unsigned int x = 0; x < myCanvas.getWidth(); ++x)
            {
                float gradient = static_cast<float>(myCanvas.getHeight()) / static_cast<float>(myCanvas.getWidth());

                // y = mx
                int blueLineIndex = static_cast<int>(gradient * x);

                // y = -mx + width - 1;
                int greenLineIndex = static_cast<int>(-gradient * x + myCanvas.getHeight() - 1);

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
    }

    fileName = "test\\alteredSource2.bmp";
    returnedError = myBitmapFile.write(fileName, myCanvas);
    printWriteSuccess(fileName, returnedError);

    //////////////////////////////////////////////////////////////////////////

    system("mspaint.exe test\\alteredSource2.bmp");

    return 0;
}