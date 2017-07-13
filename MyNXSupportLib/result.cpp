#include "MyNXSupportLib.h"

#include <NXOpen/CAE_ResultManager.hxx>
#include <NXOpen/CAE_ResultParameters.hxx>
#include <NXOpen/CAE_Loadcase.hxx>
#include <NXOpen/CAE_Iteration.hxx>
#include <NXOpen/CAE_ResultType.hxx>

#include <map>

using namespace std;
using namespace NXOpen;

/* timestep (base 1) -> result access */
map<int, CAE::ResultAccess *> x_access, y_access, z_access;

namespace MyNXSupportLib {

CAE::ResultAccess* GetResultAccess(string resulttype_name, CAE::Result::Component component, double& value, int step_base_1, int loadcase_index) {
	string iteration_name = StringConcat("Iteration[", step_base_1, "]");
	string loadcase_name = StringConcat("Loadcase[", loadcase_index, "]");

	CAE::ResultParameters *result_params = nx_session->ResultManager()->CreateResultParameters();
	result_params->SetResultComponent(component);
	result_params->SetComplexCriterion(CAE::Result::Complex::ComplexAmplitude);

	CAE::Loadcase *loadcase = dynamic_cast<CAE::Loadcase *>(nx_result->Find(loadcase_name.c_str()));

	CAE::Iteration *iteration = dynamic_cast<CAE::Iteration *>(loadcase->Find(iteration_name.c_str()));
	value = iteration->Value();

	try {
		CAE::ResultType *resulttype = dynamic_cast<CAE::ResultType *>(iteration->Find(resulttype_name.c_str()));
		result_params->SetGenericResultType(resulttype);

		CAE::ResultAccess *access = nx_session->ResultManager()->CreateResultAccess(nx_result, result_params);
		if (access)
			access->SetParameters(result_params);

		return access;
	} catch (...) {}

	return NULL;
}

void NodeGetDisplacementsXYZ(int step, int node_label, double& dx, double& dy, double& dz) {
	double value;
	int node_index = nx_result->AskNodeIndex(node_label);

	if (x_access.find(step) == x_access.end())
		x_access[step] = GetResultAccess("ResultType[[Displacement][Nodal]]", CAE::Result::Component::ComponentX, value, step+1);
	dx = x_access[step]->AskNodalResult(node_index);

	if (y_access.find(step) == y_access.end())
		y_access[step] = GetResultAccess("ResultType[[Displacement][Nodal]]", CAE::Result::Component::ComponentY, value, step+1);
	dy = y_access[step]->AskNodalResult(node_index);

	if (z_access.find(step) == z_access.end())
		z_access[step] = GetResultAccess("ResultType[[Displacement][Nodal]]", CAE::Result::Component::ComponentZ, value, step+1);
	dz = z_access[step]->AskNodalResult(node_index);
}

CAE::ResultAccess *GetTemperaturesPerNode(int loadcase, int step, double& step_name) {
	return GetResultAccess("ResultType[[Temperature][Nodal]]", CAE::Result::Component::ComponentScalar, step_name, step+1, loadcase+1);
}

}
