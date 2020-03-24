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

#include "vertex.h"
#include "edge.h"
#include <algorithm>

using namespace std;
//overloaded ostream operator
ostream &operator<<(ostream &Os, const Vertex &V) {
    Os << V.Label;
    return Os;
}

/** Creates an unvisited vertex, gives it a label, and clears its
    adjacency list.
    NOTE: A vertex must have a unique label that cannot be changed. */
Vertex::Vertex(const string &Label) : Label{Label} {}

/** Destructor. Delete all edges from this vertex to other vertices */
Vertex::~Vertex() {
    for (auto E: Neighbors)
        delete E;
}
