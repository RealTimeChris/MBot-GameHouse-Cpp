// Shop.hpp - Header for the "shop" command.
// June 2, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	enum class ItemsOrRoles { roles = 0, items = 1 };

	std::vector<SelectOptionData> getSelectOptionsVector(DiscordGuild discordGuild, ItemsOrRoles itemsOrRoles) {
		discordGuild.getDataFromDB();
		uint32_t maxIdx = 0;
		InventoryItem tempItem;
		uint32_t len = ( uint32_t )discordGuild.data.guildShop.items.size();
		for (uint32_t x = 0; x < len; x += 1) {
			maxIdx = x;
			for (uint32_t y = x + 1; y < len; y += 1) {
				if (discordGuild.data.guildShop.items.at(y).itemCost > discordGuild.data.guildShop.items.at(maxIdx).itemCost) {
					maxIdx = y;
				}
			}
			tempItem = discordGuild.data.guildShop.items.at(x);
			discordGuild.data.guildShop.items.at(x) = discordGuild.data.guildShop.items.at(maxIdx);
			discordGuild.data.guildShop.items.at(maxIdx) = tempItem;
		}

		maxIdx = 0;
		InventoryRole tempRole;
		len = ( uint32_t )discordGuild.data.guildShop.roles.size();
		for (uint32_t x = 0; x < len; x += 1) {
			maxIdx = x;
			for (uint32_t y = x + 1; y < len; y += 1) {
				if (discordGuild.data.guildShop.roles.at(y).roleCost > discordGuild.data.guildShop.roles.at(maxIdx).roleCost) {
					maxIdx = y;
				}
			}
			tempRole = discordGuild.data.guildShop.roles.at(x);
			discordGuild.data.guildShop.roles.at(x) = discordGuild.data.guildShop.roles.at(maxIdx);
			discordGuild.data.guildShop.roles.at(maxIdx) = tempRole;
		}
		discordGuild.writeDataToDB();

		std::vector<SelectOptionData> returnVector;
		if (itemsOrRoles == ItemsOrRoles::items) {
			for (auto& value: discordGuild.data.guildShop.items) {
				SelectOptionData itemOptionData;
				itemOptionData.emoji.name = value.emoji;
				itemOptionData.description =
					"Cost: " + std::to_string(value.itemCost) + " Self-Mod: " + std::to_string(value.selfMod) + " Opp-Mod: " + std::to_string(value.oppMod);
				itemOptionData.label = value.itemName;
				itemOptionData.value = convertToLowerCase(value.itemName);
				itemOptionData._default = false;
				returnVector.push_back(itemOptionData);
			}
		} else {
			for (auto& value: discordGuild.data.guildShop.roles) {
				SelectOptionData roleOptionData;
				roleOptionData.description = "Cost: " + std::to_string(value.roleCost);
				roleOptionData.label = value.roleName;
				roleOptionData.value = convertToLowerCase(value.roleName);
				roleOptionData._default = false;
				returnVector.push_back(roleOptionData);
			}
		}

		SelectOptionData goBackOption;
		goBackOption.description = "Go back to the previous menu.";
		goBackOption.emoji.name = "???";
		goBackOption.label = "Go Back";
		goBackOption.value = "go_back";
		goBackOption._default = false;
		returnVector.push_back(goBackOption);

		return returnVector;
	}

	class Shop : public BaseFunction {
	  public:
		Shop() {
			this->commandName = "shop";
			this->helpDescription = "View the server's item and role shop!";
			DiscordCoreAPI::EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /shop.\n------");
			msgEmbed.setTitle("__**Shop Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Shop>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ argsNew.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = argsNew.eventData.getAuthorId(), .guildId = argsNew.eventData.getGuildId() }).get();
				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.eventData, discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				GuildMember botMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = argsNew.discordCoreClient->getBotUser().id, .guildId = argsNew.eventData.getGuildId() }).get();
				if (!(botMember.permissions.checkForPermission(botMember, channel, Permission::Manage_Messages))) {
					std::string msgString = "------\n**I need the Manage Messages permission in this channel, for this command!**\n------";
					std::unique_ptr<DiscordCoreAPI::EmbedData> msgEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
					msgEmbed->setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed->setColor(discordGuild.data.borderColor);
					msgEmbed->setDescription(msgString);
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Permissions Issue:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					InputEventData eventNew = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				std::vector<Role> rolesArray = Roles::getGuildRolesAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				InputEventData event02 = argsNew.eventData;

				for (uint32_t x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
					bool isRoleFound = false;
					InventoryRole shopRole = discordGuild.data.guildShop.roles[x];
					for (auto& value2: rolesArray) {
						if (value2.id == shopRole.roleId) {
							isRoleFound = true;
							break;
						}
					}
					if (isRoleFound == false) {
						discordGuild.data.guildShop.roles.erase(discordGuild.data.guildShop.roles.begin() + x);
						discordGuild.writeDataToDB();
						std::string msgString = "------\n**Removing guild role " + shopRole.roleName + " from guild cache!**\n------";
						std::unique_ptr<DiscordCoreAPI::EmbedData> msgEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
						msgEmbed->setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
						msgEmbed->setColor(discordGuild.data.borderColor);
						msgEmbed->setDescription(msgString);
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Removed Guild Role:**__");
						RespondToInputEventData dataPackage(argsNew.eventData);
						dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						InputEvents::respondToInputEventAsync(dataPackage).get();
						x -= 1;
					}
				}

				std::unique_ptr<DiscordCoreAPI::EmbedData> msgEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
				msgEmbed->setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed->setDescription("------\n__**Select which part of the shop you would like to browse!**__\n------");
				msgEmbed->setColor(discordGuild.data.borderColor);
				msgEmbed->setTimeStamp(getTimeAndDate());
				msgEmbed->setTitle("__**Welcome to the Shop:**__");
				EmbedData msgEmbedItems;
				msgEmbedItems.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbedItems.setDescription("------\n__**Select one or more items which you would like to purchase, from the drop-down menu!**__\n------");
				msgEmbedItems.setColor(discordGuild.data.borderColor);
				msgEmbedItems.setTimeStamp(getTimeAndDate());
				msgEmbedItems.setTitle("__**Welcome to the Shop:**__");
				EmbedData msgEmbedRoles;
				msgEmbedRoles.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbedRoles.setDescription("------\n__**Select one or more roles which you would like to purchase, from the drop-down menu!**__\n------");
				msgEmbedRoles.setColor(discordGuild.data.borderColor);
				msgEmbedRoles.setTimeStamp(getTimeAndDate());
				msgEmbedRoles.setTitle("__**Welcome to the Shop:**__");
				std::vector<SelectMenuResponseData> values;
				RespondToInputEventData dataPackage(event02);
				dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
				event02 = InputEvents::respondToInputEventAsync(dataPackage).get();
				RespondToInputEventData dataPackage02(event02);
				dataPackage02.setResponseType(InputEventResponseType::Follow_Up_Message);
				dataPackage02.addMessageEmbed(*msgEmbed);
				dataPackage02.addButton(false, "items", "Items", ButtonStyle::Primary, "???");
				dataPackage02.addButton(false, "roles", "Roles", ButtonStyle::Primary, "????");
				dataPackage02.addButton(false, "exit", "Exit", ButtonStyle::Danger, "???");
				event02 = InputEvents::respondToInputEventAsync(dataPackage02).get();
				while (1) {
				start:
					EmbedData currentEmbed;
					std::unique_ptr<ButtonCollector> newButton{ std::make_unique<ButtonCollector>(event02) };
					auto buttonData = newButton->collectButtonData(false, 120000, 1, argsNew.eventData.getAuthorId()).get();
					if (buttonData.at(0).buttonId == "items") {
						currentEmbed = msgEmbedItems;
					} else if (buttonData.at(0).buttonId == "roles") {
						currentEmbed = msgEmbedRoles;
					}
					if (buttonData.at(0).buttonId == "roles" || buttonData.at(0).buttonId == "items") {
						RespondToInputEventData dataPackage03(*buttonData.at(0).interactionData);
						dataPackage03.addMessageEmbed(currentEmbed);
						dataPackage03.setResponseType(InputEventResponseType::Edit_Follow_Up_Message);
						if (buttonData.at(0).buttonId == "items") {
							std::vector<SelectOptionData> selectOptionDataItems = getSelectOptionsVector(discordGuild, ItemsOrRoles::items);
							dataPackage03.addSelectMenu(false, "shop_menu_itmes", selectOptionDataItems, "Choose one or more items.", ( int32_t )selectOptionDataItems.size(), 1);
						} else {
							std::vector<SelectOptionData> selectOptionDataRoles = getSelectOptionsVector(discordGuild, ItemsOrRoles::roles);
							dataPackage03.addSelectMenu(false, "shop_menu_roles", selectOptionDataRoles, "Choose one or more roles.", ( int32_t )selectOptionDataRoles.size(), 1);
						}
						InputEvents::respondToInputEventAsync(dataPackage03).get();
					} else if (buttonData.at(0).buttonId == "exit" || buttonData.at(0).buttonId == "empty") {
						break;
					}

					std::unique_ptr<SelectMenuCollector> selectMenu{ std::make_unique<SelectMenuCollector>(event02) };
					values = selectMenu->collectSelectMenuData(false, 120000, 1, argsNew.eventData.getAuthorId()).get();
					for (auto& value: values) {
						for (auto& value2: value.values) {
							if (value2 == "go_back" || values.size() == 0) {
								RespondToInputEventData dataPackage03(*value.interactionData);
								dataPackage03.setResponseType(InputEventResponseType::Edit_Follow_Up_Message);
								dataPackage03.addMessageEmbed(*msgEmbed);
								dataPackage03.addButton(false, "items", "Items", ButtonStyle::Primary, "???");
								dataPackage03.addButton(false, "roles", "Roles", ButtonStyle::Primary, "????");
								dataPackage03.addButton(false, "exit", "Exit", ButtonStyle::Danger, "???");
								InputEvents::respondToInputEventAsync(dataPackage03).get();
								goto start;
							}
						}
					}
					break;
				}
				for (auto& value: values) {
					for (auto& value02: value.values) {
						DiscordGuildMember discordGuildMember(guildMember);
						std::string objectName = value02;
						std::string objectType;
						int32_t objectShopIndex = 0;
						bool isFoundInShop = false;
						bool isFoundInInventory = false;
						for (uint32_t x = 0; x < discordGuild.data.guildShop.items.size(); x += 1) {
							if (objectName == convertToLowerCase(discordGuild.data.guildShop.items.at(x).itemName)) {
								isFoundInShop = true;
								objectShopIndex = x;
								objectType = "item";
								break;
							}
						}
						for (uint32_t x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
							if (objectName == convertToLowerCase(discordGuild.data.guildShop.roles.at(x).roleName)) {
								isFoundInShop = true;
								objectShopIndex = x;
								objectType = "role";
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
							std::string msgString = "------\n**Sorry, but you already have one of those " + objectType + "s.** \n------";
							EmbedData msgEmbed02;
							msgEmbed02.setAuthor(guildMember.userName, guildMember.userAvatar);
							msgEmbed02.setColor(discordGuild.data.borderColor);
							msgEmbed02.setDescription(msgString);
							msgEmbed02.setTimeStamp(getTimeAndDate());
							msgEmbed02.setTitle("__**Duplicate Object:**__");
							RespondToInputEventData dataPackage03(event02);
							dataPackage03.setResponseType(InputEventResponseType::Follow_Up_Message);
							dataPackage03.addMessageEmbed(msgEmbed02);
							InputEventData event01 = InputEvents::respondToInputEventAsync(dataPackage03).get();
							InputEvents::deleteInputEventResponseAsync(event01, 20000);
							continue;
						}

						if (objectType == "role") {
							uint32_t roleCost = discordGuild.data.guildShop.roles.at(objectShopIndex).roleCost;
							uint32_t userBalance = discordGuildMember.data.currency.wallet;

							if (roleCost > userBalance) {
								std::string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
								std::unique_ptr<DiscordCoreAPI::EmbedData> msgEmbed02{ std::make_unique<DiscordCoreAPI::EmbedData>() };
								msgEmbed02->setAuthor(guildMember.userName, guildMember.userAvatar);
								msgEmbed02->setColor(discordGuild.data.borderColor);
								msgEmbed02->setDescription(msgString);
								msgEmbed02->setTimeStamp(getTimeAndDate());
								msgEmbed02->setTitle("__**Insufficient Funds:**__");
								RespondToInputEventData dataPackage03(event02);
								dataPackage03.setResponseType(InputEventResponseType::Follow_Up_Message);
								dataPackage03.addMessageEmbed(*msgEmbed02);
								InputEventData event01 = InputEvents::respondToInputEventAsync(dataPackage03).get();
								InputEvents::deleteInputEventResponseAsync(event01, 20000);
								break;
							}
							auto botUser = argsNew.discordCoreClient->getBotUser();
							DiscordUser discordUser(botUser.userName, botUser.id);
							InventoryRole newRole = discordGuild.data.guildShop.roles.at(objectShopIndex);
							discordGuildMember.data.roles.push_back(newRole);
							discordGuildMember.data.currency.wallet -= roleCost;
							discordGuildMember.writeDataToDB();

							uint32_t newBalance = discordGuildMember.data.currency.wallet;

							uint64_t roleID = discordGuild.data.guildShop.roles.at(objectShopIndex).roleId;

							Roles::addGuildMemberRoleAsync({ .guildId = argsNew.eventData.getGuildId(), .userId = guildMember.id, .roleId = roleID });

							std::string msgString = "------\nCongratulations! You've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ <@&" +
								std::to_string(newRole.roleId) + "> (" + newRole.roleName + ")\n------\n__**Your new wallet balance:**__ " + std::to_string(newBalance) + " " +
								discordUser.data.currencyName + "\n------";
							std::unique_ptr<DiscordCoreAPI::EmbedData> msgEmbed02{ std::make_unique<DiscordCoreAPI::EmbedData>() };
							msgEmbed02->setTitle("__**New Role Purchased:**__");
							msgEmbed02->setTimeStamp(getTimeAndDate());
							msgEmbed02->setDescription(msgString);
							msgEmbed02->setAuthor(guildMember.userName, guildMember.userAvatar);
							msgEmbed02->setColor(discordGuild.data.borderColor);
							RespondToInputEventData dataPackage03(event02);
							dataPackage03.setResponseType(InputEventResponseType::Follow_Up_Message);
							dataPackage03.addMessageEmbed(*msgEmbed02);
							InputEventData event01 = InputEvents::respondToInputEventAsync(dataPackage03).get();

							uint32_t maxIdx = 0;
							uint32_t len = ( uint32_t )discordGuildMember.data.roles.size();
							for (uint32_t x = 0; x < len; x += 1) {
								maxIdx = x;
								for (uint32_t y = x + 1; y < len; y += 1) {
									if (discordGuildMember.data.roles.at(y).roleCost > discordGuildMember.data.roles.at(maxIdx).roleCost) {
										maxIdx = y;
									}
								}
								InventoryRole tempRole = discordGuildMember.data.roles.at(x);
								discordGuildMember.data.roles.at(x) = discordGuildMember.data.roles.at(maxIdx);
								discordGuildMember.data.roles.at(maxIdx) = tempRole;
							}
							discordGuildMember.writeDataToDB();
							continue;
						} else if (objectType == "item") {
							uint32_t itemCost = discordGuild.data.guildShop.items.at(objectShopIndex).itemCost;
							uint32_t userBalance = discordGuildMember.data.currency.wallet;

							if (itemCost > userBalance) {
								std::string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
								std::unique_ptr<DiscordCoreAPI::EmbedData> msgEmbed02{ std::make_unique<DiscordCoreAPI::EmbedData>() };
								msgEmbed02->setTimeStamp(getTimeAndDate());
								msgEmbed02->setDescription(msgString);
								msgEmbed02->setAuthor(guildMember.userName, guildMember.userAvatar);
								msgEmbed02->setColor(discordGuild.data.borderColor);
								msgEmbed02->setTitle("__**Insufficient Funds:**__");
								RespondToInputEventData dataPackage03(event02);
								dataPackage03.setResponseType(InputEventResponseType::Follow_Up_Message);
								dataPackage03.addMessageEmbed(*msgEmbed02);
								InputEventData event01 = InputEvents::respondToInputEventAsync(dataPackage03).get();
								InputEvents::deleteInputEventResponseAsync(event01, 20000);
								break;
							}

							InventoryItem newItem = discordGuild.data.guildShop.items.at(objectShopIndex);
							discordGuildMember.data.items.push_back(newItem);
							discordGuildMember.data.currency.wallet -= itemCost;
							discordGuildMember.writeDataToDB();
							auto botUser = argsNew.discordCoreClient->getBotUser();
							DiscordUser discordUser(botUser.userName, botUser.id);
							std::string itemEmoji = discordGuild.data.guildShop.items.at(objectShopIndex).emoji;
							std::string itemName = discordGuild.data.guildShop.items.at(objectShopIndex).itemName;
							uint32_t newBalance = discordGuildMember.data.currency.wallet;
							std::string msgString = "------\nCongratulations!You've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ " + itemEmoji +
								itemName + "\n------\n__**Your new wallet balance:**__ " + std::to_string(newBalance) + " " + discordUser.data.currencyName + "\n------";
							std::unique_ptr<DiscordCoreAPI::EmbedData> msgEmbed02{ std::make_unique<DiscordCoreAPI::EmbedData>() };
							msgEmbed02->setTitle("__**New Item Purchased:**__");

							uint32_t maxIdx = 0;
							InventoryItem tempItem;
							uint32_t len = ( uint32_t )discordGuildMember.data.items.size();
							for (uint32_t x = 0; x < len; x += 1) {
								maxIdx = x;
								for (uint32_t y = x + 1; y < len; y += 1) {
									if (discordGuildMember.data.items.at(y).itemCost > discordGuildMember.data.items.at(maxIdx).itemCost) {
										maxIdx = y;
									}
								}
								tempItem = discordGuildMember.data.items.at(x);
								discordGuildMember.data.items.at(x) = discordGuildMember.data.items.at(maxIdx);
								discordGuildMember.data.items.at(maxIdx) = tempItem;
							}
							discordGuildMember.writeDataToDB();
							msgEmbed02->setTimeStamp(getTimeAndDate());
							msgEmbed02->setDescription(msgString);
							msgEmbed02->setAuthor(guildMember.userName, guildMember.userAvatar);
							msgEmbed02->setColor(discordGuild.data.borderColor);
							RespondToInputEventData dataPackage03(event02);
							dataPackage03.setResponseType(InputEventResponseType::Follow_Up_Message);
							dataPackage03.addMessageEmbed(*msgEmbed02);
							InputEventData event01 = InputEvents::respondToInputEventAsync(dataPackage03).get();
						}
					}
				}

				InputEvents::deleteInputEventResponseAsync(event02);

				discordGuild.writeDataToDB();

				return;
			} catch (...) {
				reportException("Shop::execute()");
			}
		}
		~Shop(){};
	};
}
