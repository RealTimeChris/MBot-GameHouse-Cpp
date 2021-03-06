// AddShopItem.hpp - Header for the "add shop item" command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"
#include <regex>

namespace DiscordCoreAPI {

	class AddShopItem : public BaseFunction {
	  public:
		AddShopItem() {
			this->commandName = "addshopitem";
			this->helpDescription = "Add an item to the server's shop.";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter /addshopitem ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI.\n------");
			msgEmbed.setTitle("__**Add Shop Item Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<AddShopItem>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = argsNew.eventData.getChannelId() }).get();

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = argsNew.eventData.getAuthorId(), .guildId = argsNew.eventData.getGuildId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool doWeHaveAdmin = doWeHaveAdminPermissions(argsNew, argsNew.eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdmin == false) {
					return;
				}

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.eventData, discordGuild);

				if (areWeAllowed == false) {
					return;
				}
				std::cout << argsNew.optionsArgs[2] << std::endl;
				int32_t theInt = static_cast<int32_t>(std::bit_cast<int64_t>(stoull(argsNew.optionsArgs[2])));
				std::regex selfModRegExp{ "\\d{1,5}" };
				std::regex oppModRegExp{ "-{0,1}\\d{1,5}" };
				std::regex itemCostRegExp{ "\\d{1,6}" };
				std::regex emojiRegExp{ ".{1,32}" };
				if (argsNew.optionsArgs.size() < 2 || !regex_search(argsNew.optionsArgs[1], selfModRegExp) || std::stoll(argsNew.optionsArgs.at(1)) > 100 ||
					std::stoll(argsNew.optionsArgs.at(1)) < 0) {
					std::string msgString = "------\n**Please enter a valid self-mod value, between 0 and 100! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, "
											"ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEventData event = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.optionsArgs.size() < 3 || theInt < -100 || theInt > 0) {
					std::string msgString = "------\n**Please enter a valid opp-mod value between -100 and 0! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, "
											"ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEventData eventNew = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.optionsArgs.size() < 4 || !regex_search(argsNew.optionsArgs.at(3), itemCostRegExp) || std::stoll(argsNew.optionsArgs.at(3)) < 1) {
					std::string msgString = "------\n**Please enter a valid item cost! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEventData eventNew = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.optionsArgs.size() < 5 || !regex_search(argsNew.optionsArgs.at(4), emojiRegExp)) {
					std::string msgString = "------\n**Please enter a valid emoji! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEventData eventNew = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				std::string itemName = argsNew.optionsArgs.at(0);
				std::cmatch matchResults;
				regex_search(argsNew.optionsArgs.at(1).c_str(), matchResults, selfModRegExp);
				uint32_t selfMod = static_cast<uint32_t>(std::stoll(matchResults.str()));
				regex_search(argsNew.optionsArgs.at(2).c_str(), matchResults, oppModRegExp);
				int32_t oppMod = static_cast<int32_t>(theInt);
				regex_search(argsNew.optionsArgs.at(3).c_str(), matchResults, itemCostRegExp);
				uint32_t itemCost = static_cast<uint32_t>(std::stoll(matchResults.str()));
				std::string emoji = argsNew.optionsArgs.at(4);

				for (auto& value: discordGuild.data.guildShop.items) {
					if (itemName == value.itemName) {
						std::string msgString = "------\n**Sorry, but an item by that name already exists!**\n------";
						EmbedData msgEmbed{};
						msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Item Issue:**__");
						RespondToInputEventData dataPackage{ argsNew.eventData };
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						InputEventData eventNew = InputEvents::respondToInputEventAsync(dataPackage).get();
						return;
					}
				}

				InventoryItem newItem;
				newItem.emoji = emoji;
				newItem.itemCost = itemCost;
				newItem.itemName = itemName;
				newItem.oppMod = oppMod;
				newItem.selfMod = selfMod;

				discordGuild.data.guildShop.items.push_back(newItem);
				discordGuild.writeDataToDB();
				auto botUser = argsNew.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				std::string msgString = "";
				msgString = "Good job! You've added a new item to the shop, making it available for purchase by the members of this server!\n\
				The item's stats are as follows:\n__Item Name__: " +
					itemName + "\n__Self-Mod Value__: " + std::to_string(selfMod) + "\n__Opp-Mod Value__: " + std::to_string(oppMod) + "\n\
				__Item Cost__: " +
					std::to_string(itemCost) + " " + discordUser.data.currencyName + "\n__Emoji__: " + emoji;
				EmbedData msgEmbed{};
				msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**New Shop Item Added:**__");
				RespondToInputEventData dataPackage{ argsNew.eventData };
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				InputEventData eventNew = InputEvents::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (...) {
				reportException("AddShopItem::execute()");
			}
		}

		~AddShopItem(){};
	};

}
