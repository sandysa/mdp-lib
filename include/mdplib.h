#ifndef MDPLIB_MDPLIB_H
#define MDPLIB_MDPLIB_H

namespace mdplib
{
    const int dead_end_cost = 10000;
    const double epsilon = 1.0e-8;
    const unsigned long VISITED = 1;
    const unsigned long SOLVED = 2;
    const unsigned long CLOSED = 4;
}

#endif // MDPLIB_MDPLIB_H
