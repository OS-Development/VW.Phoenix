/** 
 * @file llmemory.h
 * @brief Memory allocation/deallocation header-stuff goes here.
 *
 * $LicenseInfo:firstyear=2002&license=viewergpl$
 * 
 * Copyright (c) 2002-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */
#ifndef LL_MEMORY_H
#define LL_MEMORY_H

#include <new>
#include <cstdlib>

#include "llerror.h"
#include "llmemtype.h"

const U32 LLREFCOUNT_SENTINEL_VALUE = 0xAAAAAAAA;

//----------------------------------------------------------------------------

#if LL_DEBUG
inline void* ll_aligned_malloc( size_t size, int align )
{
	void* mem = malloc( size + (align - 1) + sizeof(void*) );
	char* aligned = ((char*)mem) + sizeof(void*);
	aligned += align - ((uintptr_t)aligned & (align - 1));

	((void**)aligned)[-1] = mem;
	return aligned;
}

inline void ll_aligned_free( void* ptr )
{
	free( ((void**)ptr)[-1] );
}

inline void* ll_aligned_malloc_16(size_t size) // returned hunk MUST be freed with ll_aligned_free_16().
{
#if defined(LL_WINDOWS)
	return _mm_malloc(size, 16);
#elif defined(LL_DARWIN)
	return malloc(size); // default osx malloc is 16 byte aligned.
#else
	void *rtn;
	if (LL_LIKELY(0 == posix_memalign(&rtn, 16, size)))
		return rtn;
	else // bad alignment requested, or out of memory
		return NULL;
#endif
}

inline void ll_aligned_free_16(void *p)
{
#if defined(LL_WINDOWS)
	_mm_free(p);
#elif defined(LL_DARWIN)
	return free(p);
#else
	free(p); // posix_memalign() is compatible with heap deallocator
#endif
}

inline void* ll_aligned_malloc_32(size_t size) // returned hunk MUST be freed with ll_aligned_free_32().
{
#if defined(LL_WINDOWS)
	return _mm_malloc(size, 32);
#elif defined(LL_DARWIN)
	return ll_aligned_malloc( size, 32 );
#else
	void *rtn;
	if (LL_LIKELY(0 == posix_memalign(&rtn, 32, size)))
		return rtn;
	else // bad alignment requested, or out of memory
		return NULL;
#endif
}

inline void ll_aligned_free_32(void *p)
{
#if defined(LL_WINDOWS)
	_mm_free(p);
#elif defined(LL_DARWIN)
	ll_aligned_free( p );
#else
	free(p); // posix_memalign() is compatible with heap deallocator
#endif
}
#else // LL_DEBUG
// ll_aligned_foo are noops now that we use tcmalloc everywhere (tcmalloc aligns automatically at appropriate intervals)
#define ll_aligned_malloc( size, align ) malloc(size)
#define ll_aligned_free( ptr ) free(ptr)
#define ll_aligned_malloc_16 malloc
#define ll_aligned_free_16 free
#define ll_aligned_malloc_32 malloc
#define ll_aligned_free_32 free
#endif // LL_DEBUG

class LL_COMMON_API LLMemory
{
public:
	static void initClass();
	static void cleanupClass();
	static void freeReserve();
private:
	static char* reserveMem;
};

//----------------------------------------------------------------------------
// RefCount objects should generally only be accessed by way of LLPointer<>'s
// NOTE: LLPointer<LLFoo> x = new LLFoo(); MAY NOT BE THREAD SAFE
//   if LLFoo::LLFoo() does anything like put itself in an update queue.
//   The queue may get accessed before it gets assigned to x.
// The correct implementation is:
//   LLPointer<LLFoo> x = new LLFoo; // constructor does not do anything interesting
//   x->instantiate(); // does stuff like place x into an update queue

// see llthread.h for LLThreadSafeRefCount

//----------------------------------------------------------------------------

class LL_COMMON_API LLRefCount
{
protected:
	LLRefCount(const LLRefCount&);
private:
	LLRefCount&operator=(const LLRefCount&);

protected:
	virtual ~LLRefCount(); // use unref()
	
public:
	LLRefCount();

	void ref()
	{ 
		mRef++; 
	} 

	S32 unref()
	{
		llassert(mRef >= 1);
		if (0 == --mRef) 
		{
			delete this; 
			return 0;
		}
		return mRef;
	}	

