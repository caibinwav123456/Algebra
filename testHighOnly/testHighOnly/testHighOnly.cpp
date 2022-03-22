// testHighOnly.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MCD.h"
#include <stdio.h>
#include <vector>
#include <algorithm>
using namespace std;
static vector<int> vp;
template<class T>
void Divide(const T& a,const T& b,T& q,T& rem)
{
	assert(b>0);
	T _q,_r;
	if(a>=0)
		_q=a/b;
	else
		_q=-((-a)/b+1);
	_r=a-b*_q;
	q=_q;
	rem=_r;
}
template<class T>
inline T mul_b(T a,T b,T base)
{
	assert(base>0);
	T m=a*b;
	T q,r;
	Divide(m,base,q,r);
	return r;
}
template<class T>
inline T div_b(T a,T b,T base)
{
	//a=b*q+base*x
	T q,x,mcd;
	Divide(a,base,q,a);
	Divide(b,base,q,b);
	LCD(q,x,b,base,&mcd);
	T k,r;
	Divide(a,mcd,k,r);
	assert(r==0);
	r=mul_b(k,q,base);
	assert(mul_b(r,b,base)==a);
	return r;
}
int GetNextPrime()
{
	static int np=2;
	for(;;np++)
	{
		bool p=true;
		for(int i=0;i<(int)vp.size();i++)
		{
			if(vp[i]*vp[i]>np)
				break;
			int q=0,r=0;
			Divide(np,vp[i],q,r);
			if(r==0)
			{
				p=false;
				break;
			}
		}
		if(p)
		{
			vp.push_back(np);
			return np++;
		}
	}
	return 0;
}
inline void lowest_decomp(int k,int& p,int& order,int* factor=NULL)
{
	int q,r;
	order=0;
	int dummy;
	int* mul=(factor?factor:&dummy);
	*mul=1;
	if(p<2)
	{
		for(int i=0;i<(int)vp.size();i++)
		{
			if(vp[i]*vp[i]>k)
			{
				p=k;
				break;
			}
			Divide(k,vp[i],q,r);
			if(r==0)
			{
				p=vp[i];
				break;
			}
		}
		if(p<2)
		{
			return;
		}
	}
	for(;;)
	{
		Divide(k,p,k,r);
		if(r==0)
		{
			order++;
			(*mul)*=p;
		}
		else
			break;
	}
}
bool delete_elem(vector<int>& v, int val)
{
	if(v.size()==0)
		return false;
	int n0=0,n1=(int)v.size()-1;
	if(v[n0]>val||v[n1]<val)
		return false;
	int f;
	if(v[n0]==val)
		f=n0;
	else if(v[n1]==val)
		f=n1;
	else
	while(n0<n1)
	{
		int mid=(n0+n1)/2;
		if(mid==n0)
		{
			return false;
		}
		else if(v[mid]==val)
		{
			f=mid;
			break;
		}
		else if(v[mid]>val)
		{
			n1=mid;
		}
		else
		{
			n0=mid;
		}
	}
	v.erase(v.begin()+f);
	return true;
}
bool verify_ring(int k,int n)
{
	if(n<=1)
		return false;
	else if(n==2)
		return k==1;
	else if(k<=1)
		return false;
	vector<int> a;
	for(int i=2;i<n;i++)
	{
		a.push_back(i);
	}
	int acc=mul_b(k,1,n);
	while(acc!=1&&a.size()>0)
	{
		bool del=delete_elem(a,acc);
		if(!del)
			return false;
		acc=mul_b(acc,k,n);
	}
	if(acc==1&&a.size()==0)
		return true;
	else
		return false;
}
bool verify_high_ring(__int64 k,__int64 n)
{
	__int64 acc=1,ln=n-1,n2=n*n;
	bool bn=false;
	for(int i=0;i<(int)ln;i++)
	{
		acc=mul_b(acc,k,n2);
		if(acc>1&&acc<n)
		{
			__int64 mcd;
			MCD(mcd,(__int64)(i+1),ln);
			printf("%sfound:%lld,%lld%s\n",bn?"":"\n",acc,n,mcd>1?"":",pure");
			bn=true;
		}
	}
	if(acc==1)
		return true;
	else
		return false;
}
int BuildOneOrderRing(int n)
{
	vector<int> a,b,c,d;
	vector<int> *cur=&c,*src=&b,*dst=&d;
	int ln=n-1,order,factor;
	int p_2=2;
	int kp=0;
	for(int i=2;i<n;i++)
	{
		a.push_back(i);
	}
	if(a.size()==0)
	{
		assert(verify_ring(1,n));
		return 1;
	}
	int k=a[kp],acc=1;
	c.push_back(1);
	for(;;)
	{
		acc=mul_b(acc,k,n);
		if(acc==1)
			break;
		c.push_back(acc);
	}
	if((int)c.size()==n-1)
	{
		assert(verify_ring(2,n));
		return 2;
	}
	lowest_decomp((int)c.size(),p_2,order,&factor);
	if(order>0)
	{
		for(int i=0;i<(int)c.size();i+=factor)
		{
			b.push_back(c[i]);
		}
		if((int)c.size()==factor)
			kp++;
		swap(cur,src);
	}
	for(int i=0;i<(int)cur->size();i++)
	{
		bool del=((*cur)[i]==1)||(delete_elem(a,(*cur)[i]));
		assert(del);
	}
	while((int)a.size()>kp)
	{
		k=a[kp];
		acc=1;
		src->clear();
		src->push_back(1);
		for(;;)
		{
			acc=mul_b(acc,k,n);
			if(acc==1)
				break;
			src->push_back(acc);
		}
		if((int)src->size()==ln)
		{
			bool bv=verify_ring(k,n);
			assert(bv);
			if(!bv)
				printf("\nerror:%d\n",n);
			return k;
		}
		lowest_decomp((int)src->size(),p_2,order,&factor);
		if((int)src->size()==factor)
		{
			kp++;
			continue;
		}
		int remn=(int)src->size()/factor;
		for(int i=0;i<(int)src->size();i++)
		{
			if(mul_b(i,1,factor)!=0&&mul_b(i,1,remn)!=0)
				delete_elem(a,(*src)[i]);
		}
		int mcd;
		MCD(mcd,remn,(int)cur->size());
		remn/=mcd;
		while(remn!=1)
		{
			int pp=0;
			lowest_decomp(remn,pp,order,&factor);
			assert(order>0);
			int hif=factor;
			lowest_decomp((int)cur->size(),pp,order,&factor);
			int lof=factor;
			int step=(int)src->size()/(hif*lof);
			dst->clear();
			int bias=1;
			if(mul_b((*cur)[mul_b(hif,1,(int)cur->size())],
				(*src)[mul_b(hif*step,1,(int)src->size())],n)==1)
			{
				bias=2;
			}
			int total=cur->size()*hif;
			for(int i=0;i<total;i++)
			{
				int push=mul_b((*cur)[mul_b(i,1,(int)cur->size())],
					(*src)[mul_b(i,step*bias,(int)src->size())],n);
				dst->push_back(push);
				bool del=mul_b(i,1,hif)==0||delete_elem(a,push);
				assert(del);
			}
			swap(cur,dst);
			remn/=hif;
		}
	}
	int bin=ln/(int)cur->size();
	int index=0,bi=0;
	while(index!=bin&&a.size()>0)
	{
		bi=a[0];
		for(index=1,acc=bi;acc!=1;index++)
		{
			delete_elem(a,acc);
			acc=mul_b(acc,bi,n);
		}
	}
	assert(index==bin&&a.size()==0);
	int ret=mul_b(cur->size()>1?(*cur)[1]:1,bi,n);
	bool bv=verify_ring(ret,n);
	assert(bv);
	if(!bv)
		printf("\nfirst error:%d\n",n);
	return ret;
}
__int64 BuildHighOrderRing(__int64 k,__int64 n)
{
	__int64 n2=n*n,acc=1,q,r;
	__int64 ln=n-1;
	for(int i=0;i<(int)ln;i++)
	{
		acc=mul_b(acc,k,n2);
	}
	Divide(acc,n,q,r);
	assert(r==1);
	__int64 kl=div_b(mul_b(-q,(__int64)1,n),ln,n);
	assert(kl==q);
	__int64 kq=kl*n+1;
	__int64 kn=mul_b(k,kq,n2);
	__int64 km=mul_b(k+n*kl*k,(__int64)1,n2);
	assert(kn==km);
	return kn;
}
int _tmain(int argc, _TCHAR* argv[])
{
	printf("start...\n");
	for(int i=0;;i++)
	{
		int n=GetNextPrime();
		if(n>=32768)
			break;
		printf("\rchecking %d...",n);
		int k=BuildOneOrderRing(n);
		assert(k<n);
		printf("%d",k);
		__int64 k2=BuildHighOrderRing(k,n);
		printf(",%lld",k2);
		bool bv=verify_high_ring(k2,(__int64)n);
		assert(bv);
		if(!bv)
			printf("\nhigh error:%d\n",n);
		for(int j=0;j<20;j++)
		{
			printf(" ");
		}
		if(i>0&&i%100==0)
		{
			printf("\nContinue(Y/N)?");
			char c;
			_flushall();
			c=getc(stdin);
			if(c!='y'&&c!='Y')
				break;
		}
	}
	return 0;
}
