#include "MyNXSupportLib.h"

using namespace NXOpen;
using namespace std;

/*
Per element
Tabel tijd, flux
Element 1, tabel(teller=1)
Element 124, tabel(teller=124)

4 functies (a, b, c, d) -> zie datfile
*/

#define NR_STEPS 27
#define NR_FACES 125
/* A, B, C, D */
#define NR_ZONES 4
/* links (verstijver/dekplaat), rechts (vertijver/dekplaat) */
#define NR_REGIONS 4

int node_start_ids[NR_ZONES][NR_REGIONS] = {
			/* links verstijver, links dekplaat, rechts verstijver, rechts dekplaat */
	/* A */ {5036, 89265, 43511, 142640},
	/* B */ {5161, 89140, 43386, 142765},
	/* C */ {5286, 89015, 43261, 142890},
	/* D */ {5411, 88890, 43136, 143015}
};

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	static int table_index = 0;

	MyNXSupportLib::Init();
	MyNXSupportLib::AssumePart();
	MyNXSupportLib::AssumeSimulation();
	MyNXSupportLib::AssumeSolution();

	/* (in)dependent variables */
	auto vIndependent = vector<Fields::FieldVariable *>(1, MyNXSupportLib::GetTimeVariableX());
	auto vDependent = vector<Fields::FieldVariable *>(1, MyNXSupportLib::GetVariableY("heat flux", "Heat_Flux", MyNXSupportLib::GetUnit("HeatFlux_Metric5")));

	/* read in data: zones A, B, C, D */
	array<vector<double>, NR_ZONES> time, data;
	MyNXSupportLib::ReadRawZoneData<NR_ZONES>(MyNXSupportLib::FullFilePath("raw.csv"), time, data, NR_STEPS);
	
	/* iterate over the zones: links (verstijver/dekplaat), rechts (verstijver/dekplaat) */
	for (int region_index = 0; region_index < NR_REGIONS; region_index++) {
		/* A, B, C, D */
		for (int zone_index = 0; zone_index < NR_ZONES; zone_index++) {
			/* 0 - 124 (125 faces in depth) */
			for (int face_index = 0; face_index < NR_FACES; face_index++) {
				/* create array with all data: time, value, time, value, ... */
				vector<double> node_data;
				for (int step_index = 0; step_index < NR_STEPS; step_index++) {
					auto time_value = time[zone_index][step_index];
					if (0 < step_index && step_index < NR_STEPS-1)
						time_value += (face_index * 0.218);
					node_data.push_back(time_value);
					node_data.push_back(data[zone_index][step_index]);
				}

				MyNXSupportLib::TimeDataVectorEnsureCorrectInterpolation(node_data);

				/* create new flux load */
				string name = MyNXSupportLib::StringConcat("Flux(", table_index, ")");

				/* ... data table */
				Fields::FieldTable *table = MyNXSupportLib::CreateFieldTable(name, table_index, vIndependent, vDependent, node_data);
				
				/* ... target node/element */
				int element_id = node_start_ids[zone_index][region_index] + face_index;
				MyNXSupportLib::CreateAndAddLoad("Heat Flux", name, table_index+1,
													MyNXSupportLib::GetElementObject(element_id), "Heat Flux", table);
			}
		}
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
