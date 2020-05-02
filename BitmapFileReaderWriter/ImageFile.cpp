#include "ImageFile.h"

#include <stdlib.h>

#include "Colour.h"
#include "ImageCanvas.h"

namespace Bitmap
{
    int const ImageFile::c_fileTypeSize = 14;
    int const ImageFile::c_imageInfoSize = 40;
    char const ImageFile::c_bitmapFormatSpecifier[] = { 'B', 'M' };
    unsigned short ImageFile::c_numberOfPlanes = 1;
    unsigned short const ImageFile::c_bitsPerPixel = 24;
    unsigned int const ImageFile::c_compressionLevel = 0;

    ImageFile::ImageFile()
    {
    }

    void ImageFile::write(char const* const filename, ImageCanvas const& canvas)
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
        errno_t errNum = fopen_s(&file, filename, "wb");

        if (file && errNum == 0)
        {
            int const canvasWidth = canvas.getWidth();
            int const canvasHeight = canvas.getHeight();

            //////////////////////////////////////////////////////////////////////////
            // File header
            //////////////////////////////////////////////////////////////////////////

            writeFileHeader(*file, canvasWidth, canvasHeight);

            //////////////////////////////////////////////////////////////////////////
            // Info Header - typeof(BITMAPINFOHEADER)
            //////////////////////////////////////////////////////////////////////////

            writeInfoHeader(*file, canvasWidth, canvasHeight);

            //////////////////////////////////////////////////////////////////////////
            // Colour table
            //////////////////////////////////////////////////////////////////////////

            // skip the colour table - we only do 256-bit colours which don't need the colour table
            // you only need the colour table for pictures with bitsPerPixel <= 8;

            //////////////////////////////////////////////////////////////////////////
            // Pixel data
            //////////////////////////////////////////////////////////////////////////

            writeCanvasColourData(*file, canvas);

            fclose(file);
        }
    }

    FileHandlingErrors ImageFile::load(char const* const filename, ImageCanvas& canvas)
    {
        // written using:
        // https://itnext.io/bits-to-bitmaps-a-simple-walkthrough-of-bmp-image-format-765dc6857393
        // accessed 30/April/2020 at 23:41
        // AND
        // http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
        // accessed 30/April/2020 at 23:41

        FileHandlingErrors toReturn = FileHandlingErrors::OK;

        FILE* file = nullptr;

        errno_t errNum = fopen_s(&file, filename, "rb");

        if (file && errNum == 0)
        {
            //////////////////////////////////////////////////////////////////////////
            // File header
            //////////////////////////////////////////////////////////////////////////
            
            FileTypeHeader typeHeader;
            toReturn = loadFileHeader(*file, typeHeader);

            //////////////////////////////////////////////////////////////////////////
            // Info Header - typeof(BITMAPINFOHEADER)
            //////////////////////////////////////////////////////////////////////////

            FileInfoHeader infoHeader;
            if (toReturn == FileHandlingErrors::OK) { toReturn = loadInfoHeader(*file, infoHeader); }

            //////////////////////////////////////////////////////////////////////////
            // Colour table
            //////////////////////////////////////////////////////////////////////////

            // skip the colour table - we only do 256-bit colours which don't need the colour table
            // you only need the colour table for pictures with bitsPerPixel <= 8;

            //////////////////////////////////////////////////////////////////////////
            // Pixel data
            //////////////////////////////////////////////////////////////////////////

            if (toReturn == FileHandlingErrors::OK)
            {
                fseek(file, typeHeader.offsetToBitmapData, SEEK_SET); // move to the pixel data
                canvas.resize(infoHeader.imageWidth, infoHeader.imageHeight); // prepare the canvas
                toReturn = loadCanvasColourData(*file, canvas);
            }

            fclose(file);
        }

        return toReturn;
    }

    FileHandlingErrors ImageFile::loadFileHeader(FILE& file, FileTypeHeader& typeHeader)
    {
        FileHandlingErrors toReturn = FileHandlingErrors::OK;

        unsigned char bitmapSpecifier[] = { 0u, 0u };

        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned char>(file, bitmapSpecifier[0]); }
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned char>(file, bitmapSpecifier[1]); }

        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned int>(file, typeHeader.fileSize); }

        unsigned int reservedBytes = 0;
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned int>(file, reservedBytes); }

        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned int>(file, typeHeader.offsetToBitmapData); }

        // verify the file format is a supported type
        for (int i = 0; i < 2; ++i)
        {
            if (bitmapSpecifier[i] != c_bitmapFormatSpecifier[i])
            {
                toReturn = FileHandlingErrors::FileTypeUnknown;
            }
        }

        return toReturn;
    }

    FileHandlingErrors ImageFile::loadInfoHeader(FILE& file, FileInfoHeader& infoHeader)
    {
        FileHandlingErrors toReturn = FileHandlingErrors::OK;

        unsigned int infoHeaderSize = 0;
        toReturn = readValue<unsigned int>(file, infoHeaderSize);

        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned int>(file, infoHeader.imageWidth); }
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned int>(file, infoHeader.imageHeight); }

        unsigned short numPlanes = 0;
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned short>(file, numPlanes); }

        unsigned short bitsPerPixel = 0;
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned short>(file, bitsPerPixel); }

        unsigned int compression = 0;
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned int>(file, compression); }
        
        unsigned int compressedImageSize = 0;
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned int>(file, compressedImageSize); }

        int xPixelsPerM = 0;
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<int>(file, xPixelsPerM); }

        int yPixelsPerM = 0;
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<int>(file, yPixelsPerM); }

        unsigned int coloursUsed = 0;
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned int>(file, coloursUsed); }

        unsigned int importantColours = 0;
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<unsigned int>(file, importantColours); }

        if (infoHeaderSize != c_imageInfoSize) { toReturn = FileHandlingErrors::FileCorrupt; }
        if (numPlanes != c_numberOfPlanes) { toReturn = FileHandlingErrors::FileCorrupt; }
        if (bitsPerPixel != c_bitsPerPixel) { toReturn = FileHandlingErrors::Not24BitColourBitmap; }
        if (compression != c_compressionLevel) { toReturn = FileHandlingErrors::CompressionNotSupported; }
        if (coloursUsed != 0 && coloursUsed != 2 << 24 ) { toReturn = FileHandlingErrors::PalettisedBitmapNotSupported; }
        if (importantColours != 0 ) { toReturn = FileHandlingErrors::PalettisedBitmapNotSupported; }

        return toReturn;
    }

    FileHandlingErrors ImageFile::loadCanvasColourData(FILE& file, ImageCanvas& canvas)
    {
        FileHandlingErrors toReturn = FileHandlingErrors::OK;

        int const width = canvas.getWidth();
        int const height = canvas.getHeight();

        // zero byte padding up to nearest 4 byte boundary
        int const paddingBytes = calculateNumberOfScanlinePaddingBytes(width);

        // top to bottom
        for (int j = 0; j < height; ++j)
        {
            // left to right
            for (int i = 0; i < width; ++i)
            {
                Colour loadedColour(0u, 0u, 0u);

                loadColour(file, loadedColour);

                canvas.setPixel(i, j, loadedColour);
            }

            // read past padding
            for (int i = 0; i < paddingBytes; ++i)
            {
                unsigned char padByte = 0;
                toReturn = readValue<unsigned char>(file, padByte);
            }
        }

        return toReturn;
    }

    FileHandlingErrors ImageFile::loadColour(FILE& file, Colour& colour)
    {
        // colours are written as BGR rather than RGB

        FileHandlingErrors toReturn = readValue<ColourChannel>(file, colour.blue);
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<ColourChannel>(file, colour.green); }
        if (toReturn == FileHandlingErrors::OK) { toReturn = readValue<ColourChannel>(file, colour.red); }

        return toReturn;
    }

    void ImageFile::writeCanvasColourData(FILE& file, ImageCanvas const& canvas)
    {
        int const width = canvas.getWidth();
        int const height = canvas.getHeight();

        // zero byte padding up to nearest 4 byte boundary
        int const paddingBytes = calculateNumberOfScanlinePaddingBytes(width);

        // top to bottom
        for (int j = 0; j < height; ++j)
        {
            // left to right
            for (int i = 0; i < width; ++i)
            {
                Colour const colourToWrite = canvas.getPixel(i, j);

                writeColour(file, colourToWrite);
            }

            // write padding bytes
            for (int i = 0; i < paddingBytes; ++i)
            {
                unsigned char zeroByte = 0;
                fwrite(&zeroByte, sizeof(unsigned char), 1, &file);
            }
        }
    }

    void ImageFile::writeColour(FILE& file, Colour const& colour)
    {
        // colours are written as BGR rather than RGB

        fwrite(&colour.blue, sizeof(ColourChannel), 1, &file);
        fwrite(&colour.green, sizeof(ColourChannel), 1, &file);
        fwrite(&colour.red, sizeof(ColourChannel), 1, &file);
    }

    void ImageFile::writeInfoHeader(FILE& file, int totalImageWidth, int totalImageHeight)
    {
        // 4 bytes size of info header
        unsigned int infoHeaderSize = 40;
        fwrite(&infoHeaderSize, sizeof(unsigned int), 1, &file);

        // 4 bytes for pixel width (horizontal) of the image
        unsigned int imagePixelWidth = totalImageWidth;
        fwrite(&imagePixelWidth, sizeof(unsigned int), 1, &file);

        // 4 bytes for pixel height (vertical) of the image
        unsigned int imagePixelHeight = totalImageHeight;
        fwrite(&imagePixelHeight, sizeof(unsigned int), 1, &file);

        // 2 bytes for the number of planes?
        unsigned short numPlanes = 1;
        fwrite(&numPlanes, sizeof(unsigned short), 1, &file);

        // 2 bytess for the byte size of each pixel
        unsigned short bitsPerPixel = 24; // 24-bit colour palette so we can skip the colour palette bit
        fwrite(&bitsPerPixel, sizeof(unsigned short), 1, &file);

        // 4 bytes for the level of compression
        unsigned int compression = 0; // 0 - BI_RGB (no compression) (other compression modes are simple Run Length compression)
        fwrite(&compression, sizeof(unsigned int), 1, &file);

        // 4 bytes to specify the size of the compressed image
        unsigned int compressedImageSize = 0; // 0 as we're not using image compression
        fwrite(&compressedImageSize, sizeof(unsigned int), 1, &file);

        // 4 bytes - representing the horizontal resolution of the target device. This parameter will be adjusted by the image 
        // processing application but should be set to '0' in decimal to indicate no preference
        int xPixelsPerM = 0;
        fwrite(&xPixelsPerM, sizeof(int), 1, &file);

        // 4 bytes - representing the verical resolution of the target device (same as the above for horizontal)
        int yPixelsPerM = 0;
        fwrite(&yPixelsPerM, sizeof(int), 1, &file);

        // 4 bytes - number of colours used. Set to zero so it uses 2^bitsPerPixel
        unsigned int coloursUsed = 0;
        fwrite(&coloursUsed, sizeof(unsigned int), 1, &file);

        // 4 bytes for number of colours. We can ignore this and set it to zero
        unsigned int importantColours = 0;
        fwrite(&importantColours, sizeof(unsigned int), 1, &file);
    }

    void ImageFile::writeFileHeader(FILE& file, int totalImageWidth, int totalImageHeight)
    {
        // 2 bytes
        fwrite(c_bitmapFormatSpecifier, sizeof(char), 2, &file);

        // 4 bytes
        unsigned int fileSize = calculateTotalFileSize(totalImageWidth, totalImageHeight);
        fwrite(&fileSize, sizeof(unsigned int), 1, &file);

        // 4 bytes - reserved to be utilised by an image processing application. Initialise to zero,
        unsigned int unused = 0;
        fwrite(&unused, sizeof(unsigned int), 1, &file);

        // 4 bytes to point to the start of the bit map data
        unsigned int offsetToBitmapData = calculateOffsetIntoFileForStartOfPixelData();
        fwrite(&offsetToBitmapData, sizeof(unsigned int), 1, &file);
    }

    int ImageFile::calculateNumberOfScanlinePaddingBytes(int totalImageWidth) const
    {
        int const paddingRemainder = (totalImageWidth * sizeof(unsigned char) * 3) % 4;
        int const paddingBytes = paddingRemainder > 0 ? 4 - paddingRemainder : 0;

        return paddingBytes;
    }

    int ImageFile::calculateTotalFileSize(int totalImageWidth, int totalImageHeight) const
    {
        // 3 colour channels at 1 byte each and round up to nearest 4 byte boundary
        int const pixelRowLength = totalImageWidth * sizeof(unsigned int) * 3;

        int const paddingBytes = calculateNumberOfScanlinePaddingBytes(totalImageWidth);
        int const totalRowLength = pixelRowLength + paddingBytes;

        int totalImageBytes = totalRowLength * totalImageHeight;

        return c_fileTypeSize + c_imageInfoSize + totalImageBytes;
    }

    int ImageFile::calculateOffsetIntoFileForStartOfPixelData()
    {
        return c_fileTypeSize + c_imageInfoSize;
    }

}
