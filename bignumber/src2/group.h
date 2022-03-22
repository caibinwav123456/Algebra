#ifndef _GROUP_H_
#define _GROUP_H_
#include "mem.h"
enum E_GROUP_TYPE
{
	egt_group=0,
	egt_invring,
	egt_integral,
	egt_intring,
	egt_polynomial,
	egt_polynomialring,
	egt_max
};
extern bool _ring_type[];
class GroupType:public MemBase
{
public:
	GroupType(E_GROUP_TYPE _type)
	{
		type=_type;
	}
	E_GROUP_TYPE get_type()
	{
		return type;
	}
	bool is_ring_type()
	{
		return _ring_type[type];
	}
	virtual bool TypeEqual(const GroupType &gt) const
	{
		return type==gt.type;
	}
	virtual void Clone(const MemBase& m, bool recur=false);
private:
	E_GROUP_TYPE type;
};
class Group:public MemBase
{
public:
	Group(const E_GROUP_TYPE _type=egt_group);
	Group(const GroupType *gt);
	Group(const Wrapper<const GroupType> &gt);

	bool operator==(const Group& b) const;
	bool operator!=(const Group& b) const;
	void operator+=(const Group& b);
	void operator-=(const Group& b);
	void operator*=(const Group& b);

	virtual bool Equal(const Group& y) const=0;
	virtual void Add(const Group& y)=0;
	virtual void Sub(const Group& y)=0;
	virtual void Inv()=0;
	virtual void Mul(const Group& y)
	{
		assert(false);
	}
	virtual void Zero()=0;

	bool TypeEqual(const Group& y) const;

protected:
	Wrapper<const GroupType> type;
};
typedef class InvRing:virtual public Group
{
public:
	InvRing();
	void operator/=(const InvRing& b);
	virtual int Div(const InvRing& y)=0;
	virtual void Unit()=0;
}IRing;
#endif