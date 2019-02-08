#ifndef AGENTITERATOR_H
#define AGENTITERATOR_H

#include <utility>

#include "Allocation.h"

class AgentIteratorBase {
public:
	AgentIteratorBase(const Child & agent, const std::set<int> & candidates, const std::set<int> & positions,
			const std::vector<Child> & these, const std::vector<Child> & other, int group=0, int posn=0);
	virtual ~AgentIteratorBase() = 0;
	virtual void increment() = 0;
	virtual void begin() = 0;

	int get_position() const;
	int get_group() const;
	const Child & get_agent() const;
	const std::vector<Child> & get_these() const;
	const std::vector<Child> & get_other() const;
	const std::set<int> & get_positions()  const;
	const std::set<int> & get_candidates() const;
protected:
	void regularIncrement();

	const Child & _agent;
	int _group;
	int _position;
	const std::vector<Child> & _these;
	const std::vector<Child> & _other;
	const std::set<int> & _positions;
	const std::set<int> & _candidates;
};


class AgentIterator : public std::iterator<std::forward_iterator_tag, std::pair<int, int>, ptrdiff_t> {
public:
	AgentIterator(const Child & agent, const std::set<int> & candidates, const std::set<int> & positions,
			const std::vector<Child> & these, const std::vector<Child> & other, int mode);
	AgentIterator(AgentIterator *other, int group, int posn);
	AgentIterator(AgentIterator *other);
	~AgentIterator();
	bool operator==(const AgentIterator& other);
	bool operator!=(const AgentIterator& other);
	const std::pair<int, int> operator*();
	AgentIterator& operator++() {base->increment(); return *this; }
	AgentIterator operator++(int) {AgentIterator res(this); base->increment(); return res; }

	AgentIterator begin();
	AgentIterator end();

	int get_position() const;
	int get_group() const;
	int get_mode() const;

	const Child & get_agent() const { return base->get_agent(); }
	const std::vector<Child> & get_these() const {return base->get_these(); }
	const std::vector<Child> & get_other() const {return base->get_other(); }
	const std::set<int> & get_positions()  const {return base->get_positions(); }
	const std::set<int> & get_candidates() const {return base->get_candidates(); }
private:
	AgentIteratorBase * base;
	int _mode;
};


class DescendingIterator : public AgentIteratorBase {
	public:
		DescendingIterator(const Child & agent, const std::set<int> & candidates,
				const std::set<int> & positions, const std::vector<Child> & these,
				const std::vector<Child> & other, int group=0, int posn=0);
		~DescendingIterator() {}
		void increment();
		void begin();
};


class SkipBigIterator : public AgentIteratorBase {
	public:
		SkipBigIterator(const Child & agent, const std::set<int> & candidates,
				const std::set<int> & positions, const std::vector<Child> & these,
				const std::vector<Child> & other, int skip, int group, int posn);
		~SkipBigIterator() {}
		void increment();
		void begin();
	private:
		int _skip;
};

class BestIterator : public AgentIteratorBase {
	public:
		BestIterator(const Child & agent, const std::set<int> & candidates,
				const std::set<int> & positions, const std::vector<Child> & these,
				const std::vector<Child> & other, int group=0, int posn=0);
		~BestIterator() {}
		void increment();
		void begin();
};


#endif /* AGENTITERATOR_H */
