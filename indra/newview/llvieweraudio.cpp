/** 
 * @file llvieweraudio.cpp
 * @brief Audio functions that used to be in viewer.cpp
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

#include "llviewerprecompiledheaders.h"

#include "llaudioengine.h"
#include "llagent.h"
#include "llappviewer.h"
#include "llvieweraudio.h"
#include "llviewercamera.h"
#include "llviewercontrol.h"
#include "llviewerwindow.h"
#include "llvoiceclient.h"
#include "llviewermedia.h"

/////////////////////////////////////////////////////////

void init_audio() 
{
	if (!gAudiop) 
	{
		llwarns << "Failed to create an appropriate Audio Engine" << llendl;
		return;
	}
	LLVector3d lpos_global = gAgent.getCameraPositionGlobal();
	LLVector3 lpos_global_f;

	lpos_global_f.setVec(lpos_global);
					
	gAudiop->setListener(lpos_global_f,
						  LLVector3::zero,	// LLViewerCamera::getInstance()->getVelocity(),    // !!! BUG need to replace this with smoothed velocity!
						  LLViewerCamera::getInstance()->getUpAxis(),
						  LLViewerCamera::getInstance()->getAtAxis());

// load up our initial set of sounds we'll want so they're in memory and ready to be played

	BOOL mute_audio = gSavedSettings.getBOOL("MuteAudio");

	if (!mute_audio && FALSE == gSavedSettings.getBOOL("NoPreload"))
	{
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndAlert")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("PhoenixAvatarAgeAlertSoundUUID")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndBadKeystroke")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndChatFromObject")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndClick")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndClickRelease")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndHealthReductionF")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndHealthReductionM")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndIncomingChat")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndIncomingIM")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndInvApplyToObject")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndInvalidOp")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndInventoryCopyToInv")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndMoneyChangeDown")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndMoneyChangeUp")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndObjectCopyToInv")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndObjectCreate")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndObjectDelete")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndObjectRezIn")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndObjectRezOut")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuAppear")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuHide")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuSliceHighlight0")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuSliceHighlight1")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuSliceHighlight2")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuSliceHighlight3")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuSliceHighlight4")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuSliceHighlight5")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuSliceHighlight6")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndPieMenuSliceHighlight7")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndSnapshot")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndStartAutopilot")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndStartFollowpilot")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndStartIM")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndStopAutopilot")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndTeleportOut")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndTextureApplyToObject")));
		//gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndTextureCopyToInv")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndTyping")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndWindowClose")));
		gAudiop->preloadSound(LLUUID(gSavedSettings.getString("UISndWindowOpen")));
	}

	audio_update_volume(true);
}

void audio_update_volume(bool force_update)
{

	static LLCachedControl<bool> sMuteAudio(gSavedSettings, "MuteAudio");
	static LLCachedControl<bool> sMuteAmbient(gSavedSettings, "MuteAmbient");
	static LLCachedControl<bool> sMuteSounds(gSavedSettings, "MuteSounds");
	static LLCachedControl<bool> sMuteUI(gSavedSettings, "MuteUI");
	static LLCachedControl<bool> sMuteMusic(gSavedSettings, "MuteMusic");
	static LLCachedControl<bool> sMuteMedia(gSavedSettings, "MuteMedia");
	static LLCachedControl<bool> sMuteVoice(gSavedSettings, "MuteVoice");
	static LLCachedControl<bool> sMuteWhenMinimized(gSavedSettings, "MuteWhenMinimized");
	static LLCachedControl<F32> sAudioLevelMaster(gSavedSettings, "AudioLevelMaster");
	static LLCachedControl<F32> sAudioLevelAmbient(gSavedSettings, "AudioLevelAmbient");
	static LLCachedControl<F32> sAudioLevelUI(gSavedSettings, "AudioLevelUI");
	static LLCachedControl<F32> sAudioLevelSFX(gSavedSettings, "AudioLevelSFX");
	static LLCachedControl<F32> sAudioLevelMusic(gSavedSettings, "AudioLevelMusic");
	static LLCachedControl<F32> sAudioLevelMedia(gSavedSettings, "AudioLevelMedia");
	static LLCachedControl<F32> sAudioLevelVoice(gSavedSettings, "AudioLevelVoice");
	static LLCachedControl<F32> sAudioLevelMic(gSavedSettings, "AudioLevelMic");
	static LLCachedControl<F32> sAudioLevelDoppler(gSavedSettings, "AudioLevelDoppler");
	static LLCachedControl<F32> sAudioLevelRolloff(gSavedSettings, "AudioLevelRolloff");

	bool mute = sMuteAudio;
	if (sMuteWhenMinimized && !gViewerWindow->getActive())
	{
		mute = true;
	}
	F32 mute_volume = mute ? 0.0f : 1.0f;

	if (gAudiop) 
	{
		// Sound Effects
		gAudiop->setMasterGain(sAudioLevelMaster);

		gAudiop->setDopplerFactor(sAudioLevelDoppler);
		gAudiop->setRolloffFactor(sAudioLevelRolloff);
		gAudiop->setMuted(mute);

		if (force_update)
		{
			audio_update_wind(true);
		}

		// handle secondary gains
		gAudiop->setSecondaryGain(LLAudioEngine::AUDIO_TYPE_SFX,
								  (sMuteSounds || mute) ? 0.f : sAudioLevelSFX);
		gAudiop->setSecondaryGain(LLAudioEngine::AUDIO_TYPE_UI,
								  (sMuteUI || mute) ? 0.f : sAudioLevelUI);
		gAudiop->setSecondaryGain(LLAudioEngine::AUDIO_TYPE_AMBIENT,
								  (sMuteAmbient || mute) ? 0.f : sAudioLevelAmbient);

		// Streaming Music
		F32 music_volume = sAudioLevelMusic;
		music_volume = mute_volume * sAudioLevelMaster * music_volume * music_volume;
		gAudiop->setInternetStreamGain(sMuteMusic ? 0.f : music_volume);
	}

	// Streaming Media
	F32 media_volume = sAudioLevelMedia;
	media_volume = mute_volume * sAudioLevelMaster * media_volume * media_volume;
	LLViewerMedia::setVolume(sMuteMedia ? 0.0f : media_volume);

	// Voice
	if (LLVoiceClient::instanceExists())
	{
		F32 voice_volume = sAudioLevelVoice;
		voice_volume = mute_volume * sAudioLevelMaster * voice_volume;
		LLVoiceClient::getInstance()->setVoiceVolume(sMuteVoice ? 0.f : voice_volume);
		LLVoiceClient::getInstance()->setMicGain(sMuteVoice ? 0.f : sAudioLevelMic);

		if (sMuteWhenMinimized && !gViewerWindow->getActive())
		{
			LLVoiceClient::getInstance()->setMuteMic(true);
		}
		else
		{
			LLVoiceClient::getInstance()->setMuteMic(false);
		}
	}
}

void audio_update_listener()
{
	if (gAudiop)
	{
		// update listener position because agent has moved
		LLVector3d lpos_global = gAgent.getCameraPositionGlobal();
		LLVector3 lpos_global_f;
		lpos_global_f.setVec(lpos_global);
	
		gAudiop->setListener(lpos_global_f,
							 // LLViewerCamera::getInstance()VelocitySmoothed, 
							 // LLVector3::zero,
							 gAgent.getVelocity(),    // !!! *TODO: need to replace this with smoothed velocity!
							 LLViewerCamera::getInstance()->getUpAxis(),
							 LLViewerCamera::getInstance()->getAtAxis());
	}
}

void audio_update_wind(bool force_update)
{
	static LLCachedControl<bool> sMuteAudio(gSavedSettings, "MuteAudio");
	static LLCachedControl<bool> sMuteAmbient(gSavedSettings, "MuteAmbient");
	static LLCachedControl<F32> sAudioLevelMaster(gSavedSettings, "AudioLevelMaster");
	static LLCachedControl<F32> sAudioLevelAmbient(gSavedSettings, "AudioLevelAmbient");
	static LLCachedControl<F32> sAudioLevelRolloff(gSavedSettings, "AudioLevelRolloff");

	//
	//  Extract height above water to modulate filter by whether above/below water 
	// 
	LLViewerRegion* region = gAgent.getRegion();
	if (!gAudiop->getWindMuted() && region) // disable wind /ez
	{
		static F32 last_camera_water_height = -1000.f;
		LLVector3 camera_pos = gAgent.getCameraPositionAgent();
		F32 camera_water_height = camera_pos.mV[VZ] - region->getWaterHeight();
		
		//
		//  Don't update rolloff factor unless water surface has been crossed
		//
		if (force_update || (last_camera_water_height * camera_water_height) < 0.f)
		{
			if (camera_water_height < 0.f)
			{
				gAudiop->setRolloffFactor(sAudioLevelRolloff * LL_ROLLOFF_MULTIPLIER_UNDER_WATER);
			}
			else 
			{
				gAudiop->setRolloffFactor(sAudioLevelRolloff);
			}
		}
		// this line rotates the wind vector to be listener (agent) relative
		// unfortunately we have to pre-translate to undo the translation that
		// occurs in the transform call
		gRelativeWindVec = gAgent.getFrameAgent().rotateToLocal(gWindVec - gAgent.getVelocity());

		// don't use the setter setMaxWindGain() because we don't
		// want to screw up the fade-in on startup by setting actual source gain
		// outside the fade-in.
		F32 master_volume  = sMuteAudio ? 0.f : sAudioLevelMaster;
		F32 ambient_volume = sMuteAmbient ? 0.f : sAudioLevelAmbient;

		F32 wind_volume = master_volume * ambient_volume;
		gAudiop->mMaxWindGain = wind_volume;

		last_camera_water_height = camera_water_height;
		gAudiop->updateWind(gRelativeWindVec, camera_water_height);
	}
}
