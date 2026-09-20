#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX(x,y) (x) > (y) ? (x) : (y)
#define MAX3(x,y,z) MAX(x, MAX(y,z))

#define MIN(x,y) (x) < (y) ? (x) : (y)
#define MIN3(x,y,z) MIN(x, MIN(y,z))

void print_256i_16(__m256i val, int highlight_three_indexes) {
  _Alignas(32) uint16_t* val_buf = malloc(sizeof(uint16_t)*16);

  _mm256_store_si256((__m256i*)val_buf, val);

  printf("print_256i_16:\n");
  for (int i = 0; i < 16; i++) {
    if (i == 7) {
      printf("i: %d, value: %d\n====================\n", i, val_buf[i]);
    }
    else if (!(i % 3) && highlight_three_indexes) {
      printf("i: %d, value: %d------------\n", i, val_buf[i]);
    }
    else {
      printf("i: %d, value: %d\n", i, val_buf[i]);
    }
  }

  free(val_buf);
}

void print_128i(__m128i val) {
  _Alignas(32) int* val_buf = malloc(sizeof(int)*4);

  _mm_store_si128((__m128i*)val_buf, val);

  printf("print_128i:\n");
  for (int i = 0; i < 4; i++) {
    printf("i: %d, value: %d\n", i, val_buf[i]);

  }

  free(val_buf);
}

void print_128i_8(__m128i val, int highlight_three_indexes) {
  _Alignas(32) uint8_t* val_buf = malloc(sizeof(uint8_t)*16);

  _mm_store_si128((__m128i*)val_buf, val);

  printf("print_128i_8:\n");
  for (int i = 0; i < 16; i++) {
    if (!(i % 3) && highlight_three_indexes)
      printf("i: %d, value: %d------------\n", i, val_buf[i]);
    else
      printf("i: %d, value: %d\n", i, val_buf[i]);
  }

  free(val_buf);
}

void print_128i_16(__m128i val) {
  _Alignas(32) uint16_t* val_buf = malloc(sizeof(uint16_t)*8);

  _mm_store_si128((__m128i*)val_buf, val);

  printf("print_128i_16:\n");
  for (int i = 0; i < 8; i++) {
      printf("i: %d, value: %d\n", i, val_buf[i]);
  }

  free(val_buf);

}

void print_256i(__m256i val) {
  _Alignas(32) int* val_buf = malloc(sizeof(int)*8);

  _mm256_store_si256((__m256i*)val_buf, val);

  printf("print_256i:\n");
  for (int i = 0; i < 8; i++) {
    printf("i: %d, value: %d\n", i, val_buf[i]);

  }

  free(val_buf);
}

void print_256f(__m256 val) {
  _Alignas(32) float* val_buf = malloc(sizeof(float)*8);

  _mm256_store_ps(val_buf, val);

  printf("print_256 float:\n");
  for (int i = 0; i < 8; i++) {
    printf("i: %d, value: %f\n", i, val_buf[i]);

  }

  free(val_buf);
}


int main(void) {
  int width, height, num_of_channels;
  uint8_t* data = stbi_load("images/field_image.jpg", &width, &height, &num_of_channels, 0);

  uint8_t* greyscaled_average_image_normal = malloc(width*height*num_of_channels);
  uint8_t* greyscaled_average_image_simd   = malloc(width*height*num_of_channels);

  LARGE_INTEGER start, end, freq;
  QueryPerformanceFrequency(&freq);

  QueryPerformanceCounter(&start);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int index = num_of_channels*(y*width+x);
      uint8_t* pixel = data+index;
      uint8_t r = pixel[0];
      uint8_t g = pixel[1];
      uint8_t b = pixel[2];

      uint8_t average = (r + g + b)/3;

      memset(greyscaled_average_image_normal+index, average, 3);
    }
  }

  QueryPerformanceCounter(&end);

  double elapsed_time = end.QuadPart-start.QuadPart;
  elapsed_time /= freq.QuadPart;

  printf("Average stats\n");
  printf("Seconds: %lf, ms: %lf\n", elapsed_time, elapsed_time*1000);


  QueryPerformanceCounter(&start);
  int total = num_of_channels*height*width;
  int i = 0;
  for (; i < total; i += 15) {
    _Alignas(16) uint8_t* pixel = data+i;

    __m128i u8_vec = _mm_lddqu_si128((__m128i*)pixel);
    __m256i r_vals = _mm256_cvtepu8_epi16(u8_vec);
    uint16_t original_third_blue_value = _mm256_extract_epi16(r_vals, 8);

    __m256i g_vals = _mm256_bsrli_epi128(r_vals, 2);
    __m256i b_vals = _mm256_bsrli_epi128(g_vals, 2);
    b_vals = _mm256_insert_epi16(b_vals, original_third_blue_value, 6);

    __m256i total = _mm256_add_epi16(r_vals, g_vals);
    total = _mm256_add_epi16(total, b_vals);

    __m256i magic_num = _mm256_set1_epi16((uint16_t)43691);

    __m256i multiplied = _mm256_mulhi_epu16(total, magic_num);
    __m256i divided = _mm256_srli_epi16(multiplied, 1);

    __m128i top = _mm256_extractf128_si256(divided, 0);
    __m128i bottom = _mm256_extractf128_si256(divided, 1);

    __m128i unsorted_vals = _mm_packus_epi16(top, bottom);
    __m128i shuffle_mask = _mm_setr_epi8(0, 0, 0, 3, 3, 3, 6, 6, 6, 9, 9, 9, 12, 12, 12, 15);
    __m128i greyscaled = _mm_shuffle_epi8(unsorted_vals, shuffle_mask);

    greyscaled = _mm_insert_epi8(greyscaled, pixel[15], 15);

    _mm_store_si128((__m128i*)(greyscaled_average_image_simd+i), greyscaled);
  }

  QueryPerformanceCounter(&end);

  elapsed_time = end.QuadPart-start.QuadPart;
  elapsed_time /= freq.QuadPart;

  printf("Simd stats\n");
  printf("Seconds: %lf, ms: %lf\n", elapsed_time, elapsed_time*1000);

  stbi_image_free(data);

  int success_avg_normal = stbi_write_jpg("images/greyscale_average_normal.jpg", width, height, num_of_channels, greyscaled_average_image_normal, 100);
  int success_avg_simd = stbi_write_jpg("images/greyscale_average_simd.jpg", width, height, num_of_channels, greyscaled_average_image_simd, 100);

  if (success_avg_normal) {
    printf("Successfully saved normal average file\n");
  }
  else {
    printf("Failed to save normal average file\n");
  }

  if (success_avg_simd) {
    printf("Successfully saved simd average file\n");
  }
  else {
    printf("Failed to save simd average file\n");
  }



  return 0;
}
