#include <algorithm>
#include <iostream>
#include "Graph.h"

constexpr decltype(Graph::SOURCE) Graph::SOURCE;
constexpr decltype(Graph::SINK) Graph::SINK;

Graph::Graph(int cap_original) : _cap_original(cap_original), _cap_total(0),
  _left_total(0), max_flow(0) {
#ifdef DEBUG_GRAPH
  std::cout << "New graph" << std::endl; 
#endif /* DEBUG_GRAPH */
  this->addVertex(std::make_pair(2, Graph::SOURCE), 1); // SOURCE
  this->addVertex(std::make_pair(2, Graph::SINK), 1); // SINK
}

void Graph::addVertex(Vertex name, int capacity) {
  int index = size();
  this->_names[name] = index;
  this->_indices[index] = name;
#ifdef DEBUG_GRAPH
  std::cout << "Add vertex " << name << " with cap " << capacity << std::endl;
#endif /* DEBUG_GRAPH */
  this->_adjacents[index] = std::map<int, Edge>();
  if (name.first != 2) {
    Edge e1, e2;
    _cap_total += capacity;
    if (name.first == 0) { // left side
      e1.id = index;
      e1.flow = 0;
      e1.cap = capacity;
      e1.reverse = false;
      e2.id = Graph::SOURCE;
      e2.flow = 0;
      e2.cap = capacity;
      e2.reverse = false;
      _left_total += capacity;
    } else if (name.first == 1) { // right side
      e1.id = index;
      e1.flow = 0;
      e1.cap = capacity;
      e1.reverse = false;
      e2.id = Graph::SINK;
      e2.flow = 0;
      e2.cap = capacity;
      e2.reverse = false;
    }
    this->_adjacents[e1.id][e2.id] = e2;
    this->_adjacents[e2.id][e1.id] = e1;
  }
}

Vertex Graph::name(int vert_index) {
  return this->_indices[vert_index];
}

bool Graph::containsVertex(Vertex name) const {
  return _names.find(name) != _names.end();
}

/**
 * Adds an edge to the graph. Note that this edge must always be added in the
 * form (right, left) for things to work.
 */
void Graph::addEdge(Vertex v1, Vertex v2) {
  int i1 = this->_names[v1];
  int i2 = this->_names[v2];
  Edge e1, e2;
  e1.id = i1;
  e1.cap = std::numeric_limits<int>::max();
  e1.flow = 0;
  e1.reverse = false;
  e2.id = i2;
  e2.cap = std::numeric_limits<int>::max();
  e2.flow = 0;
  e2.reverse = true;
  _adjacents[i1][i2] = e2;
  _adjacents[i2][i1] = e1;
}

int Graph::size() const {
  return _names.size();
}

int Graph::maxFlow() const {
  return max_flow;
}

bool Graph::can_preprocess() {
  return _cap_original + cap_left() <= _cap_total - max_flow;
}

int Graph::cap_left() const {
  return _left_total;
}

int Graph::cap_right() const {
  return _cap_total - _left_total;
}

int Graph::cap_total() const {
  return _cap_total;
}

/**
 * Augment the flow.
 */
bool Graph::augment() {
#ifdef DEBUG_GRAPH
  std::cout << "Augmenting" << std::endl;
  this->printGraph();
#endif /* DEBUG_GRAPH */
  std::list<int> path;
  std::vector<bool> visited(size(), false);
  visited[Graph::SOURCE] = true;
  path.push_back(Graph::SOURCE);
  return internal_augment(Graph::SOURCE, visited, path);
}

#ifdef DEBUG_GRAPH
void Graph::printGraph() {
  for(auto & tuple : _adjacents) {
    int ind = tuple.first;
    std::cout << _indices[ind] << ":";
    for(auto & inner_tuple: tuple.second) {
      const Edge & adj = inner_tuple.second;
      std::cout << " " << _indices[adj.id] << "{" << adj.flow << "/" << adj.cap << "}";
    }
    std::cout << std::endl;
  }
}

#endif /* DEBUG_GRAPH */

/**
 * Continues an augmentation, on vertex now, which is on the right.
 */
bool Graph::internal_augment(int now, std::vector<bool> & visited,
    std::list<int> & path) {
  for(auto & tuple: _adjacents[now]) {
    Edge & next_edge = tuple.second;
    if (visited[next_edge.id]) {
      continue;
    }
    // Can't add any more flow to this edge.
    if (next_edge.flow >= next_edge.cap) {
      continue;
    }
    // If we're going "backwards" we need to use up (subtract) flow, so check we
    // have some. Note that since we are checking backwards, the flow will also
    // be negative
    if (next_edge.reverse) {
      if (next_edge.flow >= 0) {
        continue;
      }
    }
    //std::cout << "Considering " << _indices[next_edge.id] << " which has " << next_edge.flow << "/" << next_edge.cap << std::endl;
    if (next_edge.id == Graph::SINK) {
      // Found an augmenting flow. Modify flows used.
#ifdef DEBUG_GRAPH
      std::cout << "New flow found." << std::endl;
#endif /* DEBUG_GRAPH */
      path.push_back(next_edge.id);
      int start = Graph::SOURCE;
      int total_flow = std::numeric_limits<int>::max();
#ifdef DEBUG_GRAPH
      std::cout << "Path is ";
#endif /* DEBUG_GRAPH */
      for(auto p: path) {
        if (p != Graph::SOURCE) {
          int new_flow;
          if (next_edge.reverse) {
            new_flow = _adjacents[start][p].flow;
          } else {
            new_flow = _adjacents[start][p].cap - _adjacents[start][p].flow;
          }
          if (_adjacents[start][p].flow < 0 && _adjacents[start][p].cap == std::numeric_limits<int>::max()) {
            new_flow = std::numeric_limits<int>::max();
          }
          total_flow = std::min(total_flow, new_flow);
        }
#ifdef DEBUG_GRAPH
        if (p != Graph::SOURCE) {
          int new_flow;
          if (next_edge.reverse) {
            new_flow = _adjacents[start][p].flow;
          } else {
            new_flow = _adjacents[start][p].cap - _adjacents[start][p].flow;
          }
          if (_adjacents[start][p].flow < 0 && _adjacents[start][p].cap == std::numeric_limits<int>::max()) {
            new_flow = std::numeric_limits<int>::max();
          }
          std::cout << "f" << new_flow;
        }
        std::cout << " " << _indices.at(p);
#endif /* DEBUG_GRAPH */
        start = p;
      }
#ifdef DEBUG_GRAPH
      std::cout << " with flow " << total_flow << std::endl;
#endif /* DEBUG_GRAPH */
      start = Graph::SOURCE;
      while(!path.empty()) {
        int next = path.front();
        path.pop_front();
        if (next == Graph::SOURCE) {
          continue;
        }
        _adjacents[start][next].flow += total_flow;
        _adjacents[next][start].flow -= total_flow;
        start = next;
      }
      max_flow += total_flow;
      return true;
    }
    path.push_back(next_edge.id);
    visited[next_edge.id] = true;
    if (internal_augment(next_edge.id, visited, path)) {
      return true;
    }
    path.pop_back();
  }
  return false;
}

