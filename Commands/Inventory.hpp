// Inventory.hpp - Header for the "inventory" command.
// Jun 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Inventory : public BaseFunction {
	  public:
		Inventory() {
			this->commandName = "inventory";
			this->helpDescription = "Inspect your own or someone else's inventoryt";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter /inventory. Alternatively, enter /inventory @USERMENTION to view someone else's inventory.\n------");
			msgEmbed.setTitle("__**Inventory Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Inventory>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = argsNew.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.eventData, discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				uint64_t userID;

				std::regex userIDRegExp{ ".{2,3}\\d{18}>" };
				std::regex idRegExp{ "\\d{18}" };
				if (argsNew.optionsArgs.size() == 0) {
					userID = argsNew.eventData.getAuthorId();
				} else {
					std::string argZero = argsNew.optionsArgs.at(0);
					std::cmatch userIDMatch;
					regex_search(argZero.c_str(), userIDMatch, idRegExp);
					std::string userIDOne = userIDMatch.str();
					userID = stoull(userIDOne);
				}

				GuildMember currentGuildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = userID, .guildId = argsNew.eventData.getGuildId() }).get();

				DiscordGuildMember discordGuildMember(currentGuildMember);

				if (currentGuildMember.userName == "") {
					std::string msgString = "------\n**Sorry, but the specified user data could not be found!**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User Issue:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				std::string userName = currentGuildMember.userName;

				std::vector<Role> rolesArray = Roles::getGuildRolesAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				InputEventData event02 = argsNew.eventData;

				for (uint32_t x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
					bool isRoleFound = false;
					InventoryRole userRole = discordGuildMember.data.roles[x];
					for (auto& value2: rolesArray) {
						if (value2.id == userRole.roleId) {
							isRoleFound = true;
							break;
						}
					}
					if (isRoleFound == false) {
						discordGuildMember.data.roles.erase(discordGuildMember.data.roles.begin() + x);
						discordGuildMember.writeDataToDB();
						std::string msgString = "------\n**Removing role " + userRole.roleName + " from user cache!**\n------";
						EmbedData msgEmbed{};
						msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Role Issue:**__");
						RespondToInputEventData dataPackage(argsNew.eventData);
						dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
						event02 = InputEvents::respondToInputEventAsync(dataPackage).get();
						InputEvents::deleteInputEventResponseAsync(event02, 20000);
						x -= 1;
					}
				}

				std::vector<std::string> itemsMsgString;
				uint32_t currentPage = 0;

				for (uint32_t x = 0; x < discordGuildMember.data.items.size(); x += 1) {
					if (x == 0) {
						itemsMsgString.push_back("");
					}
					std::string itemsMsgStringTemp = "";
					itemsMsgStringTemp = "**| __Item:__** " + discordGuildMember.data.items[x].emoji + discordGuildMember.data.items[x].itemName + " **| __Value:__** " +
						std::to_string(discordGuildMember.data.items[x].itemCost) + " **| __Self-Mod:__** " + std::to_string(discordGuildMember.data.items[x].selfMod) +
						" **| __Opp-Mod:__** " + std::to_string(discordGuildMember.data.items[x].oppMod) + "\n";
					if (itemsMsgStringTemp.length() + itemsMsgString[currentPage].length() >= 2048) {
						currentPage += 1;
						itemsMsgString.push_back("");
					}
					itemsMsgString[currentPage] += itemsMsgStringTemp;
				}

				uint32_t currentPage2 = 0;
				std::vector<std::string> rolesMsgStrings;

				for (uint32_t x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
					if (x == 0) {
						rolesMsgStrings.push_back("");
					}
					std::string rolesMsgStringTemp = "";
					rolesMsgStringTemp = "**| __Role:__** <@&" + std::to_string(discordGuildMember.data.roles.at(x).roleId) + "> **| __Value:__** " +
						std::to_string(discordGuildMember.data.roles.at(x).roleCost) + "\n";
					if (rolesMsgStringTemp.length() + rolesMsgStrings[currentPage2].length() > 2048) {
						currentPage2 += 1;
						rolesMsgStrings.push_back("");
					}
					rolesMsgStrings[currentPage2] += rolesMsgStringTemp;
				}

				std::vector<EmbedData> itemsMessageEmbeds;

				for (uint32_t x = 0; x < itemsMsgString.size(); x += 1) {
					EmbedData newEmbed;
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**" + userName + "'s Inventory (Items) Page " + std::to_string(x + 1) + " of " +
						std::to_string(itemsMsgString.size() + rolesMsgStrings.size()) + ":**__");
					newEmbed.setDescription(itemsMsgString[x]);
					newEmbed.setColor(discordGuild.data.borderColor);
					newEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					itemsMessageEmbeds.push_back(newEmbed);
				}

				std::vector<EmbedData> rolesMsgEmbeds;

				for (uint32_t x = 0; x < rolesMsgStrings.size(); x += 1) {
					EmbedData newEmbed;
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setTitle("__**" + userName + "'s Inventory (Roles) Page " + std::to_string(itemsMessageEmbeds.size() + x + 1) + " of " +
						std::to_string(itemsMsgString.size() + rolesMsgStrings.size()) + ":**__");
					newEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					newEmbed.setDescription(rolesMsgStrings[x]);
					newEmbed.setColor(discordGuild.data.borderColor);
					rolesMsgEmbeds.push_back(newEmbed);
				}

				std::vector<EmbedData> finalMsgEmbedsArray;
				for (auto& value: itemsMessageEmbeds) {
					finalMsgEmbedsArray.push_back(value);
				}
				for (auto& value: rolesMsgEmbeds) {
					finalMsgEmbedsArray.push_back(value);
				}

				if (rolesMsgEmbeds.size() == 0 && itemsMessageEmbeds.size() == 0) {
					std::string msgString = "";
					msgString = "Sorry, but the specified user, (<@!" + std::to_string(userID) + ">) has no inventory!";
					EmbedData msgEmbed{};
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Empty Inventory:**__");
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					RespondToInputEventData dataPackage(event02);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					event02 = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				uint32_t currentPageIndex = 0;
				RespondToInputEventData dataPackage(event02);
				dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
				event02 = InputEvents::respondToInputEventAsync(dataPackage).get();


				moveThroughMessagePages(std::to_string(userID), event02, currentPageIndex, finalMsgEmbedsArray, true, 120000);
				return;
			} catch (...) {
				reportException("Inventory::execute()");
			}
		}
		~Inventory(){};
	};
}
