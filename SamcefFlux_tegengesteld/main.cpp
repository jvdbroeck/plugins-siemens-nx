#include "MyNXSupportLib.h"

using namespace NXOpen;
using namespace std;

// zones: A, B, C
#define NR_ZONES 3
// regio's: links verstijver/dekplaat, rechts verstijver/dekplaat
#define NR_REGIONS 4
// 50 elementjes in de diepte
#define NR_FACES 50
// aantal tijdstappen
#define NR_STEPS 53
// per face voegen we dit toe aan de tijdstap
#define DELTA_T_PER_FACE 0.316

int node_start_ids[NR_ZONES][NR_REGIONS] = {
			/* links verstijver, links dekplaat, rechts verstijver, rechts dekplaat */
	/* A */ {153323, 139138, 154423, 140438},
	/* B */ {153373, 139088, 154373, 140488},
	/* C */ {153423, 139038, 154323, 140538}
};

int faces[NR_ZONES][NR_REGIONS] = {
			/* links verstijver, links dekplaat, rechts verstijver, rechts dekplaat */
	/* A */ {5, 4, 2, 3},
	/* B */ {5, 4, 2, 3},
	/* C */ {5, 4, 2, 3}
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

	/* raw data */
	array<vector<double>, NR_ZONES> time, data;

	/* 1e NR_STEPS: dekplaat
	 * 2e NR_STEPS: verstijver */
	MyNXSupportLib::ReadRawZoneData<NR_ZONES>(MyNXSupportLib::FullFilePath("raw.csv"), time, data, NR_STEPS*2);
	
	/* iterate over the zones: links (verstijver/dekplaat), rechts (verstijver/dekplaat) */
	for (int region_index = 0; region_index < NR_REGIONS; region_index++) {
		/* A, B, C */
		for (int zone_index = 0; zone_index < NR_ZONES; zone_index++) {
			/* 0 - 124 (125 faces in depth) */
			for (int face_index = 0; face_index < NR_FACES; face_index++) {
				/* create array with all data: time, value, time, value, ... */
				vector<double> node_data;
				for (int step_index = 0; step_index < NR_STEPS; step_index++) {
					auto time_value = time[zone_index][step_index + NR_STEPS * (region_index % 2)];
					if (0 < step_index && step_index < NR_STEPS-1)
						time_value += (face_index * DELTA_T_PER_FACE);
					node_data.push_back(time_value);
					node_data.push_back(data[zone_index][step_index]);
				}

				MyNXSupportLib::TimeDataVectorEnsureCorrectInterpolation(node_data);

				/* create new flux load */
				string name = MyNXSupportLib::StringConcat("Flux(", table_index, ")");

				/* ... data table */
				Fields::FieldTable *table = MyNXSupportLib::CreateFieldTable(name, table_index, vIndependent, vDependent, node_data);
				
				/* ... target node/element */
				int element_id = node_start_ids[zone_index][region_index] + ((NR_FACES-1) - face_index);
				MyNXSupportLib::CreateAndAddLoad("UniformElmFlux", name, table_index+1,
													MyNXSupportLib::GetElementObject(element_id, faces[zone_index][region_index]), "HeatFlux", table);
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
