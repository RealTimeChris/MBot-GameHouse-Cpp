// Buy.hpp - Header for the "Buy" command.
// June 4, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Buy : public BaseFunction {
	  public:
		Buy() {
			this->commandName = "buy";
			this->helpDescription = "Buy an item or role from the server's shop.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /buy ITEMNAME/ROLENAME.\n------");
			msgEmbed.setTitle("__**Buy Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Buy>();
		}

		virtual void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ argsNew.eventData->getChannelId() }).get();

				bool areWeInADm = areWeInADM(*argsNew.eventData, channel);

				if (areWeInADm == true) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*argsNew.eventData)).get();

				Guild guild = Guilds::getCachedGuildAsync({ argsNew.eventData->getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedGamingInChannel(*argsNew.eventData, discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				User currentUser = Users::getUserAsync({ argsNew.eventData->getRequesterId() }).get();

				std::string objectName = argsNew.commandData.optionsArgs.at(0);
				uint32_t objectShopIndex = 0;
				std::string objectType;
				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = currentUser.id, .guildId = argsNew.eventData->getGuildId() }).get();
				DiscordGuildMember discordGuildMember(guildMember);
				std::vector<Role> rolesArray = Roles::getGuildMemberRolesAsync({ .guildMember = guildMember, .guildId = argsNew.eventData->getGuildId() }).get();

				for (uint32_t x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
					bool isRoleFound = false;
					InventoryRole shopRole = discordGuildMember.data.roles.at(x);
					for (uint32_t y = 0; y < rolesArray.size(); y += 1) {
						if (rolesArray.at(y).id == shopRole.roleId) {
							isRoleFound = true;
							break;
						}
					}
					if (isRoleFound == false) {
						for (uint32_t z = 0; z < discordGuildMember.data.roles.size(); z += 1) {
							if (shopRole.roleId == discordGuildMember.data.roles.at(z).roleId) {
								discordGuildMember.data.roles.erase(discordGuildMember.data.roles.begin() + z);
							}
						}
						std::string msgString = "------\n**Removing guild role " + shopRole.roleName + " from guild member cache!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Removed Guild Member Role:**__");
						RespondToInputEventData dataPackage{ *argsNew.eventData };
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
						x -= 1;
					}
					discordGuildMember.writeDataToDB();
				}

				bool isFoundInShop = false;
				for (uint32_t x = 0; x < discordGuild.data.guildShop.items.size(); x += 1) {
					if (objectName == discordGuild.data.guildShop.items.at(x).itemName) {
						isFoundInShop = true;
						objectShopIndex = x;
						objectType = "item";
						break;
					}
				}
				for (uint32_t x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
					if (objectName == discordGuild.data.guildShop.roles.at(x).roleName) {
						isFoundInShop = true;
						objectShopIndex = x;
						objectType = "role";
						break;
					}
				}
				if (isFoundInShop == false) {
					std::string msgString = "------\n**Sorry, but we could not find that object in the shop!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Object:**__");
					RespondToInputEventData dataPackage{ *argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
					return;
				}

				bool isFoundInInventory = false;

				for (uint32_t x = 0; x < discordGuildMember.data.items.size(); x += 1) {
					if (objectName == discordGuildMember.data.items.at(x).itemName) {
						isFoundInInventory = true;
						break;
					}
				}

				for (uint32_t x = 0; x < discordGuildMember.data.roles.size(); x += 1) {
					if (objectName == discordGuildMember.data.roles.at(x).roleName) {
						isFoundInInventory = true;
						break;
					}
				}

				if (isFoundInInventory == true) {
					std::string msgString = "------\n**Sorry, but you already have one of those " + objectType + "s.** \n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Duplicate Object:**__");
					RespondToInputEventData dataPackage{ *argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
					return;
				}

				if (objectType == "role") {
					uint32_t roleCost = discordGuild.data.guildShop.roles.at(objectShopIndex).roleCost;
					uint32_t userBalance = discordGuildMember.data.currency.wallet;

					if (roleCost > userBalance) {
						std::string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Insufficient Funds:**__");
						RespondToInputEventData dataPackage{ *argsNew.eventData };
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
						return;
					}

					InventoryRole newRole = discordGuild.data.guildShop.roles.at(objectShopIndex);
					discordGuildMember.data.roles.push_back(newRole);
					discordGuildMember.data.currency.wallet -= roleCost;
					discordGuildMember.writeDataToDB();

					uint32_t newBalance = discordGuildMember.data.currency.wallet;

					std::string roleID = discordGuild.data.guildShop.roles.at(objectShopIndex).roleId;

					Roles::addGuildMemberRoleAsync({ .guildId = argsNew.eventData->getGuildId(), .userId = currentUser.id, .roleId = roleID });
					auto botUser = argsNew.discordCoreClient->getBotUser();
					DiscordUser discordUser(botUser.userName, botUser.id);
					std::string msgString = "------\nCongratulations! You've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ <@&" +
						newRole.roleId + "> (" + newRole.roleName + ")\n------\n__**Your new wallet balance:**__ " + std::to_string(newBalance) + " " +
						discordUser.data.currencyName + "\n------";
					EmbedData msgEmbed;
					msgEmbed.setTitle("__**New Role Purchased:**__");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
					msgEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage{ *argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);

					uint32_t maxIdx = 0;
					InventoryRole tempItem;
					uint32_t len = ( uint32_t )discordGuildMember.data.roles.size();
					for (uint32_t x = 0; x < len; x += 1) {
						maxIdx = x;
						for (uint32_t y = x + 1; y < len; y += 1) {
							if (discordGuildMember.data.roles.at(y).roleCost > discordGuildMember.data.roles.at(maxIdx).roleCost) {
								maxIdx = y;
							}
						}
						tempItem = discordGuildMember.data.roles.at(x);
						discordGuildMember.data.roles.at(x) = discordGuildMember.data.roles.at(maxIdx);
						discordGuildMember.data.roles.at(maxIdx) = tempItem;
					}
					discordGuildMember.writeDataToDB();
					return;
				} else if (objectType == "item") {
					uint32_t itemCost = discordGuild.data.guildShop.items.at(objectShopIndex).itemCost;
					uint32_t userBalance = discordGuildMember.data.currency.wallet;

					if (itemCost > userBalance) {
						std::string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to purchase that!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setDescription(msgString);
						msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setTitle("__**Insufficient Funds:**__");
						RespondToInputEventData dataPackage{ *argsNew.eventData };
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
						return;
					}

					InventoryItem newItem = discordGuild.data.guildShop.items.at(objectShopIndex);
					discordGuildMember.data.items.push_back(newItem);
					discordGuildMember.data.currency.wallet -= itemCost;
					discordGuildMember.writeDataToDB();

					std::string itemEmoji = discordGuild.data.guildShop.items.at(objectShopIndex).emoji;
					std::string itemName = discordGuild.data.guildShop.items.at(objectShopIndex).itemName;
					uint32_t newBalance = discordGuildMember.data.currency.wallet;
					auto botUser = argsNew.discordCoreClient->getBotUser();
					DiscordUser discordUser(botUser.userName, botUser.id);
					std::string msgString = "------\nCongratulations!You've just purchased a new " + objectType + ".\n------\n__**It is as follows:**__ " +
						itemEmoji + itemName + "\n------\n__**Your new wallet balance:**__ " + std::to_string(newBalance) + " " +
						discordUser.data.currencyName + "\n------";
					EmbedData msgEmbed;
					msgEmbed.setTitle("__**New Item Purchased:**__");

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
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
					msgEmbed.setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage{ *argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
					return;
				}
				return;
			} catch (...) {
				reportException("Buy::execute()");
			}
		}

		virtual ~Buy(){};
	};
};
