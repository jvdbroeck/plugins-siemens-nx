#include "MyNXSupportLib.h"

using namespace NXOpen;
using namespace std;

#define DELTA_L 20
#define DELTA_W 10
#define DELTA_Z 1

#define TEKEN_BUIS1		1
#define TEKEN_BUIS2		1
#define TEKEN_LASTEEN	1

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	MyNXSupportLib::Init();
	MyNXSupportLib::AssumeBasePart();

	Unit *unit_mm = MyNXSupportLib::GetUnit("MilliMeter");

	MyNXSupportLib::SetDrawParameters(unit_mm, DELTA_L, DELTA_W, DELTA_Z);

	double max_z = 16.0;

	/* BUIS 1 */
	double R1 = 135;
	double R2 = 117;
	
#if TEKEN_BUIS1
	MyNXSupportLib::SurfaceList buis1_vlakken;

	// binnenkant
	auto buis1_inner = MyNXSupportLib::DrawCylinder(MyNXSupportLib::Axis::X, R2, -160, 160);
	auto buis1_inner_vlakken = MyNXSupportLib::ConnectPointsAndFill(buis1_inner);
	buis1_vlakken.insert(buis1_vlakken.begin(), buis1_inner_vlakken.begin(), buis1_inner_vlakken.end());

	// buitenkant
	auto buis1_outer = MyNXSupportLib::DrawCylinder(MyNXSupportLib::Axis::X, R1, -160, 160);
	auto buis1_outer_vlakken = MyNXSupportLib::ConnectPointsAndFill(buis1_outer);
	buis1_vlakken.insert(buis1_vlakken.begin(), buis1_outer_vlakken.begin(), buis1_outer_vlakken.end());

	// eindvlakken
	auto buis1_vlakken_end1 = MyNXSupportLib::ConnectPointsAndFillStart(buis1_outer, buis1_inner);
	buis1_vlakken.push_back(buis1_vlakken_end1);

	auto buis1_vlakken_end2 = MyNXSupportLib::ConnectPointsAndFillEnd(buis1_outer, buis1_inner);
	buis1_vlakken.push_back(buis1_vlakken_end2);

	MyNXSupportLib::SewSurfaces(buis1_vlakken, true);
#endif

	/* BUIS 2 */
	double r1 = 63.5;
	double r2 = 45.5;
	
#if TEKEN_BUIS2
	MyNXSupportLib::SurfaceList buis2_vlakken;

	// binnenkant, deel 1
	auto buis2_inner = MyNXSupportLib::DrawCylinder(MyNXSupportLib::Axis::Z, r2, max_z + R1, 55 + R1);
	auto buis2_inner_vlakken = MyNXSupportLib::ConnectPointsAndFill(buis2_inner);
	buis2_vlakken.insert(buis2_vlakken.begin(), buis2_inner_vlakken.begin(), buis2_inner_vlakken.end());

	// binnenkant, deel 2
	auto buis2_inner_connection = MyNXSupportLib::DrawConnectionOnCylinder(R1, r2, max_z);
	auto buis2_inner_connection_vlakken = MyNXSupportLib::ConnectPointsAndFill(buis2_inner_connection);
	buis2_vlakken.insert(buis2_vlakken.begin(), buis2_inner_connection_vlakken.begin(), buis2_inner_connection_vlakken.end());

	// binnenkant: deel 1 verbinden met deel 2
	auto buis2_verbinding_vlakken = MyNXSupportLib::ConnectPointsAndFillSingle(buis2_inner_connection[buis2_inner_connection.size()-1], buis2_inner[0]);
	buis2_vlakken.push_back(buis2_verbinding_vlakken);

	// buitenkant, deel 1
	auto buis2_outer = MyNXSupportLib::DrawCylinder(MyNXSupportLib::Axis::Z, r1, max_z + R1, 55 + R1);
	auto buis2_outer_vlakken = MyNXSupportLib::ConnectPointsAndFill(buis2_outer);
	buis2_vlakken.insert(buis2_vlakken.begin(), buis2_outer_vlakken.begin(), buis2_outer_vlakken.end());

	// buitenkant, deel 2
	//auto buis2_outer_connection = MyNXSupportLib::DrawConnectionOnCylinderSkewedInner(R1, r2, r1, max_z, false);
	//MyNXSupportLib::ConnectPointsAndFill(buis2_outer_connection);
	auto buis2_outer_connection = MyNXSupportLib::DrawConnectionOnCylinder(R1, r2, 0.0, true);

	// buitenkant: deel 1 verbinden met deel 2
	auto buis2_verbinding_vlakken2 = MyNXSupportLib::ConnectPointsAndFillSingle(buis2_outer_connection[buis2_outer_connection.size()-1], buis2_outer[0]);
	buis2_vlakken.push_back(buis2_verbinding_vlakken2);

	// eindvlakken
	auto buis2_eindvlakken = MyNXSupportLib::ConnectPointsAndFillEnd(buis2_outer, buis2_inner);
	buis2_vlakken.push_back(buis2_eindvlakken);

	MyNXSupportLib::SewSurfaces(buis2_vlakken, true);
#endif

#if TEKEN_LASTEEN
	/* LASTEEN */
	MyNXSupportLib::SurfaceList lasteen_vlakken;

	MyNXSupportLib::SetDrawParameters(unit_mm, DELTA_L, DELTA_W, max_z /* 'max_z' omdat de lasteen maar 1 element mag bevatten */);
	
	// binnenkant
	auto lasteen_inner = MyNXSupportLib::DrawConnectionOnCylinderSkewedInner(R1, r2, r1, max_z, false);
	//resultaat niet bijhouden want voorlopig toch maar 1 rij, dus geen vlakjes gemaakt
	MyNXSupportLib::ConnectPointsAndFill(lasteen_inner);

	// buitenkant
	auto lasteen_outer = MyNXSupportLib::DrawConnectionOnCylinderSkewed(R1, r1, 8.0, max_z);
	//idem opmerking als hierboven
	MyNXSupportLib::ConnectPointsAndFill(lasteen_outer);

	// eindvlakken
	auto lasteen_grondvlakken = MyNXSupportLib::ConnectPointsAndFillStart(lasteen_outer, lasteen_inner);
	lasteen_vlakken.push_back(lasteen_grondvlakken);

	// verbinden met onderkant van buis 2
	auto lasteen_top = MyNXSupportLib::DrawCircle(MyNXSupportLib::Axis::Z, r1, max_z + R1);

	auto lasteen_innervlakken = MyNXSupportLib::ConnectPointsAndFillSingle(lasteen_inner[0], lasteen_top);
	lasteen_vlakken.push_back(lasteen_innervlakken);

	auto lasteen_outervlakken = MyNXSupportLib::ConnectPointsAndFillSingle(lasteen_outer[0], lasteen_top);
	lasteen_vlakken.push_back(lasteen_outervlakken);

	// vlakken binnenin de lasteen
	MyNXSupportLib::GeometricPoints binnenpoints;
	binnenpoints.insert(binnenpoints.begin(), lasteen_inner.begin(), lasteen_inner.end());
	binnenpoints.insert(binnenpoints.begin(), lasteen_outer.begin(), lasteen_outer.end());
	binnenpoints.push_back(lasteen_top);
	auto lasteen_binnenvlakken = MyNXSupportLib::ConnectPointsAndFill2(binnenpoints);
	lasteen_vlakken.insert(lasteen_vlakken.begin(), lasteen_binnenvlakken.begin(), lasteen_binnenvlakken.end());

	MyNXSupportLib::SewSurfaces(lasteen_vlakken, false);
#endif
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
