#include <stdio.h>
#include <stdlib.h>


class Bitmap
{
public:
    int calculateNumberOfScanlinePaddingBytes(int totalImageWidth) const
    {
        int const paddingRemainder = (totalImageWidth * sizeof(unsigned char) * 3) % 4;
        int const paddingBytes = paddingRemainder > 0 ? 4 - paddingRemainder : 0;

        return paddingBytes;
    }

    int calculateTotalFileSize(int totalImageWidth, int totalImageHeight) const
    {
        int const fileTypeSize = 14;

        int const imageInfoSize = 40;

        // 3 colour channels at 1 byte each and round up to nearest 4 byte boundary
        int const pixelRowLength = totalImageWidth * sizeof(unsigned int) * 3;

        int const paddingBytes = calculateNumberOfScanlinePaddingBytes(totalImageWidth);
        int const totalRowLength = pixelRowLength + paddingBytes;

        int totalImageBytes = totalRowLength * totalImageHeight;

        return fileTypeSize + imageInfoSize + totalImageBytes;
    }

    int calculateOffsetIntoFileForStartOfPixelData()
    {
        int const fileTypeSize = 14;

        int const imageInfoSize = 40;

        return fileTypeSize + imageInfoSize;
    }

    void writeTestBitmap(int totalImageWidth, int totalImageHeight)
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
        errno_t errNum = fopen_s( &file, "test.bmp", "wb"); 

        if (file && errNum == 0)
        {
            //////////////////////////////////////////////////////////////////////////
            // File header
            //////////////////////////////////////////////////////////////////////////

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

            //////////////////////////////////////////////////////////////////////////
            // Info Header - typeof(BITMAPINFOHEADER)
            //////////////////////////////////////////////////////////////////////////

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

            //////////////////////////////////////////////////////////////////////////
            // Colour table
            //////////////////////////////////////////////////////////////////////////

            // skip the colour table - we only do 256-bit colours which don't need the colour table
            // you only need the colour table for pictures with bitsPerPixel <= 8;

            //////////////////////////////////////////////////////////////////////////
            // Pixel data
            //////////////////////////////////////////////////////////////////////////

            // each scan line must be padded up to the nearest 4-byte boundary
            // colours are written as BGR rather than RGB

            // bottom to top
            for (int j = 0; j < imagePixelHeight; ++j)
            {
                // left to right
                for (int i = 0; i < imagePixelWidth; ++i)
                {
                    //if (i < 5)
                    if (j < 5)
                    {
                        unsigned char blue = 0u;
                        fwrite(&blue, sizeof(unsigned char), 1, file);

                        unsigned char green = 255u;
                        fwrite(&green, sizeof(unsigned char), 1, file);

                        unsigned char red = 0u;
                        fwrite(&red, sizeof(unsigned char), 1, file);
                    }
                    //else if (i >= imagePixelWidth - 5)
                    else if (j >= imagePixelHeight - 5)
                    {
                        unsigned char blue = 0u;
                        fwrite(&blue, sizeof(unsigned char), 1, file);

                        unsigned char green = 0u;
                        fwrite(&green, sizeof(unsigned char), 1, file);

                        unsigned char red = 255u;
                        fwrite(&red, sizeof(unsigned char), 1, file);
                    }
                    else
                    {
                        //float ratio = static_cast<float>(i) / static_cast<float>(imagePixelWidth);
                        float ratio = static_cast<float>(j) / static_cast<float>(imagePixelHeight);

                        unsigned char blue = ratio * 255u > 255u ? 255u : ratio * 255u;
                        fwrite(&blue, sizeof(unsigned char), 1, file);
                        
                        unsigned char green = ratio * 255u > 255u ? 255u : ratio * 255u;
                        fwrite(&green, sizeof(unsigned char), 1, file);
                        
                        unsigned char red = ratio * 255u > 255u ? 255u : ratio * 255u;
                        fwrite(&red, sizeof(unsigned char), 1, file);

                        //unsigned char blue = 0u;
                        //fwrite(&blue, sizeof(unsigned char), 1, file);
                        //
                        //unsigned char green = 0u;
                        //fwrite(&green, sizeof(unsigned char), 1, file);
                        //
                        //unsigned char red = 0u;
                        //fwrite(&red, sizeof(unsigned char), 1, file);
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

            fclose(file);
        }
    }
};

int main()
{
    printf("Hello world!\n");

    Bitmap myBitmap;
    myBitmap.writeTestBitmap(200, 20);

    system("mspaint.exe test.bmp");

    return 0;
}