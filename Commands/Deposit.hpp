// Deposit.hpp - Header for the "Deposit" command.
// Jun 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DEPOSIT_
#define _DEPOSIT_

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Deposit : public BaseFunction {
	public:
		Deposit() {
			this->commandName = "deposit";
			this->helpDescription = "Deposit some currency to your bank account for safe keeping.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /deposit DEPOSITAMOUNT.\n------");
			msgEmbed.setTitle("__**Deposit Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<Deposit>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();

			bool areWeInADm = areWeInADM(*args.eventData, channel);

			if (areWeInADm == true) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			bool areWeAllowed = checkIfAllowedGamingInChannel(*args.eventData, discordGuild);

			if (!areWeAllowed) {
				return;
			}

			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId(),.guildId = args.eventData->getGuildId() }).get();
			DiscordGuildMember discordGuildMember(guildMember);

			 std::regex depositAmountRegExp("\\d{1,18}");
			 std::cmatch matchResults;
			uint32_t depositAmount = 0;
			if (args.commandData.optionsArgs.size() == 0 || args.commandData.optionsArgs[0] == "all") {
				depositAmount = discordGuildMember.data.currency.wallet;
			}
			else if (args.commandData.optionsArgs.size() == 0 ||args.commandData.optionsArgs[0] == "" || !regex_search(args.commandData.optionsArgs[0].c_str(),matchResults, depositAmountRegExp) ||  std::stoll(matchResults.str()) <= 0) {
				std::string msgString = "------\n**Please enter a valid deposit amount!(!deposit = AMOUNT)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				RespondToInputEventData dataPackage{ *args.eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			else if (regex_search(args.commandData.optionsArgs[0], depositAmountRegExp)) {
				depositAmount = (uint32_t) std::stoll(matchResults.str());
			}

			if (depositAmount > discordGuildMember.data.currency.wallet) {
				std::string msgString = "------\n**Sorry, but you do not have sufficient funds to deposit that much!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				RespondToInputEventData dataPackage{ *args.eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			auto botUser = args.discordCoreClient->getBotUser();
			DiscordUser discordUser(botUser.userName, botUser.id);
			uint32_t msPerSecond = 1000;
			uint32_t  SecondsPerMinute = 60;
			uint32_t msPerMinute = msPerSecond * SecondsPerMinute;
			uint32_t MinutesPerHour = 60;
			uint32_t msPerHour = msPerMinute * MinutesPerHour;
			uint32_t msPerDepositCycle = msPerHour * discordUser.data.hoursOfDepositCooldown;
			uint32_t currentTime = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
			
			uint32_t timeSinceLastDeposit = currentTime - discordGuildMember.data.currency.timeOfLastDeposit;

			std::string msgString = "";
			if (timeSinceLastDeposit >= msPerDepositCycle) {
				discordGuildMember.data.currency.bank += depositAmount;
				discordGuildMember.data.currency.wallet -= depositAmount;
				discordGuildMember.data.currency.timeOfLastDeposit = currentTime;
				discordGuildMember.writeDataToDB();

				msgString = "Congratulations! You've deposited " + std::to_string(depositAmount) + " " + discordUser.data.currencyName + " from your wallet into your bank!\n------\n__**Your new balances are:**__\n__Bank:__ " + std::to_string(discordGuildMember.data.currency.bank) + " ";
				msgString += discordUser.data.currencyName + "\n" + "__Wallet:__ " + std::to_string(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";
			}
			else {
				uint32_t timeRemaining = msPerDepositCycle - timeSinceLastDeposit;
				uint32_t hoursRemain = (uint32_t)trunc(timeRemaining / msPerHour);
				uint32_t minutesRemain = (uint32_t)trunc((timeRemaining % msPerHour) / msPerMinute);
				uint32_t secondsRemain = (uint32_t)trunc(((timeRemaining % msPerHour) % msPerMinute) / msPerSecond);

				if (hoursRemain > 0) {
					msgString = "Sorry, but you need to wait " + std::to_string(hoursRemain) + " hours, " + std::to_string(minutesRemain) + " minutes, and " + std::to_string(secondsRemain) + " seconds before you can make another deposit!";
				}
				else if (minutesRemain > 0) {
					msgString = "Sorry, but you need to wait " + std::to_string(minutesRemain) + " minutes, and " + std::to_string(secondsRemain) + " seconds before you can make another deposit!";
				}
				else {
					msgString = "Sorry, but you need to wait " + std::to_string(secondsRemain) + " seconds before you can make another deposit!";
				}
			}

			EmbedData messageEmbed;
			messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
			messageEmbed.setColor(discordGuild.data.borderColor);
			messageEmbed.setTitle("__**Bank Deposit:**__");
			messageEmbed.setTimeStamp(getTimeAndDate());
			messageEmbed.setDescription(msgString);
			RespondToInputEventData dataPackage{ *args.eventData };
			dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
			dataPackage.addMessageEmbed(messageEmbed);
			auto newEvent = InputEvents::respondToEvent(dataPackage);
			return;
		}
		virtual ~Deposit() {};
	};
}

#endif
