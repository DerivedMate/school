#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

typedef unsigned char uchar;

struct Pixel {
  uchar r;
  uchar g;
  uchar b;

  Pixel(uchar r, uchar g, uchar b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }

  bool operator== (Pixel& p) {
    return this->r == p.r && this->g == p.g && this->b == p.b;
  }
};

typedef unsigned char headerT[54];
typedef std::vector<Pixel> pixelsT;
typedef std::vector<unsigned char> excessT;

void read_header(std::ifstream& f, uchar* header) {
  for(int i = 0; i < 54 && f >> header[i]; i++) {}
}

void read_pixels(std::ifstream& f, pixelsT& pixels, int size) {
  for(int i = 0; i < size; i++) {
    uchar g, b, r;
    f >> g >> b >> r;
    Pixel p(r, g, b);

    pixels.push_back(p);
  }
}

void read_excess(std::ifstream& f, excessT& ex) {
  uchar u;
  while(f >> u) ex.push_back(u);
}

struct Image {
  int w;
  int h;
  headerT header;
  pixelsT pixels;
  excessT excess;
  
  Image(std::string path) {
    std::ifstream f(path);
    read_header(f, this->header);
    this->w = this->header[18];
    this->h = this->header[22];

    read_pixels(f, this->pixels, this->size());
    read_excess(f, this->excess);

    f.close();
  }

  int size() {
    return this->w * this->h;
  }
};

const uchar mx = 255;
const uchar mn = 0;

bool are_pixels_eq(pixelsT& px, int i, int j) {
  Pixel p0 = px.at(i);
  Pixel p1 = px.at(j);

  bool eq = p0 == p1;

  if(!eq) printf("(%i, %i, %i) /= (%i, %i, %i); %i /= %i\n", p0.r, p0.g, p0.b, p1.r, p1.g, p1.b, i, j);

  return eq;
}

bool is_symmetric(pixelsT pxs, int size, int w, int h) {
  for(int i = 0; i < size/2; i++) {
    int y = i / w;
    int x = i % w;
    int j = (h - y - 1) * w + x;

    if(!are_pixels_eq(pxs, i, j)) {
      return false;
    };
  }
  return true;
}

bool is_bnw(Image& img) {
  for(Pixel& p : img.pixels) {
    if(p.r != p.g && p.g != p.b) 
      return false;
  }

  return true;
}

void print_gradient(headerT headerT, excessT& excess, int w, int h, int size) {
  std::ofstream out("gradient.bmp");
  // print the header
  for(int i = 0; i < 54; i++) out << headerT[i];

  for(int i = 0; i < size; i ++) {
    int    x      = i % w;
    int    y      = i / w;
    int    a      = w/2;
    int    b      = h/2;
    int    x_r    = x - a;
    int    y_r    = y - b;

    double s      = std::abs(double(x_r*y_r) / double((w-1)*(h-1)));
           s      = std::cbrt(s);
    double red0   = 252, red1   = 3  ;
    double green0 = 219, green1 = 252; 
    double blue0  = 3  , blue1  = 248;

    uchar  red    = s * (red1   - red0)   + red0,
           green  = s * (green1 - green0) + green0,
           blue   = s * (blue1  - blue0)  + blue0;

    out << blue << green << red;
  }

  // print the rest of the input file
  for(uchar u : excess) out << u;

  out.close();
}

void print_flag(std::ofstream& out, pixelsT px, headerT headerT, excessT& excess, int w, int h, int size) {
  // print the header
  for(int i = 0; i < 54; i++) out << headerT[i];

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
  for(uchar u : excess) out << u;
}

void print_gif(int n, int w, int h, int size, headerT headerT, excessT& excess) {
  std::string f_name = "./gif-src/part";
  f_name += std::to_string(n);
  f_name += ".bmp";
  std::ofstream out(f_name);

  int a = 0, 
      b = 0,
      r = n + 1;

  for(int i = 0; i < 54; i++) out << headerT[i];

  for(int i = 0; i < size; i++) {
    int x  = i % w - w / 2;
    int y  = i / w - h / 2;
    int dx = x - a,
        dy = y - b;

    float dist          = dx * dx + dy * dy;
    float r_pr          = dist / r;
    unsigned char red   = r_pr * 255,
        green           = r_pr * r_pr * 100,
        blue            = r_pr / 2 * 24 + 100 % 255;
      
    out << green << blue << red;
  }

  for (unsigned char u : excess) out << u;

  out.close();
} 

int main() {
  Image input("./inp2.bmp");
  Image blank("./blank.bmp");
  Image blank_flag("./blank_flag.bmp");
  std::ofstream out("out.bmp");

  printf("%ix%i : %i\n", input.w, input.h, input.size());
  printf("symmetric: %s\n", (is_symmetric(input.pixels, input.size(), input.w, input.h) ? "yep" : "nope"));
  printf("black-n-white: %s\n", is_bnw(input) ? "yep" : "nope");

  print_flag(out, blank_flag.pixels, blank_flag.header, blank_flag.excess, blank_flag.w, blank_flag.h, blank_flag.size());
  
  for(int i = 0; i < 800; i++) {
    print_gif(i, blank.w, blank.h, blank.size(), blank.header, blank.excess);
  }

  print_gradient(blank.header, blank.excess, blank.w, blank.h, blank.size());

  return 0;
}
