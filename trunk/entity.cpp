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

#include <QtCore/QtGlobal>
#include <math.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

#include "entity.h"

#define HEADER_SIZE 84
#define JUNK_SIZE 80
#define SIZE_OF_FACET 50
#define ASCII_LINES_PER_FACET 7

Entity::Entity() {
  facets_ = 0;
}

Entity::~Entity() {
  close();
}

void Entity::open(const ::std::string& file_name) {
  initialize(file_name);
  allocate();
  readData(0, 1);
  file_.close();
}

void Entity::write(const ::std::string& file_name) {
  if (facets_ != 0) {
    if (stats_.type == ASCII)
      writeAscii(file_name);
    else
      writeBinary(file_name);
  }
}

int lowerCase(int c) {
  return tolower(c);
}

void Entity::initialize(const ::std::string& file_name) {

  stats_.num_facets = 0;
  stats_.num_points = 0;
  stats_.surface = -1.0;
  stats_.volume = -1.0;
  facets_ = 0;

  // Open the file
  file_.open(file_name.c_str(), ::std::ios::in|::std::ios::binary);
  if (file_.is_open()) {
    int num_facets;

    // Find length of file
    file_.seekg(0, ::std::ios::end);
    int file_size = file_.tellg();

    // Check for binary or ASCII file
    file_.seekg(0, ::std::ios::beg);
    stats_.type = ASCII;
    ::std::string type;
    file_ >> type;
    transform(type.begin(), type.end(), type.begin(), lowerCase);
    if (type.compare("solid"))
      stats_.type = BINARY;
    file_.seekg(0, ::std::ios::beg);

    // Get the header and the number of facets in the .STL file 
    // If the .STL file is binary, then do the following 
    if (stats_.type == BINARY) {
      // Test if the STL file has the right size
      if ((file_size - HEADER_SIZE) % SIZE_OF_FACET != 0) {
        ::std::cerr << "The file " << file_ << " has a wrong size." << ::std::endl;
        exit(1);
      }
      num_facets = (file_size - HEADER_SIZE) / SIZE_OF_FACET;

      // Read the header 
      file_.read(stats_.header, JUNK_SIZE);
      stats_.header[80] = '\0';

      // Read the int following the header. This should contain the number of facets
      int header_num_facets = readIntFromBytes(file_);
      if(num_facets != header_num_facets) {
        ::std::cerr << "Warning: File size doesn't match number of facets in the header." << ::std::endl;
      }
    }
    else {  // Otherwise, if the .STL file is ASCII, then do the following
      // Find the number of facets
      int j = 0;
      int num_lines = 1;
      for(int i = 0; i < file_size ; i++) {
	      j++;
	      if (file_.get() == '\n') {
          if (j > 4) {  // don't count short lines
  		      num_lines++;
		      }
          j = 0;
        }
	    }
      file_.seekg(0, ::std::ios::beg);
      // Get the header
      for(int i = 0; (i < 80) && (stats_.header[i] = file_.get()) != '\n'; i++)
        stats_.header[i] = '\0'; // Lose the '\n'
      stats_.header[80] = '\0';
      
      num_facets = num_lines / ASCII_LINES_PER_FACET;
    }
    stats_.num_facets += num_facets;
  } else {
    exit(1);
  }
}

void Entity::allocate() {
  //  Allocate memory for the entire .STL file
  facets_ = new Facet[stats_.num_facets];
  if (facets_ == 0)
    ::std::cerr << "Problem allocating memory" << ::std::endl;
}

