#include "MyNXSupportLib.h"

using namespace NXOpen;
using namespace std;

/* entry point */
extern DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
	static int load_index = 0;
	static int table_index = 0;

	MyNXSupportLib::Init();
	MyNXSupportLib::AssumePart();
	MyNXSupportLib::AssumeSimulation();
	MyNXSupportLib::AssumeSolution();

	/* (in)dependent variables */
	auto vIndependent = vector<Fields::FieldVariable *>(1, MyNXSupportLib::GetTimeVariableX());
	auto vDependent = vector<Fields::FieldVariable *>(1, MyNXSupportLib::GetVariableY("temperature", "Temperature", MyNXSupportLib::GetUnit("Celcius")));

	/* open up the input file */
	ifstream infile(MyNXSupportLib::FullFilePath("data.csv"));
	assert(infile.is_open());

	/* read first line as time data */
	vector<int> times = MyNXSupportLib::ReadIntData(infile);

	/* add load data for each node in the model */
	while (!infile.eof()) {
		/* read one line from the file and put the data in an array */
		vector<int> data = MyNXSupportLib::ReadIntData(infile);
		if (data.size() == 0) continue;

		/* construct the node data array */
		vector<double> node_data;
		for (size_t i = 1; i < data.size(); i++) {
			node_data.push_back(times[i]);
			node_data.push_back(data[i]);
		}

		/* create temperature load */
		string name = MyNXSupportLib::StringConcat("Temperature(", load_index, ")");

		/* ... data table */
		Fields::FieldTable *table = MyNXSupportLib::CreateFieldTable(name, table_index, vIndependent, vDependent, node_data);
				
		/* ... target node/element */
		int node_id = data[0];
		MyNXSupportLib::CreateAndAddLoad("tempLoad", name, table_index+1,
											MyNXSupportLib::GetNodeObject(node_id), "temperature", table);

		load_index++;
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
