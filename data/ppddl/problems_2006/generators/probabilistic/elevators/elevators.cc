#include <iostream>
#include <stdlib.h>

void
print_problem( std::ostream &os, int n, int m, int nelev, int ncoin, int seed, float prob )
{
  os << "(define (problem elev_" << n << "_" << m << "_" << nelev << "_" << ncoin << "_" << seed << ")" << std::endl
     << "  (:domain elevators)" << std::endl;

  os << "  (:objects";
  for( int f = 2; f <= n; ++f ) os << " f" << f;
  os << " - floor";
  for( int p = 2; p <= m; ++p ) os << " p" << p;
  os << " - pos";
  for( int e = 1; e <= nelev; ++e ) os << " e" << e;
  os << " - elevator";
  for( int c = 1; c <= ncoin; ++c ) os << " c" << c;
  os << " - coin)" << std::endl;

  os << "  (:init (at f1 p1)";
  for( int f = 2; f <= n; ++f ) os << " (dec_f f" << f << " f" << f-1 << ")";
  for( int p = 2; p <= m; ++p ) os << " (dec_p p" << p << " p" << p-1 << ")";
  for( int e = 1; e <= nelev; ++e )
    os << " (shaft e" << e << " p" << 1+(lrand48()%m) << ") (in e" << e << " f" << 1+(lrand48()%n) << ")";
  for( int c = 1; c <= ncoin; ++c )
    os << " (coin-at c" << c << " f" << 1+(lrand48()%n) << " p" << 1+(lrand48()%m) << ")";
  for( int f = 2; f <= n; ++f ) {
    for( int p = 2; p <= m; ++p )
      if( drand48() < prob ) os << " (gate f" << f << " p" << p << ")";
  }
  os << ")" << std::endl;

  os << "  (:goal (and";
  for( int c = 1; c <= ncoin; ++c ) os << " (have c" << c << ")";
  os << "))" << std::endl << ")" << std::endl;
}

int
main( int argc, char **argv )
{
  if( (argc == 1) || !strcmp(argv[1],"-?") ) {
    std::cerr << "usage: ./elevators <seed> <n> <m> <#elev> <#coins> <p>" << std::endl;
    return(0);
  }

  unsigned short seed = (unsigned short)atoi(argv[1]);
  int n = atoi(argv[2]);
  int m = atoi(argv[3]);
  int e = atoi(argv[4]);
  int c = atoi(argv[5]);
  float p = atof(argv[6]);

  unsigned short s[3];
  s[0] = s[1] = s[2] = seed;
  seed48( s );
  srand48( seed );
  
  print_problem( std::cout, n, m, e, c, seed, p );
  return(0);
}
