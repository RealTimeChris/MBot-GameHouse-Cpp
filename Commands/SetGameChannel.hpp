// SetGameChannel.hpp - Header for the "set game channel" command.
// jun 26, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {
	class set_game_channel : public base_function {
	  public:
		set_game_channel() {
			this->commandName	  = "setgamechannel";
			this->helpDescription = "enables channel-restriction for issuing commands on the bot, and sets the channels.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /setmusichannel add in order to add the current channel.\nAlternatively enter /setgamechannel remove "
									"to remove the current channel.\nAlso, enter /setgamechannel "
									"view or purge to view or purge the currently enabled channels.\n------");
			msgEmbed.setTitle("__**Set game channel_data usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<set_game_channel>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				auto inputEventData			 = argsNew.getInputEventData();
				bool doWeHaveAdminPermission = doWeHaveAdminPermissions(argsNew, inputEventData, discordGuild, channel, guildMember);

				if (doWeHaveAdminPermission == false) {
					return;
				}

				if (argsNew.getSubCommandName() == "add") {
					snowflake channelID = channel.id;
					for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
						if (channelID == discordGuild.data.gameChannelIds[x]) {
							jsonifier::string msgString = "------\n**that channel is already on the list of enabled channels!**\n------";
							embed_data msgEmbed{};
							msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed.setColor("fefefe");
							msgEmbed.setTimeStamp(getTimeAndDate());
							msgEmbed.setDescription(msgString);
							msgEmbed.setTitle("__**Already listed:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
							dataPackage.addMessageEmbed(msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}
					}

					discordGuild.data.gameChannelIds.emplace_back(channelID);
					discordGuild.writeDataToDB(managerAgent);
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription("------\n**You've succesfully added <#" + channelID + "> to your list of accepted gaming channels!**\n------");
					msgEmbed.setTitle("__**Game channel_data added:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.getSubCommandName() == "remove") {
					snowflake channelID;
					channelID = channel.id;

					jsonifier::string msgString;
					bool isItPresent = false;
					for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
						if (channelID == discordGuild.data.gameChannelIds[x]) {
							isItPresent = true;
							discordGuild.data.gameChannelIds.erase(discordGuild.data.gameChannelIds.begin() + x);
							discordGuild.writeDataToDB(managerAgent);
							msgString += "------\n**You've succesfully removed the channel <#" + channelID + "> from the list of enabled gaming channels!**\n------";
						}
					}

					if (isItPresent == false) {
						jsonifier::string msgString2 = "------\n**that channel is not present on the list of enabled gaming channels!**\n------";
						embed_data msgEmbed{};
						msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed.setColor("fefefe");
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setDescription(msgString2);
						msgEmbed.setTitle("__**Missing from list:**__");
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
						return;
					}

					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setTitle("__**Game channel_data removed:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.getSubCommandName() == "purge") {
					jsonifier::string msgString;

					if (discordGuild.data.gameChannelIds.size() > 0) {
						msgString = "__You've removed the following channels from your list of enabled gaming channels:__\n------\n";

						for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
							snowflake currentID = discordGuild.data.gameChannelIds[x];

							msgString += "__**Channel_data #" + jsonifier::toString(x) + "**__<#" + currentID + "> \n";
						}

						msgString += "------\n__**The gaming commands will now work in any channel!**__";

						discordGuild.data.gameChannelIds = jsonifier::vector<snowflake>();
						discordGuild.writeDataToDB(managerAgent);
					} else {
						msgString += "------\n**sorry, but there are no channels to remove!**\n------";
					}

					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setTitle("__**Game channels removed:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.getSubCommandName() == "view") {
					jsonifier::string msgString = "__You have the following channels enabled for gaming, on this server:__\n------\n";

					for (uint32_t x = 0; x < discordGuild.data.gameChannelIds.size(); x += 1) {
						snowflake currentID = discordGuild.data.gameChannelIds[x];

						msgString += "__**Channel_data #" + jsonifier::toString(x) + ":**__ <#" + currentID + "> \n";
					}

					msgString += "------\n";

					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);
					msgEmbed.setTitle("__**Game channels enabled:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				return;
			} catch (const std::exception& error) {
				std::cout << "set_game_channel::execute()" << error.what() << std::endl;
			}
		}
		~set_game_channel(){};
	};
}// namespace discord_core_api