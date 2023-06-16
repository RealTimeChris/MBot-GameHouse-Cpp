// DisplayGuildsData.hpp - Header for the "display guilds data" command.
// jun 22, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class display_guilds_data : public base_function {
	  public:
		display_guilds_data() {
			this->commandName	  = "displayguildsdata";
			this->helpDescription = "displays some info about the servers that this bot is in.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /displayguildsdata.\n------");
			msgEmbed.setTitle("__**Display guild_data's data usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<display_guilds_data>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				uint32_t currentCount				  = 0;
				jsonifier::vector<guild_data> theCache = guilds::getAllGuildsAsync();
				respond_to_input_event_data dataPackage(argsNew.getInputEventData());
				dataPackage.setResponseType(input_event_response_type::Ephemeral_Deferred_Response);
				auto inputEvent = input_events::respondToInputEventAsync(dataPackage).get();
				for (auto& valueNew : theCache) {
					auto value			  = guilds::getGuildAsync({ valueNew.id }).get();
					jsonifier::string msgString = "__Guild name:__ " + static_cast<jsonifier::string>(value.name) + "\n";
					msgString += "__Guild id:__ " + value.id + "\n";
					msgString += "__Member count:__ " + jsonifier::toString(value.memberCount) + "\n";

					user_data owner = users::getCachedUser({ value.ownerId });
					msgString +=
						"__Guild owner:__ <@!" + value.ownerId + "> " + static_cast<jsonifier::string>(owner.userName) + "#" + static_cast<jsonifier::string>(owner.discriminator) + "\n";
					msgString += "__Created at:__ " + value.id.getCreatedAtTimeStamp();

					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setThumbnail(value.getGuildImageUrl(guild_image_types::Splash));
					msgEmbed.setTitle("__**Guild_data data " + jsonifier::toString(currentCount + 1) + " of " + jsonifier::toString(theCache.size()) + "**__");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);

					respond_to_input_event_data dataPackage02(inputEvent);
					dataPackage02.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
					dataPackage02.addMessageEmbed(msgEmbed);
					inputEvent = input_events::respondToInputEventAsync(dataPackage02).get();
					currentCount += 1;
				};
				return;
			} catch (const std::exception& error) {
				std::cout << "display_guilds_data::execute()" << error.what() << std::endl;
			}
		};
		~display_guilds_data(){};
	};
}// namespace discord_core_api
