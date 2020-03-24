/**
 * Edge is the simplest structure of the graph
 * All edges are directed
 * Each edge belongs to a vertex
 */

#include "edge.h"
#include "vertex.h"
#include <iostream>

//destructor
Edge::~Edge() = default;

/** constructor with label and weight */
Edge::Edge(Vertex *From, Vertex *To, int Weight) : From{From}, To{To},
                                                   Weight{Weight} {
}

