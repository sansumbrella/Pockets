

#include "LineUtils.h"
#include "cinder/Vector.h"

using namespace cinder;
using namespace std;

template <typename T>
vector<T> pockets::curveThrough(const vector<T> &points)
{
	assert( points.size() > 2 );
	vector<T> curvePoints;

	T p1 = points.at( 0 );
	T p2 = points.at( 1 );
	T p3 = points.at( 2 );

	T b1 = p1 + (p2 - p1) / 6.0f;
	T b2 = p2 - (p3 - p1) / 6.0f;

	curvePoints.push_back( p1 );
	curvePoints.push_back( b1 );
	curvePoints.push_back( b2 );
	curvePoints.push_back( p2 );

	for( int i = 1; i < points.size() - 2; ++i )
	{
		T p0 = points.at( i - 1 );
		T p1 = points.at( i );
		T p2 = points.at( i + 1 );
		T p3 = points.at( i + 2 );

		T b1 = p1 + (p2 - p0) / 6.0f;
		T b2 = p2 - (p3 - p1) / 6.0f;

		curvePoints.push_back( p1 );
		curvePoints.push_back( b1 );
		curvePoints.push_back( b2 );
		curvePoints.push_back( p2 );
	}

	size_t i2 = points.size() - 1;
	size_t i1 = i2 - 1;
	size_t i0 = i2 - 2;
	T p0 = points.at( i0 );
	p1 = points.at( i1 );
	p2 = points.at( i2 );

	b1 = p1 + (p2 - p0) / 6.0f;
	b2 = p2 - (p2 - p1) / 6.0f;
	curvePoints.push_back( p1 );
	curvePoints.push_back( b1 );
	curvePoints.push_back( b2 );
	curvePoints.push_back( p2 );

	return curvePoints;
}

template vector<Vec2f> pockets::curveThrough<Vec2f>(const vector<Vec2f> &points);
template vector<Vec3f> pockets::curveThrough<Vec3f>(const vector<Vec3f> &points);