#include "AgentIterator.h"
#include "Allocation.h"

template <typename t_these, typename t_other>
AgentIteratorBase<t_these, t_other>::AgentIteratorBase(const t_these& agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<t_these> & these,
		const std::vector<t_other> & other, int group, int posn) :
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

template <typename t_these, typename t_other>
AgentIteratorBase<t_these, t_other>::~AgentIteratorBase() { }

template <typename t_these, typename t_other>
int AgentIteratorBase<t_these, t_other>::get_group() const {
	return _group;
}

template <typename t_these, typename t_other>
int AgentIteratorBase<t_these, t_other>::get_position() const {
	return _position;
}

template <typename t_these, typename t_other>
const t_these & AgentIteratorBase<t_these, t_other>::get_agent() const {
	return _agent;
}

template <typename t_these, typename t_other>
const std::vector<t_these> & AgentIteratorBase<t_these, t_other>::get_these() const {
	return _these;
}

template <typename t_these, typename t_other>
const std::vector<t_other> & AgentIteratorBase<t_these, t_other>::get_other() const {
	return _other;
}

template <typename t_these, typename t_other>
const std::set<int> & AgentIteratorBase<t_these, t_other>::get_positions() const {
	return _positions;
}

template <typename t_these, typename t_other>
const std::set<int> & AgentIteratorBase<t_these, t_other>::get_candidates() const {
	return _candidates;
}

template <typename t_these, typename t_other>
void AgentIteratorBase<t_these, t_other>::regularIncrement() {
	_position++;
	if ((int)_agent.preferences[_group].size() == _position) {
		_position = 0;
		_group++;
		if ((int)_agent.preferences.size() == _group) {
			_group = -1;
		}
	}
}

template <typename t_these, typename t_other>
bool AgentIterator<t_these, t_other>::operator==(const AgentIterator & other) {
	return get_group() == other.get_group() && get_position() == other.get_position();
}

template <typename t_these, typename t_other>
bool AgentIterator<t_these, t_other>::operator!=(const AgentIterator & other) {
	return ! (*this == other);
}

template <typename t_these, typename t_other>
const std::pair<int, int> AgentIterator<t_these, t_other>::operator*() {
	return std::pair<int, int>(get_group(), get_position());
}

template <typename t_these, typename t_other>
AgentIterator<t_these, t_other> AgentIterator<t_these, t_other>::begin() {
	AgentIterator starter(this, 0, 0);
	starter.base->begin();
	return starter;
}

template <typename t_these, typename t_other>
AgentIterator<t_these, t_other> AgentIterator<t_these, t_other>::end() {
	return AgentIterator(this, -1, 0);
}

template <typename t_these, typename t_other>
AgentIterator<t_these, t_other>::AgentIterator(const t_these & agent, const std::set<int> & candidates, const std::set<int> & positions,
			const std::vector<t_these> & these, const std::vector<t_other> & other, int mode) :
	_mode(mode) {
		switch (_mode) {
			default:
			case 0:
				base = new DescendingIterator<t_these, t_other>(agent, candidates, positions, these, other, 0, 0);
				break;
			case 1:
				base = new SkipBigIterator<t_these, t_other>(agent, candidates, positions, these, other, 5, 0, 0);
				break;
			case 2:
				base = new BestIterator<t_these, t_other>(agent, candidates, positions, these, other, 0, 0);
				break;
			case 3:
				base = new SkipBigIterator<t_these, t_other>(agent, candidates, positions, these, other, 15, 0, 0);
				break;
			case 4:
				base = new SkipBigIterator<t_these, t_other>(agent, candidates, positions, these, other, 50, 0, 0);
				break;
			case 5:
				base = new BestGroupIterator<t_these, t_other>(agent, candidates, positions, these, other, 0, 0);
				break;
		}
	}

