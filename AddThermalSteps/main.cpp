#include "MyNXSupportLib.h"

#include <NXOpen/CAE_SimSolutionStep.hxx>
#include <NXOpen/CAE_PropertyTable.hxx>

using namespace NXOpen;
using namespace std;

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	static int table_index = 0;

	MyNXSupportLib::Init();
	MyNXSupportLib::AssumePart();
	MyNXSupportLib::AssumeSimulation();
	MyNXSupportLib::AssumeSolution();

	/* units */
	Unit *time_unit = MyNXSupportLib::GetUnit("Second");

	/* steps 1-220, 240, 260, 280, 300, 400, 500, 750, 1000 */
	int step_value = 1;
	int increment = 1;

	/* step 1 is special as it exists already */
	CAE::SimSolutionStep *step = dynamic_cast<CAE::SimSolutionStep *>(MyNXSupportLib::nx_simulation->FindObject("SolutionStep[Step - Thermal 1]"));
	step->PropertyTable()->SetBaseScalarWithDataPropertyValue("End Time", step_value, time_unit);
	step->PropertyTable()->SetIntegerPropertyValue("Thermal Time Step Method", 0);
	step_value += increment;

	/* other steps */
	while (step_value <= 1000) {
		/* create a new step */
		string name = MyNXSupportLib::StringConcat("Step - Thermal ", step_value);

		CAE::SimSolutionStep *step = MyNXSupportLib::nx_solution->CreateStep(0, true, name.c_str());
		step->PropertyTable()->SetBaseScalarWithDataPropertyValue("End Time", step_value, time_unit);
		step->PropertyTable()->SetIntegerPropertyValue("Thermal Time Step Method", 0);

		/* adapt the step increment */
		if (step_value == 220) increment = 20;
		if (step_value == 300) increment = 100;
		if (step_value == 500) increment = 250;

		step_value += increment;
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
