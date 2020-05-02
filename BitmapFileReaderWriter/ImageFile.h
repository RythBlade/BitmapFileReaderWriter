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
    class ImageFile
    {
    public:
        ImageFile();

        void write(char const* const filaname, int totalImageWidth, int totalImageHeight);
        void writeTestBitmap(char const* const filaname, int totalImageWidth, int totalImageHeight);

        void write(char const* const filename, ImageCanvas const& canvas);

    private:
        void writeInfoHeader(FILE& file, int totalImageWidth, int totalImageHeight);
        void writeFileHeader(FILE& file, int totalImageWidth, int totalImageHeight);
        void writeTestColourData(FILE& file, int totalImageWidth, int totalImageHeight);

        void writeCanvasColourData(FILE& file, ImageCanvas const& canvas);

        void writeColour(FILE& file, Colour const& colour);

        int calculateNumberOfScanlinePaddingBytes(int totalImageWidth) const;

        int calculateTotalFileSize(int totalImageWidth, int totalImageHeight) const;

        int calculateOffsetIntoFileForStartOfPixelData();

    private:
        static int const c_fileTypeSize;
        static int const c_imageInfoSize;
    };
}

#endif