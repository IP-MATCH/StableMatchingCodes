#include <iostream>
#include "Graph.h"


// Expected biggest graph, to save on allocations
const size_t expected_size = 4096;

Graph::Graph() : _exists(2), _adjacents(2), _matching(2), _size(0), _matching_size(0) {
  _exists[0] = std::vector<bool>(expected_size, false);
  _exists[1] = std::vector<bool>(expected_size, false);
  _adjacents[0] = std::vector<std::vector<int>>(expected_size);
  _adjacents[1] = std::vector<std::vector<int>>(expected_size);
  _matching[0] = std::vector<signed int>(expected_size, -1);
  _matching[1] = std::vector<signed int>(expected_size, -1);
#ifdef DEBUG
  std::cout << "New graph" << std::endl; 
#endif /* DEBUG */
}

void Graph::addVertex(int side, int name) {
	_exists[side][name] = true;
	_size += 1;
}

bool Graph::containsVertex(int side, int name) const {
  return _exists[side][name];
}

/**
 * Adds an edge to the graph. Note that this edge must always be added in the
 * form (right, left) for things to work.
 */
void Graph::addEdge(int v1, int v2) {
	_adjacents[1][v1].push_back(v2);
	_adjacents[0][v2].push_back(v1);
}

int Graph::size() const {
  return _size;
}

int Graph::matchingSize() const {
  return _matching_size;
}

/**
 * Augment the matching, starting at vertex name which is on the right.
 */
void Graph::augment(int name) {
#ifdef DEBUG
  std::cout << "Augmenting on " << _indices.at(start) << std::endl;
  this->printGraph();
#endif /* DEBUG */
  std::list<int> path;
  std::vector<bool> visited(expected_size, false);
  path.push_back(name);
  internal_augment(name, visited, path);
}

/**
 * Continues an augmentation, on vertex now, which is on the right.
 */
bool Graph::internal_augment(int now, std::vector<bool> & visited,
    std::list<int> & path) {
  for(int next: _adjacents[1][now]) {
    // next is on the left
    if (visited[next]) {
      continue;
    }
    if (_matching[0][next] == -1) {
      // Found an augmenting path. Switch edges and return true.
#ifdef DEBUG
      std::cout << "New matching found." << std::endl;
#endif /* DEBUG */
      path.push_back(next);
#ifdef DEBUG
      std::cout << "Path is ";
      for(auto p: path) {
        std::cout << " " << _indices.at(p);
      }
      std::cout << std::endl;

#endif /* DEBUG */
      while(!path.empty()) {
        int right = path.front();
        path.pop_front();
        int left = path.front();
        path.pop_front();
        _matching[1][right] = left;
        _matching[0][left] = right;
      }
      _matching_size += 1;
      return true;
    }
    int next2 = _matching[0][next];
    path.push_back(next);
    path.push_back(next2);
    visited[next] = true;
    if (internal_augment(next2, visited, path)) {
      return true;
    }
    path.pop_back();
    path.pop_back();
  }
  return false;
}

