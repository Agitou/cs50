/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    int n = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file for write & read
    FILE *outptr = fopen(outfile, "w+");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
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
        return 4;
    }
    
     //old padding, width, height
    int oldPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int oldWidth = bi.biWidth;
    int oldHeight = abs(bi.biHeight);

    
    //scale bi.biWidth and bi.biHeight
    bi.biWidth *= n;
    bi.biHeight *= n;
    
    //new padding
    int newPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    //update bi.biSizeImage and bf.bfSize
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + newPadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0; i < oldHeight; i++)
    {
        
        //Temporary storage
        RGBTRIPLE triple;
        
        //Loop n times to duplicate vertically
        for(int c = 0; c < n; c++){
            
            // iterate over pixels in scanline
            for (int j = 0; j < oldWidth; j++){

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
         
                //Write a pixel horizontally n times
                for(int b = 0; b < n; b++){
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            
                //Add outfile paddings
                for(int k = 0; k < newPadding; k++){
                    fputc(0x00, outptr);
                }    
            }
            
            //reset cursor n - 1 times 
            if(c < n - 1)
                fseek(inptr, -(long int) (oldWidth*sizeof(RGBTRIPLE) + oldPadding), SEEK_CUR);
            
        }
        
        //skip over infile's paddings
        fseek(inptr, oldPadding, SEEK_CUR);
        
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