template <typename t_these, typename t_other>
AgentIterator<t_these, t_other>::AgentIterator(AgentIterator *other) :
	_mode(other->get_mode()) {
		switch (_mode) {
			default:
			case 0:
				base = new DescendingIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), other->get_group(), other->get_position());
				break;
			case 1:
				base = new SkipBigIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), 5, other->get_group(), other->get_position());
				break;
			case 2:
				base = new BestIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), other->get_group(), other->get_position());
				break;
			case 3:
				base = new SkipBigIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), 15, other->get_group(), other->get_position());
				break;
			case 4:
				base = new SkipBigIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), 50, other->get_group(), other->get_position());
				break;
			case 5:
				base = new BestGroupIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), other->get_group(), other->get_position());
				break;
		}
	}

template <typename t_these, typename t_other>
AgentIterator<t_these, t_other>::AgentIterator(AgentIterator *other, int group, int posn) :
	_mode(other->get_mode()) {
		switch (_mode) {
			case 0:
				base = new DescendingIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), group, posn);
				break;
			case 1:
				base = new SkipBigIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), 5, group, posn);
				break;
			case 2:
				base = new BestIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), group, posn);
				break;
			case 3:
				base = new SkipBigIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), 15, group, posn);
				break;
			case 4:
				base = new SkipBigIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), 50, group, posn);
				break;
			case 5:
				base = new BestGroupIterator<t_these, t_other>(other->get_agent(), other->get_candidates(), other->get_positions(),
						other->get_these(), other->get_other(), group, posn);
				break;
		}
	}

template <typename t_these, typename t_other>
AgentIterator<t_these, t_other>::~AgentIterator() {
	delete base;
}

template <typename t_these, typename t_other>
int AgentIterator<t_these, t_other>::get_group() const {
	return base->get_group();
}

template <typename t_these, typename t_other>
int AgentIterator<t_these, t_other>::get_position() const {
	return base->get_position();
}

template <typename t_these, typename t_other>
int AgentIterator<t_these, t_other>::get_mode() const {
	return _mode;
}

template <typename t_these, typename t_other>
DescendingIterator<t_these, t_other>::DescendingIterator(const t_these & agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<t_these> & these,
		const std::vector<t_other> & other, int group, int posn) :
	AgentIteratorBase<t_these, t_other>(agent, candidates, positions, these, other, group, posn) {
	}

template <typename t_these, typename t_other>
void DescendingIterator<t_these, t_other>::begin() {
}

template <typename t_these, typename t_other>
void DescendingIterator<t_these, t_other>::increment() {
	this->regularIncrement();
}

template <typename t_these, typename t_other>
SkipBigIterator<t_these, t_other>::SkipBigIterator(const t_these& agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<t_these> & these,
		const std::vector<t_other> & other, int skip, int group, int posn) :
	AgentIteratorBase<t_these, t_other>(agent, candidates, positions, these, other, group, posn) , _skip(skip) {
	}

template <>
void SkipBigIterator<Doctor,Hospital>::begin() {
	int num_added = 0;
	while (true) {
		if (this->get_group() == -1) {
			break;
		}
		num_added = 0;
		int other_id = this->_agent.preferences[this->_group][this->_position];
		bool break_yet = false;
		for(auto & group: this->_other[other_id-1].preferences) {
			for(auto pref: group) {
				if (this->_candidates.count(pref) == 0) {
					num_added++;
				}
				if (pref == this->_agent.id) {
					break_yet = true;
				}
			}
			if (break_yet) {
				break;
			}
		}
		if (num_added > _skip) {
			this->regularIncrement();
		} else {
			break;
		}
	}
}

template <>
void SkipBigIterator<Hospital,Doctor>::begin() {
	int num_added = 0;
	while (true) {
		if (this->get_group() == -1) {
			break;
		}
		num_added = 0;
		int other_id = this->_agent.preferences[this->_group][this->_position];
		bool break_yet = false;
		for(auto & group: this->_other[other_id-1].preferences) {
			for(auto pref: group) {
				if (this->_candidates.count(pref) == 0) {
					num_added++;
				}
				if (pref == this->_agent.id) {
					break_yet = true;
				}
			}
			if (break_yet) {
				break;
			}
		}
		if (num_added > _skip) {
			this->regularIncrement();
		} else {
			break;
		}
	}
}

