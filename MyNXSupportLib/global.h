#ifndef MYNXSUPPORTLIB_GLOBAL_H
#define MYNXSUPPORTLIB_GLOBAL_H

#include <NXOpen/Session.hxx>
#include <NXOpen/ListingWindow.hxx>

#include <NXOpen/CAE_SimPart.hxx>
#include <NXOpen/CAE_SimSimulation.hxx>
#include <NXOpen/CAE_SimSolution.hxx>
#include <NXOpen/CAE_SolutionResult.hxx>

namespace MyNXSupportLib {
extern NXOpen::Session *nx_session;
extern NXOpen::ListingWindow *nx_listingwindow;

extern NXOpen::CAE::SimPart *nx_part;
extern NXOpen::CAE::SimSimulation *nx_simulation;
extern NXOpen::CAE::SimSolution *nx_solution;
extern NXOpen::CAE::SolutionResult *nx_result;

void Init();
bool AssumePart();
bool AssumeSimulation();
bool AssumeSolution();
bool AssumeResult();
};

#endif
