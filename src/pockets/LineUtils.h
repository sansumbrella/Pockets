

#include "Pockets.h"
#include <vector>

namespace cinder { namespace pockets
{

//! returns a vector of points and control points suitable for constructing
//! a ci::BSpline. Accepts a collection of either Vec2fs or Vec3fs.
//! e.g. BSpline3f spline( curveThrough( points ), 3, false, true );
template <typename T>
std::vector<T> curveThrough( const std::vector<T> &points );

}} // ci::su
