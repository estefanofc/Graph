#include "graph.h"
#include <algorithm>
#include <cassert>
#include <climits>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <utility>

using namespace std;

Graph::Graph(bool DirectionalEdges) : DirectionalEdges{DirectionalEdges} {
}

Graph::~Graph() {
    for (const auto &Val:VMap) {
        delete Val.second;
    }
}

// Read edges from file
// first line of file is an integer, indicating number of edges
// each line represents an edge in the form of "string string int"
// vertex labels cannot contain spaces
// @return true if file successfully read
bool Graph::readFile(const string &Filename) {
    ifstream File;
    File.open(Filename);
    int Count;
    string From;
    string To;
    int Weight;
    if (!File) {
        cout << "ERROR: No File Found Matching :" << Filename;
        return false;
    }
    File >> Count;
    for (int I = 0; I < Count; ++I) {
        File >> From >> To >> Weight;
        if (!DirectionalEdges)
            connect(To, From, Weight);
        connect(From, To, Weight);
    }
    File.close();
    return true;
}

// @return total number of vertices
int Graph::verticesSize() const {
    return VMap.size();
}

// @return total number of edges
int Graph::edgesSize() const {
    return EdgesCount;
}

// @return number of edges from given vertex, -1 if vertex not found
int Graph::neighborsSize(const string &Label) const {
    if (VMap.count(Label) == 0)
        return -1;
    return VMap.at(Label)->Neighbors.size();

}

// @return true if vertex added, false if it already is in the graph
bool Graph::add(const string &Label) {
    if (VMap.count(Label) == 1)
        return false;
    VMap[Label] = new Vertex(Label);
    return true;
}

/** return true if vertex already in graph */
bool Graph::contains(const std::string &Label) const {
    return VMap.count(Label) == 1;
}

//edge comparator for ascending sorting
bool Graph::compareEdges(Edge *E1, Edge *E2) {
    int CompareLabels = E1->To->Label.compare(E2->To->Label);
    if (CompareLabels == 0)
        return (E2->Weight - E1->Weight) < 0;
    return CompareLabels < 0;
}

// @return string representing edges and weights, "" if vertex not found
// A-3->B, A-5->C should return B(3),C(5)
string Graph::getEdgesAsString(const string &Label) const {
    if (VMap.count(Label) == 0)
        return "";
    stringstream Ss;
    Vertex *V = VMap.at(Label);
    vector<Edge *> List = V->Neighbors;
    if (List.empty())
        return "";
    sort(List.begin(), List.end(), compareEdges);
    for (int I = 0; I < List.size() - 1; ++I) {
        Ss << *(List[I]->To) << "(" << List[I]->Weight << ")" << ",";
    }
    Ss << *(List[List.size() - 1]->To) << "(" << List[List.size() -
            1]->Weight << ")";
    return Ss.str();
}

// Add an edge between two vertices, create new vertices if necessary
// A vertex cannot connect to itself, cannot have P->P
// For digraphs (directed graphs), only one directed edge allowed, P->Q
// Undirected graphs must have P->Q and Q->P with same weight
// @return true if successfully connected
bool Graph::connect(const string &From, const string &To, int Weight) {
    if (From == To)
        return false;
    Vertex *VFrom;
    Vertex *VTo;
    if (VMap.count(To) == 0) {
        VTo = new Vertex(To);
        VMap[To] = VTo;
    }
    if (VMap.count(From) == 0) {
        VFrom = new Vertex(From);
        VMap[From] = VFrom;
    }
    if (!isConnected(From, To)) {
        VFrom = VMap[From];
        VTo = VMap[To];
        Edge *NewEdge = new Edge(VFrom, VTo, Weight);
        VFrom->Neighbors.push_back(NewEdge);
        EdgesCount++;
        return true;
    }
    return false;
}

// Remove edge from graph
// @return true if edge successfully deleted
bool Graph::disconnect(const string &From, const string &To) {
    bool Disconnected = false;
    Vertex *VFrom = VMap[From];
    Vertex *VTo = VMap[To];
    for (int I = 0; I < VFrom->Neighbors.size(); ++I) {
        if (VFrom->Neighbors[I]->To == VTo) {
            VFrom->Neighbors.erase(VFrom->Neighbors.begin() + I);
            delete VFrom->Neighbors[I];
            Disconnected = true;
            EdgesCount--;
            break;
        }
    }
    if (DirectionalEdges && Disconnected) {
        for (int I = 0; I < VTo->Neighbors.size(); ++I) {
            if (VTo->Neighbors[I]->To == VFrom) {
                VTo->Neighbors.erase(VTo->Neighbors.begin() + I);
                delete VTo->Neighbors[I];
                EdgesCount--;
                break;
            }
        }
    }
    return Disconnected;
}

