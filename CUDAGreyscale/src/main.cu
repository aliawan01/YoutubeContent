#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX(x,y) (x) > (y) ? (x) : (y)
#define MAX3(x,y,z) MAX(x, MAX(y,z))

#define MIN(x,y) (x) < (y) ? (x) : (y)
#define MIN3(x,y,z) MIN(x, MIN(y,z))

__global__ void greyscalify(uint8_t* orig, uint8_t* avg_img, uint8_t* lum_img, uint8_t* light_img, uint32_t width, uint32_t height, uint32_t num_of_channels) {
  //printf("blockIdx.x: %d, blockIdx.y: %d, threadIdx.x: %d, threadIdx.y: %d, blockDim.x: %d, blockDim.y: %d\n", blockIdx.x, blockIdx.y, threadIdx.x, threadIdx.x, blockDim.x, blockDim.y);

  uint32_t x = (blockIdx.x*blockDim.x)+threadIdx.x;
  uint32_t y = (blockIdx.y*blockDim.y)+threadIdx.y;

  int index = num_of_channels*(y*width+x);

  if (index < num_of_channels*width*height) {
      uint8_t* pixel = orig+index;
      uint8_t r = pixel[0];
      uint8_t g = pixel[1];
      uint8_t b = pixel[2];

      uint8_t average = (r + g + b)/3;
      uint8_t lightness = (MAX3(r,g,b) + MIN3(r,g,b))/3;
      uint8_t luminosity = 0.21*r + 0.72*g + 0.07*b;

      memset(avg_img+index, average, 3);
      memset(lum_img+index, lightness, 3);
      memset(light_img+index, luminosity, 3);
  }
}

int main(void) {
  int width, height, num_of_channels;
  uint8_t* data = stbi_load("images/field_image.jpg", &width, &height, &num_of_channels, 0);

  uint8_t* orig_image;
  uint8_t* greyscaled_average_image;
  uint8_t* greyscaled_luminosity_image;
  uint8_t* greyscaled_lightness_image;

  cudaMallocManaged(&orig_image, width*height*num_of_channels);
  memcpy(orig_image, data, width*height*num_of_channels);

  cudaMallocManaged(&greyscaled_average_image, width*height*num_of_channels);
  cudaMallocManaged(&greyscaled_luminosity_image, width*height*num_of_channels);
  cudaMallocManaged(&greyscaled_lightness_image, width*height*num_of_channels);

  greyscalify<<<dim3(width/10, height/10), dim3(10, 10)>>>(orig_image, greyscaled_average_image, greyscaled_luminosity_image, greyscaled_lightness_image, width, height, num_of_channels);
  cudaDeviceSynchronize();

  stbi_image_free(data);

  int success_avg = stbi_write_jpg("images/greyscale_cuda_average.jpg", width, height, num_of_channels, greyscaled_average_image, 100);
  int success_lightness = stbi_write_jpg("images/greyscale_cuda_lightness.jpg", width, height, num_of_channels, greyscaled_lightness_image, 100);
  int success_luminosity = stbi_write_jpg("images/greyscale_cuda_luminosity.jpg", width, height, num_of_channels, greyscaled_luminosity_image, 100);

  cudaFree(orig_image);
  cudaFree(greyscaled_average_image);
  cudaFree(greyscaled_luminosity_image);
  cudaFree(greyscaled_lightness_image);

  if (success_avg) {
    printf("Successfully saved average file\n");
  }
  else {
    printf("Failed to save average file\n");
  }

  if (success_lightness) {
    printf("Successfully saved lightness file\n");
  }
  else {
    printf("Failed to save lightness file\n");
  }

  if (success_luminosity) {
    printf("Successfully saved luminosity file\n");
  }
  else {
    printf("Failed to save luminosity file\n");
  }

  return 0;
}
