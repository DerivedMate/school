#include <stdio.h>
#include <iostream>
#include <fstream>

typedef unsigned char pixels[][3];

void read_header(std::ifstream& f, unsigned char* header) {
  for(int i = 0; i < 54 && f >> header[i]; i++) {}
}

void read_pixels(std::ifstream& f, pixels pixels, int size) { 
  for(int i = 0; i < size; i++) {
    unsigned char g, b, r;
    f >> g >> b >> r;

    pixels[i][0] = r;
    pixels[i][1] = g;
    pixels[i][2] = b;

    printf("(%i, %i, %i)\n", r, g, b);

  }
}

bool are_pixels_eq(pixels px, int i, int j) {
  int r0 = px[i][0],
      g0 = px[i][1],
      b0 = px[i][2];
  
  int r1 = px[j][0],
      g1 = px[j][1],
      b1 = px[j][2];

  bool eq = r0 == r1 && g0 == g1 && b0 == b1;

  if(!eq) printf("(%i, %i, %i) /= (%i, %i, %i)\n", r0, g0, b0, r1, g1, b1);

  return eq;
}

bool is_symmetric(pixels pixels, int size, int w, int h) {
  for(int i = 0; i < size; i++) {

    int y = i / w;
    int x = i % w;

    int j = (h - y - 1) * w + x;


    if(!are_pixels_eq(pixels, j, i)) {
      return false;
    };
  }
  return true;
}

int main() {
  std::ifstream f("./inp.bmp");
  unsigned char header[54];
  read_header(f, header);

  int width = (int)header[18];
  int height = (int)header[22];
  int size = width * height;

  for(int i = 0; i < 24*3; i++) {
    unsigned char g; f >> g;
  }

  auto pixels = new unsigned char[size][3];
  read_pixels(f, pixels, size); 

  /*for(int i = 0; i < size; i++) {
    printf("(%i, %i, %i)\n", (int)pixels[i][0], (int)pixels[i][1], (int)pixels[i][2]);
  }*/

  std::cout << width << " " << height << std::endl;
  std::cout << (is_symmetric(pixels, size, width, height) ? "sí" : "no") << std::endl;
  f.close();

  return 0;
}