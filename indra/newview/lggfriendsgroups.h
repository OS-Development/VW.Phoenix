/* Copyright (C) 2011 Greg Hendrickson (LordGregGreg Back)

   This is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.
 
   This is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.
 
   You should have received a copy of the GNU Lesser General Public
   License along with the viewer; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */


#ifndef LGG_FRIENDS_GROUPS_H
#define LGG_FRIENDS_GROUPS_H
class LGGFriendsGroups
{
	LGGFriendsGroups();
	~LGGFriendsGroups();
	static LGGFriendsGroups* sInstance;
public:
	static LGGFriendsGroups* getInstance();
	static LLColor4 toneDownColor(LLColor4 inColor, float strength);

	BOOL saveGroupToDisk(std::string groupName, std::string fileName);
	LLSD exportGroup(std::string groupName);
	LLSD getFriendsGroups();
	LLColor4 getGroupColor(std::string groupName);
	LLColor4 getFriendColor(LLUUID friend_id, std::string ignoredGroupName="");
	LLColor4 getDefaultColor();
	void setDefaultColor(LLColor4 dColor);
	std::vector<std::string> getFriendGroups(LLUUID friend_id);
	std::vector<std::string> getAllGroups();
	std::vector<LLUUID> getFriendsInGroup(std::string groupName);
	BOOL isFriendInGroup(LLUUID friend_id, std::string groupName);
	BOOL notifyForFriend(LLUUID friend_id);

	void addFriendToGroup(LLUUID friend_id, std::string groupName);
	void removeFriendFromGroup(LLUUID friend_id, std::string groupName);
	void addGroup(std::string groupName);
	void deleteGroup(std::string groupName);
	void setNotifyForGroup(std::string groupName, BOOL notify);
	BOOL getNotifyForGroup(std::string groupName);
	void setGroupColor(std::string groupName, LLColor4 color);



	void runTest();
	void save();
	

	void loadFromDisk();

private:
	void saveToDisk(LLSD newSettings);
	LLSD getExampleLLSD();	
	std::string getFileName();
	std::string getDefaultFileName();

	LLSD mFriendsGroups;

};



#endif 