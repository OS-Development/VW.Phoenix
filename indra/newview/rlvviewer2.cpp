/** 
 * $LicenseInfo:firstyear=2004&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"
#include "llagent.h"
#include "llgesturemgr.h"
#include "llviewerinventory.h"
#include "llvoavatar.h"
#include "cofmgr.h"
#include "rlvviewer2.h"

// ============================================================================
// From llinventoryobserver.cpp

void fetch_items_from_llsd(const LLSD& items_llsd);

const F32 LLInventoryFetchItemsObserver::FETCH_TIMER_EXPIRY = 60.0f;

LLInventoryFetchItemsObserver::LLInventoryFetchItemsObserver(const LLUUID& item_id)
{
	mIDs.clear();
	if (item_id != LLUUID::null)
	{
		mIDs.push_back(item_id);
	}
}

LLInventoryFetchItemsObserver::LLInventoryFetchItemsObserver(const uuid_vec_t& item_ids)
	: mIDs(item_ids)
{
}

BOOL LLInventoryFetchItemsObserver::isFinished() const
{
	return mIncomplete.empty();
}

void LLInventoryFetchItemsObserver::changed(U32 mask)
{
	lldebugs << this << " remaining incomplete " << mIncomplete.size()
			 << " complete " << mComplete.size()
			 << " wait period " << mFetchingPeriod.getRemainingTimeF32()
			 << llendl;

	// scan through the incomplete items and move or erase them as
	// appropriate.
	if (!mIncomplete.empty())
	{
		// Have we exceeded max wait time?
		bool timeout_expired = mFetchingPeriod.hasExpired();

		for (uuid_vec_t::iterator it = mIncomplete.begin(); it < mIncomplete.end(); )
		{
			const LLUUID& item_id = (*it);
			LLViewerInventoryItem* item = gInventory.getItem(item_id);
			if (item && item->isFinished())
			{
				mComplete.push_back(item_id);
				it = mIncomplete.erase(it);
			}
			else
			{
				if (timeout_expired)
				{
					// Just concede that this item hasn't arrived in reasonable time and continue on.
					llwarns << "Fetcher timed out when fetching inventory item UUID: " << item_id << LL_ENDL;
					it = mIncomplete.erase(it);
				}
				else
				{
					// Keep trying.
					++it;
				}
			}
		}

	}

	if (mIncomplete.empty())
	{
		lldebugs << this << " done at remaining incomplete "
				 << mIncomplete.size() << " complete " << mComplete.size() << llendl;
		done();
	}
	//llinfos << "LLInventoryFetchItemsObserver::changed() mComplete size " << mComplete.size() << llendl;
	//llinfos << "LLInventoryFetchItemsObserver::changed() mIncomplete size " << mIncomplete.size() << llendl;
}

void LLInventoryFetchItemsObserver::startFetch()
{
	LLUUID owner_id;
	LLSD items_llsd;
	for (uuid_vec_t::const_iterator it = mIDs.begin(); it < mIDs.end(); ++it)
	{
		LLViewerInventoryItem* item = gInventory.getItem(*it);
		if (item)
		{
			if (item->isFinished())
			{
				// It's complete, so put it on the complete container.
				mComplete.push_back(*it);
				continue;
			}
			else
			{
				owner_id = item->getPermissions().getOwner();
			}
		}
		else
		{
			// assume it's agent inventory.
			owner_id = gAgent.getID();
		}

		// Ignore categories since they're not items.  We
		// could also just add this to mComplete but not sure what the
		// side-effects would be, so ignoring to be safe.
		LLViewerInventoryCategory* cat = gInventory.getCategory(*it);
		if (cat)
		{
			continue;
		}

		// It's incomplete, so put it on the incomplete container, and
		// pack this on the message.
		mIncomplete.push_back(*it);
		
		// Prepare the data to fetch
		LLSD item_entry;
		item_entry["owner_id"] = owner_id;
		item_entry["item_id"] = (*it);
		items_llsd.append(item_entry);
	}

	mFetchingPeriod.reset();
	mFetchingPeriod.setTimerExpirySec(FETCH_TIMER_EXPIRY);

	fetch_items_from_llsd(items_llsd);
}

// ============================================================================
// From llinventoryfunctions.cpp

LLFindWearablesEx::LLFindWearablesEx(bool is_worn, bool include_body_parts)
:	mIsWorn(is_worn)
,	mIncludeBodyParts(include_body_parts)
{}

bool LLFindWearablesEx::operator()(LLInventoryCategory* cat, LLInventoryItem* item)
{
	LLViewerInventoryItem *vitem = dynamic_cast<LLViewerInventoryItem*>(item);
	if (!vitem) return false;

	// Skip non-wearables.
	if (!vitem->isWearableType() && vitem->getType() != LLAssetType::AT_OBJECT)
	{
		return false;
	}

	// Skip body parts if requested.
	if (!mIncludeBodyParts && vitem->getType() == LLAssetType::AT_BODYPART)
	{
		return false;
	}

	// Skip broken links.
	if (vitem->getIsBrokenLink())
	{
		return false;
	}

	return (bool) get_is_item_worn(item->getUUID()) == mIsWorn;
}

void change_item_parent(LLInventoryModel* model, LLViewerInventoryItem* item, const LLUUID& new_parent_id, BOOL restamp)
{
	if (item->getParentUUID() != new_parent_id)
	{
		LLInventoryModel::update_list_t update;
		LLInventoryModel::LLCategoryUpdate old_folder(item->getParentUUID(),-1);
		update.push_back(old_folder);
		LLInventoryModel::LLCategoryUpdate new_folder(new_parent_id, 1);
		update.push_back(new_folder);
		gInventory.accountForUpdate(update);

		LLPointer<LLViewerInventoryItem> new_item = new LLViewerInventoryItem(item);
		new_item->setParent(new_parent_id);
		new_item->updateParentOnServer(restamp);
		model->updateItem(new_item);
		model->notifyObservers();
	}
}

void change_category_parent(LLInventoryModel* model, LLViewerInventoryCategory* cat, const LLUUID& new_parent_id, BOOL restamp)
{
	if (!model || !cat)
	{
		return;
	}

	// Can't move a folder into a child of itself.
	if (model->isObjectDescendentOf(new_parent_id, cat->getUUID()))
	{
		return;
	}

	LLInventoryModel::update_list_t update;
	LLInventoryModel::LLCategoryUpdate old_folder(cat->getParentUUID(), -1);
	update.push_back(old_folder);
	LLInventoryModel::LLCategoryUpdate new_folder(new_parent_id, 1);
	update.push_back(new_folder);
	model->accountForUpdate(update);

	LLPointer<LLViewerInventoryCategory> new_cat = new LLViewerInventoryCategory(cat);
	new_cat->setParent(new_parent_id);
	new_cat->updateParentOnServer(restamp);
	model->updateCategory(new_cat);
	model->notifyObservers();
}

BOOL get_is_item_worn(const LLUUID& id)
{
	const LLViewerInventoryItem* item = gInventory.getItem(id);
	if (!item)
		return FALSE;

	// Consider the item as worn if it has links in COF.
// [SL:KB] - The code below causes problems across the board so it really just needs to go
//	if (LLAppearanceMgr::instance().isLinkInCOF(id))
//	{
//		return TRUE;
//	}

	switch(item->getType())
	{
		case LLAssetType::AT_OBJECT:
		{
			if (gAgent.getAvatarObject() && gAgent.getAvatarObject()->isWearingAttachment(item->getLinkedUUID()))
				return TRUE;
			break;
		}
		case LLAssetType::AT_BODYPART:
		case LLAssetType::AT_CLOTHING:
			if (gAgent.isWearingItem(item->getLinkedUUID()))
				return TRUE;
			break;
		case LLAssetType::AT_GESTURE:
			if (gGestureManager.isGestureActive(item->getLinkedUUID()))
				return TRUE;
			break;
		default:
			break;
	}
	return FALSE;
}

// ============================================================================
// From lloutfitobserver.cpp

LLCOFObserver::LLCOFObserver() :
	mCOFLastVersion(LLViewerInventoryCategory::VERSION_UNKNOWN)
{
	mItemNameHash.finalize();
	gInventory.addObserver(this);
}

LLCOFObserver::~LLCOFObserver()
{
	if (gInventory.containsObserver(this))
	{
		gInventory.removeObserver(this);
	}
}

void LLCOFObserver::changed(U32 mask)
{
	if (!gInventory.isInventoryUsable())
		return;

	checkCOF();
}

// static
S32 LLCOFObserver::getCategoryVersion(const LLUUID& cat_id)
{
	LLViewerInventoryCategory* cat = gInventory.getCategory(cat_id);
	if (!cat)
		return LLViewerInventoryCategory::VERSION_UNKNOWN;

	return cat->getVersion();
}

// static
const std::string& LLCOFObserver::getCategoryName(const LLUUID& cat_id)
{
	LLViewerInventoryCategory* cat = gInventory.getCategory(cat_id);
	if (!cat)
		return LLStringUtil::null;

	return cat->getName();
}

bool LLCOFObserver::checkCOF()
{
	LLUUID cof = LLCOFMgr::getInstance()->getCOF();
	if (cof.isNull())
		return false;

	bool cof_changed = false;
	LLMD5 item_name_hash = hashDirectDescendentNames(cof);
	if (item_name_hash != mItemNameHash)
	{
		cof_changed = true;
		mItemNameHash = item_name_hash;
	}

	S32 cof_version = getCategoryVersion(cof);
	if (cof_version != mCOFLastVersion)
	{
		cof_changed = true;
		mCOFLastVersion = cof_version;
	}

	if (!cof_changed)
		return false;
	
	mCOFChanged();

	return true;
}

LLMD5 LLCOFObserver::hashDirectDescendentNames(const LLUUID& cat_id)
{
	LLInventoryModel::cat_array_t* cat_array;
	LLInventoryModel::item_array_t* item_array;
	gInventory.getDirectDescendentsOf(cat_id,cat_array,item_array);
	LLMD5 item_name_hash;
	if (!item_array)
	{
		item_name_hash.finalize();
		return item_name_hash;
	}
	for (LLInventoryModel::item_array_t::const_iterator iter = item_array->begin();
		 iter != item_array->end();
		 iter++)
	{
		const LLViewerInventoryItem *item = (*iter);
		if (!item)
			continue;
		item_name_hash.update(item->getName());
	}
	item_name_hash.finalize();
	return item_name_hash;
}

// ============================================================================
