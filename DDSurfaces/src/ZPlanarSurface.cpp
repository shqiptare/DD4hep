#define OBSOLETE
#ifndef OBSOLETE
/*
 * ZPlanarSurface.cpp
 *
 *  Created on: Mar 7, 2014
 *      Author: cgrefe
 */

#include "DDSurfaces/ZPlanarSurface.h"

namespace DDSurfaces {

ZPlanarSurface::ZPlanarSurface() {
	// TODO Auto-generated constructor stub

}

ZPlanarSurface::~ZPlanarSurface() {
	// TODO Auto-generated destructor stub
}

/// Checks if the given point lies within the surface
bool ZPlanarSurface::isInsideBoundaries(const Vector3D& point) const {
	// TODO
	return false;
}

/// Access to the normal direction at the given point
Vector3D ZPlanarSurface::getNormal(const Vector3D& point) const {
	// TODO
	return Vector3D();

}

/// Access to the measurement directions at the given point
MeasurementDirections ZPlanarSurface::measurement(const Vector3D& point) const {
	// TODO
	return MeasurementDirections();
}

} /* namespace DDSurfaces */

#endif
