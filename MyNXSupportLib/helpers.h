#ifndef MYNXSUPPORTLIB_HELPERS_H
#define MYNXSUPPORTLIB_HELPERS_H

#include <array>
#include <string>
#include <vector>

#include <NXOpen/Fields_FieldManager.hxx>
#include <NXOpen/Unit.hxx>
#include <NXOpen/Fields_FieldVariable.hxx>
#include <NXOpen/CAE_FEModelOccurrence.hxx>
#include <NXOpen/Fields_FieldTable.hxx>
#include <NXOpen/CAE_SetObject.hxx>
#include <NXOpen/CAE_SimBCBuilder.hxx>

namespace MyNXSupportLib {

NXOpen::Fields::FieldManager *GetFieldManager();
NXOpen::Unit *GetUnit(std::string name);
NXOpen::Fields::FieldVariable *GetTimeVariableX();
NXOpen::Fields::FieldVariable *GetVariableY(std::string vname, std::string mname, NXOpen::Unit *unit);
NXOpen::CAE::FEModelOccurrence *GetFEModelOccurrence();
NXOpen::Fields::FieldTable *CreateFieldTable(std::string name, int& index, std::vector<NXOpen::Fields::FieldVariable *> vIndependent, std::vector<NXOpen::Fields::FieldVariable *> vDependent, std::vector<double>& data);
NXOpen::CAE::SetObject GetElementObject(int element_id, int face = 3);
NXOpen::CAE::SetObject GetNodeObject(int node_id);
NXOpen::CAE::SetObject GetFace(std::string face_id);
void GetFaceObjects(std::vector<std::string> face_names);

void CreateAndAddLoad(std::string descriptor, std::string name, int label,
						NXOpen::CAE::SetObject target_object, std::string target_property, NXOpen::Fields::FieldTable *target_table);

void CreateAndAddCoupling(std::string descriptor, std::string name, int label,
						  NXOpen::CAE::SetObject target_object_1, NXOpen::CAE::SetObject target_object_2,
						  std::array<bool, 6> dof_enabled);

void TimeDataVectorEnsureCorrectInterpolation(std::vector<double>& vector, double delta = 0.1);

}

#endif
