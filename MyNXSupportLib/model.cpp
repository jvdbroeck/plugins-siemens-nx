#include "MyNXSupportLib.h"

// CreateScalar, CreateAndAddPoint
#include <NXOpen/Scalar.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/Features_PointFeatureBuilder.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/ScalarCollection.hxx>
#include <NXOpen/ExpressionCollection.hxx>
#include <NXOpen/PointCollection.hxx>
#include <NXOpen/Features_BaseFeatureCollection.hxx>

#include <NXOpen/Features_AssociativeLineBuilder.hxx>
#include <NXOpen/SelectPoint.hxx>
#include <NXOpen/Features_AssociativeLine.hxx>

#include <NXOpen/Features_FillHoleBuilder.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_FreeformSurfaceCollection.hxx>
#include <NXOpen/CurveDumbRule.hxx>
#include <NXOpen/ScRuleFactory.hxx>
#include <NXOpen/Section.hxx>
#include <NXOpen/Line.hxx>
#include <NXOpen/Features_FillHole.hxx>

#include <NXOpen/Body.hxx>
#include <NXOpen/Features_SewBuilder.hxx>
#include <NXOpen/SelectDisplayableObjectList.hxx>

using namespace std;
using namespace NXOpen;

static Unit *UNIT;
static double DELTA_L = 1.0;
static double DELTA_W = 1.0;
static double DELTA_Z = 1.0;

