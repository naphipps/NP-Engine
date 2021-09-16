//
//  Polygon.hpp
//  Project Space
//
//  Created by Nathan Phipps on 1/17/19.
//  Copyright © 2019 Nathan Phipps. All rights reserved.
//

#ifndef Polygon_hpp
#define Polygon_hpp

//TODO: refine this file and cpp

#include "Point.hpp"

#define POLYGON_CCW 1
#define POLYGON_CW -1

//---------------------------------------------------------------------------
//Polygon
//---------------------------------------------------------------------------

class Polygon {
protected:
    
    ::np::math::fltPoint *points;
    long numpoints;
    bool hole;
    
public:
    
    //constructors/destructors
    Polygon();
    ~Polygon();
    
    Polygon(const Polygon &src);
    Polygon(const ::np::math::fltPointVector &pointList);
    
    //operator
    Polygon& operator=(const Polygon &src);
    
    //getters and setters
    long GetNumPoints() const {
        return numpoints;
    }
    
    bool IsHole() const {
        return hole;
    }
    
    void SetHole(bool hole) {
        bool prevHole = IsHole();
        if (prevHole != hole){
            if (hole){
                SetOrientation(POLYGON_CW);
            }
            else {
                SetOrientation(POLYGON_CCW);
            }
        }
        this->hole = hole;
    }
    
    ::np::math::fltPoint &GetPoint(long i) {
        return points[i];
    }
    
    const ::np::math::fltPoint &GetPoint(long i) const {
        return points[i];
    }
    
    ::np::math::fltPoint *GetPoints() {
        return points;
    }
    
    ::np::math::fltPoint& operator[] (int i) {
        return points[i];
    }
    
    const ::np::math::fltPoint& operator[] (int i) const {
        return points[i];
    }
    
    //clears the polygon points
    void Clear();
    
    //inits the polygon with numpoints vertices
    void Init(long numpoints);
    
    //creates a triangle with points p1,p2,p3
    void Triangle(::np::math::fltPoint &p1, ::np::math::fltPoint &p2, ::np::math::fltPoint &p3);
    
    //inverts the orfer of vertices
    void Invert();
    
    //returns the orientation of the polygon
    //possible values:
    //   POLYGON_CCW : polygon vertices are in counter-clockwise order
    //   POLYGON_CW : polygon vertices are in clockwise order
    //       0 : the polygon has no (measurable) area
    int GetOrientation() const;
    
    //sets the polygon orientation
    //orientation can be
    //   POLYGON_CCW : sets vertices in counter-clockwise order
    //   POLYGON_CW : sets vertices in clockwise order
    void SetOrientation(int orientation);
    
    //checks whether a polygon is valid or not
    inline bool Valid() const { return this->numpoints >= 3; }
};

typedef std::vector<Polygon> PolygonVector;
typedef std::vector<PolygonVector> PolygonVectorVector;

#endif /* Polygon_hpp */
