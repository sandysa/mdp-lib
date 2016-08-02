#include <iostream>
#include <math.h>
#include <rational.h>

int seed = 0;
double kappa_table[] = { 0.0, 0.1, 0.3, 0.6 };

class state {
  int x_, y_;  // (1,1)=lower-left corner
public:
  state( int x, int y ) : x_(x), y_(y) { }
  state( const state &s ) : x_(s.x_), y_(s.y_) { }
  static int dim_x_, dim_y_;
  state s() const { return( *this ); }
  unsigned x() const { return( x_ ); }
  unsigned y() const { return( y_ ); }
  state move( int a ) const
  {
    switch( a ) {
      case 0: // up
        if( y() == dim_y_-1 ) return( s() );
        return( state( x(), y()+1 ) );
        break;
      case 1: // right
        if( x() == dim_x_ - 1 ) return( s() );
        return( state( x()+1, y() ) );
        break;
      case 2: // down
        if( y() == 0 ) return( s() );
        return( state( x(), y()-1 ) );
        break;
      case 3: // left
        if( x() == 0 ) return( s() );
        return( state( x()-1, y() ) );
        break;
    }
    return( s() );
  }
  static state random() { return( state( lrand48()%dim_x_, lrand48()%dim_y_ ) ); }
  bool operator==( const state &s ) const { return( (x_ == s.x_) && (y_ == s.y_) ); }
  bool operator!=( const state &s ) const { return( !(*this == s) ); }
  void print( std::ostream &os ) const { os << "(at X" << 1+x() << " Y" << 1+y() << ")"; }
};
inline std::ostream& operator<<( std::ostream &os, const state &s ) { s.print(os); return(os); }

int state::dim_x_ = 0;
int state::dim_y_ = 0;
char *aname[4] = { "up", "right", "down", "left" };

void
emit_domain( std::ostream &os, int dim_x, int dim_y, float p )
{
  char *water = new char[dim_x*dim_y];
  for( int x = 0; x < dim_x; ++x )
    for( int y = 0; y < dim_y; ++y )
      if( drand48() < p ) { water[(x*dim_x)+y] = 1+(lrand48()%3); }

  os << "(define (domain room_" << dim_x << "_" << dim_y << "_" << seed << ")" << std::endl
     << "  (:requirements :typing :probabilistic-effects :conditional-effects)" << std::endl
     << "  (:types pos_x pos_y)" << std::endl
     << "  (:constants ";
  for( int x = 1; x <= dim_x; ++x ) os << "X" << x << " ";
  os << "- pos_x ";
  for( int y = 1; y <= dim_y; ++y ) os << "Y" << y << " ";
  os << "- pos_y)" << std::endl
     << "  (:predicates (at ?x - pos_x ?y - pos_y))" << std::endl << std::endl;

  for( int x = 0; x < dim_x; ++x ) {
    for( int y = 0; y < dim_y; ++y ) {
      state s(x,y);
      for( int a = 0; a < 4; ++a ) {
        if( s.move(a) != s ) {
          os << "  (:action " << aname[a] << "_" << 1+x << "_" << 1+y << std::endl
             << "    :precondition " << s << std::endl
             << "    :effect (probabilistic" << std::endl;
          Rational e(kappa_table[water[x,y]],5);
          Rational e2 = e*e;
          os << "              " << 1-e-e2 << " (and " << s.move(a) << " (not " << s << "))" << std::endl;
          if( e - e2 > 0 ) {
            if( s.move(a).move((a+1)%4) != s )
              os << "              " << (e-e2)/4 << " (and " << s.move(a).move((a+1)%4) << " (not " << s << "))" << std::endl;
            if( s.move(a).move((a+3)%4) != s )
              os << "              " << (e-e2)/4 << " (and " << s.move(a).move((a+3)%4) << " (not " << s << "))" << std::endl;
            if( s.move((a+1)%4) != s )
              os << "              " << (e-e2)/4 << " (and " << s.move((a+1)%4) << " (not " << s << "))" << std::endl;
            if( s.move((a+3)%4) != s )
              os << "              " << (e-e2)/4 << " (and " << s.move((a+3)%4) << " (not " << s << "))" << std::endl;
          }
          os << "            )" << std::endl << "  )" << std::endl;
        }
      }
    }
  }
  os << ")" << std::endl;
}

void
emit_problem( std::ostream &os, int dim_x, int dim_y )
{
  os << "(define (problem room_" << dim_x << "_" << dim_y << "_" << seed << ")" << std::endl
     << "  (:domain room_" << dim_x << "_" << dim_y << "_" << seed << ")" << std::endl
     << "  (:init " << state::random() << ")" << std::endl
     << "  (:goal " << state::random() << ")" << std::endl
     << ")" << std::endl;
}

int
main( int argc, char **argv )
{
  float q = 0;
  for( ++argv, --argc; *argv[0] == '-'; ++argv, --argc ) {
    if( !strcmp(*argv,"-seed") ) {
      unsigned short rseed[3];
      seed = rseed[0] = rseed[1] = rseed[2] = atoi(argv[1]);
      seed48(rseed);
      srand48(seed);
      ++argv;
      --argc;
    }
  }

  if( argc != 3 ) {
    std::cout << "usage: room [-seed <int>] [-rational] <dim_x> <dom_y> <prob>" << std::endl;
    exit(255);
  }

  int dim_x = atoi(argv[0]);
  int dim_y = atoi(argv[1]);
  float p = atof(argv[2]);

  state::dim_x_ = dim_x;
  state::dim_y_ = dim_y;
  emit_domain( std::cout, dim_x, dim_y, p );
  std::cout << std::endl;
  emit_problem( std::cout, dim_x, dim_y );
  std::cout << std::endl;
  return 0;
}

