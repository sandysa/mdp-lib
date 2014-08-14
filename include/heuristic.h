#ifndef MDPLIB_HEURISTIC_H
#define MDPLIB_HEURISTIC_H

#include "state.h"

namespace mlcore
{
    /**
     * An interface for heuristic functions.
     */
    class Heuristic
    {
    public:
        /**
         * Returns an estimate of reaching a goal state from the given state.
         */
        virtual double cost(const State* s) const =0;
    };
}

#endif // MDPLIB_HEURISTIC_H
