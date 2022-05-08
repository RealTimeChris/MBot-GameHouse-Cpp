// DisplayGuildsData.hpp - Header for the "display guilds data" command.
// Jun 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class DisplayGuildsData : public BaseFunction {
	  public:
		DisplayGuildsData() {
			this->commandName = "displayguildsdata";
			this->helpDescription = "Displays some info about the servers that this bot is in.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /displayguildsdata.\n------");
			msgEmbed.setTitle("__**Display Guild's Data Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<DisplayGuildsData>();
		}

		virtual void execute(BaseFunctionArguments& newArgs) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = newArgs.eventData.getChannelId() }).get();

				uint32_t currentCount = 0;
				std::vector<Guild> theCache = Guilds::getAllGuildsAsync().get();
				RespondToInputEventData dataPackage(newArgs.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Deferred_Response);
				auto inputEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				for (auto& value: theCache) {
					std::string msgString = "__Guild Name:__ " + value.name + "\n";
					msgString += "__Guild ID:__ " + value.id + "\n";
					msgString += "__Member Count:__ " + std::to_string(value.memberCount) + "\n";

					msgString += "__Joined At:__ " + value.joinedAt.getDateTimeStamp(TimeFormat::LongDateTime) + "\n";
					User owner = Users::getUserAsync({ value.ownerId }).get();
					msgString += "__Guild Owner:__ <@!" + value.ownerId + "> " + owner.userName + "#" + owner.discriminator + "\n";
					msgString += "__Created At:__ " + value.createdAt;

					EmbedData messageEmbed;
					messageEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					messageEmbed.setColor("FEFEFE");
					messageEmbed.setThumbnail(value.icon);
					messageEmbed.setTitle("__**Guild Data " + std::to_string(currentCount + 1) + " of " + std::to_string(theCache.size()) + "**__");
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);

					RespondToInputEventData dataPackage02(inputEvent);
					dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					dataPackage02.addMessageEmbed(messageEmbed);
					inputEvent = InputEvents::respondToInputEventAsync(dataPackage02).get();
					currentCount += 1;
				};
				return;
			} catch (...) {
				reportException("DisplayGuildsData::execute()");
			}
		};
		virtual ~DisplayGuildsData(){};
	};
}