void Entity::readData(int first_facet, int first) {
   
  if(stats_.type == BINARY) {
    file_.seekg(HEADER_SIZE, ::std::ios::beg);
  } else {
    file_.seekg(0, ::std::ios::beg);
    // Skip the first line of the file
    while(file_.get() != '\n');
  }

  Facet facet;
  for(int i = first_facet; i < stats_.num_facets; i++) {
    if(stats_.type == BINARY) {  // Read a single facet from a binary .STL file
	    facet.normal.x = readFloatFromBytes(file_);
	    facet.normal.y = readFloatFromBytes(file_);
	    facet.normal.z = readFloatFromBytes(file_);
	    facet.vector[0].x = readFloatFromBytes(file_);
	    facet.vector[0].y = readFloatFromBytes(file_);
	    facet.vector[0].z = readFloatFromBytes(file_);
	    facet.vector[1].x = readFloatFromBytes(file_);
	    facet.vector[1].y = readFloatFromBytes(file_);
	    facet.vector[1].z = readFloatFromBytes(file_);
	    facet.vector[2].x = readFloatFromBytes(file_);
	    facet.vector[2].y = readFloatFromBytes(file_);
	    facet.vector[2].z = readFloatFromBytes(file_);
	    facet.extra[0] = file_.get();
	    facet.extra[1] = file_.get();
    } else {  // Read a single facet from an ASCII .STL file
      ::std::string junk;
      file_ >> junk >> junk;
      file_ >> facet.normal.x >> facet.normal.y >> facet.normal.z;
      file_ >> junk >> junk >> junk;
      file_ >> facet.vector[0].x >> facet.vector[0].y >> facet.vector[0].z;
      file_ >> junk;
      file_ >> facet.vector[1].x >> facet.vector[1].y >> facet.vector[1].z;
      file_ >> junk;
      file_ >> facet.vector[2].x >> facet.vector[2].y >> facet.vector[2].z;
      file_ >> junk >> junk;
    }
    // Write the facet into memory.
    facets_[i] = facet;
      
    // While we are going through all of the facets, let's find the
    // maximum and minimum values for x, y, and z

    // Initialize the max and min values the first time through
    if (first) {
	    stats_.max.x = facet.vector[0].x;
	    stats_.min.x = facet.vector[0].x;
	    stats_.max.y = facet.vector[0].y;
	    stats_.min.y = facet.vector[0].y;
	    stats_.max.z = facet.vector[0].z;
	    stats_.min.z = facet.vector[0].z;
  	  
	    float diff_x = qAbs(facet.vector[0].x - facet.vector[1].x);
	    float diff_y = qAbs(facet.vector[0].y - facet.vector[1].y);
	    float diff_z = qAbs(facet.vector[0].z - facet.vector[1].z);
	    float max_diff = qMax(diff_x, diff_y);
	    max_diff = qMax(diff_z, max_diff);
	    stats_.shortest_edge = max_diff;

      first = 0;
    }
    // Now find the max and min values
    stats_.max.x = qMax(stats_.max.x, facet.vector[0].x);
    stats_.min.x = qMin(stats_.min.x, facet.vector[0].x);
    stats_.max.y = qMax(stats_.max.y, facet.vector[0].y);
    stats_.min.y = qMin(stats_.min.y, facet.vector[0].y);
    stats_.max.z = qMax(stats_.max.z, facet.vector[0].z);
    stats_.min.z = qMin(stats_.min.z, facet.vector[0].z);

    stats_.max.x = qMax(stats_.max.x, facet.vector[1].x);
    stats_.min.x = qMin(stats_.min.x, facet.vector[1].x);
    stats_.max.y = qMax(stats_.max.y, facet.vector[1].y);
    stats_.min.y = qMin(stats_.min.y, facet.vector[1].y);
    stats_.max.z = qMax(stats_.max.z, facet.vector[1].z);
    stats_.min.z = qMin(stats_.min.z, facet.vector[1].z);

    stats_.max.x = qMax(stats_.max.x, facet.vector[2].x);
    stats_.min.x = qMin(stats_.min.x, facet.vector[2].x);
    stats_.max.y = qMax(stats_.max.y, facet.vector[2].y);
    stats_.min.y = qMin(stats_.min.y, facet.vector[2].y);
    stats_.max.z = qMax(stats_.max.z, facet.vector[2].z);
    stats_.min.z = qMin(stats_.min.z, facet.vector[2].z);
  }
  stats_.size.x = stats_.max.x - stats_.min.x;
  stats_.size.y = stats_.max.y - stats_.min.y;
  stats_.size.z = stats_.max.z - stats_.min.z;
  stats_.bounding_diameter =  sqrt(stats_.size.x * stats_.size.x + stats_.size.y * stats_.size.y + stats_.size.z * stats_.size.z);
  stats_.num_points = getNumPoints();
  stats_.surface = getSurface();
  stats_.volume = getVolume();
}


void Entity::close() {
  if (facets_ != 0) {
	  delete[] facets_;
    facets_ = 0;
  }
}

