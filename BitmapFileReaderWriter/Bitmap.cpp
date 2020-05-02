#include "Bitmap.h"

#include <stdlib.h>

int const Bitmap::c_fileTypeSize = 14;
int const Bitmap::c_imageInfoSize = 40;

Bitmap::Bitmap()
{
}

void Bitmap::write(char const* const filaname, int totalImageWidth, int totalImageHeight)
{
    // written using:
    // https://itnext.io/bits-to-bitmaps-a-simple-walkthrough-of-bmp-image-format-765dc6857393
    // accessed 30/April/2020 at 23:41
    // AND
    // http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
    // accessed 30/April/2020 at 23:41

    FILE* file = nullptr;

    // MUST OPEN THE FILE IN BINARY MODE!!! IF WE DONE AND WE EVER WRITE A '\n' CHARACTER, IT WILL NORMALISE THE LINE ENDING WITH A CARRAIGE RETURN TOO - WRITING 2 BYTES, RATHER THAN 1
    // As we're writing individual bytes for the colours, if any of then are 10u (the '\n' character), an extra character will be added. Also - the return value doesn't return that 2 characters
    // were written either!!! It returns 1. You can only find this using ftell, to check the file position for instances where it skips forward too many bytes on a write.
    errno_t errNum = fopen_s(&file, filaname, "wb");

    if (file && errNum == 0)
    {
        //////////////////////////////////////////////////////////////////////////
        // File header
        //////////////////////////////////////////////////////////////////////////

        writeFileHeader(file, totalImageWidth, totalImageHeight);

        //////////////////////////////////////////////////////////////////////////
        // Info Header - typeof(BITMAPINFOHEADER)
        //////////////////////////////////////////////////////////////////////////

        writeInfoHeader(file, totalImageWidth, totalImageHeight);

        //////////////////////////////////////////////////////////////////////////
        // Colour table
        //////////////////////////////////////////////////////////////////////////

        // skip the colour table - we only do 256-bit colours which don't need the colour table
        // you only need the colour table for pictures with bitsPerPixel <= 8;

        //////////////////////////////////////////////////////////////////////////
        // Pixel data
        //////////////////////////////////////////////////////////////////////////

        writeTestColourData(file, totalImageHeight, totalImageWidth);

        fclose(file);
    }
}

void Bitmap::writeTestBitmap(char const* const filaname, int totalImageWidth, int totalImageHeight)
{
    // written using:
    // https://itnext.io/bits-to-bitmaps-a-simple-walkthrough-of-bmp-image-format-765dc6857393
    // accessed 30/April/2020 at 23:41
    // AND
    // http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
    // accessed 30/April/2020 at 23:41

    FILE* file = nullptr;

    // MUST OPEN THE FILE IN BINARY MODE!!! IF WE DONE AND WE EVER WRITE A '\n' CHARACTER, IT WILL NORMALISE THE LINE ENDING WITH A CARRAIGE RETURN TOO - WRITING 2 BYTES, RATHER THAN 1
    // As we're writing individual bytes for the colours, if any of then are 10u (the '\n' character), an extra character will be added. Also - the return value doesn't return that 2 characters
    // were written either!!! It returns 1. You can only find this using ftell, to check the file position for instances where it skips forward too many bytes on a write.
    errno_t errNum = fopen_s(&file, filaname, "wb");

    if (file && errNum == 0)
    {
        //////////////////////////////////////////////////////////////////////////
        // File header
        //////////////////////////////////////////////////////////////////////////

        writeFileHeader(file, totalImageWidth, totalImageHeight);

        //////////////////////////////////////////////////////////////////////////
        // Info Header - typeof(BITMAPINFOHEADER)
        //////////////////////////////////////////////////////////////////////////

        writeInfoHeader(file, totalImageWidth, totalImageHeight);

        //////////////////////////////////////////////////////////////////////////
        // Colour table
        //////////////////////////////////////////////////////////////////////////

        // skip the colour table - we only do 256-bit colours which don't need the colour table
        // you only need the colour table for pictures with bitsPerPixel <= 8;

        //////////////////////////////////////////////////////////////////////////
        // Pixel data
        //////////////////////////////////////////////////////////////////////////

        writeTestColourData(file, totalImageHeight, totalImageWidth);

        fclose(file);
    }
}

