// BotInfo.hpp - Header for the "bot info" command.
// Jun 19, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class BotInfo : public BaseFunction {
	  public:
		BotInfo() {
			this->commandName = "botinfo";
			this->helpDescription = "Displays some info about this bot.";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter /botinfo.\n------");
			msgEmbed.setTitle("__**Bot Info Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<BotInfo>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				auto guilds = Guilds::getAllGuildsAsync().get();
				int32_t userCount{ 0 };
				for (auto& value: guilds) {
					userCount += value.memberCount;
				}

				EmbedData msgEmbed{};
				msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed.setImage(argsNew.discordCoreClient->getBotUser().avatar);
				msgEmbed.setColor("FEFEFE");
				msgEmbed.setTitle("__**Bot Info:**__");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.addField("__Bot Name:__",
					static_cast<std::string>(argsNew.discordCoreClient->getBotUser().userName) + "#" +
						static_cast<std::string>(argsNew.discordCoreClient->getBotUser().discriminator),
					true);
				msgEmbed.addField("__Bot ID:__", std::to_string(argsNew.discordCoreClient->getBotUser().id), true);
				msgEmbed.addField("__Guild Count:__", std::to_string(guilds.size()), true);
				msgEmbed.addField("__Created At:__", argsNew.discordCoreClient->getBotUser().getCreatedAtTimestamp(TimeFormat::LongDateTime), true);
				msgEmbed.addField("__Serving Users:__", std::to_string(userCount), true);
				msgEmbed.addField("__Running On:__", "[DiscordCoreAPI Bot Library](https://discordcoreapi.com)", true);
				msgEmbed.addField("__Created By:__", "RealTime Chris#3627", true);
				RespondToInputEventData dataPackage(argsNew.eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto eventNew = InputEvents::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (...) {
				reportException("BotInfo::execute()");
			}
		}
		~BotInfo(){};
	};
}
