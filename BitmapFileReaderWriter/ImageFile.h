#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>

// forward declarations
namespace Bitmap
{
    struct Colour;
    class ImageCanvas;
}

namespace Bitmap
{
    enum class FileHandlingErrors
    {
        OK = 0
        , FileCorrupt
        , UnexpectedEndOfFile
        , FileTypeUnknown
        , Not24BitColourBitmap
        , CompressionNotSupported
        , PalettisedBitmapNotSupported
        , UnknownReadError
        , UnknownWriteError
    };

    struct FileTypeHeader
    {
        FileTypeHeader()
            : fileSize(0)
            , offsetToBitmapData(0)
        {
        }

        unsigned int fileSize;
        unsigned int offsetToBitmapData;
    };

    struct FileInfoHeader
    {
        FileInfoHeader()
            : imageWidth(0)
            , imageHeight(0)
        {
        }

        unsigned int imageWidth;
        unsigned int imageHeight;
    };

    class ImageFile
    {
    public:
        ImageFile();

        void write(char const* const filename, ImageCanvas const& canvas);
        FileHandlingErrors load(char const* const filename, ImageCanvas& canvas);

    private:
        FileHandlingErrors writeFileHeader(FILE& file, int totalImageWidth, int totalImageHeight);
        FileHandlingErrors writeInfoHeader(FILE& file, int totalImageWidth, int totalImageHeight);
        FileHandlingErrors writeCanvasColourData(FILE& file, ImageCanvas const& canvas);
        FileHandlingErrors writeColour(FILE& file, Colour const& colour);

        FileHandlingErrors loadFileHeader(FILE& file, FileTypeHeader& typeHeader);
        FileHandlingErrors loadInfoHeader(FILE& file, FileInfoHeader& infoHeader);
        FileHandlingErrors loadCanvasColourData(FILE& file, ImageCanvas& canvas);
        FileHandlingErrors loadColour(FILE& file, Colour& colour);

        int calculateNumberOfScanlinePaddingBytes(int totalImageWidth) const;

        int calculateTotalFileSize(int totalImageWidth, int totalImageHeight) const;

        int calculateOffsetIntoFileForStartOfPixelData();

        template<typename TYPE>
        FileHandlingErrors readValue(FILE& file, TYPE& value)
        {
            size_t elementsRead = fread(&value, sizeof(TYPE), 1, &file);

            int error = ferror(&file);

            FileHandlingErrors toReturn = FileHandlingErrors::OK;

            if (error == 0)
            {
                toReturn = (elementsRead == 1) ? FileHandlingErrors::OK : FileHandlingErrors::UnexpectedEndOfFile;
            }
            else
            {
                toReturn = FileHandlingErrors::UnknownReadError;
            }

            return toReturn;
        }

        template<typename TYPE>
        FileHandlingErrors writeValue(FILE& file, TYPE const& value)
        {
            size_t elementswritted = fwrite(&value, sizeof(TYPE), 1, &file);

            int error = ferror(&file);

            FileHandlingErrors toReturn = FileHandlingErrors::OK;

            if (error != 0 || elementswritted != 1)
            {
                toReturn = FileHandlingErrors::UnknownWriteError;
            }

            return toReturn;
        }

    private:
        static int const c_fileTypeSize;
        static int const c_imageInfoSize;
        static char const c_bitmapFormatSpecifier[];
        static unsigned short c_numberOfPlanes;
        static unsigned short const c_bitsPerPixel;
        static unsigned int const c_compressionLevel;
    };
}

#endif