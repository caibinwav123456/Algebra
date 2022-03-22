#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bignumber.h"
#include "intring.h"
#include "MCD.h"
using namespace std;

#define RANDOM_NUM_WIDTH 20
#define RANDOM_RING_NUM_WIDTH 10
int mute=0;
int show_verify=0;
void Test();
int TryTest()
{
	printf("testing...\n");
	char* err_msg="\nthere are test errors.\n\n";
	try
	{
		Test();
	}
	catch(AgError a)
	{
		printf("error code:%d, file:%s, line:%d\n",a.id,a.file,a.line);
		printf(err_msg);
		return -1;
	}
	catch(...)
	{
		printf(err_msg);
		return -1;
	}
	printf("\ntest complete!\n\n");
	return 0;
}
void parse_args(int argc,char** argv)
{
	for(int i=1;i<argc;i++)
	{
		if(strcmp(argv[i],"-n")==0)
		{
			mute=1;
		}
		else if(strcmp(argv[i],"-v")==0)
		{
			show_verify=1;
		}
	}
}
int main(int argc,char** argv)
{
	printf("\nhello number!\n\n");
	parse_args(argc,argv);
	if(TryTest()!=0)
	{
		return -1;
	}
	return 0;
}
#define printf if(!mute)printf
string GenerateRandomByte(bool b)
{
	char s[]={"\0\0\0"};
	uchar x=(uchar)(rand()&0xff);
	if(b)
	{
		int v=rand()%4;
		if(v==0)
			x=0;
		else if(v==1)
			x=0xff;
	}
	sprintf(s,"%02x",(int)x);
	return s;
}
BigInt GenerateRandomInt(int w,bool b)
{
	string s="";
	int n=rand()%w;
	for(int i=0;i<n;i++)
	{
		s+=GenerateRandomByte(b);
	}
	BigInt m;
	m.FromHex(s);
	if(rand()%2)
		m.Inv();
	return m;
}
void GeneratePrimeList(vector<int>& prlist)
{
	prlist.clear();
	for(int i=2;i<256;i++)
	{
		bool b=false;
		for(int j=0;j<(int)prlist.size();j++)
		{
			if(prlist[j]*prlist[j]>i)
				break;
			if(i%prlist[j]==0)
			{
				b=true;
				break;
			}
		}
		if(!b)
			prlist.push_back(i);
	}
}
BigInt GenerateRandomPrimeInt(int w)
{
	static bool b=false;
	static vector<int> prlist;
	if(!b)
	{
		b=true;
		GeneratePrimeList(prlist);
	}
	BigInt n;
	while(true)
	{
		n=GenerateRandomInt(w,false).abs();
		if(n<BigInt(2))
			continue;
		BigInt q,r;
		bool find=false;
		for(int i=0;i<(int)prlist.size();i++)
		{
			BigInt y(prlist[i]);
			if(y*y>n)
				break;
			Divide(n,y,q,r);
			if(r==BigInt(0))
			{
				find=true;
				break;
			}
		}
		if(!find)
			break;
	}
	return n;
}
void TestMCD(const BigInt& a, const BigInt& b)
{
	if(a<=BigInt(0)||b<=BigInt(0))
		return;
	string sa,sb,sc,sd,se;
	a.ToHex(sa),b.ToHex(sb);
	BigInt c,c1,d,d1,e,e1;
	MCD(c,a,b);
	MCD(c1,b,a);
	LCD(d,e,a,b);
	LCD(d1,e1,b,a);
	assert(c==c1);
	assert(d*a+e*b==c&&d1*b+e1*a==c);
	BigInt q,r1,r2;
	Divide(a,c,q,r1);
	Divide(b,c,q,r2);
	assert(r1==BigInt(0)&&r2==BigInt(0));
	c.ToHex(sc);
	printf("MCD(%s,%s)=%s\n",sa.c_str(),sb.c_str(),sc.c_str());
	d.ToHex(sd),e.ToHex(se);
	printf("%s*%s+%s*%s=%s\n",sd.c_str(),sa.c_str(),se.c_str(),sb.c_str(),sc.c_str());
	d1.ToHex(sd),e1.ToHex(se);
	printf("%s*%s+%s*%s=%s\n",sd.c_str(),sb.c_str(),se.c_str(),sa.c_str(),sc.c_str());
}
void TestAB(const BigInt& a,const BigInt& b)
{
	string s1,s2;
	a.ToHex(s1),b.ToHex(s2);
	assert(a+b-b==a);
	printf("%s+%s-%s=%s\n",s1.c_str(),s2.c_str(),s2.c_str(),s1.c_str());
	assert(a+b-a==b);
	printf("%s+%s-%s=%s\n",s1.c_str(),s2.c_str(),s1.c_str(),s2.c_str());
	assert(a-(b+a)==-b);
	printf("%s-(%s+%s)=-(%s)\n",s1.c_str(),s2.c_str(),s1.c_str(),s2.c_str());
	assert(a+(b-a)==b);
	printf("%s+(%s-%s)=%s\n",s1.c_str(),s2.c_str(),s1.c_str(),s2.c_str());
	assert(a-(a-b)==b);
	printf("%s-(%s-%s)=%s\n",s1.c_str(),s1.c_str(),s2.c_str(),s2.c_str());
	assert(a*b==b*a);
	printf("%s*%s=%s*%s\n",s1.c_str(),s2.c_str(),s2.c_str(),s1.c_str());

	BigInt a0,b0;
	a0=a,b0=b;
	a0.Mul(b0);
	if(b0!=BigInt(0))
	{
		Divide(a0,b0,a0,b0);
		assert(a0==a&&b0==BigInt(0));
	}

	BigInt a1,b1;
	a1=a,b1=b;
	if(b1!=BigInt(0))
	{
		Divide(a1,b1,a1,b1);
		assert(a1*b+b1==a);		
	}
	{
		string s;
		BigInt b;
		a.ToDecimal(s);
		b.FromDecimal(s);
		assert(a==b);
	}
	{
		TestMCD(a.abs(),b.abs());
	}
}
void TestABC(const BigInt& a,const BigInt& b,const BigInt& c)
{
	string s1,s2,s3;
	a.ToHex(s1),b.ToHex(s2),c.ToHex(s3);
	assert((a+b)+c==a+(b+c));
	assert((a*b)*c==a*(b*c));
	assert((a+b)*c==a*c+b*c);
	printf("(%s+%s)+%s=%s+(%s+%s)\n",
		s1.c_str(),s2.c_str(),s3.c_str(),
		s1.c_str(),s2.c_str(),s3.c_str());
	printf("(%s*%s)*%s=%s*(%s*%s)\n",
		s1.c_str(),s2.c_str(),s3.c_str(),
		s1.c_str(),s2.c_str(),s3.c_str());
	printf("(%s+%s)*%s=%s*%s+%s*%s\n"
		,s1.c_str(),s2.c_str(),s3.c_str(),
		s1.c_str(),s3.c_str(),s2.c_str(),s3.c_str());
	TestAB(a,b);
	TestAB(b,c);
	TestAB(c,a);
}
void TestIntRingAB(const IntRing& a,const IntRing& b)
{
	assert(a+b==b+a);
	assert(a*b==b*a);
	assert(a-b==-b+a);
	assert(a+b-b==a);
	assert(a+b-a==b);
	assert(a-(b+a)==-b);
	assert(a+(b-a)==b);
	assert(a-(a-b)==b);
	IntRing zero=a;
	zero.Zero();
	if(a!=zero&&b!=zero)
	{
		assert(a/b*b==a&&a*b/b==a);
	}
}
void TestIntRingABC(const BigInt& a,const BigInt& b,const BigInt& c,const BigInt& base)
{
	string s1,s2,s3,sb;
	a.ToHex(s1),b.ToHex(s2),c.ToHex(s3),base.ToHex(sb);
	IntRing ra(base,a),rb(base,b),rc(base,c);
	assert((ra+rb)+rc==ra+(rb+rc));
	assert((ra*rb)*rc==ra*(rb*rc));
	assert((ra+rb)*rc==ra*rc+rb*rc);
	IntRing zero=ra;
	zero.Zero();
	if(rb!=zero&&rc!=zero)
	{
		assert(ra/rb*rc==ra*rc/rb);
		assert(ra*rb/rc==ra*(rb/rc));
		assert((ra+rb)/rc==ra/rc+rb/rc);
	}
	TestIntRingAB(ra,rb);
	TestIntRingAB(rb,rc);
	TestIntRingAB(rc,ra);
	printf("verifying intring with a=%s,b=%s,c=%s,base=%s\n",
		s1.c_str(),s2.c_str(),s3.c_str(),sb.c_str());
}
#undef printf
void TestRandom()
{
	printf("\nstart random test...00%%");
	uint t=(uint)time(NULL);
	srand(t);
	for(int i=0;i<1000;i++)
	{
		BigInt a,b,c;
		try
		{
			a=GenerateRandomInt(RANDOM_NUM_WIDTH,true),
			b=GenerateRandomInt(RANDOM_NUM_WIDTH,true),
			c=GenerateRandomInt(RANDOM_NUM_WIDTH,true);
			TestABC(a,b,c);
			a=GenerateRandomInt(RANDOM_NUM_WIDTH,false),
			b=GenerateRandomInt(RANDOM_NUM_WIDTH,false),
			c=GenerateRandomInt(RANDOM_NUM_WIDTH,false);
			TestABC(a,b,c);
		}
		catch(...)
		{
			string sa,sb,sc;
			a.ToHex(sa);
			b.ToHex(sb);
			c.ToHex(sc);
			printf("Error in TestABC(%s,%s,%s)\n",sa.c_str(),sb.c_str(),sc.c_str());
			throw;
		}
		try
		{
			a=GenerateRandomInt(RANDOM_NUM_WIDTH,false),
			b=GenerateRandomInt(RANDOM_NUM_WIDTH,false),
			TestABC(a,b,BigInt(0));
		}
		catch(...)
		{
			string sa,sb;
			a.ToHex(sa);
			b.ToHex(sb);
			printf("Error in TestABC(%s,%s,0)\n",sa.c_str(),sb.c_str(),"0");
			throw;
		}
		if(i%10==0)
			printf("%s%02d%%",mute?"\b\b\b":"",i/10);
	}
	printf("%s100%%",mute?"\b\b\b":"");
	printf("...00%%");
	t=(uint)time(NULL);
	srand(t);
	for(int i=0;i<1000;i++)
	{
		BigInt a,b,c,base;
		try
		{
			base=GenerateRandomPrimeInt(RANDOM_RING_NUM_WIDTH);
			a=GenerateRandomInt(RANDOM_RING_NUM_WIDTH,false),
			b=GenerateRandomInt(RANDOM_RING_NUM_WIDTH,false),
			c=GenerateRandomInt(RANDOM_RING_NUM_WIDTH,false);
			TestIntRingABC(a,b,c,base);
		}
		catch(AgError ae)
		{
			if(ae.id==ERR_VERIFY)
			{
				if(show_verify)printf("\nignoring verify failure.\n");
			}
			else
			{
				string sa,sb,sc,sbase;
				a.ToHex(sa);
				b.ToHex(sb);
				c.ToHex(sc);
				base.ToHex(sbase);
				printf("Error in TestIntRingABC(%s,%s,%s,%s)\n",
					sa.c_str(),sb.c_str(),sc.c_str(),sbase.c_str());
				throw;
			}
		}
		if(i%10==0)
			printf("%s%02d%%",mute?"\b\b\b":"",i/10);
	}
	printf("%s100%%",mute?"\b\b\b":"");
	printf("\n");
}
#define printf if(!mute)printf
void Test()
{
	BigInt n;
	int num[]={-65535,255,-255,-234567,0};
	string s;
	for(int i=0;i<sizeof(num)/sizeof(int);i++)
	{
		BigInt mn(num[i]);
		BigInt mn1(mn);
		BigInt mn2;
		mn2=mn1;
		mn2.ToHex(s);
		printf("%d is %s, length is %d, ", num[i], s.c_str(), mn2.Length());
		mn2.Tidy();
		printf("length after tidy is %d\n", mn2.Length());
	}
	{
		BigInt mn(0);
		mn.Tidy();;
		BigInt mn1(mn);
		BigInt mn2;
		mn2=mn1;
		mn2.ToHex(s);
		printf("%d is %s, length is %d\n", 0, s.c_str(), mn2.Length());
	}
	string snum[]={"5fa","-4578B","-0x6c8E","-0x06c8E","0","-00",""};
	for(int i=0;i<sizeof(snum)/sizeof(string);i++)
	{
		n.FromHex(snum[i]);
		n.ToHex(s);
		printf("%s is %s\n",snum[i].c_str(),s.c_str());
	}
	string snum1a[]={"5fa","-00679e","005600fb","-0","56fc","-56fc"};
	string snum1b[]={"5fa","-0056edac","ca","0","56ba","-56ba"};
	assert(sizeof(snum1a)==sizeof(snum1b));
	for(int i=0;i<sizeof(snum1a)/sizeof(string);i++)
	{
		BigInt a,b;
		a.FromHex(snum1a[i]),b.FromHex(snum1b[i]);
		printf("%s,%s:==%s,<=%s,>=%s,<%s,>%s,!=%s\n",snum1a[i].c_str(),snum1b[i].c_str(),
			a==b?"t":"f",a<=b?"t":"f",a>=b?"t":"f",a<b?"t":"f",a>b?"t":"f",a!=b?"t":"f");
		printf("%s,%s:==%s,<=%s,>=%s,<%s,>%s,!=%s\n",snum1b[i].c_str(),snum1a[i].c_str(),
			b==a?"t":"f",b<=a?"t":"f",b>=a?"t":"f",b<a?"t":"f",b>a?"t":"f",b!=a?"t":"f");
	}
	string snum2a[]={"ffff","0ffff","1234","0fe","0fe","00fcabcd","0fee9"};
	string snum2b[]={"ffff","0ffff","1234","00fcab","00fcabcd","0fe","00ffabcd"};
	string snum2c[]={"ffff","0ffff","1234","001c7b","f2a9dc","03afe","00fea1c"};
	assert(sizeof(snum2a)==sizeof(snum2b));
	assert(sizeof(snum2a)==sizeof(snum2c));
	for(int m=0;m<2;m++)
	for(int n=0;n<4;n++)
	for(int i=0;i<sizeof(snum2a)/sizeof(string);i++)
	{
		BigInt a,b,a0,b0;
		string s,s0;
		a.FromHex(snum2a[i]),b.FromHex(snum2b[i]);
		if(n%2)
			a.Inv();
		if(n/2)
			b.Inv();
		string s1,s2;
		a.ToHex(s1);
		b.ToHex(s2);
		if(m==0)
		{
			a0=a;
			a.Add(b);
			a.ToHex(s);
			a.Sub(b);
			a.ToHex(s0);
			assert(a==a0);
			printf("%s+%s=%s\n",s1.c_str(),s2.c_str(),s.c_str());
			printf("%s-%s=%s\n",s.c_str(),s2.c_str(),s0.c_str());
		}
		else
		{
			a0=a;
			b0=b;
			a.Add(b);
			a.ToHex(s);
			a.Sub(a0);
			a.ToHex(s0);
			assert(a==b0);
			printf("%s+%s=%s\n",s1.c_str(),s2.c_str(),s.c_str());
			printf("%s-%s=%s\n",s.c_str(),s1.c_str(),s0.c_str());
			TestAB(a0,b0);
		}
		{
			BigInt a;
			string s,s0;
			a.FromHex(snum2b[i]);
			a.Add(a);
			a.ToHex(s);
			a.Sub(a);
			a.ToHex(s0);
			assert(a==BigInt(0));
			printf("%s+%s=%s\n",snum2b[i].c_str(),snum2b[i].c_str(),s.c_str());
			printf("%s-%s=%s\n",s.c_str(),s.c_str(),s0.c_str());
		}
	}
	for(int n=0;n<4;n++)
	for(int i=0;i<sizeof(snum2a)/sizeof(string);i++)
	{
		BigInt a,b,a0,b0,a1;
		string s,s0;
		a.FromHex(snum2a[i]),b.FromHex(snum2b[i]);
		if(n%2)
			a.Inv();
		if(n/2)
			b.Inv();
		string s1,s2;
		a.ToHex(s1);
		b.ToHex(s2);
		{
			a0=a;
			a1=a;
			b0=b;
			a.Mul(b);
			a.ToHex(s);
			printf("%s*%s=%s\n",s1.c_str(),s2.c_str(),s.c_str());
		}
		{
			a0.Mul(a0);
			a0.ToHex(s);
			printf("%s*%s=%s\n",s1.c_str(),s1.c_str(),s.c_str());
		}
		for(int m=0;m<2;m++)
		{
			BigInt c;
			string s3;
			c.FromHex(snum2c[i]);
			if(m==1)
				c.Inv();
			c.ToHex(s3);
			TestABC(a1,b0,c);
		}
	}
	string snum3[]={"1234","5678","8734","543879","0756901","063709"};
	for(int n=0;n<2;n++)
	for(int i=0;i<sizeof(snum3)/sizeof(string);i++)
	{
		BigInt a,b;
		string s;
		a.FromDecimal(snum3[i]);
		if(n)
			a.Inv();
		a.ToDecimal(s);
		b.FromDecimal(s);
		assert(a==b);
	}
	{
		int numa[]={1,3};
		int numb[]={2,5};
		assert(sizeof(numa)==sizeof(numb));
		for(int i=0;i<sizeof(numa)/sizeof(int);i++)
		{
			BigInt a(numa[i]),b(numb[i]);
			TestMCD(a,b);
		}
	}
	{
		IntRing a(5,2),b(7,3);
		Wrapper<MemBase> clone=a.CloneR(b);
		clone=a.CloneR(b,true);
	}
	TestRandom();
}