	S32 getNumRefs() const
	{
		return mRef;
	}

private: 
	S32	mRef; 
};

//----------------------------------------------------------------------------

// Note: relies on Type having ref() and unref() methods
template <class Type> class LLPointer
{
public:

	LLPointer() : 
		mPointer(NULL)
	{
	}

	LLPointer(Type* ptr) : 
		mPointer(ptr)
	{
		ref();
	}

	LLPointer(const LLPointer<Type>& ptr) : 
		mPointer(ptr.mPointer)
	{
		ref();
	}

	// support conversion up the type hierarchy.  See Item 45 in Effective C++, 3rd Ed.
	template<typename Subclass>
	LLPointer(const LLPointer<Subclass>& ptr) : 
		mPointer(ptr.get())
	{
		ref();
	}

	~LLPointer()								
	{
		unref();
	}

	Type*	get() const							{ return mPointer; }
	const Type*	operator->() const				{ return mPointer; }
	Type*	operator->()						{ return mPointer; }
	const Type&	operator*() const				{ return *mPointer; }
	Type&	operator*()							{ return *mPointer; }

	operator BOOL()  const						{ return (mPointer != NULL); }
	operator bool()  const						{ return (mPointer != NULL); }
	bool operator!() const						{ return (mPointer == NULL); }
	bool isNull() const							{ return (mPointer == NULL); }
	bool notNull() const						{ return (mPointer != NULL); }

	operator Type*()       const				{ return mPointer; }
	operator const Type*() const				{ return mPointer; }
	bool operator !=(Type* ptr) const           { return (mPointer != ptr); 	}
	bool operator ==(Type* ptr) const           { return (mPointer == ptr); 	}
	bool operator ==(const LLPointer<Type>& ptr) const           { return (mPointer == ptr.mPointer); 	}
	bool operator < (const LLPointer<Type>& ptr) const           { return (mPointer < ptr.mPointer); 	}
	bool operator > (const LLPointer<Type>& ptr) const           { return (mPointer > ptr.mPointer); 	}

	LLPointer<Type>& operator =(Type* ptr)                   
	{ 
		if( mPointer != ptr )
		{
			unref(); 
			mPointer = ptr; 
			ref();
		}

		return *this; 
	}

	LLPointer<Type>& operator =(const LLPointer<Type>& ptr)  
	{ 
		if( mPointer != ptr.mPointer )
		{
			unref(); 
			mPointer = ptr.mPointer;
			ref();
		}
		return *this; 
	}

	// support assignment up the type hierarchy. See Item 45 in Effective C++, 3rd Ed.
	template<typename Subclass>
	LLPointer<Type>& operator =(const LLPointer<Subclass>& ptr)  
	{ 
		if( mPointer != ptr.get() )
		{
			unref(); 
			mPointer = ptr.get();
			ref();
		}
		return *this; 
	}
	
	// Just exchange the pointers, which will not change the reference counts.
	static void swap(LLPointer<Type>& a, LLPointer<Type>& b)
	{
		Type* temp = a.mPointer;
		a.mPointer = b.mPointer;
		b.mPointer = temp;
	}

protected:
	void ref()                             
	{ 
		if (mPointer)
		{
			mPointer->ref();
		}
	}

	void unref()
	{
		if (mPointer)
		{
			Type *tempp = mPointer;
			mPointer = NULL;
			tempp->unref();
			if (mPointer != NULL)
			{
				llwarns << "Unreference did assignment to non-NULL because of destructor" << llendl;
				unref();
			}
		}
	}

protected:
	Type*	mPointer;
};

//template <class Type> 
//class LLPointerTraits
//{
//	static Type* null();
//};
//
// Expands LLPointer to return a pointer to a special instance of class Type instead of NULL.
// This is useful in instances where operations on NULL pointers are semantically safe and/or
// when error checking occurs at a different granularity or in a different part of the code
// than when referencing an object via a LLSafeHandle.
// 

template <class Type> 
class LLSafeHandle
{
public:
	LLSafeHandle() :
		mPointer(NULL)
	{
	}

	LLSafeHandle(Type* ptr) : 
		mPointer(NULL)
	{
		assign(ptr);
	}

	LLSafeHandle(const LLSafeHandle<Type>& ptr) : 
		mPointer(NULL)
	{
		assign(ptr.mPointer);
	}

