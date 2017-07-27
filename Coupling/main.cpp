#include "MyNXSupportLib.h"

#include <NXOpen/CAE_SimSolutionStep.hxx>
#include <NXOpen/CAE_PropertyTable.hxx>

using namespace NXOpen;
using namespace std;

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	static int index = 0;

	MyNXSupportLib::Init();
	MyNXSupportLib::AssumePart();
	MyNXSupportLib::AssumeSimulation();

	/* open up the input file */
	ifstream infile(MyNXSupportLib::FullFilePath("coupling.csv"));
	assert(infile.is_open());

	/* default values for degrees of freedom */
	array<bool, 6> dof_enabled = {true, true, true, false, false, false};

	while (!infile.eof()) {
		/* read one line from the file and put the data in an array */
		vector<int> data = MyNXSupportLib::ReadIntData(infile);
		if (data.size() < 2) continue;

		/* data[0] and data[1] should be coupled */
		string name = MyNXSupportLib::StringConcat("Manual Coupling(", index, ")");
				
		/* ... target nodes */
		auto node1 = MyNXSupportLib::GetNodeObject(data[0]);
		auto node2 = MyNXSupportLib::GetNodeObject(data[1]);

		MyNXSupportLib::CreateAndAddCoupling("Coupled DOF", name, index+1,
												node1, node2, dof_enabled);

		index++;
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
