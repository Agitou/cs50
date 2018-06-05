#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./copy infile outfile\n");
        return 1;
    }

    // remember filenames
    int enlarge = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    if (enlarge < 1 || enlarge > 100)
    {
        fprintf(stderr, "Enlargement factor must be between 1 and 100\n");
        return 1;
    }

    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 1;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 1;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 1;
    }

    int inpadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;  // determine padding for input file

    bi.biWidth = bi.biWidth * enlarge;
    bi.biHeight = bi.biHeight * enlarge;

    int outpadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;  // determine padding for output

    bi.biSizeImage = (sizeof(RGBTRIPLE) * bi.biWidth + outpadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)  //iterate for each scanline
    {
        for (int j = 0; j < enlarge; j++)  //iterate for "enlarge" number of times to make "enlarge" number of lines
        {
            for (int k = 0; k < bi.biWidth; k++)  //iterate for each pixel in line
            {
                RGBTRIPLE triple;
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                for (int m = 0; m < enlarge; m++)  //iterate for "enlarge" number of times to make "enlarge" number of copies of pixels
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);   
                }
            }
            for (int n = 0; n < outpadding; n++)  //add padding to output file
            {
                fputc(0x00, outptr);
            }
            if (j < enlarge - 1) 
            {
                fseek (inptr, -bi.biWidth * 3, SEEK_CUR); //return to beginning of scanline
            }
            else
            {
                fseek (inptr, inpadding, SEEK_CUR);  //skip over padding in input file
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}