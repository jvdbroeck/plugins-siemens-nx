#include "MyNXSupportLib.h"

#include <NXOpen/PartCollection.hxx>
#include <NXOpen/CAE_ResultManager.hxx>

using namespace NXOpen;

namespace MyNXSupportLib {
Session *nx_session;
ListingWindow *nx_listingwindow;
CAE::SimPart *nx_part = NULL;
CAE::SimSimulation *nx_simulation;
CAE::SimSolution *nx_solution;
CAE::SolutionResult *nx_result;
Part *nx_basepart = NULL;

void Init() {
	nx_session = Session::GetSession();

	nx_listingwindow = nx_session->ListingWindow();
	nx_listingwindow->Open();
}

bool AssumeBasePart() {
	nx_basepart = nx_session->Parts()->Work();

	if (nx_basepart == NULL) {
		nx_listingwindow->WriteLine("Base part must be present");
		return false;
	}

	return true;
}

bool AssumePart() {
    nx_part = dynamic_cast<CAE::SimPart *>(nx_session->Parts()->BaseWork());

    if (nx_part == NULL) {
		nx_listingwindow->WriteLine("Work part must be a sim part");
		return false;
    }

	return true;
}

bool AssumeSimulation() {
    nx_simulation = nx_part->Simulation();

    if (nx_simulation == NULL) {
		nx_listingwindow->WriteLine("Work part must have a simulation");
        return false;
    }

	return true;
}

bool AssumeSolution() {
    nx_solution = nx_simulation->ActiveSolution();

    if (nx_solution == NULL) {
		nx_listingwindow->WriteLine("Work part must have an active solution");
        return false;
    }

	return true;
}

bool AssumeResult() {
	nx_result = nx_session->ResultManager()->CreateSolutionResult(nx_solution);

	if (nx_result == NULL) {
		nx_listingwindow->WriteLine("Solution must have results");
		return false;
	}

	return true;
}
}
