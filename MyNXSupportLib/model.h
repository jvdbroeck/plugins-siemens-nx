#ifndef MYNXSUPPORTLIB_MODEL_H
#define MYNXSUPPORTLIB_MODEL_H

#include <string>
#include <vector>

#include <NXOpen/Point.hxx>
#include <NXOpen/Unit.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/Features_FillHole.hxx>

namespace MyNXSupportLib {

enum class Axis { X, Y, Z };

typedef std::vector<NXOpen::Point *> GeometricPointsElement;
typedef std::vector<GeometricPointsElement> GeometricPoints;
typedef std::vector<NXOpen::Features::FillHole *> SurfaceListElement;
typedef std::vector<SurfaceListElement> SurfaceList;

void SetDrawParameters(NXOpen::Unit *unit, double delta_l, double delta_w, double delta_z);

GeometricPointsElement DrawCircle(Axis ax, double R, double pos);
GeometricPoints DrawCylinder(Axis ax, double R, double Lmin, double Lmax);
GeometricPoints DrawConnectionOnCylinder(double R, double r, double max_z, bool draw_max_z = false, double z_start = 0.0);
GeometricPoints DrawConnectionOnCylinderSkewed(double R, double r, double r_offset, double max_z);
GeometricPoints DrawConnectionOnCylinderSkewedInner(double R, double ri, double ro, double max_z, bool draw_max_z);

SurfaceListElement ConnectPointsAndFillSingle(GeometricPointsElement pts1, GeometricPointsElement pts2);
SurfaceList ConnectPointsAndFill(GeometricPoints all_points);
SurfaceListElement ConnectPointsAndFillStart(GeometricPoints all_points_1, GeometricPoints all_points_2);
SurfaceListElement ConnectPointsAndFillEnd(GeometricPoints all_points_1, GeometricPoints all_points_2);
SurfaceList ConnectPointsAndFill2(GeometricPoints all_points, bool circular = true);

void SewSurfaces(SurfaceList surfaces, bool all_together);

}

#endif