void Bitmap::writeTestColourData(FILE* file, int totalImageHeight, int totalImageWidth)
{
    // each scan line must be padded up to the nearest 4-byte boundary
    // colours are written as BGR rather than RGB

    // top to bottom
    for (int j = 0; j < totalImageHeight; ++j)
    {
        // left to right
        for (int i = 0; i < totalImageWidth; ++i)
        {
            float const horizontalRatio = static_cast<float>(i) / totalImageWidth;
            float const verticalRatio = static_cast<float>(j) / totalImageHeight;

            unsigned char finalHorizontalValue = horizontalRatio * 127u;
            unsigned char finalVerticalValue = verticalRatio * 127u;
            unsigned char combinedValue = finalHorizontalValue + finalVerticalValue;

            float const borderWidth = 5;

            // bottom left == white
            // left == red
            // top left == blue

            // bottom right == black
            // right == green
            // top right == red + blue

            // bottom == red + green
            // top == green + blue


            if (i < borderWidth)
            {
                if (j < borderWidth)
                {
                    // bottom left
                    unsigned char blue = 255u;
                    fwrite(&blue, sizeof(unsigned char), 1, file);

                    unsigned char green = 255u;
                    fwrite(&green, sizeof(unsigned char), 1, file);

                    unsigned char red = 255u;
                    fwrite(&red, sizeof(unsigned char), 1, file);
                }
                else if (j > (totalImageHeight - borderWidth))
                {
                    // top left
                    unsigned char blue = 255u;
                    fwrite(&blue, sizeof(unsigned char), 1, file);

                    unsigned char green = 0u;
                    fwrite(&green, sizeof(unsigned char), 1, file);

                    unsigned char red = 0u;
                    fwrite(&red, sizeof(unsigned char), 1, file);
                }
                else
                {
                    // left
                    unsigned char blue = 0u;
                    fwrite(&blue, sizeof(unsigned char), 1, file);

                    unsigned char green = 0u;
                    fwrite(&green, sizeof(unsigned char), 1, file);

                    unsigned char red = 255u;
                    fwrite(&red, sizeof(unsigned char), 1, file);
                }
            }
            else if (i > (totalImageWidth - borderWidth))
            {
                if (j < 5)
                {
                    // bottom right
                    unsigned char blue = 0u;
                    fwrite(&blue, sizeof(unsigned char), 1, file);

                    unsigned char green = 0u;
                    fwrite(&green, sizeof(unsigned char), 1, file);

                    unsigned char red = 0u;
                    fwrite(&red, sizeof(unsigned char), 1, file);
                }
                else if (j > (totalImageHeight - borderWidth))
                {
                    // top right
                    unsigned char blue = 255u;
                    fwrite(&blue, sizeof(unsigned char), 1, file);

                    unsigned char green = 0u;
                    fwrite(&green, sizeof(unsigned char), 1, file);

                    unsigned char red = 255u;
                    fwrite(&red, sizeof(unsigned char), 1, file);
                }
                else
                {
                    // right
                    unsigned char blue = 0u;
                    fwrite(&blue, sizeof(unsigned char), 1, file);

                    unsigned char green = 255u;
                    fwrite(&green, sizeof(unsigned char), 1, file);

                    unsigned char red = 0u;
                    fwrite(&red, sizeof(unsigned char), 1, file);
                }
            }
            else if (j < borderWidth)
            {
                // bottom
                unsigned char blue = 0u;
                fwrite(&blue, sizeof(unsigned char), 1, file);

                unsigned char green = 255u;
                fwrite(&green, sizeof(unsigned char), 1, file);

                unsigned char red = 255u;
                fwrite(&red, sizeof(unsigned char), 1, file);
            }
            else if (j > (totalImageHeight - borderWidth))
            {
                // top
                unsigned char blue = 255u;
                fwrite(&blue, sizeof(unsigned char), 1, file);

                unsigned char green = 255u;
                fwrite(&green, sizeof(unsigned char), 1, file);

                unsigned char red = 0u;
                fwrite(&red, sizeof(unsigned char), 1, file);
            }
            else
            {
                unsigned char blue = combinedValue;
                fwrite(&blue, sizeof(unsigned char), 1, file);

                unsigned char green = combinedValue;
                fwrite(&green, sizeof(unsigned char), 1, file);

                unsigned char red = combinedValue;
                fwrite(&red, sizeof(unsigned char), 1, file);
            }
        }

        // zero byte padding up to nearest 4 byte boundary
        int const paddingBytes = calculateNumberOfScanlinePaddingBytes(totalImageWidth);

        for (int i = 0; i < paddingBytes; ++i)
        {
            unsigned char zeroByte = 0;
            fwrite(&zeroByte, sizeof(unsigned char), 1, file);
        }
    }
}

