#ifndef _MEM_H_
#define _MEM_H_
#include "common.h"
template<class T>
struct Stub
{
	mutable int ref;
	T * type;
public:
	void IncRef() const
	{
		ref++;
	}
	bool DecRef()
	{
		ref--;
		if(ref == 0)
		{
			if(type != NULL)
				delete type;
			delete this;
			return true;
		}
		return false;
	}
};
template<class T>
class Wrapper
{
	Stub<T> * entity;
public:
	T * toType() const
	{
		if(entity==NULL)
			return NULL;
		return entity->type;
	}
	T * Detach()
	{
		if(entity==NULL)
			return NULL;
		T * type = entity->type;
		entity->type = NULL;
		return type;
	}
	void Attach(T * t)
	{
		entity->type=t;
	}
	Wrapper(T * t = NULL)
	{
		Stub<T> * stub = new Stub<T>;
		stub->ref = 1;
		stub->type = t;
		entity = stub;
	}
	Wrapper(const Wrapper<T>& w)
	{
		w.entity->IncRef();
		entity = w.entity;
	}
	~Wrapper()
	{
		entity->DecRef();
	}
	//we want to handle self assignment so inc ref first
	Wrapper<T>& operator=(const Wrapper<T>& w)
	{
		w.entity->IncRef();
		entity->DecRef();
		entity = w.entity;
		return *this;
	}
	//newtype should be an unmanaged pointer
	void Slit(T * newtype)
	{
		assert(newtype!=entity->type);
		entity->DecRef();
		entity = new Stub<T>;
		entity->ref = 1;
		entity->type = newtype;
	}
};
class MemBase
{
public:
	virtual ~MemBase(){}
	virtual void Clone(const MemBase& m, bool recur=false)=0;
};
#endif