#include "AgentIterator.h"

AgentIteratorBase::AgentIteratorBase(const Child & agent, const std::set<int> & candidates,
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

AgentIteratorBase::~AgentIteratorBase() { }

int AgentIteratorBase::get_group() const {
	return _group;
}

int AgentIteratorBase::get_position() const {
	return _position;
}

const Child & AgentIteratorBase::get_agent() const {
	return _agent;
}

const std::vector<Child> & AgentIteratorBase::get_these() const {
	return _these;
}

const std::vector<Child> & AgentIteratorBase::get_other() const {
	return _other;
}
const std::set<int> & AgentIteratorBase::get_positions() const {
	return _positions;
}
const std::set<int> & AgentIteratorBase::get_candidates() const {
	return _candidates;
}

void AgentIteratorBase::regularIncrement() {
	_position++;
	if (_agent.preferences[_group].size() == _position) {
		_position = 0;
		_group++;
		if (_agent.preferences.size() == _group) {
			_group = -1;
		}
	}
}

bool AgentIterator::operator==(const AgentIterator & other) {
	return get_group() == other.get_group() && get_position() == other.get_position();
}

bool AgentIterator::operator!=(const AgentIterator & other) {
	return ! (*this == other);
}

const std::pair<int, int> AgentIterator::operator*() {
	return std::pair<int, int>(get_group(), get_position());
}

AgentIterator AgentIterator::begin() {
	AgentIterator starter(this, 0, 0);
	starter.base->begin();
	return starter;
}

AgentIterator AgentIterator::end() {
	return AgentIterator(this, -1, 0);
}

AgentIterator::AgentIterator(const Child & agent, const std::set<int> & candidates, const std::set<int> & positions,
			const std::vector<Child> & these, const std::vector<Child> & other, int mode) :
	_mode(mode) {
		switch (_mode) {
			default:
			case 0:
				base = new DescendingIterator(agent, candidates, positions, these, other, 0, 0);
				break;
			case 1:
				base = new SkipBigIterator(agent, candidates, positions, these, other, 5, 0, 0);
				break;
			case 2:
				base = new BestIterator(agent, candidates, positions, these, other, 0, 0);
				break;
			case 3:
				base = new SkipBigIterator(agent, candidates, positions, these, other, 15, 0, 0);
				break;
			case 4:
				base = new SkipBigIterator(agent, candidates, positions, these, other, 50, 0, 0);
				break;
		}
	}

AgentIterator::AgentIterator(AgentIterator *other) :
	_mode(other->get_mode()) {
		switch (_mode) {
			default:
			case 0:
				base = new DescendingIterator(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), other->get_group(), other->get_position());
				break;
			case 1:
				base = new SkipBigIterator(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), 4, other->get_group(), other->get_position());
				break;
			case 2:
				base = new BestIterator(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), other->get_group(), other->get_position());
				break;
		}
	}

AgentIterator::AgentIterator(AgentIterator *other, int group, int posn) :
	_mode(other->get_mode()) {
		switch (_mode) {
			case 0:
				base = new DescendingIterator(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), group, posn);
				break;
			case 1:
				base = new SkipBigIterator(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), 4, group, posn);
				break;
			case 2:
				base = new BestIterator(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), group, posn);
				break;
		}
	}

AgentIterator::~AgentIterator() {
	delete base;
}

int AgentIterator::get_group() const {
	return base->get_group();
}

int AgentIterator::get_position() const {
	return base->get_position();
}

int AgentIterator::get_mode() const {
	return _mode;
}

DescendingIterator::DescendingIterator(const Child & agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<Child> & these,
		const std::vector<Child> & other, int group, int posn) :
	AgentIteratorBase(agent, candidates, positions, these, other, group, posn) {
	}

void DescendingIterator::begin() {
}

void DescendingIterator::increment() {
	regularIncrement();
}

SkipBigIterator::SkipBigIterator(const Child & agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<Child> & these,
		const std::vector<Child> & other, int skip, int group, int posn) :
	AgentIteratorBase(agent, candidates, positions, these, other, group, posn) , _skip(skip) {
	}


void SkipBigIterator::begin() {
	int num_added = 0;
	while (true) {
		if (get_group() == -1) {
			break;
		}
		num_added = 0;
		int other_id = _agent.preferences[_group][_position];
		int other_rank = _agent.ranks[_group][_position];
		for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
			for(auto pref: _other[other_id].preferences[group_ind]) {
				if (_candidates.count(pref) == 0) {
					num_added++;
				}
			}
		}
		if (num_added > _skip) {
			regularIncrement();
		} else {
			break;
		}
	}
}

void SkipBigIterator::increment() {
	int num_added = 0;
	do {
		regularIncrement();
		if (get_group() == -1) {
			break;
		}
		num_added = 0;
		int other_id = _agent.preferences[_group][_position];
		int other_rank = _agent.ranks[_group][_position];
		for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
			for(auto pref: _other[other_id].preferences[group_ind]) {
				if (_candidates.count(pref) == 0) {
					num_added++;
				}
			}
		}
	} while (num_added > _skip);
}

BestIterator::BestIterator(const Child & agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<Child> & these,
		const std::vector<Child> & other, int group, int posn) :
	AgentIteratorBase(agent, candidates, positions, these, other, group, posn) {
	}

void BestIterator::increment() {
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
			int other_rank = _agent.ranks[group_no][posn];
			for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
				for(auto other_pref: _other[other_id].preferences[group_ind]) {
					if (_candidates.count(other_pref) == 0) {
						num_added++;
					}
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
		_position = 0;
	}
}

void BestIterator::begin() {
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
			int other_rank = _agent.ranks[group_no][posn];
			for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
				for(auto other_pref: _other[other_id].preferences[group_ind]) {
					if (_candidates.count(other_pref) == 0) {
						num_added++;
					}
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
		_position = 0;
	}
}
