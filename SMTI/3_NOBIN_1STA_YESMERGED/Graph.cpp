#include <iostream>
#include "Graph.h"



template<class t_name>
Graph<t_name>::Graph() {
#ifdef DEBUG
  std::cout << "New graph" << std::endl; 
#endif /* DEBUG */
}

template<class t_name>
void Graph<t_name>::addVertex(t_name name, bool open) {
  int index = size();
  this->_names[name] = index;
  this->_indices[index] = name;
#ifdef DEBUG
  std::cout << "Add vertex " << name << std::endl;
#endif /* DEBUG */
  this->_adjacents.emplace_back();
}

template<class t_name>
t_name Graph<t_name>::name(int vert_index) {
  return this->_indices[vert_index];
}

template<class t_name>
bool Graph<t_name>::containsVertex(t_name name) const {
  return _names.find(name) != _names.end();
}

/**
 * Adds an edge to the graph. Note that this edge must always be added in the
 * form (right, left) for things to work.
 */
template<class t_name>
void Graph<t_name>::addEdge(t_name v1, t_name v2) {
  int i1 = this->_names[v1];
  int i2 = this->_names[v2];
  _adjacents[i1].push_back(i2);
  _adjacents[i2].push_back(i1);
}

template<class t_name>
int Graph<t_name>::size() const {
  return _names.size();
}

template<class t_name>
int Graph<t_name>::matchingSize() const {
  // for a matched edge (a,b), our _matching map stores both _matching[a] = b
  // and _matching[b] = a.
  return _matching.size() / 2;
}

/**
 * Augment the matching, starting at vertex name which is on the right.
 */
template<class t_name>
void Graph<t_name>::augment(t_name name) {
  int start = this->_names[name];
#ifdef DEBUG
  std::cout << "Augmenting on " << _indices.at(start) << std::endl;
  this->printGraph();
#endif /* DEBUG */
  std::list<int> path;
  std::vector<bool> visited(size(), false);
  path.push_back(start);
  internal_augment(start, visited, path);
}

#ifdef DEBUG
template<class t_name>
void Graph<t_name>::printGraph() {
  for(size_t ind = 0; ind < _adjacents.size(); ++ind) {
    std::cout << _indices[ind] << ":";
    for(auto adj: _adjacents[ind]) {
      std::cout << " " << _indices[adj];
    }
    std::cout << std::endl;
  }
}

template<class t_name>
void Graph<t_name>::printMatching() const {
  for(auto p: _matching) {
    int a = p.first;
    int b = p.second;
    if (a <= b) {
      std::cout << _indices.at(a) << " is matched to " << _indices.at(b) << std::endl;
    }
  }
}

#endif /* DEBUG */

/**
 * Continues an augmentation, on vertex now, which is on the right.
 */
template<class t_name>
bool Graph<t_name>::internal_augment(int now, std::vector<bool> & visited,
    std::list<int> & path) {
  for(int next: _adjacents[now]) {
    if (visited[next]) {
      continue;
    }
    auto matched = _matching.find(next);
    if (matched == _matching.end()) {
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
        _matching[right] = left;
        _matching[left] = right;
      }

      return true;
    }
    int next2 = matched->second;
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

template class Graph<std::pair<int,int>>;
