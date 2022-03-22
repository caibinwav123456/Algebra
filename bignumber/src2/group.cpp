#include "group.h"
bool _ring_type[egt_max]={0,1,0,1,0,1};
void GroupType::Clone(const MemBase& m, bool recur)
{
	const GroupType* gt=dynamic_cast<const GroupType*>(&m);
	assert(gt!=NULL);
	*this=*gt;
}
bool Group::operator==(const Group& b) const
{
	return Equal(b);
}
bool Group::operator!=(const Group& b) const
{
	return !Equal(b);
}
void Group::operator+=(const Group& b)
{
	Add(b);
}
void Group::operator-=(const Group& b)
{
	Sub(b);
}
void Group::operator*=(const Group& b)
{
	Mul(b);
}
Group::Group(const E_GROUP_TYPE _type):type(new GroupType(_type))
{
}
Group::Group(const GroupType *gt):type(gt)
{
}
Group::Group(const Wrapper<const GroupType> &gt):type(gt)
{
}
bool Group::TypeEqual(const Group& y) const
{
	return type.toType()->TypeEqual(*y.type.toType());
}
InvRing::InvRing():Group(egt_invring)
{
}
void InvRing::operator/=(const InvRing& b)
{
	Div(b);
}
