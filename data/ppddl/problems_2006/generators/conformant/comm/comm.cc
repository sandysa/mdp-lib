#include <iostream>
#include <set>
#include <vector>

struct Problem {
  int stages_, packets_;
  Problem( int stages, int packets ) : stages_(stages), packets_(packets) { }
  ~Problem() { }
  void print_problem( std::ostream &os, bool ff ) const;
};

void
Problem::print_problem( std::ostream &os, bool ff ) const
{
  os << "(define (problem comm_" << stages_ << "_" << packets_ << ")" << std::endl
     << "  (:domain comm)" << std::endl
     << "  (:objects ";

  if( stages_ > 1 ) {
    for( int s = 1; s < stages_; ++s ) os << "s" << s << " ";
    os << "- stage ";
  }
  for( int p = 0; p < packets_; ++p ) os << "p" << p << " ";
  os << "- packet)" << std::endl;

  os << "  (:init (and (current-stage s0)" << std::endl;
  for( int p = 0; p < packets_; ++p ) {
    os << "              (in-channel p" << p << ") (seq-number p" << p << " s" << p << ") ";
    if( ff )
      os << "(unknown (noisy p" << p << "))" << std::endl;
    else
      os << "(oneof (noisy p" << p << ") (not (noisy p" << p << ")))" << std::endl;
  }
  for( int s = 1; s < stages_; ++s ) {
    os << "              (next-stage s" << s-1 << " s" << s << ")" << std::endl;
  }
  os << "         )" << std::endl << "  )" << std::endl;
  os << "  (:goal (and";
  for( int p = 0; p < packets_; ++p ) os << " (ok p" << p << ")";
  os << "))" << std::endl << ")" << std::endl;
}

int
main( int argc, char **argv )
{
  if( argc == 1 ) {
  usage:
    std::cout << "usage: comm [--ff] --stages <n> --packets <m>" << std::endl;
    exit(-1);
  }

  bool ff = false;
  int stages = -1;
  int packets = -1;

  --argc;
  ++argv;
  while( argc && (**argv == '-') ) {
    if( !strcmp(*argv,"--stages") ) {
      stages = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--packets") ) {
      packets = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--ff") ) {
      ff = true;
      argc -= 1;
      argv += 1;
    }
    else {
      goto usage;
    }
  }

  if( (stages == -1) || (packets == -1) ) goto usage;
  if( packets > stages ) {
    std::cout << "#stages must be at most #packets" << std::endl;
    exit(-1);
  }

  Problem *problem = new Problem(stages,packets);
  problem->print_problem(std::cout,ff);
  return(0);
}

