
// Adapted by Blai Bonet from:

/*
 * Rational numbers.
 *
 * Copyright (C) 2003 Carnegie Mellon University and Rutgers University
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: rational.h,v 1.1 2003/11/07 03:04:34 john Exp $
 */

#ifndef RATIONAL_H
#define RATIONAL_H

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>

namespace Utils {

  inline long gcd( long n, long m )
  {
    long a = abs(n);
    long b = abs(m);
    while( b > 0 ) {
      long c = b;
      b = a % b;
      a = c;
    }
    return( a );
  }
  inline long lcm( long n, long m ) { return( n/gcd(n, m)*m ); }
};

class Rational {
public:
  // Constructors
  Rational( int n = 0 ) : numerator_(long(n)), denominator_(1) { }
  Rational( long n = 0 ) : numerator_(n), denominator_(1) { }
  Rational( double p, int precision = 0 ) : numerator_(0), denominator_(1)
  {
    long a = long(floor(fabs(p)));
    double q = fabs(p) - a;
    for( int i = 0; (i < precision) && (q != 0); ++i ) {
      assert( (q > 0) && (q < 1) );
      long d = long(floor(10*q));
      numerator_ = 10*numerator_ + d;
      denominator_ *= 10;
      q = 10*q-d;
      if( (numerator_ < 0) || (denominator_ < 0) ) throw std::string("Rational: overflow");
    }
    long d = Utils::gcd( numerator_, denominator_ );
    numerator_ /= d;
    denominator_ /= d;
    numerator_ += a*denominator_;
    if( p < 0 ) numerator_ *= -1;
  }
  Rational( long n, long m )
  {
    if( m == 0 ) throw std::string("Rational: division by zero");
    long d = Utils::gcd(n,m);
    numerator_ = n/d;
    denominator_ = m/d;
    if( denominator_ < 0 ) {
      numerator_ *= -1;
      denominator_ *= -1;
    }
  }
  Rational( const char* s ) : numerator_(0), denominator_(1)
  {
    const char* si = s;
    for( ; (*si != '\0') && (*si != '.') && (*si != '/'); ++si ) {
      numerator_ = 10*numerator_ + (*si - '0');
    }
    if( *si == '/' ) {
      denominator_ = 0;
      for( ++si; *si != '\0'; ++si ) {
        denominator_ = 10*denominator_ + (*si - '0');
      }
      if( denominator_ == 0 ) throw std::string("Rational: division by zero");
      long d = Utils::gcd( numerator_, denominator_ );
      numerator_ /= d;
      denominator_ /= d;
    }
    else if( *si == '.' ) {
      long a = numerator_;
      numerator_ = 0;
      for( ++si; *si != '\0'; ++si ) {
        numerator_ = 10*numerator_ + (*si - '0');
        denominator_ *= 10;
      }
      long d = Utils::gcd( numerator_, denominator_ );
      numerator_ /= d;
      denominator_ /= d;
      numerator_ += a*denominator_;
    }
  }

  // Accessors
  long numerator() const { return( numerator_ ); }
  long denominator() const { return( denominator_ ); }
  double double_value() const { return( double(numerator())/denominator() ); }

  // Returns the multipliers for the two integers
  static std::pair<long,long> multipliers(long n, long m) { long f = Utils::lcm(n,m); return( std::make_pair(f/n,f/m) ); }

private:
  long numerator_;
  long denominator_;
};

inline bool operator<( const Rational& q, const Rational& p )
{
  std::pair<long,long> m = Rational::multipliers(q.denominator(),p.denominator());
  return( q.numerator()*m.first < p.numerator()*m.second );
}

inline bool operator<=( const Rational& q, const Rational& p )
{
  std::pair<long,long> m = Rational::multipliers(q.denominator(),p.denominator());
  return( q.numerator()*m.first <= p.numerator()*m.second );
}

inline bool operator==( const Rational& q, const Rational& p )
{
  std::pair<long,long> m = Rational::multipliers(q.denominator(),p.denominator());
  return( q.numerator()*m.first == p.numerator()*m.second );
}

inline bool operator!=( const Rational& q, const Rational& p )
{
  std::pair<long,long> m = Rational::multipliers(q.denominator(),p.denominator());
  return( q.numerator()*m.first != p.numerator()*m.second );
}

inline bool operator>=( const Rational& q, const Rational& p )
{
  std::pair<long,long> m = Rational::multipliers(q.denominator(),p.denominator());
  return( q.numerator()*m.first >= p.numerator()*m.second );
}

inline bool operator>( const Rational& q, const Rational& p )
{
  std::pair<long,long> m = Rational::multipliers(q.denominator(),p.denominator());
  return( q.numerator()*m.first > p.numerator()*m.second );
}

inline Rational operator+( const Rational &q, const Rational &p )
{
  std::pair<long,long> m = Rational::multipliers(q.denominator(),p.denominator());
  return( Rational(q.numerator()*m.first + p.numerator()*m.second, q.denominator()*m.first) );
}

inline Rational operator-( const Rational &q, const Rational &p )
{
  std::pair<long,long> m = Rational::multipliers(q.denominator(),p.denominator());
  return( Rational(q.numerator()*m.first - p.numerator()*m.second, q.denominator()*m.first) );
}

inline Rational operator*( const Rational &q, const Rational &p )
{
  long d1 = Utils::gcd(q.numerator(),p.denominator());
  long d2 = Utils::gcd(p.numerator(),q.denominator());
  return( Rational((q.numerator()/d1)*(p.numerator()/d2), (q.denominator()/d2)*(p.denominator()/d1)) );
}

inline Rational operator/( const Rational &q, const Rational &p )
{
  if( p == 0 ) throw std::string("Rational: division by zero");
  long d1 = Utils::gcd(q.numerator(),p.numerator());
  long d2 = Utils::gcd(p.denominator(),q.denominator());
  return( Rational((q.numerator()/d1)*(p.denominator()/d2), (q.denominator()/d2)*(p.numerator()/d1)) );
}

inline std::ostream& operator<<( std::ostream &os, const Rational &q )
{
  os << q.numerator();
  if( q.denominator() != 1 ) os << '/' << q.denominator();
  return( os );
}

#endif

