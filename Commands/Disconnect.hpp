// Disconnect.hpp - Header for the "disconnect" std::function.
// aug 25, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class disconnect : public base_function {
	  public:
		disconnect() {
			this->commandName	  = "disconnect";
			this->helpDescription = "disconnect the bot from voice chat.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /disconnect!\n------");
			msgEmbed.setTitle("__**Disconnect usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<disconnect>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };

				discord_guild discordGuild{ managerAgent, guild };

				guild_member_data guildMember{ argsNew.getGuildMemberData() };


				if (guild.areWeConnected()) {
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setDescription("------\n__**I'm disconnecting from the voice channel!**__\n------");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Disconnected:**__");
					msgEmbed.setColor("fefefe");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					input_events::deleteInputEventResponseAsync(newEvent, 20000);
					guild.disconnect();
				} else {
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setDescription("------\n__**I was already disconnected!**__\n------");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Disconnected:**__");
					msgEmbed.setColor("fefefe");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
				}

				return;
			} catch (const std::exception& error) {
				std::cout << "disconnect::execute()" << error.what() << std::endl;
			}
		}
		~disconnect(){};
	};

}// namespace discord_core_api
