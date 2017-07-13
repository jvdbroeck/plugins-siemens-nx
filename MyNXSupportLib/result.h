#ifndef MYNXSUPPORTLIB_RESULT_H
#define MYNXSUPPORTLIB_RESULT_H

#include <NXOpen/CAE_ResultAccess.hxx>

namespace MyNXSupportLib {

NXOpen::CAE::ResultAccess* GetResultAccess(std::string resulttype_name, NXOpen::CAE::Result::Component component, double& value, int step_base_1, int loadcase_index = 1);
void NodeGetDisplacementsXYZ(int step, int node_label, double& dx, double& dy, double& dz);
NXOpen::CAE::ResultAccess *GetTemperaturesPerNode(int loadcase, int step, double& step_name);

}

#endif
