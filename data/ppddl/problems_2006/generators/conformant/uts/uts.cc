#include <iostream>
#include <iomanip>

void
make_header( std::ostream &os, std::string name, int n )
{
  os << "(define (problem " << name << n << ")" << std::endl
     << "  (:domain uts)" << std::endl
     << "  (:objects ";
  for( int i = 1; i <= n; ++i ) os << "n" << i << " ";
  os << "- node)" << std::endl;
}

void
make_goal( std::ostream &os, int n )
{
  os << "  (:goal (and";
  for( int i = 1; i <= n; ++i ) os << " (visited n" << i << ")";
  os << "))" << std::endl << ")" << std::endl;
}

void
make_linear( std::ostream &os, int n )
{
  make_header( os, "l", n );
  os << "  (:init (and ";
  for( int i = 1; i < n; ++i ) {
    os << "(edge n" << i << " n" << 1+i << ") "
       << "(edge n" << 1+i << " n" << i << ")" << std::endl << std::setw(14) << "";
  }
  os << "(oneof";
  for( int i = 1; i <= n; ++i ) os << " (at n" << i << ")";
  os << ")" << std::endl << "         )" << std::endl << "  )" << std::endl;
  make_goal( os, n );
}

void
make_cycle( std::ostream &os, int n )
{
  make_header( os, "c", n );
  os << "  (:init (and ";
  for( int i = 1; i < n; ++i ) {
    os << "(edge n" << i << " n" << 1+i << ") "
       << "(edge n" << 1+i << " n" << i << ")" << std::endl << std::setw(14) << "";
  }
  os << "(edge n" << n << " n1) " << "(edge n1 n" << n << ")" << std::endl << std::setw(14) << "";
  os << "(oneof";
  for( int i = 1; i <= n; ++i ) os << " (at n" << i << ")";
  os << ")" << std::endl << "         )" << std::endl << "  )" << std::endl;
  make_goal( os, n );
}

void
make_k( std::ostream &os, int n )
{
  make_header( os, "k", n );
  os << "  (:init (and ";
  for( int i = 1; i <= n; ++i ) {
    for( int j = 1; j <= n; ++j ) {
      if( i != j ) os << "(edge n" << i << " n" << j << ")" << std::endl << std::setw(14) << "";
    }
  }
  os << "(oneof";
  for( int i = 1; i <= n; ++i ) os << " (at n" << i << ")";
  os << ")" << std::endl << "         )" << std::endl << "  )" << std::endl;
  make_goal( os, n );
}

void
make_random( std::ostream &os, int n )
{
  make_header( os, "r", n );
  os << "  (:init (and ";
  for( int i = 1; i <= n; ++i ) {
    for( int j = 1; j <= n; ++j ) {
      if( (i != j) && (lrand48()%2) ) os << "(edge n" << i << " n" << j << ")" << std::endl << std::setw(14) << "";
    }
  }
  os << "(oneof";
  for( int i = 1; i <= n; ++i ) os << " (at n" << i << ")";
  os << ")" << std::endl << "         )" << std::endl << "  )" << std::endl;
  make_goal( os, n );
}

int
main( int argc, char **argv )
{
  if( argc < 3 ) {
    std::cerr << "usage: uts [-seed <seed>] {linear|cycle|complete|random} <n>" << std::endl;
    exit(255);
  }

  if( !strcmp(argv[1],"-seed") ) {
    int s = atoi( argv[2] );
    unsigned short seed[3];
    seed[0] = seed[1] = seed[2] = s;
    seed48(seed);
    argv += 2;
  }

  std::string type = argv[1];
  int n = atoi( argv[2] );

  if( type == "linear" )
    make_linear(std::cout,n);
  else if( type == "cycle" )
    make_cycle(std::cout,n);
  else if( type == "complete" )
    make_k(std::cout,n);
  else if( type == "random" )
    make_random(std::cout,n);
  return(0);
}

