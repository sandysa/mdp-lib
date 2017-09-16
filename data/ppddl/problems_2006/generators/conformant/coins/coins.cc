#include <iostream>
#include <set>
#include <vector>

struct Problem {
  int rows_, cols_, coins_, elevators_, seed_;
  std::vector<int> coin_floor_;
  std::set<int> elevator_shafts_;
  Problem( int rows, int cols, int coins, int elevators, int seed )
    : rows_(rows), cols_(cols), coins_(coins), elevators_(elevators), seed_(seed)
  {
    while( (int)coin_floor_.size() < coins_ ) {
      int r = lrand48() % rows_;
      coin_floor_.push_back(r);
    }
    while( (int)elevator_shafts_.size() < elevators_ ) {
      int c = lrand48() % cols_;
      elevator_shafts_.insert(c); 
    }
  }
  ~Problem() { }
  void print_problem( std::ostream &os ) const;
};

void
Problem::print_problem( std::ostream &os ) const
{
  os << "(define (problem coins_" << rows_ << "_" << cols_ << "_" << elevators_ << "_" << seed_ << ")" << std::endl
     << "  (:domain coins)" << std::endl
     << "  (:objects ";

  for( int e = 0; e < elevators_; ++e ) os << "e" << e << " ";
  os << "- elevator ";
  for( int f = 0; f < rows_; ++f ) os << "f" << f << " ";
  os << "- floor ";
  for( int p = 0; p < cols_; ++p ) os << "p" << p << " ";
  os << "- pos ";
  for( int c = 0; c < coins_; ++c ) os << "c" << c << " ";
  os << "- coin)" << std::endl;

  os << "  (:init (and";
  for( int f = 1; f < rows_; ++f ) os << " (dec_f f" << f << " f" << f-1 << ")";
  for( int p = 1; p < cols_; ++p ) os << " (dec_p p" << p << " p" << p-1 << ")";

  int e = 0;
  for( std::set<int>::const_iterator si = elevator_shafts_.begin(); si != elevator_shafts_.end(); ++si, ++e ) {
    os << " (shaft e" << e << " p" << (*si) << ")";
    os << " (oneof";
    for( int f = 0; f < rows_; ++f ) os << " (in e" << e << " f" << f << ")";
    os << ")";
  }

  for( int c = 0; c < (int)coin_floor_.size(); ++c ) {
    int f = coin_floor_[c];
    os << " (oneof";
    for( int p = 0; p < cols_; ++p ) os << " (coin-at c" << c << " f" << f << " p" << p << ")";
    os << ")";
  }

  //for( int e = 0; e < elevators_; ++e ) os << " (not (inside e" << e << "))";
  //for( int c = 0; c < coins_; ++c ) os << " (not (have c" << c << "))";
  os << " (at f0 p0)))" << std::endl;

  os << "  (:goal (and";
  for( int i = 0; i < coins_; ++i ) os << " (have c" << i << ")";
  os << "))" << std::endl << ")" << std::endl;
}

int
main( int argc, char **argv )
{
  if( argc == 1 ) {
  usage:
    std::cout << "usage: coins --rows <n> --cols <m> [--coins <l>] [--elevators <e>] [--seed <n>]" << std::endl;
    exit(-1);
  }

  int rows = -1;
  int cols = -1;
  int coins = 1;
  int elevators = 1;
  unsigned short seed[3] = {0,0,0};

  --argc;
  ++argv;
  while( argc && (**argv == '-') ) {
    if( !strcmp(*argv,"--rows") ) {
      rows = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--cols") ) {
      cols = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--coins") ) {
      coins = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--elevators") ) {
      elevators = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--seed") ) {
      seed[0] = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else {
      goto usage;
    }
  }

  seed[2] = seed[1] = seed[0];
  seed48(seed);

  if( (rows == -1) || (cols == -1) ) goto usage;
  if( elevators > cols ) {
    std::cout << "#elevators must be at most #cols" << std::endl;
    exit(-1);
  }

  Problem *problem = new Problem(rows,cols,coins,elevators,seed[0]);
  problem->print_problem(std::cout);
  return(0);
}

