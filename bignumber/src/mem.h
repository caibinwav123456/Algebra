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
	void SlitR(T *(*get_new)(T*),
		void (*assign_new)(T*,T*),
		T * reftype = NULL)
	{
		assert(get_new!=NULL && assign_new!=NULL);
		if(entity->ref>1)
			Slit(get_new(reftype));
		else if(reftype!=NULL && entity->type!=reftype)
			assign_new(entity->type,reftype);
	}
};
class MemBase
{
public:
	virtual ~MemBase(){}
	virtual void Clone(const MemBase& m, bool recur=false)=0;
	virtual Wrapper<MemBase> CloneR(const MemBase& m, bool recur=false) const=0;
};
template<class T, class TInst>
T *__get_new__(T* ref_type)
{
	if(ref_type==NULL)
		return new TInst;
	TInst * t=dynamic_cast<TInst*>(ref_type);
	assert(t!=NULL);
	return new TInst(*t);
}
template<class T, class TInst>
void __assign_new__(T* type,T* ref_type)
{
	if(type==NULL || ref_type==NULL)
		return;
	TInst * t=dynamic_cast<TInst*>(type);
	assert(t!=NULL);
	TInst * tref=dynamic_cast<TInst*>(ref_type);
	assert(tref!=NULL);
	*t=*tref;
}
template<class T, class TInst>
void __assign_new_c__(const T* type,const T* ref_type)
{
	if(type==NULL || ref_type==NULL)
		return;
	T* nct=const_cast<T*>(type);
	TInst * t=dynamic_cast<TInst*>(nct);
	assert(t!=NULL);
	const TInst * tref=dynamic_cast<const TInst*>(ref_type);
	assert(tref!=NULL);
	*t=*tref;
}
#define slitx(w,t,ti,ref) ((w).SlitR(__get_new__<t,ti>,__assign_new__<t,ti>,ref))
#define slitr(w,t,ref) slitx(w,t,t,ref)
#define slit(w,t) slitr(w,t,NULL)
#define slitx_c(w,t,ti,ref) ((w).SlitR(__get_new__<const t,const ti>,__assign_new_c__<t,ti>,ref))
#define slitr_c(w,t,ref) slitx_c(w,t,t,ref)
#define slit_c(w,t) slitr_c(w,t,NULL)
#endif