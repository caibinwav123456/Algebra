#ifndef _BIGNUMBER_H_
#define _BIGNUMBER_H_
#include "common.h"
#include "group.h"
#include "predicate.h"
using namespace std;
class BigInt:virtual public Group, public IPred
{
	friend BigInt operator+(const BigInt& a, const BigInt& b);
	friend BigInt operator-(const BigInt& a, const BigInt& b);
	friend BigInt operator*(const BigInt& a, const BigInt& b);
	friend int Divide(const BigInt& a, const BigInt& b, BigInt& q, BigInt& m);

public:
	virtual bool Equal(const Group& y) const;
	virtual void Add(const Group& y);
	virtual void Sub(const Group& y);
	virtual void Inv();
	virtual void Mul(const Group& y);
	virtual void Zero();
	virtual void Clone(const MemBase& m, bool recur=false);
	virtual Wrapper<MemBase> CloneR(const MemBase& m, bool recur=false) const;
	BigInt(int n=0);
	int Length() const;
	int Tidy();
	int size() const;
	void resize(int n);
	uchar& BitIndex(int n);
	BigInt operator-() const;
	BigInt abs() const;
	void ToDecimal(string& s) const;
	void FromDecimal(const string& s);
	void ToHex(string& s) const;
	void FromHex(const string& s);

protected:
	virtual bool LessEqual(const IPred& y) const;
	static const BigInt _zero;

private:
	bool AbsLE(const BigInt& b) const;
	bool AbsEQ(const BigInt& b) const;
	void AbsAdd(const BigInt& b);
	void AbsSub(const BigInt& b, bool rev);
	void AbsMul(BigInt& pro, const BigInt& b) const;
	int AbsDiv(const BigInt& b, BigInt& q, BigInt& m, bool remp) const;
	static bool AbsStageLess(const vector<uchar>& a, const vector<uchar>& b, uint stage);
	static const Wrapper<const GroupType>& get_type();

	bool pos;
	Wrapper<vector<uchar>> inner;
};
#endif
