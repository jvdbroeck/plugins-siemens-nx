#include "MyNXSupportLib.h"

#include <fstream>

using namespace std;
using namespace NXOpen;

int node_ids[] = {
	/* xCD */
	9383, 9384, 9385, 9386, 9733, 9734,
	/* xAB */
	9585, 9586, 9587, 9588, 9738, 9739,
	/* yCD */
	9379, 9380, 9381, 9382, 9731, 9732,
	/* yAB */
	9589, 9590, 9591, 9592, 9740, 9741
};
#define N_NODES 24
#define N_STEPS 21

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
