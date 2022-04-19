// SellDrugs.hpp - Header for the "sell drugs" command.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class SellDrugs : public BaseFunction {
	  public:
		SellDrugs() {
			this->commandName = "selldrugs";
			this->helpDescription = "Gains you some currency!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /selldrugs to gain currency.\n------");
			msgEmbed.setTitle("__**Sell Drugs Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<SellDrugs>();
		}

		virtual void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ argsNew.eventData->getChannelId() }).get();

				bool areWeInADm = areWeInADM(*argsNew.eventData, channel);

				if (areWeInADm == true) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*argsNew.eventData)).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData->getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedGamingInChannel(*argsNew.eventData, discordGuild);

				if (areWeAllowed == false) {
					return;
				}
				auto botUser = argsNew.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				int64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				uint32_t msPerWorkCycle = discordUser.data.hoursOfDrugSaleCooldown * 60 * 60 * 1000;
				uint32_t msPerSecond = 1000;
				uint32_t msPerMinute = 60 * msPerSecond;
				uint32_t msPerHour = 60 * msPerMinute;

				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync(
					{ .guildMemberId = argsNew.eventData->getRequesterId(), .guildId = argsNew.eventData->getGuildId() })
											  .get();
				DiscordGuildMember discordGuildMember(guildMember);
				User currentUser = Users::getUserAsync({ .userId = argsNew.eventData->getRequesterId() }).get();
				uint32_t lastTimeWorked = discordGuildMember.data.lastTimeWorked;

				uint32_t timeDifference = ( uint32_t )currentTime - lastTimeWorked;

				if (timeDifference >= msPerWorkCycle) {
					std::mt19937_64 randomEngine{ static_cast<uint64_t>(
						std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
					uint32_t amountEarned = static_cast<uint32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 5000.0f);

					discordGuildMember.data.currency.wallet += amountEarned;
					discordGuildMember.writeDataToDB();

					std::string msgString = "";
					msgString += "You've been busy dealing drugs... and you've earned " + std::to_string(amountEarned) + " " + discordUser.data.currencyName +
						"\nNice job and watch out for cops!\nYour new wallet balance is: ";
					msgString += std::to_string(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName;
					EmbedData messageEmbed;
					messageEmbed.setAuthor(currentUser.userName, currentUser.avatar);
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Drug Dealing:**__");
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					RespondToInputEventData dataPackage{ *argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);

					discordGuildMember.data.lastTimeWorked =
						( uint32_t )std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					discordGuildMember.writeDataToDB();
				} else {
					uint32_t timeLeft = msPerWorkCycle - timeDifference;
					uint32_t hoursLeft = ( uint32_t )trunc(timeLeft / msPerHour);
					uint32_t minutesLeft = ( uint32_t )trunc((timeLeft % msPerHour) / msPerMinute);
					uint32_t secondsLeft = ( uint32_t )trunc(((timeLeft % msPerHour) % msPerMinute) / msPerSecond);

					std::string msgString = "";
					if (hoursLeft > 0) {
						msgString += "Sorry, but you need to wait " + std::to_string(hoursLeft) + " hours, " + std::to_string(minutesLeft) + " minutes, and " +
							std::to_string(secondsLeft) + " seconds before you can get paid again!";
					} else if (minutesLeft > 0) {
						msgString += "Sorry, but you need to wait " + std::to_string(minutesLeft) + " minutes, and " + std::to_string(secondsLeft) +
							" seconds before you can get paid again!";
					} else {
						msgString += "Sorry, but you need to wait " + std::to_string(secondsLeft) + " seconds before you can get paid again!";
					}
					EmbedData messageEmbed;
					messageEmbed.setAuthor(currentUser.userName, currentUser.avatar);
					messageEmbed.setDescription(msgString);
					messageEmbed.setTitle("__**Drug Dealing:**__");
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setTimeStamp(getTimeAndDate());
					RespondToInputEventData dataPackage{ *argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
				}
			} catch (...) {
				reportException("SellDrugs::execute()");
			}
		}
		virtual ~SellDrugs(){};
	};
}
