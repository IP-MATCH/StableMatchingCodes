#include "AgentIterator.h"

AgentIterator::AgentIterator(const Child & agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<Child> & these,
		const std::vector<Child> & other, int group, int posn) :
	_agent(agent),
	_these(these), _other(other),
	_positions(positions), _candidates(candidates) {
		if (_agent.preferences.size() == 0) {
			_group = -1;
		} else {
		_group = group;
		}
		_position = posn;
}

bool AgentIterator::operator==(const AgentIterator & other) {
	return _group == other.get_group() && _position == other.get_position();
}

bool AgentIterator::operator!=(const AgentIterator & other) {
	return ! (*this == other);
}

const std::pair<int, int> AgentIterator::operator*() {
	return std::pair<int, int>(_group, _position);
}

AgentIterator& AgentIterator::operator++() {
	_position++;
	if (_agent.preferences[_group].size() == _position) {
		_position = 0;
		_group++;
		if (_agent.preferences.size() == _group) {
			_group = -1;
		}
	}
	return *this;
}

AgentIterator AgentIterator::operator++(int) {
	AgentIterator ret = *this;
	_position++;
	if (_agent.preferences[_group].size() == _position) {
		_position = -1;
		_group++;
		if (_agent.preferences.size() == _group) {
			_group = -1;
		}
	}
	return ret;
}

AgentIterator AgentIterator::begin() {
	return AgentIterator(_agent, _candidates, _positions, _these, _other, 0, 0);
}

AgentIterator AgentIterator::end() {
	return AgentIterator(_agent, _candidates, _positions, _these, _other, -1, 0);
}

int AgentIterator::get_group() const {
	return _group;
}

int AgentIterator::get_position() const {
	return _position;
}


template <int step>
SkipBigIterator<step>::SkipBigIterator(const Child & agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<Child> & these,
		const std::vector<Child> & other, int group, int posn) :
	AgentIterator(agent, candidates, positions, these, other, group, posn) {
	}

template <int step>
SkipBigIterator<step>& SkipBigIterator<step>::operator++() {
	int num_added;
	do {
		AgentIterator::operator++();
		num_added = 0;
		int other_id = _agent.preferences[_group][_position];
		for(auto group: _other[other_id].preferences) {
			bool found = false;
			for(auto pref: group) {
				if (_candidates.count(pref) == 0) {
					num_added++;
				}
				if (pref == _agent.id) {
					found = true;
				}
			}
			if (found) {
				break;
			}
		}
	} while ((*this != end()) && num_added > step);
	return *this;
}

template <int step>
SkipBigIterator<step> SkipBigIterator<step>::operator++(int) {
	SkipBigIterator<step> ret = *this;
	int num_added = 0;
	do {
		AgentIterator::operator++();
		num_added = 0;
		int other_id = _agent.preferences[_group][_position];
		for(auto group: _other[other_id].preferences) {
			bool found = false;
			for(auto pref: group) {
				if (_candidates.count(pref) == 0) {
					num_added++;
				}
				if (pref == _agent.id) {
					found = true;
				}
			}
			if (found) {
				break;
			}
		}
	} while ((*this != end()) && num_added > step);
	return ret;
}

template class SkipBigIterator<5>;


BestIterator::BestIterator(const Child & agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<Child> & these,
		const std::vector<Child> & other, int group, int posn) :
	AgentIterator(agent, candidates, positions, these, other, group, posn) {
	}

BestIterator& BestIterator::operator++() {
	int lowest_added = -1;
	int best_group = -1;
	int best_posn = -1;
	for(int group_no = 0; group_no < _agent.preferences.size(); ++group_no) {
		auto & group = _agent.preferences[group_no];
		for(int posn = 0; posn < group.size(); ++posn) {
			int other_id = group[posn];
			if (_positions.count(other_id) != 0) {
				continue;
			}
			int num_added = 0;
			bool found = false;
			for(auto & other_group: _other[other_id].preferences) {
				for(auto other_pref: other_group) {
					if (_candidates.count(other_pref) == 0) {
						num_added++;
					}
					if (other_pref == _agent.id) {
						found = true;
					}
				}
				if (found) {
					break;
				}
			}
			if ((lowest_added == -1) || (num_added < lowest_added)) {
				lowest_added = num_added;
				best_group = group_no;
				best_posn = posn;
			}
		}
	}
	if (lowest_added != -1) {
		_group = best_group;
		_position = best_posn;
	} else {
		_group = -1;
		_position = -1;
	}
	return *this;
}

BestIterator BestIterator::operator++(int) {
	BestIterator ret = *this;
	int lowest_added = -1;
	int best_group = -1;
	int best_posn = -1;
	for(int group_no = 0; group_no < _agent.preferences.size(); ++group_no) {
		auto & group = _agent.preferences[group_no];
		for(int posn = 0; posn < group.size(); ++posn) {
			int other_id = group[posn];
			if (_positions.count(other_id) != 0) {
				continue;
			}
			int num_added = 0;
			bool found = false;
			for(auto & other_group: _other[other_id].preferences) {
				for(auto other_pref: other_group) {
					if (_candidates.count(other_pref) == 0) {
						num_added++;
					}
					if (other_pref == _agent.id) {
						found = true;
					}
				}
				if (found) {
					break;
				}
			}
			if ((lowest_added == -1) || (num_added < lowest_added)) {
				lowest_added = num_added;
				best_group = group_no;
				best_posn = posn;
			}
		}
	}
	if (lowest_added != -1) {
		_group = best_group;
		_position = best_posn;
	} else {
		_group = -1;
		_position = -1;
	}
	return ret;
}
