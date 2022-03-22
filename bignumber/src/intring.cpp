#include "intring.h"
#include "MCD.h"
const BigInt IntRing::_zero(0);
const BigInt IntRing::_identical(1);
void IntRingType::Clone(const MemBase& m, bool recur)
{
	const IntRingType* gt=dynamic_cast<const IntRingType*>(&m);
	assert(gt!=NULL);
	assert(GroupType::TypeEqual(*gt));
	ring_base.Clone(gt->ring_base,recur);
}
Wrapper<MemBase> IntRingType::CloneR(const MemBase& m, bool recur) const
{
	IntRingType* newtype=new IntRingType;
	newtype->Clone(m,recur);
	return Wrapper<MemBase>(newtype);
}
bool IntRingType::TypeEqual(const GroupType &gt) const
{
	if(!GroupType::TypeEqual(gt))
		return false;
	const IntRingType* rgt=dynamic_cast<const IntRingType*>(&gt);
	assert(rgt!=NULL);
	return ring_base==rgt->ring_base;
}
IntRing::IntRing(const BigInt &base, const BigInt &num):
	Group(new IntRingType(base)),
	ring_num(num)
{
	assert(get_base()>BigInt(0));
	Normalize();
}
bool IntRing::Equal(const Group& y) const
{
	assert(TypeEqual(y));
	const IntRing* b=dynamic_cast<const IntRing*>(&y);
	assert(b!=NULL);
	return ring_num==b->ring_num;
}
const BigInt &IntRing::get_base() const
{
	const IntRingType* t=dynamic_cast<const IntRingType*>(type.toType());
	assert(t!=NULL);
	return t->get_base();
}
void IntRing::Normalize()
{
	BigInt q;
	Divide(ring_num,get_base(),q,ring_num);
}
IntRing operator+(const IntRing& a, const IntRing& b)
{
	IntRing ret(a);
	ret.Add(b);
	return ret;
}
IntRing operator-(const IntRing& a, const IntRing& b)
{
	IntRing ret(a);
	ret.Sub(b);
	return ret;
}
IntRing operator*(const IntRing& a, const IntRing& b)
{
	IntRing ret(a);
	ret.Mul(b);
	return ret;
}
IntRing operator/(const IntRing& a, const IntRing& b)
{
	IntRing ret(a);
	ret.Div(b);
	return ret;
}
void IntRing::Add(const Group& y)
{
	assert(TypeEqual(y));
	const IntRing* b=dynamic_cast<const IntRing*>(&y);
	assert(b!=NULL);
	ring_num.Add(b->ring_num);
	Normalize();
}
void IntRing::Sub(const Group& y)
{
	assert(TypeEqual(y));
	const IntRing* b=dynamic_cast<const IntRing*>(&y);
	assert(b!=NULL);
	ring_num.Sub(b->ring_num);
	Normalize();
}
void IntRing::Inv()
{
	ring_num.Inv();
	Normalize();
}
void IntRing::Mul(const Group& y)
{
	assert(TypeEqual(y));
	const IntRing* b=dynamic_cast<const IntRing*>(&y);
	assert(b!=NULL);
	ring_num.Mul(b->ring_num);
	Normalize();
}
int IntRing::Div(const InvRing& y)
{
	assert(TypeEqual(y));
	const IntRing* b=dynamic_cast<const IntRing*>(&y);
	assert(b!=NULL);
	BigInt mcd,q,c;
	LCD(c,q,b->ring_num,b->get_base(),&mcd);
	vassert(mcd==BigInt(1));
	ring_num*=c;
	Normalize();
	return 0;
}
void IntRing::Zero()
{
	ring_num=_zero;
}
void IntRing::Unit()
{
	ring_num=_identical;
}
void IntRing::Clone(const MemBase& m, bool recur)
{
	const IntRing * b=dynamic_cast<const IntRing*>(&m);
	assert(b!=NULL);
#ifndef USE_CLONER
	IntRingType tnew;
	tnew.Clone(*b->type.toType(),recur);
	slitx_c(type,GroupType,IntRingType,&tnew);
#else
	Wrapper<MemBase> clone=type.toType()->CloneR(*b->type.toType());
	IntRingType* t=dynamic_cast<IntRingType*>(clone.toType());
	assert(t!=NULL);
	slitx_c(type,GroupType,IntRingType,t);
#endif
	ring_num.Clone(b->ring_num,recur);
}
Wrapper<MemBase> IntRing::CloneR(const MemBase& m, bool recur) const
{
	IntRing* newintring=new IntRing;
	newintring->Clone(m,recur);
	return Wrapper<MemBase>(newintring);
}
IntRing IntRing::operator-() const
{
	IntRing ret(*this);
	ret.ring_num.Inv();
	ret.Normalize();
	return ret;
}
