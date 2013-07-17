/*
 * BasicObject.cpp
 *
 *  Created on: 2013年7月1日
 *      Author: zhanggyb
 */

#include <iostream>

#include <BIL/BasicObject.h>

using namespace std;

namespace BIL {

	uint64_t BasicObject::id_last = 1;

	map<uint64_t, BasicObject*> BasicObject::map;

	BasicObject::BasicObject (BasicObject* parent)
			: _parent(parent)
	{
		// generate a unique id
		uint64_t temp = id_last;

		while (BasicObject::map.count(id_last) == 1) {
			id_last++;
			if (temp == id_last)
				throw;	// TODO: define exception
		}

		_id = id_last;

		registerObj();
		id_last++;

		if (_parent != NULL) {
			(_parent->_children).push_back(this);
		}
	}

	BasicObject::~BasicObject ()
	{
		BasicObject* item = NULL;
		while (_children.size() > 0) {
			item = _children.back();
			_children.pop_back();
			if (item != NULL) delete item;
		}

		_children.clear();

		if (_parent != NULL) {
			_parent->removeChild(this);	// Be careful of this line
		}

		unregisterObj();
	}

	inline bool BasicObject::registerObj (void)
	{
		BasicObject::map[_id] = this;
		return true;
	}

	BasicObject* BasicObject::find (uint64_t id)
	{
		BasicObject *ret = NULL;
		if (BasicObject::map.count(id) == 1)
			ret = BasicObject::map[id];

		return ret;
	}

	inline bool BasicObject::unregisterObj (void)
	{
		BasicObject::map.erase(_id);
		return true;
	}

	bool BasicObject::setParent (BasicObject* parent)
	{
		_parent = parent;

		if (_parent != NULL) {
			return (_parent->addChild(this));
		}

		return true;
	}

	bool BasicObject::addChild (BasicObject* child)
	{
		if (child == NULL)
			return false;

		if (child->_parent != NULL) {
			(child->_parent)->removeChild(child);
		}

		_children.push_back(child);
		child->_parent = this;

		return true;
	}

	bool BasicObject::removeChild (BasicObject* child)
	{
		if (child == NULL)
			return false;

		_children.erase(child);
		child->setParent(NULL);

		return true;
	}


	void BasicObject::deleteChildren (void)
	{
		BasicObject* item = NULL;

		while (_children.size() > 0) {
			item = _children.back();
			_children.pop_back();
			if (item != NULL) delete item;
		}

		_children.clear();
	}

} /* namespace BIL */