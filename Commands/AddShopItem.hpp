// AddShopItem.hpp - Header for the "add shop item" command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class AddShopItem : public BaseFunction {
	  public:
		AddShopItem() {
			this->commandName = "addshopitem";
			this->helpDescription = "Add an item to the server's shop.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /addshopitem ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI.\n------");
			msgEmbed.setTitle("__**Add Shop Item Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<AddShopItem>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = args.eventData->getChannelId() }).get();

				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId(), .guildId = args.eventData->getGuildId() }).get();

				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm == true) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool doWeHaveAdmin = doWeHaveAdminPermissions(args, *args.eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdmin == false) {
					return;
				}

				bool areWeAllowed = checkIfAllowedGamingInChannel(*args.eventData, discordGuild);

				if (areWeAllowed == false) {
					return;
				}
				int32_t theInt = std::bit_cast<int32_t, int32_t>(static_cast<int32_t>(std::stoll(args.commandData.optionsArgs[2])));
				std::regex selfModRegExp("\\d{1,5}");
				std::regex oppModRegExp("-{0,1}\\d{1,5}");
				std::regex itemCostRegExp("\\d{1,6}");
				std::regex emojiRegExp(".{1,32}");
				if (args.commandData.optionsArgs.size() < 2 || ! regex_search(args.commandData.optionsArgs.at(1), selfModRegExp) || std::stoll(args.commandData.optionsArgs.at(1)) > 100 ||
					std::stoll(args.commandData.optionsArgs.at(1)) < 0) {
					std::string msgString = "------\n**Please enter a valid self-mod value, between 0 and 100! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event = InputEvents::respondToEvent(dataPackage);
					return;
				}
				if (args.commandData.optionsArgs.size() < 3 || theInt < -100 || theInt > 0) {
					std::string msgString = "------\n**Please enter a valid opp-mod value between -100 and 0! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> eventNew = InputEvents::respondToEvent(dataPackage);
					return;
				}
				if (args.commandData.optionsArgs.size() < 4 || ! regex_search(args.commandData.optionsArgs.at(3), itemCostRegExp) || std::stoll(args.commandData.optionsArgs.at(3)) < 1) {
					std::string msgString = "------\n**Please enter a valid item cost! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event = InputEvents::respondToEvent(dataPackage);
					return;
				}
				if (args.commandData.optionsArgs.size() < 5 || ! regex_search(args.commandData.optionsArgs.at(4), emojiRegExp)) {
					std::string msgString = "------\n**Please enter a valid emoji! (!addshopitem = ITEMNAME, SELFMOD, OPPMOD, ITEMCOST, EMOJI)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event = InputEvents::respondToEvent(dataPackage);
					return;
				}

				std::string itemName = args.commandData.optionsArgs.at(0);
				std::cmatch matchResults;
				regex_search(args.commandData.optionsArgs.at(1).c_str(), matchResults, selfModRegExp);
				uint32_t selfMod = ( uint32_t )std::stoll(matchResults.str());
				regex_search(args.commandData.optionsArgs.at(2).c_str(), matchResults, oppModRegExp);
				int32_t oppMod = static_cast<int32_t>(theInt);
				regex_search(args.commandData.optionsArgs.at(3).c_str(), matchResults, itemCostRegExp);
				uint32_t itemCost = ( uint32_t )std::stoll(matchResults.str());
				std::string emoji = args.commandData.optionsArgs.at(4);

				for (auto& value: discordGuild.data.guildShop.items) {
					if (itemName == value.itemName) {
						std::string msgString = "------\n**Sorry, but an item by that name already exists!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Item Issue:**__");
						RespondToInputEventData dataPackage{ *args.eventData };
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						std::unique_ptr<InputEventData> event = InputEvents::respondToEvent(dataPackage);
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
				auto botUser = args.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				std::string msgString = "";
				msgString = "Good job! You've added a new item to the shop, making it available for purchase by the members of this server!\n\
				The item's stats are as follows:\n__Item Name__: " +
					itemName + "\n__Self-Mod Value__: " + std::to_string(selfMod) + "\n__Opp-Mod Value__: " + std::to_string(oppMod) + "\n\
				__Item Cost__: " +
					std::to_string(itemCost) + " " + discordUser.data.currencyName + "\n__Emoji__: " + emoji;
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**New Shop Item Added:**__");
				RespondToInputEventData dataPackage{ *args.eventData };
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				std::unique_ptr<InputEventData> event = InputEvents::respondToEvent(dataPackage);
				return;
			} catch (...) {
				reportException("AddShopItem::execute()");
			}
		}

		virtual ~AddShopItem(){};
	};

}