// depth-first traversal starting from given startLabel
void Graph::dfs(const string &StartLabel, void Visit(const string &Label)) {
    if (VMap.count(StartLabel) == 1) {
        stack<Vertex *> Stk;
        set<Vertex *> Vis;
        Stk.push(VMap[StartLabel]);
        Vis.insert(VMap[StartLabel]);
        while (!Stk.empty()) {
            Vertex *V = Stk.top();
            Stk.pop();
            Visit(V->Label);
            vector<Edge *> List = V->Neighbors;
            sort(List.begin(), List.end(), descendingOrder);
            for (Edge *E : List) {
                if (Vis.count(E->To) == 0) {
                    Vis.insert(E->To);
                    Stk.push(E->To);
                }
            }
        }
    }
}

// breadth-first traversal starting from startLabel
// call the function visit on each vertex label */
void Graph::bfs(const string &StartLabel, void Visit(const string &Label)) {
    if (VMap.count(StartLabel) == 1) {
        queue<Vertex *> Q;
        set<Vertex *> Vis;
        Q.push(VMap[StartLabel]);
        Vis.insert(VMap[StartLabel]);
        while (!Q.empty()) {
            Vertex *V = Q.front();
            Q.pop();
            Visit(V->Label);
            vector<Edge *> List = V->Neighbors;
            sort(List.begin(), List.end(), compareEdges);
            for (Edge *E : List) {
                if (Vis.count(E->To) == 0) {
                    Vis.insert(E->To);
                    Q.push(E->To);
                }
            }
        }
    }
}

// store the weights in a map
// store the previous label in a map
pair<map<string, int>, map<string, string>>
Graph::dijkstra(const string &StartLabel) const {
    map<string, int> Weights;
    map<string, int> Distances;
    map<string, string> Previous;
    if (VMap.count(StartLabel) == 0)
        return make_pair(Weights, Previous);
    priority_queue<VPair, vector<VPair>, CustomCompare> Pq;
    for (const auto &Val: VMap) {
        Distances[Val.first] = INT_MAX;
        Pq.push(make_pair(Val.first, INT_MAX));
    }
    Pq.push(make_pair(StartLabel, 0));
    Distances[StartLabel] = 0;
    while (!Pq.empty()) {
        string Curr = Pq.top().first;
        Pq.pop();
        vector<Edge *> List = VMap.at(Curr)->Neighbors;
        for (Edge *E: List) {
            if (Distances[Curr] != INT_MAX) {
                int Weight = Distances[Curr] + E->Weight;
                if (Weight < Distances[E->To->Label]) {
                    Distances[E->To->Label] = Weight;
                    Previous[E->To->Label] = Curr;
                    Pq.push(make_pair(E->To->Label, Weight));
                }
            }
        }
    }
    for (const auto &Val: Distances) {
        if (Val.second != INT_MAX && Val.second != 0)
            Weights[Val.first] = Val.second;
    }
    return make_pair(Weights, Previous);
}

// minimum spanning tree
// ONLY works for NONDIRECTED graphs
// ASSUMES the edge [P->Q] has the same weight as [Q->P]
// @return length of the minimum spanning tree or -1 if start vertex not
int Graph::mst(const string &StartLabel, void Visit(const string &From, const
string &To, int Weight)) const {
    assert(!DirectionalEdges);
    auto Length = 0;
    if (VMap.count(StartLabel) == 0)
        return -1;
    set<string> Vis;
    map<string, string> Parent;
    map<string, int> Keys;
    priority_queue<VPair, vector<VPair>, CustomCompare> Pq;
    for (const auto &Val: VMap)
        Keys[Val.first] = INT_MAX;
    Pq.push(make_pair(StartLabel, 0));
    Keys[StartLabel] = 0;
    while (!Pq.empty()) {
        string Curr = Pq.top().first;
        Pq.pop();
        Vis.insert(Curr);
        vector<Edge *> List = VMap.at(Curr)->Neighbors;
        for (Edge *E: List) {

            string V = E->To->Label;
            int Weight = E->Weight;
            if (Vis.count(V) == 0 && Keys[V] > Weight) {
                Keys[V] = Weight;
                Pq.push(make_pair(V, Keys[V]));
                Parent[V] = Curr;
            }
        }
    }
    for (const auto &Val: Parent) {
        int W = Keys[Val.first];
        Visit(Val.second, Val.first, W);
        Length += W;
    }
    return Length;
}

//helper method to check if two vertices are connected
bool Graph::isConnected(const string &From, const string &To) const {
    Vertex *VFrom = VMap.at(From);
    Vertex *VTo = VMap.at(To);
    for (auto Neighbor : VFrom->Neighbors) {
        if (Neighbor->To == VTo)
            return true;
    }
    return false;
}

//edge comparator for descending sorting
bool Graph::descendingOrder(Edge *E1, Edge *E2) {
    int CompareLabels = E1->To->Label.compare(E2->To->Label);
    if (CompareLabels == 0)
        return (E2->Weight - E1->Weight) > 0;
    return CompareLabels > 0;
}

