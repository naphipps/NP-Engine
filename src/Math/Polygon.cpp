//
//  Polygon.cpp
//  Project Space
//
//  Created by Nathan Phipps on 1/17/19.
//  Copyright © 2019 Nathan Phipps. All rights reserved.
//

#include "Polygon.hpp"

Polygon::Polygon() {
    hole = false;
    numpoints = 0;
    points = nullptr;
}

Polygon::Polygon(const np::math::fltPointVector &pointList) : Polygon() {
    this->Init(pointList.size());
    std::copy(pointList.begin(), pointList.end(), points);
}

Polygon::Polygon(const Polygon &src) : Polygon() {
    hole = src.hole;
    numpoints = src.numpoints;
    
    if(numpoints > 0) {
        points = new np::math::fltPoint[numpoints];
        memcpy(points, src.points, numpoints*sizeof(np::math::fltPoint));
    }
}

Polygon& Polygon::operator=(const Polygon &src) {
    Clear();
    hole = src.hole;
    numpoints = src.numpoints;
    
    if(numpoints > 0) {
        points = new np::math::fltPoint[numpoints];
        memcpy(points, src.points, numpoints*sizeof(np::math::fltPoint));
    }
    
    return *this;
}

Polygon::~Polygon() {
    if(points) delete [] points;
}

void Polygon::Clear() {
    if(points) delete [] points;
    hole = false;
    numpoints = 0;
    points = nullptr;
}

void Polygon::Init(long numpoints) {
    Clear();
    this->numpoints = numpoints;
    points = new np::math::fltPoint[numpoints];
}

void Polygon::Triangle(np::math::fltPoint &p1, np::math::fltPoint &p2, np::math::fltPoint &p3) {
    Init(3);
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
}

int Polygon::GetOrientation() const {
    long i1,i2;
    float area = 0;
    for(i1=0; i1<numpoints; i1++) {
        i2 = i1+1;
        if(i2 == numpoints) i2 = 0;
        area += points[i1].x * points[i2].y - points[i1].y * points[i2].x;
    }
    
    if(area > 0){
        return POLYGON_CCW;
    }
    
    if(area < 0){
        return POLYGON_CW;
    }
    
    return 0;
}

void Polygon::SetOrientation(int orientation) {
    int polyorientation = GetOrientation();
    if(polyorientation && (polyorientation != orientation)) {
        Invert();
    }
}

void Polygon::Invert() {
    std::reverse(points, points + numpoints);
}
