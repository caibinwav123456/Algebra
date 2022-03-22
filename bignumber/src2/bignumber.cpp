#include "bignumber.h"
const BigInt BigInt::_zero(0);
const Wrapper<const GroupType>& BigInt::get_type()
{
	static const Wrapper<const GroupType> _gt_integer(new GroupType(egt_integral));
	return _gt_integer;
}
bool BigInt::Equal(const Group& y) const
{
	assert(TypeEqual(y));
	const BigInt * b=dynamic_cast<const BigInt*>(&y);
	assert(b!=NULL);
	if(pos!=b->pos)
	{
		return AbsEQ(_zero)&&b->AbsEQ(_zero);
	}
	return AbsEQ(*b);
}
bool BigInt::LessEqual(const IPred& y) const
{
	const BigInt * b=dynamic_cast<const BigInt*>(&y);
	assert(b!=NULL);
	if(!pos&&b->pos)
		return true;
	else if(pos&&!b->pos)
	{
		return AbsEQ(_zero)&&b->AbsEQ(_zero);
	}
	else if(pos&&b->pos)
	{
		return AbsLE(*b);
	}
	else
	{
		return b->AbsLE(*this);
	}
}
void BigInt::Zero()
{
	*this=_zero;
}
void BigInt::Clone(const MemBase& m, bool recur)
{
	const BigInt * b=dynamic_cast<const BigInt*>(&m);
	assert(b!=NULL);
	assert(b->inner.toType()!=NULL);
	pos=b->pos;
	inner.Slit(new vector<uchar>(*b->inner.toType()));
}
BigInt operator+(const BigInt& a, const BigInt& b)
{
	BigInt ret(a);
	ret.Add(b);
	return ret;
}
BigInt operator-(const BigInt& a, const BigInt& b)
{
	BigInt ret(a);
	ret.Sub(b);
	return ret;
}
BigInt BigInt::operator-() const
{
	BigInt ret(*this);
	ret.Inv();
	return ret;
}
BigInt operator*(const BigInt& a, const BigInt& b)
{
	BigInt ret;
	a.AbsMul(ret, b);
	ret.pos=(a.pos==b.pos?true:false);
	return ret;
}
void BigInt::Add(const Group& y)
{
	assert(TypeEqual(y));
	const BigInt * v=dynamic_cast<const BigInt*>(&y);
	assert(v!=NULL);
	Clone(*this);
	if((pos&&v->pos)||(!pos&&!v->pos))
	{
		AbsAdd(*v);
	}
	else
	{
		bool b=AbsLE(*v);
		AbsSub(*v, b);
		if(b)
			pos=v->pos;
	}
}
void BigInt::Sub(const Group& y)
{
	const BigInt * v=dynamic_cast<const BigInt*>(&y);
	assert(v!=NULL);
	BigInt y0=*v;
	y0.pos=!y0.pos;
	Add(y0);
}
void BigInt::Inv()
{
	pos=!pos;
}
void BigInt::Mul(const Group& b)
{
	assert(TypeEqual(b));
	const BigInt * v=dynamic_cast<const BigInt*>(&b);
	assert(v!=NULL);
	BigInt pro;
	AbsMul(pro, *v);
	pro.pos=(pos==v->pos?true:false);
	*this=pro;
}
void BigInt::AbsAdd(const BigInt& y)
{
	int n=max(Length(),y.Length());
	vector<uchar>& a=*inner.toType();
	vector<uchar>& b=*y.inner.toType();
	if((int)a.size()<n)
		a.resize(n+1,0);
	uint carry=0;
	for(int i=0;i<n;i++)
	{
		uint s=(uint)a[i]+(i>=(int)b.size()?0:(uint)b[i])+carry;
		a[i]=(uchar)(s&0xff);
		carry=(s>>8);
	}
	if(carry)
	{
		if((int)a.size()<n+1)
		{
			a.resize(n+1,0);
		}
		a[n]=(uchar)carry;
	}
}
void BigInt::AbsSub(const BigInt& y, bool rev)
{
	int na=Length(),nb=y.Length();
	vector<uchar>* a=inner.toType();
	vector<uchar>* b=y.inner.toType();
	vector<uchar>* r=a;
	if(rev)
	{
		swap(a, b);
		swap(na,nb);
	}
	assert(na>=nb);
	if((int)r->size()<na)
		r->resize(na,0);
	uint carry=0;
	for(int i=0;i<na;i++)
	{
		uint s=(uint)(*a)[i];
		uint c=carry;
		uint d=i>=nb?0:(uint)(*b)[i];
		if(s<d+c)
		{
			s+=0x100;
			carry=1;
		}
		else
			carry=0;
		(*r)[i]=(uchar)(s-(d+c));
	}
	assert(carry==0);
}
void BigInt::AbsMul(BigInt& pro, const BigInt& b) const
{
	int na=Length(), nb=b.Length();
	vector<uchar> *vp=pro.inner.toType(),
		*va=inner.toType(),*vb=b.inner.toType();
	vp->resize(na+nb,0);
	for(int i=0;i<na+nb;i++)
	{
		(*vp)[i]=0;
	}
	for(int i=0;i<na;i++)
	{
		uint carry=0;
		for(int j=0;j<nb;j++)
		{
			uint p=(uint)(*va)[i]*(uint)(*vb)[j];
			uint v=(uint)(*vp)[i+j]+p+carry;
			(*vp)[i+j]=(uchar)(v&0xff);
			carry=(v>>8);
		}
		for(int j=i+nb;j<na+nb;j++)
		{
			if(carry==0)
				break;
			uint v=(uint)(*vp)[j]+carry;
			(*vp)[j]=(uchar)(v&0xff);
			carry=(v>>8);
		}
		assert(carry==0);
	}
}
int Divide(const BigInt& a, const BigInt& b, BigInt& q, BigInt& m)
{
	assert(&q!=&m);
	if(&a==&b||&a==&q||&a==&m
		||&b==&q||&b==&m)
	{
		BigInt _a(a);
		BigInt _b(b);
		return Divide(_a,_b,q,m);
	}
	q.inner.Slit(new vector<uchar>);
	m.inner.Slit(new vector<uchar>);
	q.pos=(a.pos==b.pos);
	m.pos=b.pos;
	return a.AbsDiv(b,q,m,!q.pos);
}
int BigInt::AbsDiv(const BigInt& b, BigInt& q, BigInt& m, bool remp) const
{
	int na=Length(), nb=b.Length();
	assert(nb>0);
	vector<uchar> *vp=q.inner.toType(),
		*va=inner.toType(),*vb=b.inner.toType();
	vp->resize(na-nb+1>0?na-nb+1:0,0);
	for(int i=0;i<na-nb+1;i++)
	{
		(*vp)[i]=0;
	}
	vector<uchar>& r=*m.inner.toType();
	r=*va;
	vector<uchar> pro;
	pro.resize(nb+1,0);
	uint carry;
	for(int i=na-nb;i>=0;i--)
	{
		uint num=(uint)r[i+nb-1];
		if(i+nb<na)
		{
			num+=(((uint)r[i+nb])<<8);
		}
		uint den=(uint)((*vb)[nb-1]);
		uint quot=num/den;
		if(quot>0xff)
			quot=0xff;
		carry=0;
		for(int j=0;j<nb;j++)
		{
			uint p=((uint)(*vb)[j])*quot+carry;
			pro[j]=(uchar)(p&0xff);
			carry=(p>>8);
		}
		pro[nb]=(uchar)carry;
		while(AbsStageLess(r,pro,i))
		{
			quot--;
			carry=0;
			for(int j=0;j<nb;j++)
			{
				uint s=(uint)pro[j];
				uint c=carry+(uint)(*vb)[j];
				if(s<c)
				{
					carry=1;
					s+=0x100;
				}
				else
					carry=0;
				s-=c;
				pro[j]=(uchar)s;
			}
			assert((uint)pro[nb]>=carry);
			pro[nb]-=(uchar)carry;
		}
		(*vp)[i]=(uchar)quot;
		carry=0;
		for(int j=0;j<nb+1&&i+j<na;j++)
		{
			uint s=(uint)r[i+j];
			uint c=carry+(uint)pro[j];
			if(s<c)
			{
				carry=1;
				s+=0x100;
			}
			else
				carry=0;
			s-=c;
			r[i+j]=(uchar)s;
		}
		assert(carry==0);
	}
	assert(!b.AbsLE(m));
	if(remp&&m!=_zero)
	{
		carry=1;
		for(int i=0;i<na-nb+1;i++)
		{
			if(carry==0)
				break;
			uint s=(uint)(*vp)[i]+carry;
			(*vp)[i]=(uchar)(s&0xff);
			carry=(s>>8);
		}
		if(carry!=0)
		{
			vp->resize(na>=nb?na-nb+2:1,0);
			(*vp)[na>=nb?na-nb+1:0]=(uchar)carry;
		}
		carry=0;
		r.resize(nb,0);
		for(int i=0;i<nb;i++)
		{
			uint s=(uint)(*vb)[i];
			uint c=(uint)r[i]+carry;
			if(s<c)
			{
				s+=0x100;
				carry=1;
			}
			else
				carry=0;
			s-=c;
			r[i]=(uchar)s;
		}
		assert(carry==0);
	}
	return 0;
}
bool BigInt::AbsStageLess(const vector<uchar>& a, const vector<uchar>& b, uint stage)
{
	int na=a.size(),nb=b.size();
	int nmax=max(na,nb+(int)stage),
		nmin=min(na,nb+(int)stage);
	if(na>nb+(int)stage)
	{
		for(int i=nmax-1;i>=nmin;i--)
		{
			if(a[i]!=0)
				return false;
		}
	}
	else
	{
		for(int i=nmax-1;i>=nmin;i--)
		{
			if(i-stage>=0&&b[i-stage]!=0)
				return true;
		}
	}
	for(int i=nmin-1;i>=0;i--)
	{
		if(i-(int)stage<0)
		{
			if(a[i]!=0)
				return false;
		}
		else
		{
			if(a[i]<b[i-stage])
				return true;
			else if(a[i]>b[i-stage])
				return false;
		}
	}
	return false;
}
BigInt BigInt::abs() const
{
	BigInt n(*this);
	n.pos=true;
	return n;
}
bool BigInt::AbsEQ(const BigInt& b) const
{
	const vector<uchar>* m = inner.toType();
	const vector<uchar>* n = b.inner.toType();
	if(m->size()<n->size())
		swap(m,n);
	for(int i=(int)(m->size())-1;i>=(int)(n->size());i--)
	{
		if((*m)[i]!=0)
			return false;
	}
	for(int i=(int)(n->size())-1;i>=0;i--)
	{
		if((*m)[i]!=(*n)[i])
			return false;
	}
	return true;
}
bool BigInt::AbsLE(const BigInt& b) const
{
	vector<uchar>* m=inner.toType();
	vector<uchar>* n=b.inner.toType();
	bool l=m->size()>=n->size();
	int l0=max(m->size(),n->size()),
		l1=min(m->size(),n->size());
	if(l)
	{
		for(int i=l0-1;i>=l1;i--)
		{
			if((*m)[i]!=0)
				return false;
		}
	}
	else
	{
		for(int i=l0-1;i>=l1;i--)
		{
			if((*n)[i]!=0)
				return true;
		}
	}
	for(int i=l1-1;i>=0;i--)
	{
		if((*m)[i]>(*n)[i])
			return false;
		else if((*m)[i]<(*n)[i])
			return true;
	}
	return true;
}
BigInt::BigInt(int n):Group(get_type())
{
	bool p=n>=0;
	uint absval=(uint)(p?n:-n);
	vector<uchar>* v=new vector<uchar>;
	v->resize(4,0);
	for(int i=0;i<4;i++)
	{
		(*v)[i]=(uchar)(absval&0xff);
		absval>>=8;
	}
	inner.Attach(v);
	pos=p;
}
int BigInt::Length() const
{
	int i;
	assert(inner.toType()!=NULL);
	for(i=inner.toType()->size();i>0;i--)
	{
		if((*inner.toType())[i-1]!=0)
			break;
	}
	return i;
}
int BigInt::Tidy()
{
	int i;
	assert(inner.toType()!=NULL);
	for(i=inner.toType()->size();i>0;i--)
	{
		if((*inner.toType())[i-1]!=0)
			break;
		else
			inner.toType()->pop_back();
	}
	return i;
}
int BigInt::size() const
{
	assert(inner.toType()!=NULL);
	return inner.toType()->size();
}
void BigInt::resize(int n)
{
	assert(inner.toType()!=NULL);
	inner.toType()->resize(n,0);
}
uchar& BigInt::BitIndex(int n)
{
	assert(inner.toType()!=NULL);
	return (*inner.toType())[n];
}
inline uchar ToDigit(uchar d, bool dec)
{
	uchar digit=d;
	if(digit>=0&&digit<10)
	{
		return (uchar)(digit+'0');
	}
	else if(!dec&&digit>=10&&digit<16)
	{
		return (uchar)(digit-10+'a');
	}
	return 0;
}
inline uint FromDigit(uchar c, bool dec)
{
	if(c>='0'&&c<='9')
		return (uint)(c-'0');
	else if(!dec&&c>='a'&&c<='f')
		return (uint)(c-'a'+10);
	else if(!dec&&c>='A'&&c<='F')
		return (uint)(c-'A'+10);
	else
		return (uint)-1;
}
void BigInt::ToDecimal(string& s) const
{
	BigInt base(10);
	s.clear();
	if(*this==_zero)
	{
		s+="0";
		return;
	}
	BigInt r,d;
	r=abs();
	char c[2]={0,0};
	while(r!=_zero)
	{
		Divide(r,base,r,d);
		assert(d>=_zero&&d<base);
		c[0]=ToDigit(d.inner.toType()->size()!=0?
			(*d.inner.toType())[0]:0,true);
		s=string(c)+s;
	}
	if(!pos)
		s=string("-")+s;
}
void BigInt::FromDecimal(const string& s)
{
	bool positive;
	const char* cs=s.c_str();
	Zero();
	if(*cs=='-')
		positive=false,cs++;
	else
		positive=true;
	BigInt base(10);
	for(;*cs!=0;cs++)
	{
		uint d=FromDigit((uchar)*cs,true);
		if(!(d>=0&&d<10))
			break;
		*this=*this*base+BigInt((int)d);
	}
	pos=positive;
}
void BigInt::ToHex(string& s) const
{
	s.clear();
	if(*this==_zero)
	{
		s+="0";
		return;
	}
	if(!pos)
		s+="-";
	int length=Length();
	for(int i=length-1;i>=0;i--)
	{
		uchar hex=(*inner.toType())[i];
		if(i!=length-1||(hex>>4)!=0)
		{
			s+=ToDigit(hex>>4,false);
		}
		s+=ToDigit(hex&0xf,false);
	}
}
void BigInt::FromHex(const string& s)
{
	const char* cs=s.c_str();
	inner.Slit(new vector<uchar>);
	if(*cs=='-')
		pos=false,cs++;
	else
		pos=true;
	if(cs[0]=='0'&&cs[1]=='x')
	{
		cs+=2;
	}
	const char* begin=cs,*end;
	for(end=begin;FromDigit(*end,false)!=(uint)-1;end++);
	inner.toType()->resize(((end-begin)+1)/2,0);
	int i;
	for(i=0,end--;end>=begin;i++,end--)
	{
		uchar c=FromDigit(*end,false);
		if(end-1>=begin)
			c|=(FromDigit(*(--end),false)<<4);
		(*inner.toType())[i]=c;
	}
}
