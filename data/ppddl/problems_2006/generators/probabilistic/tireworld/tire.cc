#include <iostream>
#include <iomanip>
#include <set>
#include <vector>

void
dfs( int n, std::vector<bool> &nodes, const std::set<std::pair<int,int> > &edges )
{
  if( nodes[n] )
    return;
  else {
    nodes[n] = true;
    for( std::set<std::pair<int,int> >::const_iterator ei = edges.begin(); ei != edges.end(); ++ei ) {
      if( (*ei).first == n )
        dfs( (*ei).second, nodes, edges );
      else if( (*ei).second == n )
        dfs( (*ei).first, nodes, edges );
    }
  }
}

bool
connected( int n, const std::set<std::pair<int,int> > &edges )
{
  std::vector<bool> nodes(n,false);
  dfs(0,nodes,edges);
  for( int i = 0; i < n; ++i ) if( !nodes[i] ) return(false);
  return(true);
}

void
make_random_connected_graph( int n_nodes, int n_edges, std::set<std::pair<int,int> > &edges )
{
  // generate first set of edges
  while( (int)edges.size() < n_edges ) {
    int p = lrand48()%(n_nodes-1);
    int q = p + 1 + (lrand48()%(n_nodes-p-1));
    edges.insert( std::make_pair(p,q) );
  }

  // add edges until connectedness
  while( !connected(n_nodes,edges) ) {
    int p = lrand48()%(n_nodes-1);
    int q = p + 1 + (lrand48()%(n_nodes-p-1));
    edges.insert( std::make_pair(p,q) );
  }
}

int
random_walk( int n, int steps, const std::set<std::pair<int,int> > &edges )
{
  if( steps == 0 )
    return(n);
  else {
    std::vector<int> neighbors;
    for( std::set<std::pair<int,int> >::const_iterator ei = edges.begin(); ei != edges.end(); ++ei ) {
      if( (*ei).first == n )
        neighbors.push_back((*ei).second);
      else if( (*ei).second == n )
        neighbors.push_back((*ei).first);
    }
    return( random_walk(neighbors[lrand48()%neighbors.size()],--steps,edges) );
  }
}

void
make_problem( std::ostream &os, int suffix, int n_nodes, int n_edges, float prob )
{
  os << "(define (problem tire_" << n_nodes << "_" << n_edges << "_" << suffix << ")" << std::endl
     << "  (:domain tire)" << std::endl
     << "  (:objects ";
  for( int i = 0; i < n_nodes; ++i ) os << "n" << i << " ";
  os << "- location)" << std::endl;

  std::set<std::pair<int,int> > edges;
  make_random_connected_graph(n_nodes,n_edges,edges);
  int init = lrand48()%n_nodes, goal = init;
  while( goal == init ) goal = random_walk(init,100*n_nodes,edges);

  os << "  (:init (vehicle-at n" << init << ")" << std::endl;
  for( std::set<std::pair<int,int> >::const_iterator ei = edges.begin(); ei != edges.end(); ++ei ) {
    int p = (*ei).first, q = (*ei).second;
    os << "         (road n" << p << " n" << q << ") (road n" << q << " n" << p << ")" << std::endl;
  }
  for( int n = 0; n < n_nodes; ++n ) {
    if( drand48() < prob ) os << "         (spare-in n" << n << ")" << std::endl;
  }
  os << "         (not-flattire)" << std::endl
     << "  )" << std::endl
     << "  (:goal (vehicle-at n" << goal << "))" << std::endl
     << ")" << std::endl;
}

int
main( int argc, char **argv )
{
  if( argc == 1 ) {
  usage:
    std::cout << "usage: tire [--seed <seed>] [--edges <m>] [--nodes <n>] [--prob <tire-prob>]" << std::endl;
    exit(-1);
  }

  unsigned short seed[3] = {0,0,0};
  int n_nodes = 0;
  int n_edges = 0;
  float prob = 0.5;

  --argc;
  ++argv;
  while( argc && (**argv == '-') ) {
    if( !strcmp(*argv,"--seed") ) {
      seed[0] = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--edges") ) {
      n_edges = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--nodes") ) {
      n_nodes = atoi(argv[1]);
      argc -= 2;
      argv += 2;
    }
    else if( !strcmp(*argv,"--prob") ) {
      prob = atof(argv[1]);
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

  make_problem(std::cout,seed[0],n_nodes,n_edges,prob);
  return(0);
}

