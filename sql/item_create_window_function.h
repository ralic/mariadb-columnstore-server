/* Copyright (C) 2013 Calpont Corp.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

/***********************************************************************
*   $Id: item_create_window_function.h 9559 2013-05-22 17:58:13Z xlou $
*
*
***********************************************************************/
#include <my_config.h>
#include "item.h"
#include "sql_error.h"
#include "errorids.h"
/* Functions to create a window function item. Used by sql/sql_yacc.yy */

#ifndef ITEM_CREATE_WINDOW_FUNCTION_H
#define ITEM_CREATE_WINDOW_FUNCTION_H

/**
  Public window function builder interface.
  The parser (sql/sql_yacc.yy) uses a factory / builder pattern to
  construct a Window Function Item object for each window function call.
  All the concrete function builders implements this interface,
  either directly or indirectly with some adapter helpers.
  Keeping the function creation separated from the bison grammar allows
  to simplify the parser, and avoid the need to introduce a new token
  for each function, which has undesirable side effects in the grammar.
*/

class Create_window_func
{
public:
  /** Constructor */
  Create_window_func(): respectNulls(true) {}
  	
  /**
    The builder create method.
    Given the function name and list or arguments, this method creates
    a window function item that represents the function call.
    In case or errors, a NULL item is returned, and an error is reported.
    
    @param thd The current thread
    @param name The function name
    @param item_list The list of arguments to the function, can be NULL
    @return An item representing the parsed function call, or NULL
  */
  virtual Item *create(THD *thd, LEX_STRING name, List<Item> *item_list) = 0;
  bool respectNulls; // for some window functions

protected:
  /** Destructor */
  virtual ~Create_window_func() {}
};

/**
  Adapter for functions that takes exactly zero arguments.
*/
class Create_window_func_arg0 : public Create_window_func
{
public:
	virtual Item *create(THD *thd, LEX_STRING name, List<Item> *item_list)
	{
		int arg_count= 0;

		if (item_list != NULL)
			arg_count= item_list->elements;

		if (arg_count != 0)
		{
			LEX_STRING args[1];
			args[0] = name;
			IDB_set_error(thd, logging::ERR_WF_WRONG_ARGS, args, 1);
			return NULL;
		}

		return create(thd, name);
	}

	/**
	  Builder method, with one argument.
	  @param thd The current thread
	  @param name The name of the function
	  @return An item representing the function call
	*/
	virtual Item *create(THD *thd, LEX_STRING name)
	{
		return new (thd->mem_root) Item_func_window(thd, name);
	}

	static Create_window_func_arg0 s_singleton;

protected:
	/** Constructor. */
	Create_window_func_arg0() {}
	/** Destructor. */
	virtual ~Create_window_func_arg0() {}
};

/**
  Adapter for functions that takes exactly one arguments.
*/
class Create_window_func_arg1 : public Create_window_func
{
public:
	virtual Item *create(THD *thd, LEX_STRING name, List<Item> *item_list)
	{
		int arg_count= 0;

		if (item_list)
			arg_count= item_list->elements;

		if (arg_count != 1)
		{
			LEX_STRING args[1];
			args[0] = name;
			IDB_set_error(thd, logging::ERR_WF_WRONG_ARGS, args, 1);
			return NULL;
		}

		Item *param_1= item_list->pop();

		if (! param_1->is_autogenerated_name)
		{
			LEX_STRING args[1];
			args[0] = name;
			IDB_set_error(thd, logging::ERR_WF_WRONG_ARGS, args, 1);
			return NULL;
		}
	
		return create(thd, name, param_1);
	}

	/**
	  Builder method, with one argument.
	  @param thd The current thread
	  @param name The name of the function
	  @param arg1 The first argument of the function
	  @return An item representing the function call
	*/ 
	virtual Item *create(THD *thd, LEX_STRING name, Item *arg1)
	{
		return new (thd->mem_root) Item_func_window(thd, name, arg1);
	}

	static Create_window_func_arg1 s_singleton;

protected:
	/** Constructor. */
	Create_window_func_arg1() {}
	/** Destructor. */
	virtual ~Create_window_func_arg1() {}
};

