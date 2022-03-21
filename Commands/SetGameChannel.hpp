// SetGameChannel.hpp - Header for the "set game channel" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SET_GAME_CHANNEL_
#define _SET_GAME_CHANNEL_

#include "Index.hpp"
#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class SetGameChannel :public BaseFunction {
	public:
		SetGameChannel() {
			this->commandName = "setgamechannel";
			this->helpDescription = "Enables channel-restriction for issuing commands on the bot, and sets the channels.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /setmusichannel add in order to add the current channel.\nAlternatively enter /setgamechannel remove to remove the current channel.\nAlso, enter /setgamechannel view or purge to view or purge the currently enabled channels.\n------");
			msgEmbed.setTitle("__**Set Game Channel Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<SetGameChannel>();
		}

		virtual void execute( std::unique_ptr<BaseFunctionArguments> args) {
			Channel channel = Channels::getCachedChannelAsync({ args->eventData->getChannelId() }).get();

			bool areWeInADm = areWeInADM(*args->eventData, channel);

			if (areWeInADm == true) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args->eventData)).get();
			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args->eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);
			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args->eventData->getAuthorId() ,.guildId = args->eventData->getGuildId() }).get();
			bool doWeHaveAdminPermission = doWeHaveAdminPermissions(*args, *args->eventData, discordGuild, channel, guildMember);

			if (doWeHaveAdminPermission == false) {
				return;
			}

			if (args->commandData.subCommandName == "add") {

				std::string channelID = channel.id;
				for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
					if (channelID ==  discordGuild.data.gameChannelIds[x]) {
						std::string msgString = "------\n**That channel is already on the list of enabled channels!**\n------";
						EmbedData messageEmbed;
						messageEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
						messageEmbed.setColor(discordGuild.data.borderColor);
						messageEmbed.setTimeStamp(getTimeAndDate());
						messageEmbed.setDescription(msgString);
						messageEmbed.setTitle("__**Already Listed:**__");
						RespondToInputEventData dataPackage(*args->eventData);
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(messageEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						return;
					}
				}

				discordGuild.data.gameChannelIds.push_back(channelID);
				discordGuild.writeDataToDB();
				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription("------\n**You've succesfully added <#" + channelID + "> to your list of accepted gaming channels!**\n------");
				messageEmbed.setTitle("__**Game Channel Added:**__");
				RespondToInputEventData dataPackage(*args->eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(messageEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			if (args->commandData.subCommandName == "remove") {
				std::string channelID;
				channelID = channel.id;

				std::string msgString;
				bool isItPresent = false;
				for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
					if (channelID == discordGuild.data.gameChannelIds[x]) {
						isItPresent = true;
						discordGuild.data.gameChannelIds.erase(discordGuild.data.gameChannelIds.begin() + x);
						discordGuild.writeDataToDB();
						msgString += "------\n**You've succesfully removed the channel <#" + channelID + "> from the list of enabled gaming channels!**\n------";
					}
				}

				if (isItPresent == false) {
					std::string msgString2 = "------\n**That channel is not present on the list of enabled gaming channels!**\n------";
					EmbedData messageEmbed;
					messageEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString2);
					messageEmbed.setTitle("__**Missing from List:**__");
					RespondToInputEventData dataPackage(*args->eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					return;
				}

				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Game Channel Removed:**__");
				RespondToInputEventData dataPackage(*args->eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(messageEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			if (args->commandData.subCommandName == "purge") {
				std::string msgString;

				if (discordGuild.data.gameChannelIds.size() > 0) {
					msgString = "__You've removed the following channels from your list of enabled gaming channels:__\n------\n";

					for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
						std::string currentID = discordGuild.data.gameChannelIds[x];

						msgString += "__**Channel #" + std::to_string(x) + "**__<#" + currentID + "> \n";
					}

					msgString += "------\n__**The gaming commands will now work in ANY CHANNEL!**__";

					discordGuild.data.gameChannelIds = std::vector<std::string>();
					discordGuild.writeDataToDB();
				}
				else {
					msgString += "------\n**Sorry, but there are no channels to remove!**\n------";
				}

				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Game Channels Removed:**__");
				RespondToInputEventData dataPackage(*args->eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(messageEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			if (args->commandData.optionsArgs.at(0).find("view") != std::string::npos) {
				std::string msgString = "__You have the following channels enabled for gaming, on this server:__\n------\n";

				for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
					std::string currentID = discordGuild.data.gameChannelIds[x];

					msgString += "__**Channel #" + std::to_string(x) + ":**__ <#" + currentID + "> \n";
				}

				msgString += "------\n";

				EmbedData messageEmbed;
				messageEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				messageEmbed.setColor(discordGuild.data.borderColor);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setDescription(msgString);
				messageEmbed.setTitle("__**Game Channels Enabled:**__");
				RespondToInputEventData dataPackage(*args->eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(messageEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}

			return;
		}
		virtual ~SetGameChannel() {};
	};
}
#endif