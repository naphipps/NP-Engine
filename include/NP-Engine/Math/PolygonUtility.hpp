//
//  PolygonUtility.hpp
//  Project Space
//
//  Created by Nathan Phipps on 1/13/19.
//  Copyright © 2019 Nathan Phipps. All rights reserved.
//

#ifndef PolygonUtility_hpp
#define PolygonUtility_hpp

#include <limits>
#include <cmath>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <set>
#include <stdexcept>
#include "NP-Engine/Polygon.hpp"
#include "NP-Engine/Point.hpp"

// TODO: refine this file and cpp

#define POLYGON_UTILITY_VERTEXTYPE_REGULAR 0
#define POLYGON_UTILITY_VERTEXTYPE_START 1
#define POLYGON_UTILITY_VERTEXTYPE_END 2
#define POLYGON_UTILITY_VERTEXTYPE_SPLIT 3
#define POLYGON_UTILITY_VERTEXTYPE_MERGE 4

//---------------------------------------------------------------------------
// PolygonUtility
//---------------------------------------------------------------------------

class PolygonUtility
{
public:
	static np::math::i32PointVector Simplify(const np::math::i32PointVector& polygon);
	static np::math::fltPointVector Simplify(const np::math::fltPointVector& polygon);

	//---------------------------------------------------------------------------
	// PolygonPartitioner
	//---------------------------------------------------------------------------

	class PolygonPartitioner
	{
	protected:
		struct PartitionVertex
		{
			bool isActive;
			bool isConvex;
			bool isEar;

			np::math::fltPoint p;
			float angle;
			PartitionVertex* previous;
			PartitionVertex* next;

			PartitionVertex();
		};

		struct MonotoneVertex
		{
			np::math::fltPoint p;
			long previous;
			long next;
		};

		class VertexSorter
		{
			MonotoneVertex* vertices;

		public:
			VertexSorter(MonotoneVertex* v): vertices(v) {}
			bool operator()(long index1, long index2);
		};

		struct Diagonal
		{
			long index1;
			long index2;
		};

		typedef std::vector<Diagonal> DiagonalList;

		// dynamic programming state for minimum-weight triangulation
		struct DPState
		{
			bool visible;
			float weight;
			long bestvertex;
		};

		// dynamic programming state for convex partitioning
		struct DPState2
		{
			bool visible;
			long weight;
			DiagonalList pairs;
		};

		// edge that intersects the scanline
		struct ScanLineEdge
		{
			mutable long index;
			np::math::fltPoint p1;
			np::math::fltPoint p2;

			// determines if the edge is to the left of another edge
			bool operator<(const ScanLineEdge& other) const;

			bool IsConvex(const np::math::fltPoint& p1, const np::math::fltPoint& p2, const np::math::fltPoint& p3) const;
		};

		// standard helper functions
		bool IsConvex(np::math::fltPoint& p1, np::math::fltPoint& p2, np::math::fltPoint& p3);
		bool IsReflex(np::math::fltPoint& p1, np::math::fltPoint& p2, np::math::fltPoint& p3);
		bool IsInside(np::math::fltPoint& p1, np::math::fltPoint& p2, np::math::fltPoint& p3, np::math::fltPoint& p);

		bool InCone(np::math::fltPoint& p1, np::math::fltPoint& p2, np::math::fltPoint& p3, np::math::fltPoint& p);
		bool InCone(PartitionVertex* v, np::math::fltPoint& p);

		int Intersects(np::math::fltPoint& p11, np::math::fltPoint& p12, np::math::fltPoint& p21, np::math::fltPoint& p22);

		np::math::fltPoint Normalize(const np::math::fltPoint& p);
		float Distance(const np::math::fltPoint& p1, const np::math::fltPoint& p2);

		// helper functions for Triangulate_EC
		void UpdateVertexReflexity(PartitionVertex* v);
		void UpdateVertex(PartitionVertex* v, PartitionVertex* vertices, long numvertices);

		// helper functions for ConvexPartition_OPT
		void UpdateState(long a, long b, long w, long i, long j, DPState2** dpstates);
		void TypeA(long i, long j, long k, PartitionVertex* vertices, DPState2** dpstates);
		void TypeB(long i, long j, long k, PartitionVertex* vertices, DPState2** dpstates);

		// helper functions for MonotonePartition
		bool Below(np::math::fltPoint& p1, np::math::fltPoint& p2);
		void AddDiagonal(MonotoneVertex* vertices, long* numvertices, long index1, long index2, char* vertextypes,
						 std::set<ScanLineEdge>::iterator* edgeTreeIterators, std::set<ScanLineEdge>* edgeTree, long* helpers);

		// triangulates a monotone polygon, used in Triangulate_MONO
		int TriangulateMonotone(Polygon* inPoly, PolygonVector* triangles);

	public:
		// triangulates a polygons by firstly partitioning it into monotone polygons
		// time complexity: O(n*log(n)), n is the number of vertices
		// space complexity: O(n)
		// params:
		//    poly : an input polygon to be triangulated
		//           vertices have to be in counter-clockwise order
		//    triangles : a list of triangles (result)
		// returns 1 on success, 0 on failure
		int Triangulate_MONO(Polygon* poly, PolygonVector* triangles);

