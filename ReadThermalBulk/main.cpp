#include "MyNXSupportLib.h"

#include <NXOpen/CAE_ResultAccess.hxx>

#include <fstream>
#include <map>

using namespace std;
using namespace NXOpen;

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	MyNXSupportLib::Init();
	MyNXSupportLib::AssumePart();
	MyNXSupportLib::AssumeSimulation();
	MyNXSupportLib::AssumeSolution();
	MyNXSupportLib::AssumeResult();

	// construct filename and open the output file
	ofstream outfile(MyNXSupportLib::FullFilePath("data.csv"));
	assert(outfile.is_open());

	// map time step to results
	map<int, CAE::ResultAccess*> resultmap;

	string line = "node_label,";
	// we can't do GetLoadCases() here because for some reason an empty vector is returned
	for (auto loadcase_idx = 0; loadcase_idx < MyNXSupportLib::nx_result->AskNumLoadcases(); loadcase_idx++) {
		for (auto iteration_idx = 0; iteration_idx < MyNXSupportLib::nx_result->AskNumIterations(loadcase_idx); iteration_idx++) {
			// load results and display name
			double name = 0.0;
			auto values = MyNXSupportLib::GetTemperaturesPerNode(loadcase_idx, iteration_idx, name);

			// skip the time-invariant step
			if (values == NULL) continue;

			// time steps are assumed to be whole numbers
			int timestep = static_cast<int>(name);
			resultmap[timestep] = values;
			line += to_string(timestep) + ",";
		}
	}
	// remove the last comma
	line.pop_back();
	outfile << line << endl;
	
	// write the results to the output file
	MyNXSupportLib::debug("There are %d nodes in this model", MyNXSupportLib::nx_result->AskNumNodes());
	for (int idx = 0; idx < MyNXSupportLib::nx_result->AskNumNodes(); idx++)
	{
		// node label
		int node_label = MyNXSupportLib::nx_result->AskNodeLabel(idx+1);
		line = to_string(node_label) + ",";

		for (auto time_result : resultmap) {
			// result value for this node
			double node_value = time_result.second->AskNodalResult(idx+1);
			line += to_string(static_cast<int>(node_value + 0.5)) + ",";
		}

		line.pop_back();
		outfile << line << endl;
	}

	// close the output file
	outfile.close();
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
