#include <cassert>
#include <ctime>

#include "../../../include/State.h"
#include "../../../include/solvers/VISolver.h"
#include "../../../include/solvers/DeterministicSolver.h"

#include "../../../include/domains/EV/EVProblem.h"
#include "../../../include/domains/EV/EVState.h"
#include "../../../include/domains/EV/EVDetHeuristic.h"
using namespace EV;
using namespace std;
using namespace mlsolvers;

EVDetHeuristic::EVDetHeuristic(int startsoc, int endsoc, int starttime, int endtime, int rewardcase)
{
    detProblem_ = new EVProblem(startsoc, endsoc, starttime, endtime, rewardcase);
    detProblem_->generateAll();
    Heuristic* heuristic = nullptr;
    Solver* solver = new DeterministicSolver(detProblem_,
                                            mlsolvers::det_most_likely,
                                            heuristic);
    solver->solve(detProblem_->initialState());
//    mlsolvers::VISolver vi(detProblem_, 100000, 0.01);
 //   std::cout << " solving VI in EVDETHeuristic" << std::endl;
 //   vi.solve();
 //   std::cout << " solved VI" << std::endl;
}

double EVDetHeuristic::cost(const mlcore::State* s)
{
    const EVState* rts = static_cast<const EVState*>(s);
    EVState* tmp = new EVState(detProblem_, rts->soc(), rts->timestep(),
                                EV::START_DEMAND_DIST,EV::START_PRICE_DIST, rts->exit_comm());

    mlcore::StateSet::const_iterator it =
        detProblem_->states().find((mlcore::State *) tmp);
    double hval = 0;
    if (it != detProblem_->states().end())
        hval = (*it)->cost();

    delete tmp;
    return hval;
}
