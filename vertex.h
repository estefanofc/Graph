/**
 * A Graph is made up of Vertex objects that hold data values
 * A vertex is connected to other vertices via Edges
 * A vertex can be visited/unvisited
 * Can connect to another vertex via directed edge with weight
 * The edge can be disconnected
 * A vertex cannot have an edge back to itself
 * getNextNeighbor returns the next neighbor each time it is called
 * when there are no more neighbors, the vertex label is returned
 */

#ifndef VERTEX_H
#define VERTEX_H

#include "edge.h"
#include <string>
#include <vector>

using namespace std;

class Vertex {
  friend class Graph;

  friend class Edge;

  //overloaded ostream operator
  friend ostream &operator<<(ostream &Os, const Vertex &V);

public:
  /** Creates an unvisited vertex, gives it a label, and clears its
      adjacency list.
      NOTE: A vertex must have a unique label that cannot be changed. */
  explicit Vertex(const string &Label = "");

  /** Destructor. Delete all edges from this vertex to other vertices */
  ~Vertex();

private:
  string Label;
  vector<Edge *> Neighbors;
};

#endif  //  ASS3_GRAPHS_VERTEX_H
