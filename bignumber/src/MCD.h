#ifndef _MCD_H_
#define _MCD_H_
#include "common.h"
using namespace std;
template<class T>
void MCD(T &mcd, const T &a, const T &b)
{
	assert(a!=(T)0&&b!=(T)0);
	T _a(a),_b(b),q;
	T *pa=&_a, *pb=&_b;
	while(*pb!=(T)0)
	{
		Divide(*pa,*pb,q,*pa);
		swap(pa,pb);
	}
	mcd=*pa;
}
template<class T>
void LCD(T &lcda, T &lcdb, const T &a, const T &b, T *mcd=NULL)
{
	assert(&lcda!=&lcdb && &lcda!=mcd && &lcdb!=mcd);
	assert(a!=(T)0&&b!=(T)0);
	vector<T> sq;
	T _a(a),_b(b),q;
	T *pa=&_a, *pb=&_b;
	while(*pb!=(T)0)
	{
		Divide(*pa,*pb,q,*pa);
		swap(pa,pb);
		sq.push_back(q);
	}
	if(mcd!=NULL)
	{
		*mcd=*pa;
	}
	sq.pop_back();
	if(pa==&_a)
		pa=&lcda,pb=&lcdb;
	else
		pa=&lcdb,pb=&lcda;
	*pa=(T)1;
	pb->Zero();
	while((int)sq.size()>0)
	{
		*pb+=*pa*(-sq.back());
		sq.pop_back();
		swap(pa,pb);
	}
}
#endif