namespace MyNXSupportLib {

void SetDrawParameters(Unit *unit, double delta_l, double delta_w, double delta_z) {
	UNIT = unit;
	DELTA_L = delta_l;
	DELTA_W = delta_w;
	DELTA_Z = delta_z;
}

static
Scalar *CreateScalar(string name, string expr, Unit *unit) {
	Expression *expression;

	/* create unique expressions */
	static unsigned int expression_id = 0;
	bool searching = true;
	while (searching) {
		searching = false;
		try {
			string full = name + to_string(expression_id) + "=" + expr;
			expression = nx_basepart->Expressions()->CreateSystemExpressionWithUnits(full.c_str(), unit);
		}
		catch (...) {
			searching = true;
			expression_id++;
		}
	}

	return nx_basepart->Scalars()->CreateScalarExpression(expression, Scalar::DimensionalityType::DimensionalityTypeNone, SmartObject::UpdateOption::UpdateOptionWithinModeling);
}

static
Point *CreateAndAddPoint(string x_expr, string y_expr, string z_expr, Unit *unit) {
	Scalar *sX = CreateScalar("X", x_expr, unit);
	Scalar *sY = CreateScalar("Y", y_expr, unit);
	Scalar *sZ = CreateScalar("Z", z_expr, unit);

	Point *point = nx_basepart->Points()->CreatePoint(sX, sY, sZ, SmartObject::UpdateOption::UpdateOptionWithinModeling);
	point->SetVisibility(SmartObject::VisibilityOption::VisibilityOptionVisible);

	Features::PointFeatureBuilder *builder = nx_basepart->BaseFeatures()->CreatePointFeatureBuilder(NULL);
	builder->SetPoint(point);
	builder->Commit();

	builder->Destroy();

	return point;
}

GeometricPointsElement DrawCircle(Axis ax, double R, double pos) {
	string str_R = to_string(R);
	string z = to_string(pos);

	GeometricPointsElement points;
	for (double w = 0.0; w < 360; w += DELTA_W) {
		string str_w = to_string(w);

		string x = str_R + "*cos(" + str_w + ")";
		string y = str_R + "*sin(" + str_w + ")";

		Point *p;
		switch (ax) {
		case Axis::X:
			p = CreateAndAddPoint(z, x, y, UNIT);
			break;

		case Axis::Y:
			p = CreateAndAddPoint(x, z, y, UNIT);
			break;

		case Axis::Z:
			p = CreateAndAddPoint(x, y, z, UNIT);
			break;
		}

		points.push_back(p);
	}

	return points;
}

GeometricPoints DrawCylinder(Axis ax, double R, double Lmin, double Lmax) {
	GeometricPoints all_points;

	string str_R = to_string(R);

	for (double L = Lmin; L <= Lmax; L+= DELTA_L)
		all_points.push_back(DrawCircle(ax, R, L));

	return all_points;
}

GeometricPoints DrawConnectionOnCylinder(double R, double r, double max_z, bool draw_max_z, double z_start) {
	GeometricPoints all_points;

	string str_R = to_string(R);
	string str_r = to_string(r);

	for (double z_offset = z_start; z_offset <= max_z; z_offset += DELTA_Z) {
		if (z_offset == max_z
			&& !draw_max_z)
			continue;

		GeometricPointsElement points;

		string str_Z = to_string(z_offset);

		for (double w = 0.0; w < 360; w += DELTA_W) {
			string str_w = to_string(w);

			string x = str_r + "*cos(" + str_w + ")";
			string y = str_r + "*sin(" + str_w + ")";
			string z = str_Z + "+sqrt(" + str_R + "^2-" + str_r + "^2*sin(" + str_w + ")^2)";

			Point *p = CreateAndAddPoint(x, y, z, UNIT);
			points.push_back(p);
		}

		all_points.push_back(points);
	}

	return all_points;
}

GeometricPoints DrawConnectionOnCylinderSkewed(double R, double r, double r_offset, double max_z) {
	GeometricPoints all_points;

	string str_R = to_string(R);

	double delta_r = static_cast<double>(DELTA_Z) * r_offset/max_z;
	double running_r = r + r_offset;

	for (double z_offset = 0.0; z_offset < max_z; z_offset += DELTA_Z, running_r -= delta_r) {
		GeometricPointsElement points;

		string str_r = to_string(running_r);
		string str_Z = to_string(z_offset);

		for (double w = 0.0; w < 360; w += DELTA_W) {
			string str_w = to_string(w);

			string x = str_r + "*cos(" + str_w + ")";
			string y = str_r + "*sin(" + str_w + ")";
			string z = str_Z + "+sqrt(" + str_R + "^2-" + str_r + "^2*sin(" + str_w + ")^2)";

			Point *p = CreateAndAddPoint(x, y, z, UNIT);
			points.push_back(p);
		}

		all_points.push_back(points);
	}

	return all_points;
}

GeometricPoints DrawConnectionOnCylinderSkewedInner(double R, double ri, double ro, double max_z, bool draw_max_z) {
	GeometricPoints all_points;

	string str_R = to_string(R);

	double delta_r = static_cast<double>(DELTA_Z) * (ro - ri)/max_z;
	double running_r = ri;

	for (double z_offset = 0; z_offset <= max_z; z_offset += DELTA_Z, running_r += delta_r) {
		if (z_offset == max_z
			&& !draw_max_z)
			continue;

		GeometricPointsElement points;

		string str_r = to_string(running_r);
		string str_Z = to_string(z_offset);

		for (double w = 0.0; w < 360; w += DELTA_W) {
			string str_w = to_string(w);

			string x = str_r + "*cos(" + str_w + ")";
			string y = str_r + "*sin(" + str_w + ")";
			string z = str_Z + "+sqrt(" + str_R + "^2-" + str_r + "^2*sin(" + str_w + ")^2)";

			Point *p = CreateAndAddPoint(x, y, z, UNIT);
			points.push_back(p);
		}

		all_points.push_back(points);
	}

	return all_points;
}

static
Features::AssociativeLine *DrawLine(Point *a, Point *b) {
	Features::AssociativeLineBuilder *builder = nx_basepart->BaseFeatures()->CreateAssociativeLineBuilder(static_cast<Features::AssociativeLine*>(NULL));

	builder->SetStartPointOptions(Features::AssociativeLineBuilder::StartOption::StartOptionPoint);
	builder->SetEndPointOptions(Features::AssociativeLineBuilder::EndOption::EndOptionPoint);

	// here we _need_ to recreate the point!
	builder->StartPoint()->SetValue(nx_basepart->Points()->CreatePoint(a, NULL, SmartObject::UpdateOption::UpdateOptionWithinModeling));
	builder->EndPoint()->SetValue(nx_basepart->Points()->CreatePoint(b, NULL, SmartObject::UpdateOption::UpdateOptionWithinModeling));

	auto line = builder->Commit();
	builder->Destroy();

	return static_cast<Features::AssociativeLine *>(line);
}

static
Features::FillHole *FillSurface(vector<Features::AssociativeLine *> lines) {
	Features::FillHoleBuilder *builder = nx_basepart->Features()->FreeformSurfaceCollection()->CreateFillHoleBuilder(NULL);
	builder->SetTolerance(0.01);

	vector<Features::FillHoleBuilder::BorderContinuity> continuity;
	for (auto assoc_line : lines) {
		Line *line = dynamic_cast<Line *>(assoc_line->FindObject("CURVE 1"));

		vector<IBaseCurve *> curves;
		curves.push_back(dynamic_cast<IBaseCurve *>(line));

		CurveDumbRule *dumb_rule = nx_basepart->ScRuleFactory()->CreateRuleBaseCurveDumb(curves);
		vector<SelectionIntentRule *> rules;
		rules.push_back(dynamic_cast<SelectionIntentRule *>(dumb_rule));

		Point3d helper = Point3d(0, 0, 0);
		builder->CurveChain()->AddToSection(rules, line, NULL, NULL, helper, Section::Mode::ModeCreate, false);
				
		Features::FillHoleBuilder::BorderContinuity cont;
		cont.BorderObject = assoc_line;
		cont.Continuity = Features::FillHoleBuilder::ContinuityTypes::ContinuityTypesG0;
		continuity.push_back(cont);
	}
	builder->SetBorderTypeItems(continuity);

	Features::FillHole *filled = static_cast<Features::FillHole *>(builder->Commit());
	builder->Destroy();

	return filled;
}

SurfaceListElement ConnectPointsAndFillSingle(GeometricPointsElement pts1, GeometricPointsElement pts2) {
	SurfaceListElement result;

	for (size_t idx = 0; idx < pts1.size(); idx++) {
		size_t AC_idx = idx;
		size_t BD_idx = (idx+1) % pts1.size();

		Point *A = pts1[AC_idx];
		Point *B = pts1[BD_idx];
		Point *C = pts2[AC_idx];
		Point *D = pts2[BD_idx];

		vector<Features::AssociativeLine *> lines;
		lines.push_back(DrawLine(A, B));
		lines.push_back(DrawLine(A, C));
		lines.push_back(DrawLine(B, D));
		lines.push_back(DrawLine(C, D));

		result.push_back(FillSurface(lines));
	}

	return result;
}

SurfaceList ConnectPointsAndFill(GeometricPoints all_points) {
	SurfaceList result;

	if (all_points.size() == 1)
		return result;

	for (size_t point_vector_idx = 0; point_vector_idx < all_points.size() - 1; point_vector_idx++) {
		size_t AB_idx = point_vector_idx;
		size_t CD_idx = point_vector_idx+1;
		if (CD_idx == all_points.size())
			CD_idx = 0;

		result.push_back(ConnectPointsAndFillSingle(all_points[AB_idx], all_points[CD_idx]));
	}
}

SurfaceListElement ConnectPointsAndFillStart(GeometricPoints all_points_1, GeometricPoints all_points_2) {
	return ConnectPointsAndFillSingle(all_points_1[0], all_points_2[0]);
}

SurfaceListElement ConnectPointsAndFillEnd(GeometricPoints all_points_1, GeometricPoints all_points_2) {
	return ConnectPointsAndFillSingle(all_points_1[all_points_1.size()-1], all_points_2[all_points_2.size()-1]);
}

SurfaceList ConnectPointsAndFill2(GeometricPoints all_points, bool circular) {
	SurfaceList duplicates(2);

	for (int nr = 0; nr < 2; nr++) {
		for (size_t i = 0; i < all_points[0].size(); i++) {
			// create list of points to connect
			vector<Point *> points_to_connect;
			for (size_t j = 0; j < all_points.size(); j++)
				points_to_connect.push_back(all_points[j][i]);

			// create list of lines
			vector<Features::AssociativeLine *> lines;
			for (size_t j = 0; j < points_to_connect.size(); j++) {
				Point *A = points_to_connect[j];
				Point *B = points_to_connect[(j == points_to_connect.size()-1) ? 0 : j+1];

				lines.push_back(DrawLine(A, B));
			}

			duplicates[nr].push_back(FillSurface(lines));
		}
	}
	
	SurfaceListElement binnenvlakken_1;
	SurfaceListElement binnenvlakken_2;
	for (size_t i = 0; i < duplicates[0].size(); i++) {
		binnenvlakken_1.push_back(duplicates[0][i]);

		if (i == duplicates[0].size() - 1)
			binnenvlakken_2.push_back(duplicates[1][0]);
		else
			binnenvlakken_2.push_back(duplicates[1][i+1]);
	}

	SurfaceList result;
	result.push_back(binnenvlakken_1);
	result.push_back(binnenvlakken_2);
	return result;
}

static
void SewBodies(vector<Body *> bodies) {
	Features::SewBuilder *builder = nx_basepart->Features()->CreateSewBuilder(NULL);
	builder->SetTolerance(0.01);

	bool target_added = false;
	for (auto body : bodies) {
		if (target_added)
			builder->ToolBodies()->Add(body);
		else
			builder->TargetBodies()->Add(body);

		target_added = true;
	}

	builder->Commit();
	builder->Destroy();
}

void SewSurfaces(SurfaceList surfaces, bool all_together) {
	vector<Body *> all_bodies;

	for (size_t i = 0; i < surfaces[0].size(); i++) {

		for (auto v : surfaces)
			for (auto body : v[i]->GetBodies())
				all_bodies.push_back(body);

		if (!all_together) {
			SewBodies(all_bodies);
			all_bodies.clear();
		}
	}

	if (all_together)
		SewBodies(all_bodies);
}

}
