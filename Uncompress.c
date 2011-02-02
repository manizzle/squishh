/*
 * Copyright (c) 2009 Murtaza Munaim
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX_ARGS 3
#define COMPRESS_SIZE 7
#define UNCOMPRESS_SIZE 8
#define SHIFT 8
#define MSB_CLEAR 127

long long ByteArrayToLongLong (unsigned char* arr, int bytes, int type);
double MyCeil(double input);
int MyPow(int base , int exponent);

int main(int argc, char** argv) {
   FILE *in, *out;
   long long origSize = 0;
   int ndx = 0, evenSize = 0, ctr = 0, endVal = 0;
   unsigned char byte, *decompress, readSize[UNCOMPRESS_SIZE];
   char *compress, byteBlock[UNCOMPRESS_SIZE];
   struct stat inStat, outStat;
   
   if (argc != MAX_ARGS) {
      printf("Usage: Compress fromFile toFile\n");
      exit(0);
   }   
   if (stat(argv[1], &inStat)) {
      printf("Input file %s doesn't exist or is unreadable.\n", argv[1]);
      exit(0);
   }
   if (!stat(argv[2], &outStat)) {
      printf("Cannot create %s, perhaps because", argv[2]);
      printf(" it already exists.\n");
      exit(0);
   }
   
   in = fopen(argv[1], "rb");
   out = fopen(argv[2], "w");  
   for (ndx = 0; ndx < UNCOMPRESS_SIZE; ndx++)
      fread(readSize + ndx, 1, 1, in);    
   origSize = ByteArrayToLongLong (readSize, UNCOMPRESS_SIZE, 1); 
   endVal = (int) MyCeil( (double)((origSize * (COMPRESS_SIZE * 1.0))
    / (UNCOMPRESS_SIZE * 1.0)));
   
   fseek(in, 0, SEEK_END);
   evenSize = ftell(in);
   fseek(in, 0, SEEK_SET);   
   if (endVal != (evenSize - UNCOMPRESS_SIZE)) {
      printf("Incorrect file length in compressed data.\n");
      exit(0);
   }
      
   while ((evenSize % COMPRESS_SIZE) != 0)
      evenSize++;  
   compress = (char*) malloc(evenSize * sizeof(char));
   memset(compress, 0, evenSize);
   decompress = (unsigned char*) 
    malloc((evenSize * UNCOMPRESS_SIZE)/ COMPRESS_SIZE);
 
   fseek(in, UNCOMPRESS_SIZE, SEEK_SET);
   for (ndx = 0; ndx < endVal; ndx++) {
      if (!feof(in))
         compress[ndx] = (char)fgetc(in); 
   }
   
   for (ndx = 0; ndx < evenSize ; ndx += COMPRESS_SIZE) {
      memset(byteBlock, 0, UNCOMPRESS_SIZE);
      endVal = ndx;
      ctr = 0;
      for (; ctr < COMPRESS_SIZE; ctr++, endVal++)
         byteBlock[ctr] = compress[endVal];
      byteBlock[COMPRESS_SIZE] = 0;
      
      for (ctr = COMPRESS_SIZE; ctr > 0; ctr--)
			byteBlock[ctr] = (unsigned char) (((byteBlock[ctr] >> (ctr + 1)) 
			 & (int)(MyPow(2, COMPRESS_SIZE - ctr) - 1)) 
			 | (byteBlock[ctr - 1] << (COMPRESS_SIZE - ctr))
			 & MSB_CLEAR );
      byteBlock[0] = (unsigned char) (byteBlock[0] >> 1 & MSB_CLEAR);
      
      endVal = ((ndx *UNCOMPRESS_SIZE) / COMPRESS_SIZE);
      ctr = 0;
      while (ctr < UNCOMPRESS_SIZE)
         decompress[endVal++] = byteBlock[ctr++];
   }
   
   for (ndx = 0; ndx < origSize; ndx++)
      fwrite(decompress + ndx, sizeof(unsigned char), 1, out);
   
   fclose(in);
   fclose(out);
}    

long long ByteArrayToLongLong (unsigned char* arr, int bytes, int type) { 
   long long num = (long long) 0, tmp;
   int ndx;
   for (ndx = 0; ndx < bytes; ndx++)
   {
      tmp = (long long) 0;
      tmp = arr[bytes - ndx - 1];
      num |= (tmp << ((ndx & COMPRESS_SIZE) << MAX_ARGS));
   }
   return num;
}

double MyCeil(double input) {
   double returns; 
   char *str;
   int ndx;
   ndx = snprintf(0, 0, "%.0f", input);
   str =(char *) malloc(ndx + 1); 
   sprintf(str, "%.0f", input); 
   sscanf(str, "%lf", &returns); 
   free(str);
   return input > returns ? returns + 1.0 : returns; 
}

int MyPow(int base , int exponent) {
   if (exponent == 0)
      return 1;
   else
      return (base << (exponent - 1));
}