#include "MyNXSupportLib.h"

#include <NXOpen/UnitCollection.hxx>
#include <NXOpen/TaggedObject.hxx>
#include <NXOpen/CAE_FEElement.hxx>
#include <NXOpen/CAE_SimBCBuilder.hxx>
#include <NXOpen/CAE_SetManager.hxx>
#include <NXOpen/CAE_PropertyTable.hxx>
#include <NXOpen/CAE_FENode.hxx>
#include <NXOpen/Fields_FieldExpression.hxx>

using namespace std;
using namespace NXOpen;

static Fields::FieldManager *cached_fieldmanager = nullptr;
static CAE::FEModelOccurrence *cached_occurrence = nullptr;

namespace MyNXSupportLib {

Fields::FieldManager *GetFieldManager() {
	/* use caching because this code is HOT */
	if (cached_fieldmanager == nullptr)
		cached_fieldmanager = dynamic_cast<Fields::FieldManager *>(nx_part->FindObject("FieldManager"));

	return cached_fieldmanager;
}

CAE::FEModelOccurrence *GetFEModelOccurrence() {
	/* use caching because this code is HOT */
	if (cached_occurrence == nullptr) {
		int counter = 1;

		while (cached_occurrence == nullptr) {
			try {
				string name = StringConcat("FEModelOccurrence[", counter, "]");
				cached_occurrence = dynamic_cast<CAE::FEModelOccurrence *>(nx_part->FindObject(name.c_str()));
			} catch (...) {
				/* model occurent not found... */
			}

			counter++;
		}
	}
		

	return cached_occurrence;
}

Unit *GetUnit(string name) {
	return nx_part->UnitCollection()->FindObject(name.c_str());
}

Fields::FieldVariable *GetTimeVariableX() {
	auto fm = GetFieldManager();
	return fm->CreateIndependentVariable(NULL,
		fm->GetNameVariable("time", "Time"), GetUnit("Second"),
		Fields::FieldVariable::ValueType::ValueTypeReal, false, true, 0.0, false, true, 1.0, false, 2, false, 1.0);
}

Fields::FieldVariable *GetVariableY(string vname, string mname, Unit *unit) {
	auto fm = GetFieldManager();
	return fm->CreateDependentVariable(NULL,
		fm->GetNameVariable(vname.c_str(), mname.c_str()), unit,
		Fields::FieldVariable::ValueType::ValueTypeReal);
}

Fields::FieldTable *CreateFieldTable(string name, int& index, vector<Fields::FieldVariable *> vIndependent, vector<Fields::FieldVariable *> vDependent, vector<double>& data) {
	Fields::FieldTable *table = GetFieldManager()->CreateFieldTable(name.c_str(), vIndependent, vDependent, data);
	table->EditFieldTableComplexDisplay(vector<bool>(1, false), vector<bool>(1, false));
	table->SetValuesOutsideTableInterpolation(Fields::FieldEvaluator::ValuesOutsideTableInterpolationEnum::ValuesOutsideTableInterpolationEnumConstant);
	table->SetLinearLogOption(Fields::FieldEvaluator::LinearLogOptionEnum::LinearLogOptionEnumLinearLinear);
	table->SetIndependentValueShiftOption(false);
	table->SetIndependentValueDivisorOption(false);
	table->SetSpatialMap(NULL);

	bool assigned = false;
	while (!assigned) {
		try {
			table->SetIdLabel(index);
			assigned = true;
		} catch (...) {
			/* table index already in use, increment */
		}
		index++;
	}

	return table;
}

CAE::SetObject GetElementObject(int element_id) {
	CAE::SetObject obj;
	obj.Obj = static_cast<TaggedObject *>(GetFEModelOccurrence()->FeelementLabelMap()->GetElement(element_id));
	obj.SubType = CAE::CaeSetObjectSubType::CaeSetObjectSubTypeElementFace;
	obj.SubId = 3;

	return obj;
}

CAE::SetObject GetNodeObject(int node_id) {
	CAE::SetObject obj;
	obj.Obj = static_cast<TaggedObject *>(GetFEModelOccurrence()->FenodeLabelMap()->GetNode(node_id));
	obj.SubType = CAE::CaeSetObjectSubType::CaeSetObjectSubTypeNone;
	obj.SubId = 0;

	return obj;
}

void CreateAndAddLoad(string descriptor, string name, int label,
						CAE::SetObject target_object, string target_property, Fields::FieldTable *target_table) {
	CAE::SimBCBuilder *builder = nx_simulation->CreateBcBuilderForLoadDescriptor(descriptor.c_str(), name.c_str(), label);

	/* create */
	builder->TargetSetManager()->SetTargetSetMembers(0, vector<CAE::SetObject>(1, target_object));
	builder->PropertyTable()->SetScalarFieldWrapperPropertyValue(target_property.c_str(), MyNXSupportLib::GetFieldManager()->CreateScalarFieldWrapperWithField(target_table, 1.0));

	/* add */
	builder->CommitAddBc();
	builder->Destroy();
}

void CreateAndAddCoupling(string descriptor, string name, int label,
						  CAE::SetObject target_object_1, CAE::SetObject target_object_2,
						  array<bool, 6> dof_enabled) {
	CAE::SimBCBuilder *builder = nx_simulation->CreateBcBuilderForConstraintDescriptor(descriptor.c_str(), name.c_str(), label);

	/* create */
	builder->TargetSetManager()->SetTargetSetMembers(0, vector<CAE::SetObject>(1, target_object_1));
	builder->TargetSetManager()->SetTargetSetMembers(1, vector<CAE::SetObject>(1, target_object_2));

	/* degree of freedom */
	for (int i = 0; i < dof_enabled.size(); i++)
		if (dof_enabled[i]) {
			vector<Fields::FieldVariable *> independent_variables;
			string name = StringConcat("DOF", i+1);

			auto prop = builder->PropertyTable()->GetScalarFieldPropertyValue(name.c_str());
			prop->EditFieldExpression("1", NULL, independent_variables, false);

			builder->PropertyTable()->SetScalarFieldPropertyValue(name.c_str(), prop);
		}

	/* add */
	builder->CommitAddBc();
	builder->Destroy();
}

void TimeDataVectorEnsureCorrectInterpolation(std::vector<double>& data, double delta) {
	/* insert additional elements for zero */
	{
		/* element: time(0) */
		auto it = data.begin();
		/* element: value(0) */
		it++;
		/* element: time(1) */
		it++;

		double time_value = *it;

		vector<double> new_data;
		new_data.push_back(time_value - delta);
		new_data.push_back(0);

		data.insert(it, new_data.begin(), new_data.end());
	}

	{
		/* element: value(1000) */
		auto it = data.rbegin();
		/* element: time(1000) */
		it++;

		/* element: value(last) */
		it++;
		auto insert_before = it;
		/* element: time(last) */
		it++;

		auto time_value = *it;

		vector<double> new_data;
		new_data.push_back(time_value + delta);
		new_data.push_back(0);

		data.insert(insert_before.base(), new_data.begin(), new_data.end());
	}
}

}
