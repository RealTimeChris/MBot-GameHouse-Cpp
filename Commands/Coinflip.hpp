// Coinflip.hpp - Header for the "coinflip" game/command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COIN_FLIP_
#define _COIN_FLIP_

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Coinflip :public BaseFunction {
	public:
		Coinflip() {
			this->commandName = "coinflip";
			this->helpDescription = "Gamble some currency on a coinflip!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /coinflip BETAMOUNT.\n------");
			msgEmbed.setTitle("__**Coinflip Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<Coinflip>();
		}

		virtual void execute( std::unique_ptr<BaseFunctionArguments> args) {
		try {
			Channel channel = Channels::getCachedChannelAsync({ args->eventData->getChannelId() }).get();

			bool areWeInADm = areWeInADM(*args->eventData, channel);
			
			if (areWeInADm == true) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args->eventData)).get();

			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args->eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			bool areWeAllowed = checkIfAllowedGamingInChannel(*args->eventData, discordGuild);

			if (areWeAllowed == false) {
				return;
			}

			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args->eventData->getRequesterId(),.guildId = args->eventData->getGuildId() }).get();
			GuildMember botMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args->discordCoreClient->getBotUser().id,.guildId = args->eventData->getGuildId() }).get();
			std::unique_ptr<InputEventData> inputData = std::make_unique<InputEventData>();
			if (!botMember.permissions.checkForPermission(botMember, channel, Permission::Manage_Messages)) {
				std::string msgString = "------\n**I need the Manage Messages permission in this channel, for this game!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Permissions Issue:**__");
				RespondToInputEventData dataPackage{ *args->eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			
			 std::regex betAmountRegExp("\\d{1,18}");
			auto botUser = args->discordCoreClient->getBotUser();
			DiscordUser discordUser(botUser.userName, botUser.id);
			if (args->commandData.optionsArgs.size() == 0 || !std::regex_search(args->commandData.optionsArgs.at(0), betAmountRegExp) ||  std::stoll(args->commandData.optionsArgs.at(0)) < 1) {
				std::string msgString = "------\n**Please enter a valid amount to bet! 1 " + discordUser.data.currencyName + " or more! (!coinflip = BETAMOUNT)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing or Invalid Arguments:**__");
				RespondToInputEventData dataPackage{ *args->eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}

			DiscordGuildMember discordGuildMember(guildMember);

			 std::cmatch matchResults;
			std::regex_search(args->commandData.optionsArgs.at(0).c_str(), matchResults, betAmountRegExp);
			int32_t betAmount = (int32_t) std::stoll(matchResults.str());
			int32_t currencyAmount = discordGuildMember.data.currency.wallet;

			if (betAmount > currencyAmount) {
				std::string msgString = "------\n**Sorry, but you have insufficient funds in your wallet to place that wager!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Insufficient Funds:**__");
				RespondToInputEventData dataPackage{ *args->eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}

			std::string newBetString = "Welcome, <@!" + guildMember.user.id + "> , you have placed a bet of **" + std::to_string(betAmount) + " " + discordUser.data.currencyName + "**.\n";
			newBetString += "React with :exploding_head: to choose heads, or with :snake: to choose tails!";

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
			msgEmbed.setColor("0000FF");
			msgEmbed.setDescription(newBetString);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Heads, or Tails!?**__");
			std::unique_ptr<InputEventData> inputEvent = std::make_unique<InputEventData>(*args->eventData);
			RespondToInputEventData dataPackage{ *args->eventData };
			dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
			dataPackage.addButton(false, "Heads", "Heads", ButtonStyle::Success, "🤯");
			dataPackage.addButton(false, "Tails", "Tails", ButtonStyle::Success, "🐍");
			dataPackage.addMessageEmbed(msgEmbed);
			inputData = InputEvents::respondToEvent(dataPackage);
			ButtonCollector button2(*inputData);
			std::vector<ButtonResponseData> buttonInteractionData = button2.collectButtonData(false, 120000, 1, args->eventData->getAuthorId()).get();
			if (buttonInteractionData.at(0).buttonId == ""){
				std::string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
				EmbedData msgEmbed2;
				msgEmbed2.setColor("FF0000");
				msgEmbed2.setTimeStamp(getTimeAndDate());
				msgEmbed2.setTitle("__**Heads, or Tails?:**__");
				msgEmbed2.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed2.setDescription(timeOutString);
				RespondToInputEventData dataPackage{ buttonInteractionData.at(0).interactionData };
				dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed2);
				InputEvents::respondToEvent(dataPackage);
				return;
			}
			
			std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
			float number = static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max());
			uint32_t newBalance = 0;

			discordGuildMember.getDataFromDB();
			currencyAmount = discordGuildMember.data.currency.wallet;

			if (betAmount > currencyAmount) {
				std::string completionString = "------\nSorry, but you have insufficient funds in your wallet to place that wager.\n------";
				EmbedData msgEmbed3;
				msgEmbed3.setColor("FF0000");
				msgEmbed3.setDescription(completionString);
				msgEmbed3.setTimeStamp(getTimeAndDate());
				msgEmbed3.setTitle("__**Heads, or Tails**__");
				msgEmbed3.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				RespondToInputEventData dataPackage{ *inputData };
				dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed3);
				InputEvents::respondToEvent(dataPackage);
				return;
			}

			EmbedData msgEmbed4;
			if (buttonInteractionData.at(0).buttonId == "Heads" && number > 0.50 || buttonInteractionData.at(0).buttonId == "Tails" && number < 0.50) {
				discordGuildMember.data.currency.wallet += betAmount;
				discordGuildMember.writeDataToDB();
				discordGuild.data.casinoStats.totalCoinFlipPayout += betAmount;
				discordGuild.data.casinoStats.totalPayout += betAmount;
				if (betAmount > discordGuild.data.casinoStats.largestCoinFlipPayout.amount) {
					discordGuild.data.casinoStats.largestCoinFlipPayout.amount = betAmount;
					discordGuild.data.casinoStats.largestCoinFlipPayout.timeStamp = getTimeAndDate();
					discordGuild.data.casinoStats.largestCoinFlipPayout.userId = args->eventData->getAuthorId();
					discordGuild.data.casinoStats.largestCoinFlipPayout.userName = args->eventData->getUserName();
					discordGuild.writeDataToDB();
				}
				newBalance = discordGuildMember.data.currency.wallet;
				std::string completionString = "------\nNICELY DONE FAGGOT! YOU WON!\nYour new wallet balance is: " + std::to_string(newBalance) + " " + discordUser.data.currencyName + ".\n------";
				msgEmbed4.setColor("00FF00");
				msgEmbed4.setDescription(completionString);
				msgEmbed4.setTimeStamp(getTimeAndDate());
				msgEmbed4.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed4.setTitle("__**Heads, or Tails?**__");
				RespondToInputEventData dataPackage{ buttonInteractionData.at(0).interactionData };
				dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed4);
				InputEvents::respondToEvent(dataPackage);
			}
			else if (buttonInteractionData.at(0).buttonId == "Heads" && number <= 0.50 || buttonInteractionData.at(0).buttonId == "Tails" && number >= 0.50) {
				discordGuildMember.data.currency.wallet -= betAmount;
				discordGuildMember.writeDataToDB();
				discordGuild.data.casinoStats.totalCoinFlipPayout -= betAmount;
				discordGuild.data.casinoStats.totalPayout -= betAmount;
				discordGuild.writeDataToDB();
				newBalance = discordGuildMember.data.currency.wallet;
				std::string completionString = "------\nOWNED FUCK FACE! YOU LOST!\nYour new wallet balance is: " + std::to_string(newBalance) + " " + discordUser.data.currencyName + ".\n------";
				msgEmbed4.setColor("FF0000");
				msgEmbed4.setDescription(completionString);
				msgEmbed4.setTimeStamp(getTimeAndDate());
				msgEmbed4.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed4.setTitle("__**Heads, or Tails?**__");
				RespondToInputEventData dataPackage{ buttonInteractionData.at(0).interactionData };
				dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed4);
				InputEvents::respondToEvent(dataPackage);
			}
			return;
		}
		catch (...) {
			reportException("Coinflip::executeAsync()");
		}
	}
	virtual ~Coinflip() {};

	};
};
#endif