void Bitmap::writeInfoHeader(FILE* file, int totalImageWidth, int totalImageHeight)
{
    // 4 bytes size of info header
    unsigned int infoHeaderSize = 40;
    fwrite(&infoHeaderSize, sizeof(unsigned int), 1, file);

    // 4 bytes for pixel width (horizontal) of the image
    unsigned int imagePixelWidth = totalImageWidth;
    fwrite(&imagePixelWidth, sizeof(unsigned int), 1, file);

    // 4 bytes for pixel height (vertical) of the image
    unsigned int imagePixelHeight = totalImageHeight;
    fwrite(&imagePixelHeight, sizeof(unsigned int), 1, file);

    // 2 bytes for the number of planes?
    unsigned short numPlanes = 1;
    fwrite(&numPlanes, sizeof(unsigned short), 1, file);

    // 2 bytess for the byte size of each pixel
    unsigned short bitsPerPixel = 24; // 24-bit colour palette so we can skip the colour palette bit
    fwrite(&bitsPerPixel, sizeof(unsigned short), 1, file);

    // 4 bytes for the level of compression
    unsigned int compression = 0; // 0 - BI_RGB (no compression) (other compression modes are simple Run Length compression)
    fwrite(&compression, sizeof(unsigned int), 1, file);

    // 4 bytes to specify the size of the compressed image
    unsigned int compressedImageSize = 0; // 0 as we're not using image compression
    fwrite(&compressedImageSize, sizeof(unsigned int), 1, file);

    // 4 bytes - representing the horizontal resolution of the target device. This parameter will be adjusted by the image 
    // processing application but should be set to '0' in decimal to indicate no preference
    int xPixelsPerM = 0;
    fwrite(&xPixelsPerM, sizeof(int), 1, file);

    // 4 bytes - representing the verical resolution of the target device (same as the above for horizontal)
    int yPixelsPerM = 0;
    fwrite(&yPixelsPerM, sizeof(int), 1, file);

    // 4 bytes - number of colours used. Set to zero so it uses 2^bitsPerPixel
    unsigned int coloursUsed = 0;
    fwrite(&coloursUsed, sizeof(unsigned int), 1, file);

    // 4 bytes for number of colours. We can ignore this and set it to zero
    unsigned int importantColours = 0;
    fwrite(&importantColours, sizeof(unsigned int), 1, file);
}

void Bitmap::writeFileHeader(FILE* file, int totalImageWidth, int totalImageHeight)
{
    // 2 bytes
    char const bitmapSpecifier[] = { 'B', 'M' };
    fwrite(bitmapSpecifier, sizeof(char), 2, file);

    // 4 bytes
    unsigned int fileSize = calculateTotalFileSize(totalImageWidth, totalImageHeight);
    fwrite(&fileSize, sizeof(unsigned int), 1, file);

    // 4 bytes - reserved to be utilised by an image processing application. Initialise to zero,
    unsigned int unused = 0;
    fwrite(&unused, sizeof(unsigned int), 1, file);

    // 4 bytes to point to the start of the bit map data
    unsigned int offsetToBitmapData = calculateOffsetIntoFileForStartOfPixelData();
    fwrite(&offsetToBitmapData, sizeof(unsigned int), 1, file);
}

int Bitmap::calculateNumberOfScanlinePaddingBytes(int totalImageWidth) const
{
    int const paddingRemainder = (totalImageWidth * sizeof(unsigned char) * 3) % 4;
    int const paddingBytes = paddingRemainder > 0 ? 4 - paddingRemainder : 0;

    return paddingBytes;
}

int Bitmap::calculateTotalFileSize(int totalImageWidth, int totalImageHeight) const
{
    // 3 colour channels at 1 byte each and round up to nearest 4 byte boundary
    int const pixelRowLength = totalImageWidth * sizeof(unsigned int) * 3;

    int const paddingBytes = calculateNumberOfScanlinePaddingBytes(totalImageWidth);
    int const totalRowLength = pixelRowLength + paddingBytes;

    int totalImageBytes = totalRowLength * totalImageHeight;

    return c_fileTypeSize + c_imageInfoSize + totalImageBytes;
}

int Bitmap::calculateOffsetIntoFileForStartOfPixelData()
{
    return c_fileTypeSize + c_imageInfoSize;
}
