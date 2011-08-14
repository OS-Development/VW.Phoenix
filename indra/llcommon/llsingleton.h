/** 
 * @file llsingleton.h
 *
 * $LicenseInfo:firstyear=2002&license=viewergpl$
 * 
 * Copyright (c) 2010, Linden Research, Inc.
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
#ifndef LLSINGLETON_H
#define LLSINGLETON_H

#include "llerror.h"	// *TODO: eliminate this

// LLSingleton implements the getInstance() method part of the Singleton
// pattern. It can't make the derived class constructors protected, though, so
// you have to do that yourself.
//
// There are two ways to use LLSingleton. The first way is to inherit from it
// while using the typename that you'd like to be static as the template
// parameter, like so:
//
//   class Foo: public LLSingleton<Foo>{};
//
//   Foo& instance = Foo::instance();
//
// The second way is to use the singleton class directly, without inheritance:
//
//   typedef LLSingleton<Foo> FooSingleton;
//
//   Foo& instance = FooSingleton::instance();
//
// In this case, the class being managed as a singleton needs to provide an
// initSingleton() method since the LLSingleton virtual method won't be
// available
//
// As currently written, it is not thread-safe.
template <typename T>
class LLSingleton
{
public:
	virtual ~LLSingleton() {}
#ifdef  LL_MSVC7
// workaround for VC7 compiler bug
// adapted from http://www.codeproject.com/KB/tips/VC2003MeyersSingletonBug.aspx
// our version doesn't introduce a nested struct so that you can still declare LLSingleton<MyClass>
// a friend and hide your constructor
	static T* getInstance()
    {
        LLSingleton<T> singleton;
        return singleton.vsHack();
    }

	T* vsHack()
#else
	static T* getInstance()
#endif
	{
		static T instance;
		static bool needs_init = true;
		if (needs_init)
		{
			needs_init = false;
			instance.initSingleton();
		}
		return &instance;
	}

	static T& instance()
	{
		return *getInstance();
	}

private:
	virtual void initSingleton() {}
};

#endif