template <typename t_these, typename t_other>
void SkipBigIterator<t_these, t_other>::begin() {
	int num_added = 0;
	while (true) {
		if (this->get_group() == -1) {
			break;
		}
		num_added = 0;
		int other_id = this->_agent.preferences[this->_group][this->_position];
		int other_rank = this->_agent.ranks[this->_group][this->_position];
		bool break_yet = false;
		for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
			for(auto pref: this->_other[other_id-1].preferences[group_ind]) {
				if (this->_candidates.count(pref) == 0) {
					num_added++;
				}
				if (pref == this->_agent.id) {
					break_yet = true;
				}
			}
			if (break_yet) {
				break;
			}
		}
		if (num_added > _skip) {
			this->regularIncrement();
		} else {
			break;
		}
	}
}

template <>
void SkipBigIterator<Hospital, Doctor>::increment() {
	int num_added = 0;
	do {
		this->regularIncrement();
		if (this->get_group() == -1) {
			break;
		}
		num_added = 0;
		int other_id = this->_agent.preferences[this->_group][this->_position];
		bool break_yet = false;
		for(auto & group: _other[other_id-1].preferences) {
			for(auto pref: group) {
				if (this->_candidates.count(pref) == 0) {
					num_added++;
				}
				if (pref == _agent.id) {
					break_yet = true;
				}
			}
			if (break_yet) {
				break;
			}
		}
	} while (num_added > _skip);
}

template<>
void SkipBigIterator<Doctor, Hospital>::increment() {
	int num_added = 0;
	do {
		this->regularIncrement();
		if (this->get_group() == -1) {
			break;
		}
		num_added = 0;
		int other_id = this->_agent.preferences[this->_group][this->_position];
		bool break_yet = false;
		for(auto & group: _other[other_id-1].preferences) {
			for(auto pref: group) {
				if (this->_candidates.count(pref) == 0) {
					num_added++;
				}
				if (pref == _agent.id) {
					break_yet = true;
				}
			}
			if (break_yet) {
				break;
			}
		}
	} while (num_added > _skip);
}

template <typename t_these, typename t_other>
void SkipBigIterator<t_these, t_other>::increment() {
	int num_added = 0;
	do {
		this->regularIncrement();
		if (this->get_group() == -1) {
			break;
		}
		num_added = 0;
		int other_id = this->_agent.preferences[this->_group][this->_position];
		int other_rank = this->_agent.ranks[this->_group][this->_position];
		for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
			for(auto pref: this->_other[other_id-1].preferences[group_ind]) {
				if (this->_candidates.count(pref) == 0) {
					num_added++;
				}
			}
		}
	} while (num_added > _skip);
}

template <typename t_these, typename t_other>
BestIterator<t_these, t_other>::BestIterator(const t_these & agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<t_these> & these,
		const std::vector<t_other> & other, int group, int posn) :
	AgentIteratorBase<t_these, t_other>(agent, candidates, positions, these, other, group, posn) {
	}

