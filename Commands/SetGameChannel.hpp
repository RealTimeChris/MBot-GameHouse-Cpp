// SetGameChannel.hpp - Header for the "set game channel" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class SetGameChannel : public BaseFunction {
	  public:
		SetGameChannel() {
			this->commandName = "setgamechannel";
			this->helpDescription = "Enables channel-restriction for issuing commands on the bot, and sets the channels.";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /setmusichannel add in order to add the current channel.\nAlternatively enter /setgamechannel remove "
									"to remove the current channel.\nAlso, enter /setgamechannel "
									"view or purge to view or purge the currently enabled channels.\n------");
			msgEmbed.setTitle("__**Set Game Channel Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<SetGameChannel>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ argsNew.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = argsNew.eventData.getAuthorId(), .guildId = argsNew.eventData.getGuildId() }).get();
				bool doWeHaveAdminPermission = doWeHaveAdminPermissions(argsNew, argsNew.eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdminPermission == false) {
					return;
				}

				if (argsNew.subCommandName == "add") {
					uint64_t channelID = channel.id;
					for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
						if (channelID == discordGuild.data.gameChannelIds[x]) {
							std::string msgString = "------\n**That channel is already on the list of enabled channels!**\n------";
							EmbedData msgEmbed{};
							msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
							msgEmbed.setColor(discordGuild.data.borderColor);
							msgEmbed.setTimeStamp(getTimeAndDate());
							msgEmbed.setDescription(msgString);
							msgEmbed.setTitle("__**Already Listed:**__");
							RespondToInputEventData dataPackage(argsNew.eventData);
							dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(msgEmbed);
							auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
							return;
						}
					}

					discordGuild.data.gameChannelIds.push_back(channelID);
					discordGuild.writeDataToDB();
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription("------\n**You've succesfully added <#" + std::to_string(channelID) + "> to your list of accepted gaming channels!**\n------");
					msgEmbed.setTitle("__**Game Channel Added:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.subCommandName == "remove") {
					uint64_t channelID;
					channelID = channel.id;

					std::string msgString;
					bool isItPresent = false;
					for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
						if (channelID == discordGuild.data.gameChannelIds[x]) {
							isItPresent = true;
							discordGuild.data.gameChannelIds.erase(discordGuild.data.gameChannelIds.begin() + x);
							discordGuild.writeDataToDB();
							msgString +=
								"------\n**You've succesfully removed the channel <#" + std::to_string(channelID) + "> from the list of enabled gaming channels!**\n------";
						}
					}

					if (isItPresent == false) {
						std::string msgString2 = "------\n**That channel is not present on the list of enabled gaming channels!**\n------";
						EmbedData msgEmbed{};
						msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setDescription(msgString2);
						msgEmbed.setTitle("__**Missing from List:**__");
						RespondToInputEventData dataPackage(argsNew.eventData);
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
						return;
					}

					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setTitle("__**Game Channel Removed:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.subCommandName == "purge") {
					std::string msgString;

					if (discordGuild.data.gameChannelIds.size() > 0) {
						msgString = "__You've removed the following channels from your list of enabled gaming channels:__\n------\n";

						for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
							uint64_t currentID = discordGuild.data.gameChannelIds[x];

							msgString += "__**Channel #" + std::to_string(x) + "**__<#" + std::to_string(currentID) + "> \n";
						}

						msgString += "------\n__**The gaming commands will now work in ANY CHANNEL!**__";

						discordGuild.data.gameChannelIds = std::vector<uint64_t>();
						discordGuild.writeDataToDB();
					} else {
						msgString += "------\n**Sorry, but there are no channels to remove!**\n------";
					}

					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setTitle("__**Game Channels Removed:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.optionsArgs.at(0).find("view") != std::string::npos) {
					std::string msgString = "__You have the following channels enabled for gaming, on this server:__\n------\n";

					for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
						uint64_t currentID = discordGuild.data.gameChannelIds[x];

						msgString += "__**Channel #" + std::to_string(x) + ":**__ <#" + std::to_string(currentID) + "> \n";
					}

					msgString += "------\n";

					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setTitle("__**Game Channels Enabled:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				return;
			} catch (...) {
				reportException("SetGameChannel::execute()");
			}
		}
		~SetGameChannel(){};
	};
}