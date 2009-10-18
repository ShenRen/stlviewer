// Copyright (c) 2009 Olivier Crave
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <fstream>

#include "vector.h"

class Entity {
 public:
  Entity();
  ~Entity();
  void open(const ::std::string&);
  void write(const ::std::string&);
  void setFormat(const int format);
  void close();
  enum Format {
    ASCII,
    BINARY
  };
  /*typedef struct {
    float x;
    float y;
    float z;
  } Vector;*/
  typedef struct {
    float x;
    float y;
    float z;
  } Normal;
  typedef char Extra[2];
  typedef struct {
    Normal normal;
    Vector vector[3];
    Extra extra;
  } Facet;
  typedef struct {
    char          header[81];
    Format        type;
    int           num_facets;
    int           num_points;
    Vector       max;
    Vector       min;
    Vector       size;
    float         bounding_diameter;
    float         shortest_edge;
    float         volume;
    float         surface;
  } Stats;
  Stats stats() const { return stats_; };
  Facet* facets() const { return facets_; };

 private:
  void initialize(const ::std::string&);
  void writeBinary(const ::std::string&);
  void writeAscii(const ::std::string&);
  int readIntFromBytes(::std::ifstream&);
  float readFloatFromBytes(::std::ifstream&);
  void writeBytesFromInt(::std::ofstream&, int);
  void writeBytesFromFloat(::std::ofstream& file, float);
  void readData(int, int);
  void allocate();
  int getNumPoints();
  float getVolume();
  float getSurface();
  float getArea(Facet *facet);
  void calculateNormal(float normal[], Facet *facet);
  void normalizeVector(float v[]);
  ::std::ifstream file_;
  Facet *facets_;
  Stats stats_;
};

#endif  // ENTITY_H