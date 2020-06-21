#include <algorithm>
#include <iostream>
#include "Graph.h"


Graph::Graph(int cap_original, size_t expected_size) :
  _expected_size(expected_size), _cap_original(cap_original), _cap_total(0),
  _left_total(0), max_flow(0), _exists(2), _adjmat(expected_size),
  _adjacents(expected_size), _cap_remaining(expected_size),
  _on_right(expected_size) {
#ifdef DEBUG_GRAPH
  std::cout << "New graph" << std::endl;
#endif /* DEBUG_GRAPH */
  this->_exists[0] = std::vector<char>(_expected_size, (char)false);
  this->_exists[1] = std::vector<char>(_expected_size, (char)false);
}

Graph::~Graph() {
  for(auto & tuple: this->_names) {
    auto & name = tuple.second;
    auto & adj = _adjacents[name];
    for(auto & e: adj) {
      delete e;
    }
  }
}

void Graph::addVertex(Vertex name, int capacity) {
  int index = size();
  this->_names[name] = index;
  this->_indices[index] = name;
#ifdef DEBUG_GRAPH
  std::cout << "Add vertex " << name << " with cap " << capacity << std::endl;
#endif /* DEBUG_GRAPH */
  this->_adjmat[index] = std::vector<Edge*>(_expected_size, nullptr);
  this->_adjacents[index] = std::vector<Edge*>();
  _cap_total += capacity;
  this->_cap_remaining[index] = capacity;
  if (name.first == 1) {
    this->_on_right[index] = (char)true;
  } else {
    this->_on_right[index] = (char)false;
    this->_left_total += capacity;
  }
  this->_exists[name.first][name.second] = (char)true;
}

Vertex Graph::name(int vert_index) {
  return this->_indices[vert_index];
}

bool Graph::containsVertex(Vertex name) const {
  return (bool)this->_exists[name.first][name.second];
}

/**
 * Adds an edge to the graph. Note that this edge must always be added in the
 * form (right, left) for things to work.
 */
void Graph::addEdge(Vertex v1, Vertex v2) {
  int i1 = this->_names[v1];
  int i2 = this->_names[v2];
  Edge *e1 = new Edge;
  Edge *e2 = new Edge;
  e1->id = i1;
  e1->cap = 1;
  e1->flow = 0;
  e1->reverse = true;
  e2->id = i2;
  e2->cap = 1;
  e2->flow = 0;
  e2->reverse = false;
  _adjacents[i1].push_back(e2);
  _adjacents[i2].push_back(e1);
  _adjmat[i1][i2] = e2;
  _adjmat[i2][i1] = e1;
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
bool Graph::augment(Vertex source) {
  std::list<int_id> path;
  int source_name = this->_names[source];
#ifdef DEBUG_GRAPH
  this->printGraph();
  std::cout << "Augmenting from " << source << std::endl;
#endif /* DEBUG_GRAPH */
  std::vector<char> visited(size(), false);
  visited[source_name] = (char)true;
  path.push_back(source_name);
  return internal_augment(source_name, visited, path);
}

#ifdef DEBUG_GRAPH
void Graph::printGraph() {
  for(auto & tuple: this->_names) {
    auto & name = tuple.second;
    auto & adj = _adjacents[name];
    std::cout << tuple.first << " (" << _cap_remaining[name] << ") :";
    std::cout << adj.size() << " edges";
    for(auto & e: adj) {
      std::cout << " " << _indices[e->id] << "{" << e->flow << "/" << e->cap << "}";
    }
    std::cout << std::endl;
  }
}
#endif /* DEBUG_GRAPH */

/**
 * Continues an augmentation, on vertex now
 */
bool Graph::internal_augment(int_id now, std::vector<char> & visited,
    std::list<int_id> & path) {
#ifdef DEBUG_GRAPH
  std::cout << "Augment path is";
  for(auto & p: path) {
    std::cout << " " << _indices[p];
  }
  std::cout << std::endl;
#endif /* DEBUG_GRAPH */
  for(size_t i = 0; i < _adjacents[now].size(); ++i) {
    const Edge next_edge = *_adjacents[now][i];
#ifdef DEBUG_GRAPH
    std::cout << "try " << _indices[next_edge.id];
    if (next_edge.reverse) {
      std::cout << " which is reverse";
    }
    std::cout << std::endl;
#endif /* DEBUG_GRAPH */
    if ((bool)visited[next_edge.id]) {
      continue;
    }
    if (_on_right[now]) {
      // We're on right, going left, need capacity available.
      // Note that next_edge will be in reverse, so it will have negative flow
      if ((-next_edge.flow) == next_edge.cap) {
        continue;
      }
      // next_edge.id will be on left. If it has capacity, remaining, done!
      if (_cap_remaining[next_edge.id] > 0) {
        // Found an augmenting flow. Modify flows used.
        _cap_remaining[next_edge.id] -= 1;
        path.push_back(next_edge.id);

#ifdef DEBUG_GRAPH
        std::cout << "Found augmenting path:";
        for(auto & p: path) {
          std::cout << " " << _indices[p];
        }
        std::cout << std::endl;
#endif /* DEBUG_GRAPH */
        int start = path.front();
        path.pop_front();
        int next = path.front();
        path.pop_front();
        // Start is on the right, and next is on the left
        // Thus we need to increase the flow from next to start
        _adjmat[start][next]->flow -= 1;
        _adjmat[next][start]->flow += 1;
        start = next;
        while(!path.empty()) {
          next = path.front();
          path.pop_front();
          // Now start is on the left, and next is on the right
          // So increase from start to next
          // And reduce from next to start
          _adjmat[start][next]->flow += 1;
          _adjmat[next][start]->flow -= 1;
          start = next;
          next = path.front();
          path.pop_front();
          // At this point, start is on the right, and next is on the left
          // Thus we need to increase the flow from next to start
          _adjmat[start][next]->flow -= 1;
          _adjmat[next][start]->flow += 1;
          start = next;
        }
        max_flow += 1;
        return true;
      }
    } else {
      // We're on the left, so we want this edge to be used
      if (next_edge.flow <= 0) {
        continue;
      }
    }
    path.push_back(next_edge.id);
    visited[next_edge.id] = (char)true;
    if (internal_augment(next_edge.id, visited, path)) {
      return true;
    }
    path.pop_back();
  }
  return false;
}

