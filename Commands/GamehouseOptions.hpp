// GamehouseOptions.hpp - Header for the "gamehouse options" command.
// Jun 23, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _GAMEHOUSE_OPTIONS_
#define _GAMEHOUSE_OPTIONS_

#include "Index.hpp"

namespace DiscordCoreAPI {
	class GamehouseOptions : public BaseFunction {
	public:
		GamehouseOptions() {
			this->commandName = "gamehouseoptions";
			this->helpDescription = "View some of the options-state for this bot.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /gamehouseoptions.\n------");
			msgEmbed.setTitle("__**GameHouse Options Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		};

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<GamehouseOptions>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			Channel channel = Channels::getCachedChannelAsync({ .channelId = args.eventData->getChannelId() }).get();

			bool areWeInADm = areWeInADM(*args.eventData, channel);

			if (areWeInADm) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);
			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId(),.guildId = args.eventData->getGuildId() }).get();
			bool doWeHaveAdminPerms = doWeHaveAdminPermissions(args, *args.eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPerms == false) {
				return;
			}

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**GameHouse Options:**__");
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription("**Enter '!help = COMMANDNAME to get instructions for each option!**");

			std::vector<EmbedFieldData> fields;
			std::string resultIcon = "❌";
			if (discordGuild.data.guildShop.items.size() > 0) {
				resultIcon = "✅";
			}
			EmbedFieldData shopItemsField; shopItemsField.name = "__**Shop Items:**__";
			shopItemsField.value = "__Active:__ " + resultIcon + "\n__Command(s):__ '!addshopitem', '!removeshopitem'";
			shopItemsField.Inline = true;

			fields.push_back(shopItemsField);

			resultIcon = "❌";
			if (discordGuild.data.guildShop.roles.size() > 0) {
				resultIcon = "✅";
			}
			EmbedFieldData shopRolesField;
			shopRolesField.name = "__**Shop Roles:**__";
			shopRolesField.value = "__Active:__ " + resultIcon + "\n__Command(s):__ '!addshoprole', '!removeshoprole'";
			shopRolesField.Inline = true;

			fields.push_back(shopRolesField);

			resultIcon = "❌";
			if (discordGuild.data.gameChannelIds.size() > 0) {
				resultIcon = "✅";
			}
			EmbedFieldData gameChannelsField;
			gameChannelsField.name = "__**Restricted Game Activity To Specific Channels:**__";
			gameChannelsField.value = "__Active:__ " + resultIcon + "\n__Command(s):__ '!setgamechannel'";
			gameChannelsField.Inline = true;
			fields.push_back(gameChannelsField);

			msgEmbed.fields = fields;
			RespondToInputEventData dataPackage(*args.eventData);
			dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
			dataPackage.addMessageEmbed(msgEmbed);
			InputEvents::respondToEvent(dataPackage);

			return;
		}
		virtual ~GamehouseOptions() {};
	};
}

#endif 