		// triangulates a list of polygons by firstly partitioning them into monotone polygons
		// time complexity: O(n*log(n)), n is the number of vertices
		// space complexity: O(n)
		// params:
		//    inpolys : a list of polygons to be triangulated (can contain holes)
		//              vertices of all non-hole polys have to be in counter-clockwise order
		//              vertices of all hole polys have to be in clockwise order
		//    triangles : a list of triangles (result)
		// returns 1 on success, 0 on failure
		int Triangulate_MONO(PolygonVector* inpolys, PolygonVector* triangles);

		// creates a monotone partition of a list of polygons that can contain holes
		// time complexity: O(n*log(n)), n is the number of vertices
		// space complexity: O(n)
		// params:
		//    inpolys : a list of polygons to be triangulated (can contain holes)
		//              vertices of all non-hole polys have to be in counter-clockwise order
		//              vertices of all hole polys have to be in clockwise order
		//    monotonePolys : a list of monotone polygons (result)
		// returns 1 on success, 0 on failure
		int MonotonePartition(PolygonVector* inpolys, PolygonVector* monotonePolys);

		// simple heuristic procedure for removing holes from a list of polygons
		// works by creating a diagonal from the rightmost hole vertex to some visible vertex
		// time complexity: O(h*(n^2)), h is the number of holes, n is the number of vertices
		// space complexity: O(n)
		// params:
		//    inpolys : a list of polygons that can contain holes
		//              vertices of all non-hole polys have to be in counter-clockwise order
		//              vertices of all hole polys have to be in clockwise order
		//    outpolys : a list of polygons without holes
		// returns 1 on success, 0 on failure
		int RemoveHoles(PolygonVector* inpolys, PolygonVector* outpolys);

		// triangulates a polygon by ear clipping
		// time complexity O(n^2), n is the number of vertices
		// space complexity: O(n)
		// params:
		//    poly : an input polygon to be triangulated
		//           vertices have to be in counter-clockwise order
		//    triangles : a list of triangles (result)
		// returns 1 on success, 0 on failure
		int Triangulate_EC(Polygon* poly, PolygonVector* triangles);

		// triangulates a list of polygons that may contain holes by ear clipping algorithm
		// first calls RemoveHoles to get rid of the holes, and then Triangulate_EC for each resulting polygon
		// time complexity: O(h*(n^2)), h is the number of holes, n is the number of vertices
		// space complexity: O(n)
		// params:
		//    inpolys : a list of polygons to be triangulated (can contain holes)
		//              vertices of all non-hole polys have to be in counter-clockwise order
		//              vertices of all hole polys have to be in clockwise order
		//    triangles : a list of triangles (result)
		// returns 1 on success, 0 on failure
		int Triangulate_EC(PolygonVector* inpolys, PolygonVector* triangles);

		// creates an optimal polygon triangulation in terms of minimal edge length
		// time complexity: O(n^3), n is the number of vertices
		// space complexity: O(n^2)
		// params:
		//    poly : an input polygon to be triangulated
		//           vertices have to be in counter-clockwise order
		//    triangles : a list of triangles (result)
		// returns 1 on success, 0 on failure
		int Triangulate_OPT(Polygon* poly, PolygonVector* triangles);

		// partitions a polygon into convex polygons by using Hertel-Mehlhorn algorithm
		// the algorithm gives at most four times the number of parts as the optimal algorithm
		// however, in practice it works much better than that and often gives optimal partition
		// uses triangulation obtained by ear clipping as intermediate result
		// time complexity O(n^2), n is the number of vertices
		// space complexity: O(n)
		// params:
		//    poly : an input polygon to be partitioned
		//           vertices have to be in counter-clockwise order
		//    parts : resulting list of convex polygons
		// returns 1 on success, 0 on failure
		int ConvexPartition_HM(Polygon* poly, PolygonVector* parts);

		// partitions a list of polygons into convex parts by using Hertel-Mehlhorn algorithm
		// the algorithm gives at most four times the number of parts as the optimal algorithm
		// however, in practice it works much better than that and often gives optimal partition
		// uses triangulation obtained by ear clipping as intermediate result
		// time complexity O(n^2), n is the number of vertices
		// space complexity: O(n)
		// params:
		//    inpolys : an input list of polygons to be partitioned
		//              vertices of all non-hole polys have to be in counter-clockwise order
		//              vertices of all hole polys have to be in clockwise order
		//    parts : resulting list of convex polygons
		// returns 1 on success, 0 on failure
		int ConvexPartition_HM(PolygonVector* inpolys, PolygonVector* parts);

		// optimal convex partitioning (in terms of number of resulting convex polygons)
		// using the Keil-Snoeyink algorithm
		// M. Keil, J. Snoeyink, "On the time bound for convex decomposition of simple polygons", 1998
		// time complexity O(n^3), n is the number of vertices
		// space complexity: O(n^3)
		//    poly : an input polygon to be partitioned
		//           vertices have to be in counter-clockwise order
		//    parts : resulting list of convex polygons
		// returns 1 on success, 0 on failure
		int ConvexPartition_OPT(Polygon* poly, PolygonVector* parts);
	};
};

#endif /* PolygonUtility_hpp */
