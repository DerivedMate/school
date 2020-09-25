#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

typedef unsigned char header[54];
typedef unsigned char pixels[][3];
typedef std::vector<unsigned char> excess;

const unsigned char mx = 255;
const unsigned char mn = 0;

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

  if(!eq) printf("(%i, %i, %i) /= (%i, %i, %i); %i /= %i\n", r0, g0, b0, r1, g1, b1, i, j);

  return eq;
}

bool is_symmetric(pixels pixels, int size, int w, int h) {
  for(int i = 0; i < size/2; i++) {
    int y = i / w;
    int x = i % w;
    int j = (h - y - 1) * w + x;

    if(!are_pixels_eq(pixels, i, j)) {
        return false;
    };
  }
  return true;
}

void print_gradient(header header, excess& excess, int w, int h, int size) {
  std::ofstream out("gradient.bmp");
  // print the header
  for(int i = 0; i < 54; i++) {
    out << header[i];
  }

  for(int i = 0; i < size; i ++) {
    int           x     = i % w;
    int           y     = i / w;

    double        s     = double(x*y) / double(w*w);
    unsigned char red   = s * 255,
                  green = s * 100 + 100,
                  blue  = s * 255;

    out << green << blue << red;
  }

  // print the rest of the input file
  for(unsigned char u : excess) {
    out << u;
  }

  out.close();
}

void print_flag(std::ofstream& out, pixels px, header header, excess& excess, int w, int h, int size) {
  // print the header
  for(int i = 0; i < 54; i++) {
    out << header[i];
  }

  // print the flag
  for(int i = 0; i < size; i++) {
    int y = i / w;
    
    if(y < h / 2) {
      out << mn << mn << mx;
    } else {
      out << mx << mx << mx;
    }
  }

  // print the rest of the input file
  for(unsigned char u : excess) {
    out << u;
  }
}

bool is_circle(int i, int w, int h, int a, int b, int r) {
  int x  = i % w;
  int y  = i / w;
  int dx = x - a,
      dy = y - b;

  return dx*dx + dy*dy <= r*r;
}

void print_gif(int n, int w, int h, int size, header header, excess& excess) {
  std::string f_name = "./gif-src/part";
  f_name += std::to_string(n);
  f_name += ".bmp";
  std::ofstream out(f_name);

  int a = 0, 
      b = 0,
      r = n + 1;

  for(int i = 0; i < 54; i++) out << header[i];

  for(int i = 0; i < size; i++) {
    int x  = i % w - w / 2;
    int y  = i / w - h / 2;
    int dx = x-a,
        dy = y-b;

    float dist          = dx*dx + dy*dy;
    float r_pr          = dist / r;
    unsigned char red   = r_pr * 255,
        green           = r_pr*r_pr * 100,
        blue            = r_pr / 2 * 24 + 100 % 255;
      
    out << green << blue << red;
    
  }

  for (unsigned char u : excess) out << u;
  

  out.close();
}

int main() {
  std::ifstream f("./blank.bmp");
  std::ofstream out("./out.bmp");
  header header;
  read_header(f, header);

  int width  = (int)header[18];
  int height = (int)header[22];
  int size   = width * height;

  auto pixels = new unsigned char[size][3];
  read_pixels(f, pixels, size);

  std::cout << width << " " << height << " " << size << std::endl;
  std::cout << "symmetric: " << (is_symmetric(pixels, size, width, height) ? "yep" : "no") << std::endl;

  excess excess; 
  {
    unsigned char u;
    while (f >> u) excess.push_back(u);
  }

  print_flag(out, pixels, header, excess, width, height, size);
  
  for(int i = 0; i < 500; i++) {
    print_gif(i, width, height, size, header, excess);
  }

  print_gradient(header, excess, width, height, size);

  f.close();
  out.close();

  return 0;
}
