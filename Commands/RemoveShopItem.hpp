// RemoveShopItem.hpp - Header for the "remove shop item" command.
// Jun 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class RemoveShopItem : public BaseFunction {
	public:
		RemoveShopItem() {
			this->commandName = "removeshopitem";
			this->helpDescription = "Remove's an item from the server's shop.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /removeshopitem ITEMNAME.\n------");
			msgEmbed.setTitle("__**Remove Shop Item Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			 std::unique_ptr<BaseFunction> newPtr =  std::make_unique<RemoveShopItem>();
			return newPtr;
		}

		virtual void execute(BaseFunctionArguments& args) {
			Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();

			bool areWeInADm = areWeInADM(*args.eventData, channel);

			if (areWeInADm ==  true) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId() ,.guildId = args.eventData->getGuildId(), }).get();
			DiscordGuildMember discordGuildMember(guildMember);
			bool doWeHaveAdminPermission = doWeHaveAdminPermissions(args, *args.eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPermission == false) {
				return;
			}

			std::string itemName = args.commandData.optionsArgs.at(0);

			uint32_t itemIndex = 0;
			bool itemFound = false;
			for (uint32_t x = 0; x < discordGuild.data.guildShop.items.size(); x += 1) {
				if (itemName == discordGuild.data.guildShop.items[x].itemName) {
					itemIndex = x;
					itemFound = true;
					break;
				}
			}

			if (itemFound ==  false) {
				std::string msgString = "------\n**Sorry, but that item was not found in the shop's inventory!**\n------";
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

			msgString = "Alrighty then! You've removed an item from the shop!\n------\n__The removed item: __ " + discordGuild.data.guildShop.items[itemIndex].emoji + discordGuild.data.guildShop.items[itemIndex].itemName + "\n------"; 

			discordGuild.data.guildShop.items.erase(discordGuild.data.guildShop.items.begin() + itemIndex);
			discordGuild.writeDataToDB();

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription(msgString);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Shop Item Removed:**__");
			RespondToInputEventData dataPackage(*args.eventData);
			dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
			dataPackage.addMessageEmbed(msgEmbed);
			auto newEvent = InputEvents::respondToEvent(dataPackage);
			return;
		}
		virtual ~RemoveShopItem() {};
	};
}
