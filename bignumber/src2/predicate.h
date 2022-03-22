#ifndef _PREDICATE_H_
#define _PREDICATE_H_
class IPred
{
	friend bool operator>(const IPred& a, const IPred& b);
	friend bool operator<(const IPred& a, const IPred& b);
	friend bool operator>=(const IPred& a, const IPred& b);
	friend bool operator<=(const IPred& a, const IPred& b);
protected:
	virtual bool LessEqual(const IPred& y) const=0;
};
#endif