/**
  Adapter for functions that takes exactly two arguments.
*/
class Create_window_func_arg2 : public Create_window_func
{
public:
	virtual Item *create(THD *thd, LEX_STRING name, List<Item> *item_list)
	{
		int arg_count= 0;
	
		if (item_list)
			arg_count= item_list->elements;
	
		if (arg_count != 2)
		{
			LEX_STRING args[1];
			args[0] = name;
			IDB_set_error(thd, logging::ERR_WF_WRONG_ARGS, args, 1);
			return NULL;
		}
	
		Item *param_1= item_list->pop();
		Item *param_2= item_list->pop();
	
		if ((!param_1->is_autogenerated_name) || 
			 (!param_2->is_autogenerated_name))
		{
			LEX_STRING args[1];
			args[0] = name;
			IDB_set_error(thd, logging::ERR_WF_WRONG_ARGS, args, 1);
			return NULL;
		}
	
		return create(thd, name, param_1, param_2);
	}

	/**
	  Builder method, with two arguments.
	  @param thd The current thread
	  @param name The name of the function
	  @param arg1 The first argument of the function
	  @param arg2 The second argument of the function
	  @return An item representing the function call
	*/
	virtual Item *create(THD *thd, LEX_STRING name, Item *arg1, Item *arg2)
	{
		return new (thd->mem_root) Item_func_window(thd, name, arg1, arg2);
	}
	
	static Create_window_func_arg2 s_singleton;

protected:
	/** Constructor. */
	Create_window_func_arg2() {}
	/** Destructor. */
	virtual ~Create_window_func_arg2() {}
};

/**
  Adapter for functions that takes exactly three arguments.
*/
class Create_window_func_arg3 : public Create_window_func
{
public:
	virtual Item *create(THD *thd, LEX_STRING name, List<Item> *item_list)
	{
		int arg_count= 0;
	
		if (item_list)
			arg_count= item_list->elements;

		if (arg_count != 3)
		{
			IDB_set_error(thd, logging::ERR_WF_WRONG_ARGS, name.str);
			return NULL;
		}

		Item *param_1= item_list->pop();
		Item *param_2= item_list->pop();
		Item *param_3= item_list->pop();

		if ((!param_1->is_autogenerated_name) || 
			  (!param_2->is_autogenerated_name) ||
			  (!param_3->is_autogenerated_name))
		{
			IDB_set_error(thd, logging::ERR_WF_WRONG_ARGS, name.str);
			return NULL;
		}

		return create(thd, name, param_1, param_2, param_3);
	}

	/**
	  Builder method, with three arguments.
	  @param thd The current thread
	  @param arg1 The first argument of the function
	  @param arg2 The second argument of the function
	  @param arg3 The third argument of the function
	  @return An item representing the function call
	*/
	virtual Item *create(THD *thd, LEX_STRING name, Item *arg1, Item *arg2, Item *arg3)
	{
		return new (thd->mem_root) Item_func_window(thd, name, arg1, arg2, arg3);
	}

	static Create_window_func_arg3 s_singleton;

	protected:
	/** Constructor. */
	Create_window_func_arg3() {}
	/** Destructor. */
	virtual ~Create_window_func_arg3() {}
};

class Create_window_func_nth_value : public Create_window_func
{
public:
	Create_window_func_nth_value() : fromFirst(true) {}
	virtual Item *create(THD *thd, LEX_STRING name, List<Item> *item_list);
	virtual Item *create(THD *thd, LEX_STRING name, Item *arg1, Item *arg2, Item* arg3, Item* arg4);
	static Create_window_func_nth_value s_singleton;
	bool fromFirst;
protected:
	virtual ~Create_window_func_nth_value() {}
};


/**
  Find the native function builder associated with a given function name.
  @param thd The current thread
  @param name The native function name
  @return The native function builder associated with the name, or NULL
*/
extern Create_window_func * find_native_window_function_builder(THD *thd, LEX_STRING name);

/**
  Find the native function builder associated with a given function name.
  This is for window function syntax with RESPECT|IGNORE NULLS
  @param thd The current thread
  @param name The native function name
  @return The native function builder associated with the name, or NULL
*/
extern Create_window_func * find_native_window_function_builder_nulls(THD *thd, LEX_STRING name);

#endif
