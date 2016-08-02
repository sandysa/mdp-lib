#include <iostream>
#include <iomanip>

int
main( int argc, char **argv )
{
  if( argc == 1 ) {
    std::cout << "usage: adder <n>" << std::endl;
    exit(-1);
  }

  int n = atoi( argv[1] );

  std::cout
    << "(define (problem p" << n << ")" << std::endl
    << "  (:domain adder)" << std::endl
    << "  (:objects t f tmp1 tmp2";

  for( int i = 1; i <= n; ++i ) std::cout << " x" << i << " y" << i << " r" << i << " z" << i;
  std::cout << " r" << 1+n << " z" << 1+n << " - bit)" << std::endl;

  std::cout << "  (:init (and (high t) (low f)" << std::endl << std::setw(14) << "";
  for( int i = 1; i <= 1+n; ++i ) std::cout << "(low r" << i << ") ";
  std::cout << std::endl << std::setw(14) << "";
  for( int i = 1; i <= 1+n; ++i ) std::cout << "(low z" << i << ") ";
  std::cout << std::endl << std::setw(14) << "";

  for( int i = 1; i <= n; ++i ) {
    std::cout << "(oneof (low x" << i << ") (high x" << i << "))" << std::endl << std::setw(14) << "";
    std::cout << "(oneof (low y" << i << ") (high y" << i << "))" << std::endl << std::setw(14) << "";
  }

  for( int i = 1; i <= n; ++i ) std::cout << "(constant x" << i << ") ";
  std::cout << std::endl << std::setw(14) << "";
  for( int i = 1; i <= n; ++i ) std::cout << "(constant y" << i << ") ";

  std::cout
    << std::endl << std::setw(9) << ")" << std::endl << "  )" << std::endl
    << "  (:goal (and (low r1)" << std::endl << std::setw(14) << ""
    << "(or (low z" << 1+n << ") (high r" << 1+n << "))" << std::endl << std::setw(14) << ""
    << "(or (low r" << 1+n << ") (high z" << 1+n << "))" << std::endl << std::setw(14) << "";

  for( int i = 2; i <= 1+n; ++i ) {
    std::cout
      << "(or (low r" << i << ") (high x" << i-1 << ") (high y" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low r" << i << ") (high x" << i-1 << ") (high r" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low r" << i << ") (high y" << i-1 << ") (high r" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low x" << i-1 << ") (low y" << i-1 << ") (high r" << i << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low x" << i-1 << ") (low r" << i-1 << ") (high r" << i << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low y" << i-1 << ") (low r" << i-1 << ") (high r" << i << "))" << std::endl
      << std::setw(14) << ""

      << "(or (low z" << i-1 << ") (low x" << i-1 << ") (high y" << i-1 << ") (low r" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low z" << i-1 << ") (low x" << i-1 << ") (low y" << i-1 << ") (high r" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low z" << i-1 << ") (high x" << i-1 << ") (low y" << i-1 << ") (low r" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low z" << i-1 << ") (high x" << i-1 << ") (high y" << i-1 << ") (high r" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low x" << i-1 << ") (low y" << i-1 << ") (low r" << i-1 << ") (high z" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (low x" << i-1 << ") (high y" << i-1 << ") (high r" << i-1 << ") (high z" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (high x" << i-1 << ") (low y" << i-1 << ") (high r" << i-1 << ") (high z" << i-1 << "))" << std::endl
      << std::setw(14) << ""
      << "(or (high x" << i-1 << ") (high y" << i-1 << ") (low r" << i-1 << ") (high z" << i-1 << "))" << std::endl;
    if( i < 1+n ) std::cout << std::setw(14) << "";
  }
  std::cout << std::setw(9) << ")" << std::endl << "  )" << std::endl << ")" << std::endl;
}

