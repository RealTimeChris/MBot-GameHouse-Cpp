// RemoveObject.hpp - Header for the "remove object" command.
// Jun 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class RemoveObject :public BaseFunction {
	public:
		RemoveObject() {
			this->commandName = "removeobject";
			this->helpDescription = "Remove an object from your own or someone else's inventory.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter or /removeobject OBJECTNAME. Alternatively, /removeobject = OBJECTNAME, @USERMENTION.\n------");
			msgEmbed.setTitle("__**Remove Object Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<RemoveObject>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();

			bool areWeInADm = areWeInADM(*args.eventData, channel);

			if (areWeInADm ==  true) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData));

			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId() ,.guildId = args.eventData->getGuildId(), }).get();
			DiscordGuildMember discordGuildMember(guildMember);

			bool doWeHaveAdminPermission = doWeHaveAdminPermissions(args, *args.eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPermission == false) {
				return;
			}

			std::regex idRegExp("\\d{18}");
			std::cmatch matchResults;
			std::string roleID;
			std::string objectName;
			if (regex_search(args.commandData.optionsArgs.at(0).c_str(), matchResults, idRegExp)) {
				roleID = matchResults.str();
			}
			else {
				objectName = args.commandData.optionsArgs.at(0);
			}

			std::string userID;
			std::regex userMentionRegExp("<@!\\d{18}>");
			if (args.commandData.optionsArgs.size() == 1) {
				userID = args.eventData->getAuthorId();
			}
			else {
				std::string argOne = args.commandData.optionsArgs.at(1);
				std::cmatch matchResultsNew;
				regex_search(argOne.c_str(), matchResultsNew, idRegExp);
				std::string userIDOne = matchResultsNew.str();
				userID = userIDOne;
			}

			GuildMember targetMember = GuildMembers::getGuildMemberAsync({ .guildMemberId = userID ,.guildId = args.eventData->getGuildId(), }).get();

			if (targetMember.user.userName == "") {
				std::string msgString = "------\n**Sorry, but that user could not be found!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**User Issue:**__");
				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}

			DiscordGuildMember guildMemberData(targetMember);

			if (guildMemberData.data.userName == "") {
				std::string msgString = "------\n**Sorry, but the specified user data could not be found!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**User Issue:**__");
				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}

			std::string objectType;
			bool isObjectFound = false;

			for (uint32_t x = 0; x < guildMemberData.data.items.size(); x += 1) {
				if (objectName ==  guildMemberData.data.items[x].itemName) {
					objectType = "item";
					isObjectFound = true;
					guildMemberData.data.items.erase(guildMemberData.data.items.begin() + x);
					guildMemberData.writeDataToDB();
				}
			}

			for (uint32_t x = 0; x < guildMemberData.data.roles.size(); x += 1) {
				if (roleID ==  guildMemberData.data.roles[x].roleId || objectName == guildMemberData.data.roles[x].roleName) {
					objectType = "role";
					isObjectFound = true;
					objectName = guildMemberData.data.roles[x].roleName;
					roleID = guildMemberData.data.roles[x].roleId;
					guildMemberData.data.roles.erase(guildMemberData.data.roles.begin() + x);
					guildMemberData.writeDataToDB();
				}
			}

			EmbedData messageEmbed;

			if (isObjectFound == false) {
				std::string msgString= "-------\n**Sorry, but the item was not found in the inventory!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Item Issue:**__");
				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}

			std::string msgString;

			if (objectType ==  "role") {
				Roles::removeGuildMemberRoleAsync({ .guildId = args.eventData->getGuildId(), .userId = targetMember.user.id, .roleId = roleID, }).get();
				msgString = "------\n**You've removed the following role from <@!" + userID + ">'s inventory:**\n------\n __**" + objectName + "**__\n------";
				messageEmbed.setTitle("__**Role Removed:**__");
			}
			else if (objectType == "item") {
				msgString = "------\n**You've removed the following item from <@!" + userID + ">'s inventory:**\n------\n __**" + objectName + "**__\n------";
				messageEmbed.setTitle("__**Item Removed:**__");
			}

			messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl()).setColor(discordGuild.data.borderColor).setDescription(msgString).setTimeStamp(getTimeAndDate());
			RespondToInputEventData dataPackage(*args.eventData);
			dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
			dataPackage.addMessageEmbed(messageEmbed);
			auto newEvent = InputEvents::respondToEvent(dataPackage);

			return;
		}
		virtual ~RemoveObject() {};
	};
}
