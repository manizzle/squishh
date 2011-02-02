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
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#define MAX_ARGS 3
#define COMPRESS_SIZE 7
#define UNCOMPRESS_SIZE 8
#define CHAR_LIM 128
#define BYTE 0xFF

unsigned char * LittleToBig (long long *num);

int main (int argc, char** argv) {
   FILE *in, *out;
   struct stat inStat, outStat;
   unsigned char *input, aByte, *compress,
    bitFlip[UNCOMPRESS_SIZE], *writeSize;
   long long origSize = 0;
   int size = 0, ndx = 0, ctr = 0, endVal = 0, compSize = 0;
   memset(bitFlip, 0, UNCOMPRESS_SIZE);
   
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
   
   in = fopen(argv[1], "r");
   out = fopen(argv[2], "wb");
   
   fseek(in, 0, SEEK_END);
   size = ftell(in);
   fseek(in, 0, SEEK_SET);
   origSize = (long long)size;
   
   while ((size % UNCOMPRESS_SIZE) != 0)
      size++;
   input = (unsigned char *) malloc(size * sizeof(unsigned char));
   memset(input, 0, size);
   
   for (ndx = 0; ndx < origSize && (aByte = getc(in)) >= 0 &&
    aByte < CHAR_LIM ;) {
      input[ndx] = (unsigned char)aByte;
      ndx++;
   }
   if (aByte >= CHAR_LIM || aByte < 0) {
      printf("Input file includes nontext byte: %d\n", aByte);
      exit(0);
   }
   
   compSize = ((size * COMPRESS_SIZE) /
    UNCOMPRESS_SIZE) * sizeof(unsigned char);
   compress = (unsigned char*) malloc(compSize);
    
   for (ndx = 0; ndx < size; ndx += UNCOMPRESS_SIZE) {
      memset(bitFlip, 0, UNCOMPRESS_SIZE);
      endVal = ndx;
      ctr = 0;
      
      while (ctr < UNCOMPRESS_SIZE) 
         bitFlip[ctr++] = input[endVal++];
      for (ctr = 0; ctr < COMPRESS_SIZE ; ctr++)
         bitFlip[ctr] = (unsigned char) ((bitFlip[ctr] << (ctr+1)) |
          (bitFlip[ctr+1] >> ((COMPRESS_SIZE - 1) - ctr)));
          
      endVal = ((ndx * COMPRESS_SIZE) / UNCOMPRESS_SIZE);
      ctr = 0;
      while (ctr < COMPRESS_SIZE)
         compress[endVal++] = bitFlip[ctr++];
   }
   
   writeSize = LittleToBig(&origSize);
   for (ndx = 0; ndx < UNCOMPRESS_SIZE; ndx++) {
      fwrite(writeSize + ndx, 1, 1, out);
   }
     
   for (ndx = 0; ndx < compSize && compress[ndx] != 0; ndx++)
      fwrite(compress + ndx, sizeof(unsigned char), 1, out);
   fclose(in);
   fclose(out);
   
   return 0;
}

unsigned char* LittleToBig (long long *num) {
   int ndx, bytes = (sizeof(long long) * sizeof(unsigned char)), shift;
   unsigned char aByte, *vals;
   vals = (unsigned char*) malloc(bytes);
   
   for (ndx = 0; ndx < bytes; ndx++) {
      shift = (ndx & COMPRESS_SIZE) << MAX_ARGS;
      aByte = ((*num) >> shift ) & BYTE;
      vals[bytes - ndx - 1] = aByte;
   }  
   return vals;
}    