	// support conversion up the type hierarchy.  See Item 45 in Effective C++, 3rd Ed.
	template<typename Subclass>
	LLSafeHandle(const LLSafeHandle<Subclass>& ptr) : 
		mPointer(NULL)
	{
		assign(ptr.get());
	}

	~LLSafeHandle()								
	{
		unref();
	}

	const Type*	operator->() const				{ return nonNull(mPointer); }
	Type*	operator->()						{ return nonNull(mPointer); }

	Type*	get() const							{ return mPointer; }
	// we disallow these operations as they expose our null objects to direct manipulation
	// and bypass the reference counting semantics
	//const Type&	operator*() const			{ return *nonNull(mPointer); }
	//Type&	operator*()							{ return *nonNull(mPointer); }

	operator BOOL()  const						{ return mPointer != NULL; }
	operator bool()  const						{ return mPointer != NULL; }
	bool operator!() const						{ return mPointer == NULL; }
	bool isNull() const							{ return mPointer == NULL; }
	bool notNull() const						{ return mPointer != NULL; }


	operator Type*()       const				{ return mPointer; }
	operator const Type*() const				{ return mPointer; }
	bool operator !=(Type* ptr) const           { return (mPointer != ptr); 	}
	bool operator ==(Type* ptr) const           { return (mPointer == ptr); 	}
	bool operator ==(const LLSafeHandle<Type>& ptr) const           { return (mPointer == ptr.mPointer); 	}
	bool operator < (const LLSafeHandle<Type>& ptr) const           { return (mPointer < ptr.mPointer); 	}
	bool operator > (const LLSafeHandle<Type>& ptr) const           { return (mPointer > ptr.mPointer); 	}

	LLSafeHandle<Type>& operator =(Type* ptr)                   
	{ 
		assign(ptr);
		return *this; 
	}

	LLSafeHandle<Type>& operator =(const LLSafeHandle<Type>& ptr)  
	{ 
		assign(ptr.mPointer);
		return *this; 
	}

	// support assignment up the type hierarchy. See Item 45 in Effective C++, 3rd Ed.
	template<typename Subclass>
	LLSafeHandle<Type>& operator =(const LLSafeHandle<Subclass>& ptr)  
	{ 
		assign(ptr.get());
		return *this; 
	}

public:
	typedef Type* (*NullFunc)();
	static const NullFunc sNullFunc;

protected:
	void ref()                             
	{ 
		if (mPointer)
		{
			mPointer->ref();
		}
	}

	void unref()
	{
		if (mPointer)
		{
			Type *tempp = mPointer;
			mPointer = NULL;
			tempp->unref();
			if (mPointer != NULL)
			{
				llwarns << "Unreference did assignment to non-NULL because of destructor" << llendl;
				unref();
			}
		}
	}

	void assign(Type* ptr)
	{
		if( mPointer != ptr )
		{
			unref(); 
			mPointer = ptr; 
			ref();
		}
	}

	static Type* nonNull(Type* ptr)
	{
		return ptr == NULL ? sNullFunc() : ptr;
	}

protected:
	Type*	mPointer;
};

// LLInitializedPointer is just a pointer with a default constructor that initializes it to NULL
// NOT a smart pointer like LLPointer<>
// Useful for example in std::map<int,LLInitializedPointer<LLFoo> >
//  (std::map uses the default constructor for creating new entries)
template <typename T> class LLInitializedPointer
{
public:
	LLInitializedPointer() : mPointer(NULL) {}
	~LLInitializedPointer() { delete mPointer; }
	
	const T* operator->() const { return mPointer; }
	T* operator->() 			{ return mPointer; }
	const T& operator*() const 	{ return *mPointer; }
	T& operator*() 				{ return *mPointer; }
	operator const T*() const	{ return mPointer; }
	operator T*()				{ return mPointer; }
	T* operator=(T* x)				{ return (mPointer = x); }
	operator bool() const		{ return mPointer != NULL; }
	bool operator!() const		{ return mPointer == NULL; }
	bool operator==(T* rhs)		{ return mPointer == rhs; }
	bool operator==(const LLInitializedPointer<T>* rhs) { return mPointer == rhs.mPointer; }

protected:
	T* mPointer;
};	


// Return the resident set size of the current process, in bytes.
// Return value is zero if not known.
LL_COMMON_API U64 getCurrentRSS();

#endif
