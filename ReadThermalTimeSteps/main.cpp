#include "MyNXSupportLib.h"

#include <fstream>

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

	// we can't do GetLoadCases() here because for some reason an empty vector is returned
	for (auto loadcase_idx = 0; loadcase_idx < MyNXSupportLib::nx_result->AskNumLoadcases(); loadcase_idx++) {

		for (auto iteration_idx = 0; iteration_idx < MyNXSupportLib::nx_result->AskNumIterations(loadcase_idx); iteration_idx++) {

			// load results and display name
			double name = 0.0;
			auto values = MyNXSupportLib::GetTemperaturesPerNode(loadcase_idx+1, iteration_idx+1, name);

			// skip the time-invariant step
			if (values == NULL) continue;

			// construct filename and open the output file
			ofstream outfile(MyNXSupportLib::FullFilePath(MyNXSupportLib::Stringf("time%f.csv", name)));
			assert(outfile.is_open());

			// write the results to the output file
			for (int idx = 0; idx < MyNXSupportLib::nx_result->AskNumNodes(); idx++)
			{
				// result value for this node
				int node_label = MyNXSupportLib::nx_result->AskNodeLabel(idx+1);
				double node_value = values->AskNodalResult(idx+1) / 18;

				// write it to a string an put it in the file
				outfile << MyNXSupportLib::Stringf("%d,%.20f", node_label, node_value) << endl;
			}

			// close the output file
			outfile.close();
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
