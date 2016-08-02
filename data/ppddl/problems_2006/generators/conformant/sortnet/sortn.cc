#include <iostream>
#include <iomanip>

void
make( std::ostream &os, int n, bool ff )
{
  os << "(define (problem s" << n << ")" << std::endl
     << "  (:domain sortnet)" << std::endl
     << "  (:objects ";
  for( int i = 1; i <= n; ++i ) os << "l" << i << " ";
  os << "- line)" << std::endl;

  os << "  (:init (and ";
  for( int i = 1; i < n; ++i ) {
    for( int j = 1+i; j <= n; ++j ) {
      os << "(less l" << i << " l" << j << ")" << std::endl << std::setw(14) << "";
    }
  }
  for( int i = 1; i <= n; ++i ) {
    if( !ff )
      os << "(oneof (high l" << i << ") (not (high l" << i << ")))" << std::endl;
    else
      os << "(unknown (high l" << i << "))" << std::endl;
    if( i < n ) os << std::setw(14) << "";
  }
  os << "         )" << std::endl << "  )" << std::endl;

  os << "  (:goal (and";
  for( int i = 1; i < n; ++i ) {
    os << " (or (not (high l" << i << ")) (high l" << 1+i << "))" << std::endl;
    if( i < n-1 ) os << std::setw(13) << "";
  }
  os << "         )" << std::endl << "  )" << std::endl << ")" << std::endl;
}

int
main( int argc, char **argv )
{
  bool ff = false;
  if( argc < 2 ) {
    std::cerr << "usage: sortn [-ff] <n>" << std::endl;
    exit(255);
  }
  if( !strcmp(argv[1],"--ff") ) {
    ff = true;
    --argc;
    ++argv;
  }
  int n = atoi( argv[1] );
  make( std::cout, n, ff );
  return(0);
}