int Entity::readIntFromBytes(::std::ifstream& file) {
  int value;
  value  =  file.get() & 0xFF;
  value |= (file.get() & 0xFF) << 0x08;
  value |= (file.get() & 0xFF) << 0x10;
  value |= (file.get() & 0xFF) << 0x18;
  return(value);
}

float Entity::readFloatFromBytes(::std::ifstream& file) {
  union {
    int int_value;
    float float_value;
  } value;
  
  value.int_value  =  file.get() & 0xFF;
  value.int_value |= (file.get() & 0xFF) << 0x08;
  value.int_value |= (file.get() & 0xFF) << 0x10;
  value.int_value |= (file.get() & 0xFF) << 0x18;
  return(value.float_value);
}

void Entity::writeBytesFromInt(::std::ofstream& file, int value_in) {
  union {
    int int_value;
    char char_value[4];
  } value;
  value.int_value = value_in;
  int new_value  = value.char_value[0] & 0xFF;
  new_value |= (value.char_value[1] & 0xFF) << 0x08;
  new_value |= (value.char_value[2] & 0xFF) << 0x10;
  new_value |= (value.char_value[3] & 0xFF) << 0x18;
  file.write(reinterpret_cast<char *>(&new_value), sizeof(new_value));
}

void Entity::writeBytesFromFloat(::std::ofstream& file, float value_in) {
  union {
    float float_value;
    char char_value[4];
  } value;
  value.float_value = value_in;
  int new_value  = value.char_value[0] & 0xFF;
  new_value |= (value.char_value[1] & 0xFF) << 0x08;
  new_value |= (value.char_value[2] & 0xFF) << 0x10;
  new_value |= (value.char_value[3] & 0xFF) << 0x18;
  file.write(reinterpret_cast<char *>(&new_value), sizeof(new_value));
}

void Entity::writeBinary(const ::std::string& file_name) {

  // Open the file
  ::std::ofstream file(file_name.c_str(), ::std::ios::out|::std::ios::binary);
  if (file.is_open()) {
    
    for (int i = 0; i < JUNK_SIZE; i++) file.put(0);

    writeBytesFromInt(file, stats_.num_facets);
  
    for(int i = 0; i < stats_.num_facets; i++) {
      writeBytesFromFloat(file, facets_[i].normal.x);
      writeBytesFromFloat(file, facets_[i].normal.y);
      writeBytesFromFloat(file, facets_[i].normal.z);
      writeBytesFromFloat(file, facets_[i].vector[0].x);
      writeBytesFromFloat(file, facets_[i].vector[0].y);
      writeBytesFromFloat(file, facets_[i].vector[0].z);
      writeBytesFromFloat(file, facets_[i].vector[1].x);
      writeBytesFromFloat(file, facets_[i].vector[1].y);
      writeBytesFromFloat(file, facets_[i].vector[1].z);
      writeBytesFromFloat(file, facets_[i].vector[2].x);
      writeBytesFromFloat(file, facets_[i].vector[2].y);
      writeBytesFromFloat(file, facets_[i].vector[2].z);
      file << facets_[i].extra[0];
      file << facets_[i].extra[1];
    }
  
    file.close();
  } else {
    exit(1);
  }
}

void Entity::writeAscii(const ::std::string& file_name) {

  // Open the file
  ::std::ofstream file(file_name.c_str(), ::std::ios::out);
  file.setf(::std::ios::scientific);
  file.precision(8);
  if (file.is_open()) {
    
    file << "solid" << ::std::endl;

    for(int i = 0; i < stats_.num_facets; i++) {
      file << "  facet normal " << facets_[i].normal.x << " " << facets_[i].normal.y << " " << facets_[i].normal.z << ::std::endl;
      file << "    outer loop " << ::std::endl;
      file << "      Vector " << facets_[i].vector[0].x << " " << facets_[i].vector[0].y << " " << facets_[i].vector[0].z << ::std::endl;
      file << "      Vector " << facets_[i].vector[1].x << " " << facets_[i].vector[1].y << " " << facets_[i].vector[1].z << ::std::endl;
      file << "      Vector " << facets_[i].vector[2].x << " " << facets_[i].vector[2].y << " " << facets_[i].vector[2].z << ::std::endl;
      file << "    endloop" << ::std::endl;
      file << "  endfacet" << ::std::endl;
    }
  
    file << "endsolid" << ::std::endl;

    file.close();
  } else {
    exit(1);
  }
}

