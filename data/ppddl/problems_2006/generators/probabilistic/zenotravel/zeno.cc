#include <iostream>
#include <iomanip>
#include <set>
#include <vector>

void
make_problem( std::ostream &os, int suffix, int n_cities, int n_persons, int n_aircrafts, int fuel_level )
{
  os << "(define (problem zeno_" << n_cities << "_" << n_persons << "_" << n_aircrafts << "_" << suffix << ")" << std::endl
     << "  (:domain zenotravel)" << std::endl
     << "  (:objects";
  if( n_cities > 0 ) {
    for( int c = 0; c < n_cities; ++c ) os << " c" << c;
    os << " - city";
  }
  if( n_persons > 0 ) {
    for( int p = 0; p < n_persons; ++p ) os << " p" << p;
    os << " - person";
  }
  if( n_aircrafts > 0 ) {
    for( int a = 0; a < n_aircrafts; ++a ) os << " a" << a;
    os << " - aircraft";
  }
  for( int f = 0; f < fuel_level; ++f ) os << " f" << f;
  os << " - flevel)" << std::endl;

  os << "  (:init";
  for( int f = 1; f < fuel_level; ++f ) os << " (next f" << f-1 << " f" << f << ")";
  os << std::endl;
  for( int p = 0; p < n_persons; ++p ) {
    os << "         (at-person p" << p << " c" << (lrand48()%n_cities) << ") "
       << "(not-boarding p" << p << ") (not-debarking p" << p << ")" << std::endl;
  }
  for( int a = 0; a < n_aircrafts; ++a ) {
    os << "         (at-aircraft a" << a << " c" << (lrand48()%n_cities) << ") "
       << "(fuel-level a" << a << " f" << (lrand48()%fuel_level) << ") (not-refueling a" << a << ")" << std::endl;
  }
  os << "  )" << std::endl;

  os << "  (:goal (and";
  for( int p = 0; p < n_persons; ++p ) {
    os << " (at-person p" << p << " c" << (lrand48()%n_cities) << ")";
  }
  os << "))" << std::endl << ")" << std::endl;
}

int
main( int argc, char **argv )
{
  if( argc == 1 ) {
  usage:
    std::cout << "usage: zeno [--seed <seed>] [--cities <n>] [--persons <n>] [--aircrafts <n>]" << std::endl;
    exit(-1);
  }

  unsigned short seed[3] = {0,0,0};
  int n_cities = 0;
  int n_persons = 0;
  int n_aircrafts = 0;
  int fuel_level = 5;

  --argc;
  ++argv;
  while( argc && (**argv == '-') ) {
    if( !strcmp(*argv,"--seed") ) {
      seed[0] = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--cities") ) {
      n_cities = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--persons") ) {
      n_persons = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--aircrafts") ) {
      n_aircrafts = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else {
      goto usage;
    }
  }
  if( argc > 0 ) goto usage;

  seed[2] = seed[1] = seed[0];
  srand48(seed[0]);
  seed48(seed);

  make_problem(std::cout,seed[0],n_cities,n_persons,n_aircrafts,fuel_level);
  return(0);
}

