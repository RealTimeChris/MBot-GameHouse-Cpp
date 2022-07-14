// DisplayGuildsData.hpp - Header for the "display guilds data" command.
// Jun 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class DisplayGuildsData : public BaseFunction {
	  public:
		DisplayGuildsData() {
			this->commandName = "displayguildsdata";
			this->helpDescription = "Displays some info about the servers that this bot is in.";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter /displayguildsdata.\n------");
			msgEmbed.setTitle("__**Display Guild's Data Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<DisplayGuildsData>();
		}

		void execute(BaseFunctionArguments& newArgs) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = newArgs.eventData.getChannelId() }).get();

				uint32_t currentCount = 0;
				std::vector<GuildData> theCache = Guilds::getAllGuildsAsync().get();
				RespondToInputEventData dataPackage(newArgs.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Deferred_Response);
				auto inputEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				for (auto& valueNew: theCache) {
					auto value = Guilds::getGuildAsync({ valueNew.id }).get();
					std::string msgString = "__Guild Name:__ " + static_cast<std::string>(value.name) + "\n";
					msgString += "__Guild ID:__ " + std::to_string(value.id) + "\n";
					msgString += "__Member Count:__ " + std::to_string(value.memberCount) + "\n";

					msgString += "__Joined At:__ " + value.joinedAt.getDateTimeStamp(TimeFormat::LongDateTime) + "\n";
					User owner = Users::getUserAsync({ value.ownerId }).get();
					msgString += "__Guild Owner:__ <@!" + std::to_string(value.ownerId) + "> " + static_cast<std::string>(owner.userName) + "#" +
						static_cast<std::string>(owner.discriminator) + "\n";
					msgString += "__Created At:__ " + value.getCreatedAtTimestamp(TimeFormat::LongDateTime);

					EmbedData msgEmbed{};
					msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					msgEmbed.setColor("FEFEFE");
					msgEmbed.setThumbnail(value.icon);
					msgEmbed.setTitle("__**Guild Data " + std::to_string(currentCount + 1) + " of " + std::to_string(theCache.size()) + "**__");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(msgString);

					RespondToInputEventData dataPackage02(inputEvent);
					dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					dataPackage02.addMessageEmbed(msgEmbed);
					inputEvent = InputEvents::respondToInputEventAsync(dataPackage02).get();
					currentCount += 1;
				};
				return;
			} catch (...) {
				reportException("DisplayGuildsData::execute()");
			}
		};
		~DisplayGuildsData(){};
	};
}
