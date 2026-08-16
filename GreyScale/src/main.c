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

int main(void) {
  int width, height, num_of_channels;
  uint8_t* data = stbi_load("images/field_image.jpg", &width, &height, &num_of_channels, 0);

  uint8_t* greyscaled_average_image = malloc(width*height*num_of_channels);
  uint8_t* greyscaled_lightness_image = malloc(width*height*num_of_channels);
  uint8_t* greyscaled_luminosity_image = malloc(width*height*num_of_channels);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int index = num_of_channels*(y*width+x);
      uint8_t* pixel = data+index;
      uint8_t r = pixel[0];
      uint8_t g = pixel[1];
      uint8_t b = pixel[2];

      uint8_t average = (r + g + b)/3;
      uint8_t lightness = (MAX3(r,g,b) + MIN3(r,g,b))/3;
      uint8_t luminosity = 0.21*r + 0.72*g + 0.07*b;

      memset(greyscaled_average_image+index, average, 3);
      memset(greyscaled_lightness_image+index, lightness, 3);
      memset(greyscaled_luminosity_image+index, luminosity, 3);
    }
  }

  stbi_image_free(data);

  int success_avg = stbi_write_jpg("images/greyscale_average.jpg", width, height, num_of_channels, greyscaled_average_image, 100);
  int success_lightness = stbi_write_jpg("images/greyscale_lightness.jpg", width, height, num_of_channels, greyscaled_lightness_image, 100);
  int success_luminosity = stbi_write_jpg("images/greyscale_luminosity.jpg", width, height, num_of_channels, greyscaled_luminosity_image, 100);

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
