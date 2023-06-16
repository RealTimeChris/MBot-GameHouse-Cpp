// Shop.hpp - Header for the "shop" command.
// june 2, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	enum class items_or_roles { roles = 0, items = 1 };

	jsonifier::vector<select_option_data> getSelectOptionsVector(discord_guild discordGuild, items_or_roles itemsOrRoles) {
		discordGuild.getDataFromDB(managerAgent);
		uint32_t maxIdx = 0;
		inventory_item tempItem;
		uint32_t len = (uint32_t)discordGuild.data.guildShop.items.size();
		for (uint32_t x = 0; x < len; x += 1) {
			maxIdx = x;
			for (uint32_t y = x + 1; y < len; y += 1) {
				if (discordGuild.data.guildShop.items.at(y).itemCost > discordGuild.data.guildShop.items.at(maxIdx).itemCost) {
					maxIdx = y;
				}
			}
			tempItem									 = discordGuild.data.guildShop.items.at(x);
			discordGuild.data.guildShop.items.at(x)		 = discordGuild.data.guildShop.items.at(maxIdx);
			discordGuild.data.guildShop.items.at(maxIdx) = tempItem;
		}

		maxIdx = 0;
		inventory_role tempRole;
		len = (uint32_t)discordGuild.data.guildShop.roles.size();
		for (uint32_t x = 0; x < len; x += 1) {
			maxIdx = x;
			for (uint32_t y = x + 1; y < len; y += 1) {
				if (discordGuild.data.guildShop.roles.at(y).roleCost > discordGuild.data.guildShop.roles.at(maxIdx).roleCost) {
					maxIdx = y;
				}
			}
			tempRole									 = discordGuild.data.guildShop.roles.at(x);
			discordGuild.data.guildShop.roles.at(x)		 = discordGuild.data.guildShop.roles.at(maxIdx);
			discordGuild.data.guildShop.roles.at(maxIdx) = tempRole;
		}
		discordGuild.writeDataToDB(managerAgent);

		jsonifier::vector<select_option_data> returnVector;
		if (itemsOrRoles == items_or_roles::items) {
			for (auto& value : discordGuild.data.guildShop.items) {
				select_option_data itemOptionData;
				itemOptionData.emoji.name = value.emoji;
				itemOptionData.description =
					"cost: " + jsonifier::toString(value.itemCost) + " self-mod: " + jsonifier::toString(value.selfMod) + " opp-mod: " + jsonifier::toString(value.oppMod);
				itemOptionData.label	= value.itemName;
				itemOptionData.value	= convertToLowerCase(value.itemName);
				itemOptionData._default = false;
				returnVector.emplace_back(itemOptionData);
			}
		} else {
			for (auto& value : discordGuild.data.guildShop.roles) {
				select_option_data roleOptionData;
				roleOptionData.description = "cost: " + jsonifier::toString(value.roleCost);
				roleOptionData.label	   = value.roleName;
				roleOptionData.value	   = convertToLowerCase(value.roleName);
				roleOptionData._default	   = false;
				returnVector.emplace_back(roleOptionData);
			}
		}

		select_option_data goBackOption;
		goBackOption.description = "go back to the previous menu.";
		goBackOption.emoji.name	 = "❌";
		goBackOption.label		 = "go back";
		goBackOption.value		 = "go_back";
		goBackOption._default	 = false;
		returnVector.emplace_back(goBackOption);

		return returnVector;
	}

	class shop : public base_function {
	  public:
		shop() {
			this->commandName	  = "shop";
			this->helpDescription = "view the server's item and role shop!";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /shop.\n------");
			msgEmbed.setTitle("__**Shop usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<shop>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };
				discordGuild.getDataFromDB(managerAgent);

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				guild_member_data botMember = guild_members::getCachedGuildMember({ .guildMemberId = discord_core_client::getInstance()->getBotUser().id, .guildId = guild.id });

				if (!(permissions{ botMember.permissions }.checkForPermission(botMember, channel, permission::Manage_Messages))) {
					jsonifier::string msgString = "------\n**I need the manage messages permission in this channel, for this command!**\n------";
					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setColor("fefefe");
					msgEmbed->setDescription(msgString);
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Permissions issue:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					input_event_data eventNew = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				jsonifier::vector<role_data> rolesArray = roles::getGuildRolesAsync({ .guildId = guild.id }).get();
				input_event_data event02				   = argsNew.getInputEventData();

				for (uint32_t x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
					bool isRoleFound	   = false;
					inventory_role shopRole = discordGuild.data.guildShop.roles[x];
					for (auto& value2 : rolesArray) {
						if (value2.id == shopRole.roleId) {
							isRoleFound = true;
							break;
						}
					}
					if (isRoleFound == false) {
						discordGuild.data.guildShop.roles.erase(discordGuild.data.guildShop.roles.begin() + x);
						discordGuild.writeDataToDB(managerAgent);
						jsonifier::string msgString = "------\n**Removing guild role " + shopRole.roleName + " from guild cache!**\n------";
						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed->setColor("fefefe");
						msgEmbed->setDescription(msgString);
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Removed guild_data role_data:**__");
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						input_events::respondToInputEventAsync(dataPackage).get();
						x -= 1;
					}
				}
				discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
				msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed->setDescription("------\n__**Select which part of the shop you would like to browse!**__\n------");
				msgEmbed->setColor("fefefe");
				msgEmbed->setTimeStamp(getTimeAndDate());
				msgEmbed->setTitle("__**Welcome to the shop:**__");
				embed_data msgEmbedItems;

				msgEmbedItems.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));

				msgEmbedItems.setDescription("------\n__**Select one or more items which you would like to purchase, from the drop-down menu!**__\n------");
				msgEmbedItems.setColor("fefefe");
				msgEmbedItems.setTimeStamp(getTimeAndDate());
				msgEmbedItems.setTitle("__**Welcome to the shop:**__");
				embed_data msgEmbedRoles;
				msgEmbedRoles.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));

				msgEmbedRoles.setDescription("------\n__**Select one or more roles which you would like to purchase, from the drop-down menu!**__\n------");
				msgEmbedRoles.setColor("fefefe");
				msgEmbedRoles.setTimeStamp(getTimeAndDate());
				msgEmbedRoles.setTitle("__**Welcome to the shop:**__");
				jsonifier::vector<select_menu_response_data> data;
				respond_to_input_event_data dataPackage(event02);
				dataPackage.setResponseType(input_event_response_type::Deferred_Response);
				event02 = input_events::respondToInputEventAsync(dataPackage).get();
				respond_to_input_event_data dataPackage02(event02);
				dataPackage02.setResponseType(input_event_response_type::Follow_Up_Message);
				dataPackage02.addMessageEmbed(*msgEmbed);
				dataPackage02.addButton(false, "items", "items", button_style::Primary, "☑");
				dataPackage02.addButton(false, "roles", "roles", button_style::Primary, "🔥");
				dataPackage02.addButton(false, "exit", "exit", button_style::Danger, "❌");
				event02 = input_events::respondToInputEventAsync(dataPackage02).get();

				while (1) {
				start:
					embed_data currentEmbed{};
					discord_core_api::unique_ptr<button_collector> newButton{ discord_core_api::makeUnique<button_collector>(event02) };
					auto createResponseData = discord_core_api::makeUnique<create_interaction_response_data>();
					auto embedData			= discord_core_api::makeUnique<embed_data>();
					embedData->setColor("fefefe");
					embedData->setTitle("__**Permissions issue:**__");
					embedData->setTimeStamp(getTimeAndDate());
					embedData->setDescription("sorry, but that button can only be pressed by <@" + argsNew.getUserData().id + ">!");
					createResponseData->addMessageEmbed(*embedData);
					createResponseData->setResponseType(interaction_callback_type::Channel_Message_With_Source);
					createResponseData->setFlags(64);
					auto buttonData = newButton->collectButtonData(false, 120000, 1, *createResponseData, argsNew.getUserData().id).get();
					if (buttonData.at(0).buttonId == "items") {
						currentEmbed = msgEmbedItems;
					} else if (buttonData.at(0).buttonId == "roles") {
						currentEmbed = msgEmbedRoles;
					}
					if (buttonData.at(0).buttonId == "roles" || buttonData.at(0).buttonId == "items") {
						respond_to_input_event_data dataPackage03(*buttonData.at(0).interactionData);
						dataPackage03.addMessageEmbed(currentEmbed);
						dataPackage03.setResponseType(input_event_response_type::Edit_Follow_Up_Message);
						if (buttonData.at(0).buttonId == "items") {
							jsonifier::vector<select_option_data> selectOptionDataItems = getSelectOptionsVector(discordGuild, items_or_roles::items);
							dataPackage03.addSelectMenu(false,
								"shop_menu_itmes",
								selectOptionDataItems,
								"choose one or more items.",
								(int32_t)selectOptionDataItems.size(),
								1,
								select_menu_type::String_Select);
						} else {
							jsonifier::vector<select_option_data> selectOptionDataRoles = getSelectOptionsVector(discordGuild, items_or_roles::roles);
							dataPackage03.addSelectMenu(false,
								"shop_menu_roles",
								selectOptionDataRoles,
								"choose one or more roles.",
								(int32_t)selectOptionDataRoles.size(),
								1,
								select_menu_type::String_Select);
						}
						event02 = input_events::respondToInputEventAsync(dataPackage03).get();
					} else if (buttonData.at(0).buttonId == "exit" || buttonData.at(0).buttonId == "empty") {
						input_events::deleteInputEventResponseAsync(event02).get();
						break;
					}

					discord_core_api::unique_ptr<select_menu_collector> selectMenu{ discord_core_api::makeUnique<select_menu_collector>(event02) };
					data = selectMenu->collectSelectMenuData(false, 120000, 1, *createResponseData, argsNew.getUserData().id).get();

					for (auto& value : data) {
						for (auto& value2 : value.values) {
							if (value2 == "go_back" || data.size() == 0) {
								respond_to_input_event_data dataPackage03(*value.interactionData);
								dataPackage03.setResponseType(input_event_response_type::Edit_Follow_Up_Message);
								dataPackage03.addMessageEmbed(currentEmbed);
								dataPackage03.addButton(false, "items", "items", button_style::Primary, "☑");
								dataPackage03.addButton(false, "roles", "roles", button_style::Primary, "🔥");
								dataPackage03.addButton(false, "exit", "exit", button_style::Danger, "❌");
								event02 = input_events::respondToInputEventAsync(dataPackage03).get();
								goto start;
							}
						}
						input_events::deleteInputEventResponseAsync(event02).get();
					}


					for (auto& value : data) {
						for (auto& value02 : value.values) {
							discord_guild_member discordGuildMember(managerAgent, guildMember);
							jsonifier::string objectName = value02;
							jsonifier::string objectType;
							int32_t objectShopIndex = 0;
							bool isFoundInShop		= false;
							bool isFoundInInventory = false;
							for (uint32_t x = 0; x < discordGuild.data.guildShop.items.size(); x += 1) {
								if (objectName == convertToLowerCase(discordGuild.data.guildShop.items.at(x).itemName)) {
									isFoundInShop	= true;
									objectShopIndex = x;
									objectType		= "item";
									break;
								}
							}
							for (uint32_t x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
								if (objectName == convertToLowerCase(discordGuild.data.guildShop.roles.at(x).roleName)) {
									isFoundInShop	= true;
									objectShopIndex = x;
									objectType		= "role";
									break;
								}
							}

							for (uint32_t x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
								if (objectName == convertToLowerCase(discordGuildMember.data.roles.at(x).roleName)) {
									isFoundInInventory = true;
									break;
								}
							}

							for (uint32_t x = 0; x < discordGuildMember.data.items.size(); x += 1) {
								if (objectName == convertToLowerCase(discordGuildMember.data.items.at(x).itemName)) {
									isFoundInInventory = true;
									break;
								}
							}

							if (isFoundInInventory == true) {
								jsonifier::string msgString = "------\n**sorry, but you already have one of those " + objectType + "s.** \n------";
								embed_data msgEmbed02;
								msgEmbed02.setAuthor(guildMember.getUserData().userName, guildMember.getGuildMemberImageUrl(guild_member_image_types::Avatar));
								msgEmbed02.setColor("fefefe");
								msgEmbed02.setDescription(msgString);
								msgEmbed02.setTimeStamp(getTimeAndDate());
								msgEmbed02.setTitle("__**Duplicate object:**__");
								respond_to_input_event_data dataPackage03(event02);
								dataPackage03.setResponseType(input_event_response_type::Follow_Up_Message);
								dataPackage03.addMessageEmbed(msgEmbed02);
								event02 = input_events::respondToInputEventAsync(dataPackage03).get();
								input_events::deleteInputEventResponseAsync(event02, 20000);
								continue;
							}

							if (objectType == "role") {
								uint32_t roleCost	 = discordGuild.data.guildShop.roles.at(objectShopIndex).roleCost;
								uint32_t userBalance = discordGuildMember.data.currency.wallet;

								if (roleCost > userBalance) {
									jsonifier::string msgString = "------\n**sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
									discord_core_api::unique_ptr<embed_data> msgEmbed02{ discord_core_api::makeUnique<embed_data>() };
									msgEmbed02->setAuthor(guildMember.getUserData().userName, guildMember.getGuildMemberImageUrl(guild_member_image_types::Avatar));
									msgEmbed02->setColor("fefefe");
									msgEmbed02->setDescription(msgString);
									msgEmbed02->setTimeStamp(getTimeAndDate());
									msgEmbed02->setTitle("__**Insufficient funds:**__");
									respond_to_input_event_data dataPackage03(event02);
									dataPackage03.setResponseType(input_event_response_type::Follow_Up_Message);
									dataPackage03.addMessageEmbed(*msgEmbed02);
									event02 = input_events::respondToInputEventAsync(dataPackage03).get();
									input_events::deleteInputEventResponseAsync(event02, 20000);
									break;
								}
								auto botUser = discord_core_client::getInstance()->getBotUser();
								jsonifier::string theString{ botUser.userName };
								discord_user discordUser(managerAgent, theString, botUser.id);
								inventory_role newRole = discordGuild.data.guildShop.roles.at(objectShopIndex);
								discordGuildMember.data.roles.emplace_back(newRole);
								discordGuildMember.data.currency.wallet -= roleCost;
								discordGuildMember.writeDataToDB(managerAgent);

								uint32_t newBalance = discordGuildMember.data.currency.wallet;

								snowflake roleID{ discordGuild.data.guildShop.roles.at(objectShopIndex).roleId };

								roles::addGuildMemberRoleAsync({ .guildId = guild.id, .userId = guildMember.user.id, .roleId = roleID });

								jsonifier::string msgString = "------\nCongratulations! you've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ <@&" +
														newRole.roleId + "> (" + newRole.roleName + ")\n------\n__**Your new wallet balance:**__ " + jsonifier::toString(newBalance) +
														" " + discordUser.data.currencyName + "\n------";
								discord_core_api::unique_ptr<embed_data> msgEmbed02{ discord_core_api::makeUnique<embed_data>() };
								msgEmbed02->setTitle("__**New role_data purchased:**__");
								msgEmbed02->setTimeStamp(getTimeAndDate());
								msgEmbed02->setDescription(msgString);
								msgEmbed02->setAuthor(guildMember.getUserData().userName, guildMember.getGuildMemberImageUrl(guild_member_image_types::Avatar));
								msgEmbed02->setColor("fefefe");
								respond_to_input_event_data dataPackage03(event02);
								dataPackage03.setResponseType(input_event_response_type::Follow_Up_Message);
								dataPackage03.addMessageEmbed(*msgEmbed02);
								event02 = input_events::respondToInputEventAsync(dataPackage03).get();

								uint32_t maxIdx = 0;
								uint32_t len	= (uint32_t)discordGuildMember.data.roles.size();
								for (uint32_t x = 0; x < len; x += 1) {
									maxIdx = x;
									for (uint32_t y = x + 1; y < len; y += 1) {
										if (discordGuildMember.data.roles.at(y).roleCost > discordGuildMember.data.roles.at(maxIdx).roleCost) {
											maxIdx = y;
										}
									}
									inventory_role tempRole					 = discordGuildMember.data.roles.at(x);
									discordGuildMember.data.roles.at(x)		 = discordGuildMember.data.roles.at(maxIdx);
									discordGuildMember.data.roles.at(maxIdx) = tempRole;
								}
								discordGuildMember.writeDataToDB(managerAgent);
								continue;
							} else if (objectType == "item") {
								uint32_t itemCost	 = discordGuild.data.guildShop.items.at(objectShopIndex).itemCost;
								uint32_t userBalance = discordGuildMember.data.currency.wallet;

								if (itemCost > userBalance) {
									jsonifier::string msgString = "------\n**sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
									discord_core_api::unique_ptr<embed_data> msgEmbed02{ discord_core_api::makeUnique<embed_data>() };
									msgEmbed02->setTimeStamp(getTimeAndDate());
									msgEmbed02->setDescription(msgString);
									msgEmbed02->setAuthor(guildMember.getUserData().userName, guildMember.getGuildMemberImageUrl(guild_member_image_types::Avatar));
									msgEmbed02->setColor("fefefe");
									msgEmbed02->setTitle("__**Insufficient funds:**__");
									respond_to_input_event_data dataPackage03(event02);
									dataPackage03.setResponseType(input_event_response_type::Follow_Up_Message);
									dataPackage03.addMessageEmbed(*msgEmbed02);
									event02 = input_events::respondToInputEventAsync(dataPackage03).get();
									input_events::deleteInputEventResponseAsync(event02, 20000);
									break;
								}

								inventory_item newItem = discordGuild.data.guildShop.items.at(objectShopIndex);
								discordGuildMember.data.items.emplace_back(newItem);
								discordGuildMember.data.currency.wallet -= itemCost;
								discordGuildMember.writeDataToDB(managerAgent);
								auto botUser = discord_core_client::getInstance()->getBotUser();
								jsonifier::string theString{ botUser.userName };
								discord_user discordUser(managerAgent, theString, botUser.id);
								jsonifier::string itemEmoji = discordGuild.data.guildShop.items.at(objectShopIndex).emoji;
								jsonifier::string itemName  = discordGuild.data.guildShop.items.at(objectShopIndex).itemName;
								uint32_t newBalance	  = discordGuildMember.data.currency.wallet;
								jsonifier::string msgString = "------\nCongratulations!you've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ " + itemEmoji +
														itemName + "\n------\n__**Your new wallet balance:**__ " + jsonifier::toString(newBalance) + " " +
														discordUser.data.currencyName + "\n------";
								discord_core_api::unique_ptr<embed_data> msgEmbed02{ discord_core_api::makeUnique<embed_data>() };
								msgEmbed02->setTitle("__**New item purchased:**__");

								uint32_t maxIdx = 0;
								inventory_item tempItem;
								uint32_t len = (uint32_t)discordGuildMember.data.items.size();
								for (uint32_t x = 0; x < len; x += 1) {
									maxIdx = x;
									for (uint32_t y = x + 1; y < len; y += 1) {
										if (discordGuildMember.data.items.at(y).itemCost > discordGuildMember.data.items.at(maxIdx).itemCost) {
											maxIdx = y;
										}
									}
									tempItem								 = discordGuildMember.data.items.at(x);
									discordGuildMember.data.items.at(x)		 = discordGuildMember.data.items.at(maxIdx);
									discordGuildMember.data.items.at(maxIdx) = tempItem;
								}
								discordGuildMember.writeDataToDB(managerAgent);
								msgEmbed02->setTimeStamp(getTimeAndDate());
								msgEmbed02->setDescription(msgString);
								msgEmbed02->setAuthor(guildMember.getUserData().userName, guildMember.getGuildMemberImageUrl(guild_member_image_types::Avatar));
								msgEmbed02->setColor("fefefe");
								respond_to_input_event_data dataPackage03(event02);
								dataPackage03.setResponseType(input_event_response_type::Follow_Up_Message);
								dataPackage03.addMessageEmbed(*msgEmbed02);
								event02 = input_events::respondToInputEventAsync(dataPackage03).get();
							}
						}
					}
				}

				// input_events::deleteInputEventResponseAsync(event02);

				discordGuild.writeDataToDB(managerAgent);

				return;
			} catch (const std::exception& error) {
				std::cout << "shop::execute()" << error.what() << std::endl;
			}
		}
		~shop(){};
	};
}// namespace discord_core_api
