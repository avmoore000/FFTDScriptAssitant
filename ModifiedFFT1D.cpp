/* --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                            */
/* 5724-S84                                                        */
/* (C) Copyright IBM Corp. 2007,2008, All Rights Reserved          */
/* US Government Users Restricted Rights - Use, duplication or     */
/* disclosure restricted by GSA ADP Schedule Contract with         */
/* IBM Corp.                                                       */
/* --------------------------------------------------------------- */
/* PROLOG END TAG zYx                                              */

/*

  FFT1D_sample.c - a simple routine to drive the fft library

*/

/* NOTE:
 * Computing a forward followed by a backward transform (or vice versa) will
 * result in the original data multiplied by the size of the transform
 * (the product of the dimensions).
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>/* --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                            */
/* 5724-S84                                                        */
/* (C) Copyright IBM Corp. 2007,2008, All Rights Reserved          */
/* US Government Users Restricted Rights - Use, duplication or     */
/* disclosure restricted by GSA ADP Schedule Contract with         */
/* IBM Corp.                                                       */
/* --------------------------------------------------------------- */
/* PROLOG END TAG zYx                                              */

/*

  FFT1D_sample.c - a simple routine to drive the fft library

*/

/* NOTE:
 * Computing a forward followed by a backward transform (or vice versa) will
 * result in the original data multiplied by the size of the transform
 * (the product of the dimensions).
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include <libfft.h>

#define HUGE_TLB
#define HUGE_PAGE_SIZE (16*1024*1024) /* 16MB */

// Forward declare.
int test_1D(int numberOfFfts, int sizeOfFfts, int spusToUse, int flavor, int hugepage_flag);


// Main.
int main(int argc, char *argv[])
{
 if (argc < 6 || argc > 7)
 {
   fprintf(stdout, "Usage: %s <function type = c2c, r2c, c2r> <number of ffts> <spus to use> <size of each fft> <hugepage_flag>\n", argv[0]);
   exit(1);
 }

 int numberOfFfts = atoi(argv[2]);
 int spusToUse = atoi(argv[3]);
 int sizeOfFfts = atoi(argv[4]);
 int hugepage_flag = atoi(argv[5]);

 int flavor;
 if (strcasecmp(argv[1], "c2c") == 0)
 {
   flavor = FFT_TYPE_C2C;
 }
 else if (strcasecmp(argv[1], "r2c") == 0)
 {
   flavor = FFT_TYPE_R2C;
 }
 else if (strcasecmp(argv[1], "c2r") == 0)
 {
   flavor = FFT_TYPE_C2R;
 }
 else
 {
   fprintf(stdout, "Bad function type.\n");
   exit(1);
 }

	while (true)
	{
 		int res = test_1D(numberOfFfts, sizeOfFfts, spusToUse, flavor, hugepage_flag);
	}

 return res;
} // end main

// Generate complex numbers as input.
void generateC2Cdata(int numberOfFfts, int sizeOfFfts, float **problems)
{
 int i;
 for (i=0; i<numberOfFfts; ++i)
 {
   int j;
   for (j=0; j<sizeOfFfts; ++j)
   {
     problems[i][j*2] = rand() % 1024; // Real
     problems[i][j*2+1] = rand() % 1024; // Imag
   }
 }
}

// Generate packed reals as input.
void generateR2Cdata(int numberOfFfts, int sizeOfFfts, float **problems)
{
 int i;
 for (i=0; i<numberOfFfts; ++i)
 {
   int j;
   for (j=0; j<sizeOfFfts; ++j)
   {
     problems[i][j] = rand() % 1024; // Real
   }
   // Don't care about the elements in the other half of the array,
   // since they don't get used.
 }
}

// Generate complex conjugates as input.
void generateC2Rdata(int numberOfFfts, int sizeOfFfts, float **problems)
{
 int i;
 for (i=0; i<numberOfFfts; ++i)
 {
   problems[i][0] = rand() % 1024; // Real
   problems[i][1] = 0; // Imag
   int j;
   for (j=1; j<(sizeOfFfts+1)/2; ++j)
   {
     problems[i][j*2] = rand() % 1024; // Real
     problems[i][j*2+1] = rand() % 1024; // Imag
     // Complex conjugate.
     problems[i][(sizeOfFfts-j)*2] = problems[i][j*2]; // Real
     problems[i][(sizeOfFfts-j)*2+1] = -problems[i][j*2+1]; // Imag
   }
   if (!(sizeOfFfts % 2)) // Size is even.
   {
     problems[i][sizeOfFfts] = rand() % 1024; // Real
     problems[i][sizeOfFfts+1] = 0; // Imag
   }
 }
}

