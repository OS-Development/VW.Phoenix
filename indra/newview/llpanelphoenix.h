/** 
 * @file llpanelgeneral.h
 * @author ppl
 * @brief General preferences panel in preferences floater
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
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
 * online at http://secondlifegrid.net/programs/open_source/licensing/flossexception
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

#ifndef LL_LLPanelPhoenix_H
#define LL_LLPanelPhoenix_H

#include "llpanel.h"


#include "llviewerinventory.h"

class JCInvDropTarget : public LLView
{
public:
	JCInvDropTarget(const std::string& name, const LLRect& rect, void (*callback)(LLViewerInventoryItem*));
	~JCInvDropTarget();

	void doDrop(EDragAndDropType cargo_type, void* cargo_data);

	//
	// LLView functionality
	virtual BOOL handleDragAndDrop(S32 x, S32 y, MASK mask, BOOL drop,
		EDragAndDropType cargo_type,
		void* cargo_data,
		EAcceptance* accept,
		std::string& tooltip_msg);
protected:
	void	(*mDownCallback)(LLViewerInventoryItem*);
};

class LLPanelPhoenix : public LLPanel
{
public:
	LLPanelPhoenix();
	virtual ~LLPanelPhoenix();

	virtual BOOL postBuild();
	void refresh();
	void apply();
	void cancel();

private:
	//static void onSelectSkin(LLUICtrl* ctrl, void* data);
	//static void onClickClassic(void* data);
	//static void onClickSilver(void* data);
	static void onClickBoobReset(void* data);
	static void onClickVoiceRevertProd(void* data);
	static void onCustomBeam(void* data);
	static void onCustomBeamColor(void* data);

	static void onSpellAdd(void* data);
	static void onSpellRemove(void* data);
	static void onSpellGetMore(void* data);
	static void onSpellEditCustom(void* data);

	static void onStealth(void* data);
	static void callbackPhoenixStealth(const LLSD &notification, const LLSD &response);
	static void callbackPhoenixNoStealth(const LLSD &notification, const LLSD &response);
	static void onNoStealth(void* data);
	static void onClickVoiceRevertDebug(void* data);
	static void onRefresh(void* data);
	static void onKeywordAllertButton(void * data);
	static void onAutoCorrectButton(void * data);
	static void onBeamDelete(void* data);
	static void onBeamColorDelete(void* data);
	static void onCommitApplyControl(LLUICtrl* caller, void* user_data);
	static void onCommitAvatarModifier(LLUICtrl* ctrl, void* userdata);
	static void onTexturePickerCommit(LLUICtrl* ctrl, void* userdata);
	static void onComboBoxCommit(LLUICtrl* ctrl, void* userdata);
	static void onTagsBoxCommit(LLUICtrl* ctrl, void* userdata);
	static void onTagComboBoxCommit(LLUICtrl* ctrl, void* userdata);
	static void onSpellBaseComboBoxCommit(LLUICtrl* ctrl, void* userdata);	
	static void beamUpdateCall(LLUICtrl* ctrl, void* userdata);
	static void onClickSetMirror(void*);
	static void onClickSetHDDInclude(void*);
	static void onClickSetXed(void*);
    static void onClickOtrHelp(void* data); // [$PLOTR$/]
	static void onConditionalPreferencesChanged(LLUICtrl* ctrl, void* userdata);
	//static void onCommitVoiceDebugServerName(LLUICtrl* caller, void* user_data);
	//static void onCommitAvatarEffectsChange(LLUICtrl* caller, void* user_data);
	//static void onCommitAutoResponse(LLUICtrl* caller, void* user_data);
	static void onPhoenixInstantMessageAnnounceIncoming(LLUICtrl* ctrl, void* userdata);

private:
	std::string mSkin;
	static LLPanelPhoenix* sInstance;
	static JCInvDropTarget* mObjectDropTarget;
	static JCInvDropTarget* mBuildObjectDropTarget;
	static void IMAutoResponseItemDrop(LLViewerInventoryItem* item);
	static void BuildAutoResponseItemDrop(LLViewerInventoryItem* item);

protected:
	void initHelpBtn(const std::string& name, const std::string& xml_alert);
	static void onClickHelp(void* data);

};

#endif // LL_LLPanelPhoenix_H
