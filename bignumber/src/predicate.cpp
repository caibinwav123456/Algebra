#include "predicate.h"
bool operator>(const IPred& a, const IPred& b)
{
	return !a.LessEqual(b);
}
bool operator<(const IPred& a, const IPred& b)
{
	return !b.LessEqual(a);
}
bool operator>=(const IPred& a, const IPred& b)
{
	return b.LessEqual(a);
}
bool operator<=(const IPred& a, const IPred& b)
{
	return a.LessEqual(b);
}