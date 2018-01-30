#include "MyNXSupportLib.h"

#include <fstream>

using namespace std;
using namespace NXOpen;

int node_ids[] = {
	/* xAB, xCD, yAB, yCD */
	9738, 9733, 9740, 9731,
	9585, 9383, 9592, 9382,
	9586, 9384, 9591, 9381,
	9587, 9385, 9590, 9380,
	9588, 9386, 9589, 9379,
	9739, 9734, 9741, 9732
};
#define N_NODES 24
#define N_STEPS 20
#define PRINT_NODE_ID 0

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	MyNXSupportLib::Init();
	MyNXSupportLib::AssumePart();
	MyNXSupportLib::AssumeSimulation();
	MyNXSupportLib::AssumeSolution();
	MyNXSupportLib::AssumeResult();

	/* collect data from results */
	double all_dx[N_STEPS][N_NODES];
	double all_dy[N_STEPS][N_NODES];
	double all_dz[N_STEPS][N_NODES];
	for (int idx = 0; idx < N_NODES; idx++)
	{
		int node_id = node_ids[idx];
		for (int step_idx = 0; step_idx < N_STEPS; step_idx++)
		{
			double dx, dy, dz;
			MyNXSupportLib::NodeGetDisplacementsXYZ(step_idx, node_id, dx, dy, dz);

			all_dx[step_idx][idx] = dx;
			all_dy[step_idx][idx] = dy;
			all_dz[step_idx][idx] = dz;
		}
	}

	/* write data to files */
	ofstream outfile(MyNXSupportLib::FullFilePath("nodedata.txt"));
	assert(outfile.is_open());

	for (int step_idx = 0; step_idx < N_STEPS; step_idx++) {
		int node_idx = 0;
		while (node_idx < N_NODES) {
			outfile << MyNXSupportLib::Stringf(
#if PRINT_NODE_ID
				"%d %d %.20f %d %.20f %d %.20f %d %.20f",
#else
				"%d %.20f %.20f %.20f %.20f",
#endif
				step_idx+1, 
#if PRINT_NODE_ID
				node_ids[node_idx+0],
#endif
				all_dx[step_idx][node_idx+0],
#if PRINT_NODE_ID
				node_ids[node_idx+1],
#endif
				all_dx[step_idx][node_idx+1],
#if PRINT_NODE_ID
				node_ids[node_idx+2],
#endif
				all_dy[step_idx][node_idx+2],
#if PRINT_NODE_ID
				node_ids[node_idx+3],
#endif
				all_dy[step_idx][node_idx+3]) << endl;
			node_idx += 4;
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
