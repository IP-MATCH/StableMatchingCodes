#ifndef AGENTITERATOR_H
#define AGENTITERATOR_H

#include <utility>

#include "Allocation.h"

template <typename t_these, typename t_other>
class AgentIteratorBase {
public:
	AgentIteratorBase(const t_these & agent, const std::set<int> & candidates, const std::set<int> & positions,
			const std::vector<t_these> & these, const std::vector<t_other> & other, int group=0, int posn=0);
	virtual ~AgentIteratorBase() = 0;
	virtual void increment() = 0;
	virtual void begin() = 0;

	int get_position() const;
	int get_group() const;
	const t_these & get_agent() const;
	const std::vector<t_these> & get_these() const;
	const std::vector<t_other> & get_other() const;
	const std::set<int> & get_positions()  const;
	const std::set<int> & get_candidates() const;
protected:
	void regularIncrement();

	const t_these & _agent;
	int _group;
	int _position;
	const std::vector<t_these> & _these;
	const std::vector<t_other> & _other;
	const std::set<int> & _positions;
	const std::set<int> & _candidates;
};


template <typename t_these, typename t_other>
class AgentIterator : public std::iterator<std::forward_iterator_tag, std::pair<int, int>, ptrdiff_t> {
public:
	AgentIterator(const t_these & agent, const std::set<int> & candidates, const std::set<int> & positions,
			const std::vector<t_these> & these, const std::vector<t_other> & other, int mode);
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

	const t_these & get_agent() const { return base->get_agent(); }
	const std::vector<t_these> & get_these() const {return base->get_these(); }
	const std::vector<t_other> & get_other() const {return base->get_other(); }
	const std::set<int> & get_positions()  const {return base->get_positions(); }
	const std::set<int> & get_candidates() const {return base->get_candidates(); }
private:
	AgentIteratorBase<t_these, t_other> * base;
	int _mode;
};


template <typename t_these, typename t_other>
class DescendingIterator : public AgentIteratorBase<t_these, t_other> {
	public:
		DescendingIterator(const t_these & agent, const std::set<int> & candidates,
				const std::set<int> & positions, const std::vector<t_these> & these,
				const std::vector<t_other> & other, int group=0, int posn=0);
		~DescendingIterator() {}
		void increment();
		void begin();
};


template <typename t_these, typename t_other>
class SkipBigIterator : public AgentIteratorBase<t_these, t_other> {
	public:
		SkipBigIterator(const t_these & agent, const std::set<int> & candidates,
				const std::set<int> & positions, const std::vector<t_these> & these,
				const std::vector<t_other> & other, int skip, int group, int posn);
		~SkipBigIterator() {}
		void increment();
		void begin();
	private:
		int _skip;
};

template <typename t_these, typename t_other>
class BestIterator : public AgentIteratorBase<t_these, t_other> {
	public:
		BestIterator(const t_these & agent, const std::set<int> & candidates,
				const std::set<int> & positions, const std::vector<t_these> & these,
				const std::vector<t_other> & other, int group=0, int posn=0);
		~BestIterator() {}
		void increment();
		void begin();
};

template <typename t_these, typename t_other>
class BestGroupIterator : public AgentIteratorBase<t_these, t_other> {
	public:
		BestGroupIterator(const t_these & agent, const std::set<int> & candidates,
				const std::set<int> & positions, const std::vector<t_these> & these,
				const std::vector<t_other> & other, int group=0, int posn=0);
		~BestGroupIterator() {}
		void increment();
		void begin();
};

#endif /* AGENTITERATOR_H */
