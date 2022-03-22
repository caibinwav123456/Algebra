#ifndef _INT_RING_H_
#define _INT_RING_H_
#include "common.h"
#include "bignumber.h"
using namespace std;
class IntRingType:public GroupType
{
public:
	IntRingType(const BigInt &base):GroupType(egt_intring),
		ring_base(base)
	{
	}
	virtual bool TypeEqual(const GroupType &gt) const;
	virtual void Clone(const MemBase& m, bool recur=false);
	const BigInt &get_base() const
	{
		return ring_base;
	}
private:
	BigInt ring_base;
};
class IntRing:public InvRing
{
	friend IntRing operator+(const IntRing& a, const IntRing& b);
	friend IntRing operator-(const IntRing& a, const IntRing& b);
	friend IntRing operator*(const IntRing& a, const IntRing& b);
	friend IntRing operator/(const IntRing& a, const IntRing& b);

public:
	virtual bool Equal(const Group& y) const;
	virtual void Add(const Group& y);
	virtual void Sub(const Group& y);
	virtual void Inv();
	virtual void Mul(const Group& y);
	virtual int Div(const InvRing& y);
	virtual void Zero();
	virtual void Unit();
	virtual void Clone(const MemBase& m, bool recur=false);
	IntRing(const BigInt &base, const BigInt &num=BigInt(0));
	IntRing operator-() const;

protected:
	static const BigInt _zero;
	static const BigInt _identical;

private:
	void Normalize();
	const BigInt &get_base() const;

	BigInt ring_num;
};
#endif