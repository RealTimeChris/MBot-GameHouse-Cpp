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

		virtual void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = argsNew.eventData->getChannelId() }).get();

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*argsNew.eventData)).get();
				Guild guild = Guilds::getCachedGuildAsync({ argsNew.eventData->getGuildId() }).get();
				DiscordGuild discordGuild(guild);
				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = argsNew.eventData->getAuthorId(), .guildId = argsNew.eventData->getGuildId() })
						.get();
				bool doWeHaveAdminPermission = doWeHaveAdminPermissions(argsNew, *argsNew.eventData, discordGuild, channel, guildMember);
				if (!doWeHaveAdminPermission) {
					return;
				}

				uint32_t currentCount = 0;
				std::vector<Guild> theCache = Guilds::getAllGuildsAsync().get();
				std::unique_ptr<InputEventData> inputEvent = std::make_unique<InputEventData>(*argsNew.eventData);
				for (auto& value: theCache) {
					std::string msgString = "__Guild Name:__ " + value.name + "\n";
					msgString += "__Guild ID:__ " + value.id + "\n";
					msgString += "__Member Count:__ " + std::to_string(value.memberCount) + "\n";

					msgString += "__Joined At:__ " + value.joinedAt.getDateTimeStamp(TimeFormat::LongDateTime) + "\n";
					User owner = Users::getUserAsync({ value.ownerId }).get();
					msgString += "__Guild Owner:__ <@!" + value.ownerId + "> " + owner.userName + "#" + owner.discriminator + "\n";
					msgString += "__Created At:__ " + value.createdAt;

					EmbedData messageEmbed;
					messageEmbed.setAuthor(argsNew.eventData->getUserName(), argsNew.eventData->getAvatarUrl());
					messageEmbed.setColor("FEFEFE");
					messageEmbed.setThumbnail(value.icon);
					messageEmbed.setTitle("__**Guild Data " + std::to_string(currentCount + 1) + " of " + std::to_string(theCache.size()) + "**__");
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setDescription(msgString);

					if (currentCount == 0) {
						RespondToInputEventData dataPackage(*argsNew.eventData);
						dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
						dataPackage.addMessageEmbed(messageEmbed);
						inputEvent = InputEvents::respondToEvent(dataPackage);
						RespondToInputEventData dataPackage02(*argsNew.eventData);
						dataPackage02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
						dataPackage02.addMessageEmbed(messageEmbed);
						inputEvent = InputEvents::respondToEvent(dataPackage02);
					} else {
						RespondToInputEventData dataPackage(*argsNew.eventData);
						dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
						dataPackage.addMessageEmbed(messageEmbed);
						InputEvents::respondToEvent(dataPackage);
					}
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