// Allocate space for data and perform the FFT.
int test_1D(int numberOfFfts, int sizeOfFfts, int spusToUse, int flavor, int hugepage_flag)
{
 // Allocate storage for input and output data.
 void *ptr;
 int i;
 posix_memalign(&ptr, 128, sizeof(float *) * numberOfFfts);
 float **input_data = (float **)ptr;
 posix_memalign(&ptr, 128, sizeof(float *) * numberOfFfts);
 float **output_data = (float **)ptr;
 unsigned int mallocLen = sizeof(float) * 2 * sizeOfFfts; // Real + imaginary
 mallocLen += mallocLen % 16;
 unsigned int dataLen = 0;
 if (hugepage_flag)
 /* Using hugepage can significantly reduce the TLB miss thus improve the performance */
 {
   int fmem;
   char *mem_file = "/huge/FFT1D_sample_mem.bin";
   if ((fmem = open(mem_file, O_CREAT | O_RDWR, 0755)) == -1)
   {
      fprintf(stdout, "ERROR: unable to open file %s (errno=%d).\n", mem_file, errno);
      return -1;
   }
   else
   {
     remove(mem_file);
     dataLen = numberOfFfts * mallocLen * 2;
     dataLen = ( dataLen + HUGE_PAGE_SIZE-1 ) & ~ (HUGE_PAGE_SIZE-1);
     ptr = mmap(0, dataLen, PROT_READ | PROT_WRITE, MAP_PRIVATE, fmem, 0);
     if (ptr == MAP_FAILED) {
        printf("ERROR: unable to mmap file %s (errno=%d).\n", mem_file, errno);
        close (fmem);
        return -1;
     }
     for (i=0; i<numberOfFfts; i++)
     {
       input_data[i] = (float *)ptr;
       /* If the input data are no longer used after computation,
        * the input and output data can share the same buffer.
        * In this in-place case, TLB miss can be further reduced.
        */
       ptr += mallocLen;
       output_data[i] = (float *)ptr;
       ptr += mallocLen;
     }
   }
 }
 else
 {
   for (i=0; i<numberOfFfts; ++i)
   {
     posix_memalign(&ptr, 128, mallocLen);
     input_data[i] = (float *)ptr;
     posix_memalign(&ptr, 128, mallocLen);
     output_data[i] = (float *)ptr;
   }
 }

 // Populate input data.
 srand(time(NULL));

 if (flavor == FFT_TYPE_C2C)
 {
   generateC2Cdata(numberOfFfts, sizeOfFfts, input_data);
 }
 else if (flavor == FFT_TYPE_R2C)
 {
   generateR2Cdata(numberOfFfts, sizeOfFfts, input_data);
 }
 else if (flavor == FFT_TYPE_C2R)
 {
   generateC2Rdata(numberOfFfts, sizeOfFfts, input_data);
 }

 // Start timer.
 struct timeval start, end;
 gettimeofday(&start, NULL);

 // Call library to process.
 fft_handle_t handle;
 int res = fft_1d_sp_initialize(&handle, spusToUse);
 if (res == FFT_RC_SUCCESS)
 {
   // Perform the transform.
   res = fft_1d_sp_perform(handle, numberOfFfts, sizeOfFfts, (void **)input_data, (void **)output_data, 0, flavor);
   if (res != FFT_RC_SUCCESS)
   {
     fprintf(stdout, "FFT failure: %d\n", res);
   }
   // Cleanup.
   fft_1d_sp_terminate(handle);
 }
 else
 {
   fprintf(stdout, "FFT failed to initialize: %d\n", res);
 }

 // Stop timer.
 gettimeofday(&end, NULL);
 unsigned int elapsed = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
 fprintf(stdout, "Calculation time took %u usec.\n", elapsed);

 // Cleanup.
 if (hugepage_flag)
 {
   munmap(input_data[0], dataLen);
 }
 else
 {
   for (i=0; i<numberOfFfts; ++i)
   {
     free(input_data[i]);
     free(output_data[i]);
   }
 }
 free(input_data);
 free(output_data);
 return res;
}
