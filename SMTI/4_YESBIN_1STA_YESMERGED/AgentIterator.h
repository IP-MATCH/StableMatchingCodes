#ifndef AGENTITERATOR_H
#define AGENTITERATOR_H

#include <utility>

#include "Allocation.h"

	class AgentIterator : public std::iterator<std::forward_iterator_tag, std::pair<int, int>, ptrdiff_t> {
	public:
		AgentIterator(const Child & agent, const std::set<int> & candidates, const std::set<int> & positions,
				const std::vector<Child> & these, const std::vector<Child> & other, int group=0, int posn=0);
		bool operator==(const AgentIterator& other);
		bool operator!=(const AgentIterator& other);
		const std::pair<int, int> operator*();
		AgentIterator& operator++();
		AgentIterator operator++(int);

		AgentIterator begin();
		AgentIterator end();

		int get_position() const;
		int get_group() const;

	protected:
		const Child & _agent;
		int _group;
		int _position;
		const std::vector<Child> & _these;
		const std::vector<Child> & _other;
		const std::set<int> & _positions;
		const std::set<int> & _candidates;
	};

template <int step>
class SkipBigIterator : public AgentIterator {
	public:
		SkipBigIterator(const Child & agent, const std::set<int> & candidates,
				const std::set<int> & positions, const std::vector<Child> & these,
				const std::vector<Child> & other, int group=0, int posn=0);
		SkipBigIterator& operator++();
		SkipBigIterator operator++(int);
};

class BestIterator : public AgentIterator {
	public:
		BestIterator(const Child & agent, const std::set<int> & candidates,
				const std::set<int> & positions, const std::vector<Child> & these,
				const std::vector<Child> & other, int group=0, int posn=0);
		BestIterator& operator++();
		BestIterator operator++(int);
};


#endif /* AGENTITERATOR_H */
