#include "MyNXSupportLib.h"

using namespace NXOpen;
using namespace std;

#define DELTA_L 20
#define DELTA_W 10
#define DELTA_Z 1

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	MyNXSupportLib::Init();
	MyNXSupportLib::AssumeBasePart();

	Unit *unit_mm = MyNXSupportLib::GetUnit("MilliMeter");

	MyNXSupportLib::SetDrawParameters(unit_mm, DELTA_L, DELTA_W, DELTA_Z);

	double max_z = 10.0;
	
	/* buis 1 (1 keer met kleine R, 1 keer met grote R)
	 *	L: -160 ... 160
	 *  R:  135, 117
	 *  w:  0 ... 360
	 */
	// x = L
	// y = R * cos(w)
	// z = R * sin(w)
	double R1 = 135;
	double R2 = 117;
	auto buis1_outer_points = MyNXSupportLib::DrawCylinder(MyNXSupportLib::Axis::X, R1, -160, 160);
	auto buis1_inner_points = MyNXSupportLib::DrawCylinder(MyNXSupportLib::Axis::X, R2, -160, 160);

	/* buis 2 (1 keer met kleine R, 1 keer met grote R)
	 *	L: 0 ... 55
	 *  r: 63.5, 45.5
	 *  w: 0 ... 360
	 */
	// x = r * cos(w)
	// y = r * sin(w)
	// z = L
	double r1 = 63.5;
	double r2 = 45.5;
	auto buis2_outer_points = MyNXSupportLib::DrawCylinder(MyNXSupportLib::Axis::Z, r1, max_z + R1, 55 + R1);
	auto buis2_inner_points = MyNXSupportLib::DrawCylinder(MyNXSupportLib::Axis::Z, r2, max_z + R1, 55 + R1); 

	/* snijpunt
	 *  - R = 135, r = 63.5
	 *  - R = 135, r = 45.5
	 *  w: 0 ... 360
	 */
	// x = r * cos(w)
	// y = r * sin(w)
	// z = +- sqrt[ R^2 - r^2 * sin(w)^2 ]
	auto connection_outer_points = MyNXSupportLib::DrawConnectionOnCylinder(R1, r1, max_z);
	auto connection_inner_points = MyNXSupportLib::DrawConnectionOnCylinder(R1, r2, max_z); 

	/* connection aan buis2 plakken */
	MyNXSupportLib::ConnectPointsAndFillSingle(buis2_outer_points[0], connection_outer_points[connection_outer_points.size()-1]);
	MyNXSupportLib::ConnectPointsAndFillSingle(buis2_inner_points[0], connection_inner_points[connection_inner_points.size()-1]); 

	/* connection: zijvlakken en beginvlakken */
	MyNXSupportLib::ConnectPointsAndFill(connection_inner_points);
	MyNXSupportLib::ConnectPointsAndFill(connection_outer_points);
	MyNXSupportLib::ConnectPointsAndFillStart(connection_outer_points, connection_inner_points);
	MyNXSupportLib::ConnectPointsAndFillEnd(connection_outer_points, connection_inner_points);

	/* buis1: cilinder en begin- en eindvlakken */
	MyNXSupportLib::ConnectPointsAndFill(buis1_inner_points);
	MyNXSupportLib::ConnectPointsAndFill(buis1_outer_points);
	MyNXSupportLib::ConnectPointsAndFillStart(buis1_inner_points, buis1_outer_points);
	MyNXSupportLib::ConnectPointsAndFillEnd(buis1_inner_points, buis1_outer_points);
	
	/* buis2: cilinder en begin- en eindvlakken */
	MyNXSupportLib::ConnectPointsAndFill(buis2_inner_points);
	MyNXSupportLib::ConnectPointsAndFill(buis2_outer_points);
	MyNXSupportLib::ConnectPointsAndFillStart(buis2_inner_points, buis2_outer_points);
	MyNXSupportLib::ConnectPointsAndFillEnd(buis2_inner_points, buis2_outer_points); 
}

/* Unload Handler
**     This function specifies when to unload your application from Unigraphics.
**     If your application registers a callback (from a MenuScript item or a
**     User Defined Object for example), this function MUST return
**     "Session::LibraryUnloadOptionImmediately". */
extern "C" DllExport int ufusr_ask_unload()
{
  return (int)Session::LibraryUnloadOptionImmediately;
}
