// Inventory.hpp - Header for the "inventory" command.
// jun 23, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class inventory : public base_function {
	  public:
		inventory() {
			this->commandName	  = "inventory";
			this->helpDescription = "Inspect your own or someone else's inventoryt";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /inventory. alternatively, enter /inventory @usermention to view someone else's inventory.\n------");
			msgEmbed.setTitle("__**Inventory Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<inventory>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				snowflake userID{};

				std::regex userIDRegExp{ ".{2,3}\\d{18,20}>" };
				std::regex idRegExp{ "\\d{18,20}" };
				if (argsNew.getCommandArguments().values.size() == 0) {
					userID = argsNew.getUserData().id;
				} else {
					jsonifier::string userIDOne = argsNew.getCommandArguments().values.at("user").operator jsonifier::string();
					userID						= jsonifier::strToUint64(userIDOne);
				}

				guild_member_data currentGuildMember = argsNew.getGuildMemberData();

				discord_guild_member discordGuildMember(managerAgent, currentGuildMember);

				if (currentGuildMember.getUserData().userName == "") {
					jsonifier::string msgString = "------\n**Sorry, but the specified user data could not be found!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User Data Issue:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				jsonifier::string userName = currentGuildMember.getUserData().userName;

				jsonifier::vector<role_data> rolesArray = roles::getGuildRolesAsync({ .guildId = guild.id }).get();
				input_event_data event02				   = argsNew.getInputEventData();

				for (uint32_t x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
					bool isRoleFound	   = false;
					inventory_role userRole = discordGuildMember.data.roles[x];
					for (auto& value2 : rolesArray) {
						if (value2.id == userRole.roleId) {
							isRoleFound = true;
							break;
						}
					}
					if (isRoleFound == false) {
						discordGuildMember.data.roles.erase(discordGuildMember.data.roles.begin() + x);
						discordGuildMember.writeDataToDB(managerAgent);
						jsonifier::string msgString = "------\n**Removing role " + userRole.roleName + " from user cache!**\n------";
						embed_data msgEmbed{};
						msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed.setColor("fefefe");
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Role Data Issue:**__");
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Interaction_Response);
						event02 = input_events::respondToInputEventAsync(dataPackage).get();
						input_events::deleteInputEventResponseAsync(event02, 20000);
						x -= 1;
					}
				}

				jsonifier::vector<jsonifier::string> itemsMsgString;
				uint32_t currentPage = 0;

				for (uint32_t x = 0; x < discordGuildMember.data.items.size(); x += 1) {
					if (x == 0) {
						itemsMsgString.emplace_back("");
					}
					jsonifier::string itemsMsgStringTemp = "";
					itemsMsgStringTemp = "**| __Item:__** " + discordGuildMember.data.items[x].emoji + discordGuildMember.data.items[x].itemName + " **| __Value:__** " +
										 jsonifier::toString(discordGuildMember.data.items[x].itemCost) + " **| __Self-mod:__** " +
										 jsonifier::toString(discordGuildMember.data.items[x].selfMod) + " **| __Opp-mod:__** " +
										 jsonifier::toString(discordGuildMember.data.items[x].oppMod) + "\n";
					if (itemsMsgStringTemp.size() + itemsMsgString[currentPage].size() >= 2048) {
						currentPage += 1;
						itemsMsgString.emplace_back("");
					}
					itemsMsgString[currentPage] += itemsMsgStringTemp;
				}

				uint32_t currentPage2 = 0;
				jsonifier::vector<jsonifier::string> rolesMsgStrings;

				for (uint32_t x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
					if (x == 0) {
						rolesMsgStrings.emplace_back("");
					}
					jsonifier::string rolesMsgStringTemp = "";
					rolesMsgStringTemp			   = "**| __Role:__** <@&" + discordGuildMember.data.roles.at(x).roleId + "> **| __Value:__** " +
										 jsonifier::toString(discordGuildMember.data.roles.at(x).roleCost) + "\n";
					if (rolesMsgStringTemp.size() + rolesMsgStrings[currentPage2].size() > 2048) {
						currentPage2 += 1;
						rolesMsgStrings.emplace_back("");
					}
					rolesMsgStrings[currentPage2] += rolesMsgStringTemp;
				}

				jsonifier::vector<embed_data> itemsMessageEmbeds{};

				for (uint32_t x = 0; x < itemsMsgString.size(); x += 1) {
					embed_data msgEmbed{};
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**" + userName + "'s inventory (items) page " + jsonifier::toString(x + 1) + " of " +
									  jsonifier::toString(itemsMsgString.size() + rolesMsgStrings.size()) + ":**__");
					msgEmbed.setDescription(itemsMsgString[x]);
					msgEmbed.setColor("fefefe");
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					itemsMessageEmbeds.emplace_back(msgEmbed);
				}

				jsonifier::vector<embed_data> rolesMsgEmbeds{};

				for (uint32_t x = 0; x < rolesMsgStrings.size(); x += 1) {
					embed_data msgEmbed{};
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**" + userName + "'s inventory (roles) page " + jsonifier::toString(itemsMessageEmbeds.size() + x + 1) + " of " +
									  jsonifier::toString(itemsMsgString.size() + rolesMsgStrings.size()) + ":**__");
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setDescription(rolesMsgStrings[x]);
					msgEmbed.setColor("fefefe");
					rolesMsgEmbeds.emplace_back(msgEmbed);
				}

				jsonifier::vector<embed_data> finalMsgEmbedsArray;
				for (auto& value : itemsMessageEmbeds) {
					finalMsgEmbedsArray.emplace_back(value);
				}
				for (auto& value : rolesMsgEmbeds) {
					finalMsgEmbedsArray.emplace_back(value);
				}

				if (rolesMsgEmbeds.size() == 0 && itemsMessageEmbeds.size() == 0) {
					jsonifier::string msgString = "";
					msgString			  = "Sorry, but the specified user, (<@!" + userID + ">) has no inventory!";
					embed_data msgEmbed{};
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Empty Inventory:**__");
					msgEmbed.setColor("fefefe");
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					respond_to_input_event_data dataPackage(event02);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					event02 = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				uint32_t currentPageIndex = 0;
				respond_to_input_event_data dataPackage(event02);
				dataPackage.setResponseType(input_event_response_type::Deferred_Response);
				event02 = input_events::respondToInputEventAsync(dataPackage).get();


				moveThroughMessagePages(userID, event02, currentPageIndex, finalMsgEmbedsArray, true, 120000, false);
				return;
			} catch (const std::exception& error) {
				std::cout << "inventory::execute()" << error.what() << std::endl;
			}
		}
		~inventory(){};
	};
}// namespace discord_core_api
