#include "MyNXSupportLib.h"

using namespace NXOpen;
using namespace std;

#define DELTA_T 1.847

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	static int table_index = 1;

	MyNXSupportLib::Init();
	MyNXSupportLib::AssumePart();
	MyNXSupportLib::AssumeSimulation();
	MyNXSupportLib::AssumeSolution();

	/* (in)dependent variables */
	auto vIndependent = vector<Fields::FieldVariable *>(1, MyNXSupportLib::GetTimeVariableX());
	auto vDependent = vector<Fields::FieldVariable *>(1, MyNXSupportLib::GetVariableY("heat flux", "Heat_Flux", MyNXSupportLib::GetUnit("HeatFlux_Metric2")));

	/* read time, data */
	vector<double> time, data;
	MyNXSupportLib::ReadRawData(MyNXSupportLib::FullFilePath("raw.csv"), time, data);

	/* read list of faces */
	vector<string> faces;
	MyNXSupportLib::ReadListData(MyNXSupportLib::FullFilePath("faces.txt"), faces);

	for (int face_index = 0; face_index < faces.size(); face_index++) {
		auto face = MyNXSupportLib::GetFace(faces[face_index]);

		vector<double> node_data;
		for (int step_index = 0; step_index < time.size(); step_index++) {
			auto time_value = time[step_index];
			if (0 < step_index && step_index < time.size() - 1)
				time_value += (face_index * DELTA_T);

			node_data.push_back(time_value);
			node_data.push_back(data[step_index]);
		}

		MyNXSupportLib::TimeDataVectorEnsureCorrectInterpolation(node_data);

		/* create new flux load */
		string name = MyNXSupportLib::StringConcat("Surface Flux(", table_index, ")");

		/* ... data table */
		Fields::FieldTable *table = MyNXSupportLib::CreateFieldTable(name, table_index, vIndependent, vDependent, node_data);
				
		/* ... target node/element */
		MyNXSupportLib::CreateAndAddLoad("UniformElmFlux", name, table_index+1,
											face, "HeatFlux", table);
	}
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