template <>
void BestIterator<Hospital,Doctor>::increment() {
	int lowest_added = -1;
	int best_group = -1;
	int best_posn = -1;
	for(size_t group_no = 0; group_no < this->_agent.preferences.size(); ++group_no) {
		auto & group = this->_agent.preferences[group_no];
		for(size_t posn = 0; posn < group.size(); ++posn) {
			int other_id = group[posn];
			if (this->_positions.count(other_id) != 0) {
				continue;
			}
			int num_added = 0;
			bool break_yet = false;
			for(auto & group: this->_other[other_id-1].preferences) {
				for(auto other_pref: group) {
					if (this->_candidates.count(other_pref) == 0) {
						num_added++;
					}
					if (other_pref == this->_agent.id) {
						break_yet = true;
					}
				}
				if (break_yet) {
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
		this->_group = best_group;
		this->_position = best_posn;
	} else {
		this->_group = -1;
		this->_position = 0;
	}
}

template <>
void BestIterator<Doctor, Hospital>::increment() {
	int lowest_added = -1;
	int best_group = -1;
	int best_posn = -1;
	for(size_t group_no = 0; group_no < this->_agent.preferences.size(); ++group_no) {
		auto & group = this->_agent.preferences[group_no];
		for(size_t posn = 0; posn < group.size(); ++posn) {
			int other_id = group[posn];
			if (this->_positions.count(other_id) != 0) {
				continue;
			}
			int num_added = 0;
			bool break_yet = false;
			for(auto & group: this->_other[other_id-1].preferences) {
				for(auto other_pref: group) {
					if (this->_candidates.count(other_pref) == 0) {
						num_added++;
					}
					if (other_pref == this->_agent.id) {
						break_yet = true;
					}
					if (break_yet) {
						break;
					}
				}
			}
			if ((lowest_added == -1) || (num_added < lowest_added)) {
				lowest_added = num_added;
				best_group = posn;
				best_posn = 0;
			}
		}
	}
	if (lowest_added != -1) {
		this->_group = best_group;
		this->_position = best_posn;
	} else {
		this->_group = -1;
		this->_position = 0;
	}
}

template <typename t_these, typename t_other>
void BestIterator<t_these, t_other>::increment() {
	int lowest_added = -1;
	int best_group = -1;
	int best_posn = -1;
	for(int group_no = 0; group_no < this->_agent.preferences.size(); ++group_no) {
		auto & group = this->_agent.preferences[group_no];
		for(int posn = 0; posn < group.size(); ++posn) {
			int other_id = group[posn];
			if (this->_positions.count(other_id) != 0) {
				continue;
			}
			int num_added = 0;
			int other_rank = this->_agent.ranks[group_no][posn];
			for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
				for(auto other_pref: this->_other[other_id-1].preferences[group_ind]) {
					if (this->_candidates.count(other_pref) == 0) {
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
		this->_group = best_group;
		this->_position = best_posn;
	} else {
		this->_group = -1;
		this->_position = 0;
	}
}

template <>
void BestIterator<Doctor,Hospital>::begin() {
	int lowest_added = -1;
	int best_group = -1;
	int best_posn = -1;
	for(size_t group_no = 0; group_no < this->_agent.preferences.size(); ++group_no) {
		auto & group = this->_agent.preferences[group_no];
		for(size_t posn = 0; posn < group.size(); ++posn) {
			int other_id = group[posn];
			if (this->_positions.count(other_id) != 0) {
				continue;
			}
			int num_added = 0;
			bool break_yet = false;
			for(size_t group_ind = 0; group_ind < this->_other[other_id-1].preferences.size(); group_ind++) {
				for(auto other_pref: this->_other[other_id-1].preferences[group_ind]) {
					if (this->_candidates.count(other_pref) == 0) {
						num_added++;
					}
					if (other_pref == this->_agent.id) {
						break_yet = true;
					}
				}
				if (break_yet) {
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
		this->_group = best_group;
		this->_position = best_posn;
	} else {
		this->_group = -1;
		this->_position = 0;
	}
}

template <>
void BestIterator<Hospital,Doctor>::begin() {
	int lowest_added = -1;
	int best_group = -1;
	int best_posn = -1;
	for(size_t group_no = 0; group_no < this->_agent.preferences.size(); ++group_no) {
		auto & group = this->_agent.preferences[group_no];
		for(size_t posn = 0; posn < group.size(); ++posn) {
			int other_id = group[posn];
			if (this->_positions.count(other_id) != 0) {
				continue;
			}
			int num_added = 0;
			bool break_yet = false;
			for(size_t group_ind = 0; group_ind < this->_other[other_id-1].preferences.size(); group_ind++) {
				for(auto other_pref: this->_other[other_id-1].preferences[group_ind]) {
					if (this->_candidates.count(other_pref) == 0) {
						num_added++;
					}
					if (other_pref == this->_agent.id) {
						break_yet = true;
					}
				}
				if (break_yet) {
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
		this->_group = best_group;
		this->_position = best_posn;
	} else {
		this->_group = -1;
		this->_position = 0;
	}
}

template <typename t_these, typename t_other>
void BestIterator<t_these, t_other>::begin() {
	int lowest_added = -1;
	int best_group = -1;
	int best_posn = -1;
	for(size_t group_no = 0; group_no < this->_agent.preferences.size(); ++group_no) {
		auto & group = this->_agent.preferences[group_no];
		for(size_t posn = 0; posn < group.size(); ++posn) {
			int other_id = group[posn];
			if (this->_positions.count(other_id) != 0) {
				continue;
			}
			int num_added = 0;
			int other_rank = this->_agent.ranks[group_no][posn];
			for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
				for(auto other_pref: this->_other[other_id-1].preferences[group_ind]) {
					if (this->_candidates.count(other_pref) == 0) {
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
		this->_group = best_group;
		this->_position = best_posn;
	} else {
		this->_group = -1;
		this->_position = 0;
	}
}

template <typename t_these, typename t_other>
BestGroupIterator<t_these, t_other>::BestGroupIterator(const t_these & agent, const std::set<int> & candidates,
		const std::set<int> & positions, const std::vector<t_these> & these,
		const std::vector<t_other> & other, int group, int posn) :
	AgentIteratorBase<t_these, t_other>(agent, candidates, positions, these, other, group, posn) {
	}

template <>
void BestGroupIterator<Doctor,Hospital>::increment() {
	int lowest_added = -1;
	int best_posn = -1;
	const std::vector<int> & group = this->_agent.preferences[this->_group];
	for(size_t posn = 0; posn < group.size(); ++posn) {
		int other_id = group[posn];
		if (this->_positions.count(other_id) != 0) {
			continue;
		}
		int num_added = 0;
		for(auto & other_group: this->_other[other_id-1].preferences) {
			for(auto other_pref: other_group) {
				if (this->_candidates.count(other_pref) == 0) {
					num_added++;
				}
				if (other_pref == this->_agent.id) {
					break;
				}
			}
			if ((lowest_added == -1) || (num_added < lowest_added)) {
				lowest_added = num_added;
				best_posn = posn;
			}
		}
	}
	if (lowest_added != -1) {
		this->_position = best_posn;
	} else {
		this->_group++;
		if (this->_group == (int)this->_agent.preferences.size()) {
			this->_position = 0;
			this->_group = -1;
		} else {
			increment();
		}
	}
}

template<>
void BestGroupIterator<Hospital,Doctor>::increment() {
	int lowest_added = -1;
	int best_posn = -1;
	const std::vector<int> & group = this->_agent.preferences[this->_group];
	for(size_t posn = 0; posn < group.size(); ++posn) {
		int other_id = group[posn];
		if (this->_positions.count(other_id) != 0) {
			continue;
		}
		int num_added = 0;
		for(auto & other_group: this->_other[other_id-1].preferences) {
			for(auto other_pref: other_group) {
				if (this->_candidates.count(other_pref) == 0) {
					num_added++;
				}
				if (other_pref == this->_agent.id) {
					break;
				}
			}
		}
		if ((lowest_added == -1) || (num_added < lowest_added)) {
			lowest_added = num_added;
			best_posn = posn;
		}
	}
	if (lowest_added != -1) {
		this->_position = best_posn;
	} else {
		this->_group++;
		if (this->_group == (int)this->_agent.preferences.size()) {
			this->_position = 0;
			this->_group = -1;
		} else {
			increment();
		}
	}
}

template <typename t_these, typename t_other>
void BestGroupIterator<t_these, t_other>::increment() {
	int lowest_added = -1;
	int best_posn = -1;
	auto & group = this->_agent.preferences[this->_group];
	for(int posn = 0; posn < group.size(); ++posn) {
		int other_id = group[posn];
		if (this->_positions.count(other_id) != 0) {
			continue;
		}
		int num_added = 0;
		int other_rank = this->_agent.ranks[this->_group][posn];
		for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
			for(auto other_pref: this->_other[other_id-1].preferences[group_ind]) {
				if (this->_candidates.count(other_pref) == 0) {
					num_added++;
				}
			}
		}
		if ((lowest_added == -1) || (num_added < lowest_added)) {
			lowest_added = num_added;
			best_posn = posn;
		}
	}
	if (lowest_added != -1) {
		this->_position = best_posn;
	} else {
		this->_group++;
		if (this->_group == this->_agent.preferences.size()) {
			this->_position = 0;
			this->_group = -1;
		} else {
			increment();
		}
	}
}

template <>
void BestGroupIterator<Hospital, Doctor>::begin() {
	int lowest_added = -1;
	int best_posn = -1;
	if (this->_group == -1) {
		return;
	}
	auto & group = this->_agent.preferences[this->_group];
	for(size_t posn = 0; posn < group.size(); ++posn) {
		int other_id = group[posn];
		if (this->_positions.count(other_id) != 0) {
			continue;
		}
		int num_added = 0;
		bool break_yet = false;
		for(size_t group_ind = 0; group_ind < this->_other[other_id-1].preferences.size(); group_ind++) {
			for(auto other_pref: this->_other[other_id-1].preferences[group_ind]) {
				if (this->_candidates.count(other_pref) == 0) {
					num_added++;
				}
				if (other_pref == this->_agent.id) {
					break_yet = true;
				}
			}
			if (break_yet) {
				break;
			}
		}
		if ((lowest_added == -1) || (num_added < lowest_added)) {
			lowest_added = num_added;
			best_posn = posn;
		}
	}
	if (lowest_added != -1) {
		this->_position = best_posn;
	} else {
		this->_group++;
		if (this->_group == (int)this->_agent.preferences.size()) {
			this->_position = 0;
			this->_group = -1;
		} else {
			increment();
		}
	}
}

template <>
void BestGroupIterator<Doctor, Hospital>::begin() {
	int lowest_added = -1;
	int best_posn = -1;
	if (this->_group == -1) {
		return;
	}
	auto & group = this->_agent.preferences[this->_group];
	for(size_t posn = 0; posn < group.size(); ++posn) {
		int other_id = group[posn];
		if (this->_positions.count(other_id) != 0) {
			continue;
		}
		int num_added = 0;
		bool break_yet = false;
		for(size_t group_ind = 0; group_ind < this->_other[other_id-1].preferences.size(); group_ind++) {
			for(auto other_pref: this->_other[other_id-1].preferences[group_ind]) {
				if (this->_candidates.count(other_pref) == 0) {
					num_added++;
				}
				if (other_pref == this->_agent.id) {
					break_yet = true;
				}
			}
			if (break_yet) {
				break;
			}
		}
		if ((lowest_added == -1) || (num_added < lowest_added)) {
			lowest_added = num_added;
			best_posn = posn;
		}
	}
	if (lowest_added != -1) {
		this->_position = best_posn;
	} else {
		this->_group++;
		if (this->_group == (int)this->_agent.preferences.size()) {
			this->_position = 0;
			this->_group = -1;
		} else {
			increment();
		}
	}
}

template <typename t_these, typename t_other>
void BestGroupIterator<t_these, t_other>::begin() {
	int lowest_added = -1;
	int best_posn = -1;
	if (this->_group == -1) {
		return;
	}
	auto & group = this->_agent.preferences[this->_group];
	for(size_t posn = 0; posn < group.size(); ++posn) {
		int other_id = group[posn];
		if (this->_positions.count(other_id) != 0) {
			continue;
		}
		int num_added = 0;
		int other_rank = this->_agent.ranks[this->_group][posn];
		for(int group_ind = 0; group_ind <= other_rank; group_ind++) {
			for(auto other_pref: this->_other[other_id-1].preferences[group_ind]) {
				if (this->_candidates.count(other_pref) == 0) {
					num_added++;
				}
			}
		}
		if ((lowest_added == -1) || (num_added < lowest_added)) {
			lowest_added = num_added;
			best_posn = posn;
		}
	}
	if (lowest_added != -1) {
		this->_position = best_posn;
	} else {
		this->_group++;
		if (this->_group == this->_agent.preferences.size()) {
			this->_position = 0;
			this->_group = -1;
		} else {
			increment();
		}
	}
}

template class AgentIterator<Doctor, Hospital>;
template class AgentIterator<Hospital, Doctor>;

