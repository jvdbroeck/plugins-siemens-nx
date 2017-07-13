#include "MyNXSupportLib.h"

#include <fstream>

using namespace std;
using namespace NXOpen;

int node_ids[] = {
	/* xCD */
	2274, 2275, 2276, 2277, 2347, 2348,
	/* xAB */
	2287, 2288, 2289, 2290, 2349, 2350,
	/* yCD */
	2291, 2292, 2293, 2294, 2351, 2352,
	/* yAB */
	2295, 2296, 2297, 2298, 2353, 2354
};
#define N_NODES 24
#define N_STEPS 11

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	MyNXSupportLib::Init();
	MyNXSupportLib::AssumePart();
	MyNXSupportLib::AssumeSimulation();
	MyNXSupportLib::AssumeSolution();
	MyNXSupportLib::AssumeResult();

	ofstream outfile(MyNXSupportLib::FullFilePath("nodedata.txt"));
	assert(outfile.is_open());

	for (int idx = 0; idx < N_NODES; idx++)
	{
		int node_id = node_ids[idx];
		for (int step_idx = 0; step_idx < N_STEPS; step_idx++)
		{
			double dx, dy, dz;
			MyNXSupportLib::NodeGetDisplacementsXYZ(step_idx, node_id, dx, dy, dz);

			outfile << MyNXSupportLib::Stringf("%d %d %.20f %.20f %.20f", node_id, step_idx, dx, dy, dz) << endl;
		}
	}

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
