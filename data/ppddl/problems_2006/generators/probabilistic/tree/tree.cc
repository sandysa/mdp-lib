#include <iostream>
#include <math.h>
#include <rational.h>

bool use_rationals = false;

void
emit_domain( std::ostream &os, int n, float p )
{
  Rational pr(p,5);
  os << "(define (domain tree_" << n << "_" << pr.numerator() << "_" << pr.denominator() << ")" << std::endl
     << "  (:requirements :typing :probabilistic-effects :conditional-effects)" << std::endl
     << "  (:types level db)" << std::endl
     << "  (:constants ";
  for( int i = 0; i <= n; ++i ) os << "L" << i << " ";
  os << "- level ";
  for( int i = 0; i <= n; ++i ) os << "db" << i << " ";
  os << "- db)" << std::endl
     << "  (:predicates (at ?l - level) (bit ?l - level) (noise ?d - db))" << std::endl << std::endl;

  for( int i = 0; i < n; ++i ) {
    for( int d = 0; d <= i; ++d ) {
      float q = powf(p,(float)d);
      Rational r(q,5);
      os << "  (:action left_" << i << "_" << d << std::endl
         << "    :precondition (and (at L" << i << ") (noise db" << d << "))" << std::endl;
      if( q == 1.0 )
        os << "    :effect (and (at L" << i+1 << ") (not (at L" << i << ")))" << std::endl;
      else {
        os << "    :effect (probabilistic" << std::endl;
        if( use_rationals )
          os << "               " << r;
        else
          os << "               " << q;
        os << " (and (at L" << i+1 << ") (not (at L" << i << ")))" << std::endl;
        if( use_rationals )
          os << "               " << 1-r;
        else
          os << "               " << 1-q;
        os << " (and (at L" << i-1 << ") (not (at L" << i << ")) "
           << "(when (bit L" << i << ") (and (not (bit L" << i << ")) (noise db" << d-1 << ") (not (noise db" << d << "))))))" << std::endl;
      }
      os << "  )" << std::endl;
    }
  }

  os << std::endl;
  for( int i = 0; i < n; ++i ) {
    for( int d = 0; d <= i; ++d ) {
      float q = powf(p,(float)d);
      Rational r(q,5);
      os << "  (:action right_" << i << "_" << d << std::endl
         << "    :precondition (and (at L" << i << ") (noise db" << d << "))" << std::endl;
      if( q == 1.0 )
        os << "    :effect (and (at L" << i+1 << ") (not (at L" << i << ")) (noise db" << d+1 << ") (not (noise db" << d << ")) (bit L" << i+1 << "))" << std::endl;
      else {
        os << "    :effect (probabilistic" << std::endl;
        if( use_rationals )
          os << "               " << r;
        else
          os << "               " << q;
        os << " (and (at L" << i+1 << ") (not (at L" << i << ")) (noise db" << d+1 << ") (not (noise db" << d << ")) (bit L" << i+1 << "))" << std::endl;
        if( use_rationals )
          os  << "               " << 1-r;
        else
          os  << "               " << 1-q;
        os << " (and (at L" << i-1 << ") (not (at L" << i << ")) "
           << "(when (bit L" << i << ") (and (not (bit L" << i << ")) (noise db" << d-1 << ") (not (noise db" << d << "))))))" << std::endl;
      }
      os << "  )" << std::endl;
    }
  }

  os << ")" << std::endl;
}

void
emit_problem( std::ostream &os, int n, float p )
{
  Rational pr(p,5);
  os << "(define (problem tree_" << n << "_" << pr.numerator() << "_" << pr.denominator() << ")" << std::endl
     << "  (:domain tree_" << n << "_" << pr.numerator() << "_" << pr.denominator() << ")" << std::endl
     << "  (:init (at L0) (noise db0))" << std::endl
     << "  (:goal (at L" << n << "))" << std::endl
     << ")" << std::endl;
}

int
main( int argc, char **argv )
{
  ++argv;
  --argc;
  if( !strcmp(*argv,"-rational") ) {
    use_rationals = true;
    ++argv;
    --argc;
  }

  if( argc != 2 ) {
    std::cout << "usage: tree [-rational] <depth> <prob>" << std::endl;
    exit(255);
  }

  int n = atoi(argv[0]);
  float p = atof(argv[1]);

  emit_domain( std::cout, n, p );
  std::cout << std::endl;
  emit_problem( std::cout, n, p );
  std::cout << std::endl;
  return 0;
}