int Entity::getNumPoints() {
  ::std::vector<Vector> vectors;
  for(int i = 0; i < stats_.num_facets; i++) {
    for(int j = 0; j < 3; j++) {
      vectors.push_back(facets_[i].vector[j]);
    }
  }
  ::std::sort(vectors.begin(), vectors.end());
  vectors.erase(::std::unique(vectors.begin(), vectors.end()), vectors.end());
  return vectors.size();
}

float Entity::getVolume() {
  Vector p0;
  Vector p;
  float volume = 0.0;

  // Choose a point, any point as the reference
  p0.x = facets_[0].vector[0].x;
  p0.y = facets_[0].vector[0].y;
  p0.z = facets_[0].vector[0].z;

  for(int i = 0; i < stats_.num_facets; i++) {
    p.x = facets_[i].vector[0].x - p0.x;
    p.y = facets_[i].vector[0].y - p0.y;
    p.z = facets_[i].vector[0].z - p0.z;
    // Do dot product to get distance from point to plane
    Normal n = facets_[i].normal;
    float height = (n.x * p.x) + (n.y * p.y) + (n.z * p.z);
    float area = getArea(&facets_[i]);
    volume += (area * height) / 3.0;
  }
	if(volume < 0.0) {
		volume = -volume;
	}
  return volume;
}

float Entity::getSurface() {
  float surface = 0.0;

  for(int i = 0; i < stats_.num_facets; i++) {
    float area = getArea(&facets_[i]);
    surface += area;
  }
	if(surface < 0.0) {
		surface = -surface;
	}
  return surface;
}

float Entity::getArea(Facet *facet) {
	float cross[3][3];
	float sum[3];
	float n[3];
	
	for(int i = 0; i < 3; i++) {
    cross[i][0]=((facet->vector[i].y * facet->vector[(i + 1) % 3].z) -
    (facet->vector[i].z * facet->vector[(i + 1) % 3].y));
    cross[i][1]=((facet->vector[i].z * facet->vector[(i + 1) % 3].x) -
    (facet->vector[i].x * facet->vector[(i + 1) % 3].z));
    cross[i][2]=((facet->vector[i].x * facet->vector[(i + 1) % 3].y) -
    (facet->vector[i].y * facet->vector[(i + 1) % 3].x));
	}
	
	sum[0] = cross[0][0] + cross[1][0] + cross[2][0];
	sum[1] = cross[0][1] + cross[1][1] + cross[2][1];
	sum[2] = cross[0][2] + cross[1][2] + cross[2][2];

	// This should already be done.  But just in case, let's do it again
	calculateNormal(n, facet);
	normalizeVector(n);

	float area = 0.5 * (n[0] * sum[0] + n[1] * sum[1] + n[2] * sum[2]);
	return area;
}

void Entity::calculateNormal(float normal[], Facet *facet) {
  float v1[3];
  float v2[3];

  v1[0] = facet->vector[1].x - facet->vector[0].x;
  v1[1] = facet->vector[1].y - facet->vector[0].y;
  v1[2] = facet->vector[1].z - facet->vector[0].z;
  v2[0] = facet->vector[2].x - facet->vector[0].x;
  v2[1] = facet->vector[2].y - facet->vector[0].y;
  v2[2] = facet->vector[2].z - facet->vector[0].z;

  normal[0] = (float)((double)v1[1] * (double)v2[2]) - ((double)v1[2] * (double)v2[1]);
  normal[1] = (float)((double)v1[2] * (double)v2[0]) - ((double)v1[0] * (double)v2[2]);
  normal[2] = (float)((double)v1[0] * (double)v2[1]) - ((double)v1[1] * (double)v2[0]);
}

void Entity::normalizeVector(float v[]) {
  double length = sqrt((double)v[0] * (double)v[0] + (double)v[1] * (double)v[1] + (double)v[2] * (double)v[2]);
  float min_normal_length = 0.000000000001;
  if(length < min_normal_length) {
    v[0] = 1.0;
    v[1] = 0.0;
    v[2] = 0.0;
    return;
  }  
  double factor = 1.0 / length;
  v[0] *= factor;
  v[1] *= factor;
  v[2] *= factor;
}