// SetBalance.hpp - Header for the "set balance" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SET_BALANCE_
#define _SET_BALANCE_

#include "Index.hpp"
#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class SetBalance:public BaseFunction{
	public:
		SetBalance() {
			this->commandName = "setbalance";
			this->helpDescription = "Sets your own or another server member's currency balances.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /setbalance NEWBALANCE, BALANCETYPE, @USERMENTION or to set your own balance it's simply /setbalance = NEWBALANCE, BALANCETYPE.\n------");
			msgEmbed.setTitle("__**Set Balance Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<SetBalance>();
		}

		virtual void execute( std::unique_ptr<BaseFunctionArguments> args) {
			Channel channel = Channels::getCachedChannelAsync({ args->eventData->getChannelId() }).get();
			bool areWeInADm = areWeInADM(*args->eventData, channel);

			if (areWeInADm == true ) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args->eventData)).get();

			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args->eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args->eventData->getAuthorId(),.guildId = args->eventData->getGuildId() }).get();
			
			bool areWeAllowed = checkIfAllowedGamingInChannel(*args->eventData, discordGuild);

			if(!areWeAllowed){
				return;
			}

			bool areTheyACommander = doWeHaveAdminPermissions(*args, *args->eventData, discordGuild, channel, guildMember);

			if (!areTheyACommander) {
				return;
			}

			std::regex balanceRegExp("\\d{1,18}");
			std::regex userMentionRegExp("<@!\\d{18}>");
			std::regex userIDRegExp("\\d{18}");
			std::string targetUserID;

			if (args->commandData.optionsArgs.size() == 0|| !regex_search(args->commandData.optionsArgs.at(0),balanceRegExp) ||  std::stoll(args->commandData.optionsArgs.at(0)) < 0) {
				std::string msgString = "------\n**Please enter a valid desired balance! (!setbalance = NEWBALANCE, BALANCETYPE, @USERMENTION, or just !setbalance = NEWBALANCE, BALANCETYPE)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				RespondToInputEventData dataPackage{ *args->eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			if (args->commandData.optionsArgs.size() < 2|| (args->commandData.optionsArgs.at(1) !=  "bank" && args->commandData.optionsArgs.at(1) !=  "wallet")) {
				std::string msgString = "------\n**Please enter a valid balance type! Bank or Wallet! (!setbalance = NEWBALANCE, BALANCETYPE, @USERMENTION, or just !setbalance = NEWBALANCE, BALANCETYPE)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				RespondToInputEventData dataPackage{ *args->eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			if (args->commandData.optionsArgs.size() < 3) {
				targetUserID = args->eventData->getRequesterId();
			}
			else if (args->commandData.optionsArgs.size() == 3 && !regex_search(args->commandData.optionsArgs.at(2), userMentionRegExp) && !regex_search(args->commandData.optionsArgs.at(2), userIDRegExp)) {
				std::string msgString = "------\n**Please enter a valid target user mention, or leave it blank to select yourself as the target! (!setbalance = NEWBALANCE, BALANCETYPE, @USERMENTION, or just !setbalance = NEWBALANCE, BALANCETYPE)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				RespondToInputEventData dataPackage{ *args->eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			else if (args->commandData.optionsArgs.at(2) != "") {
				 std::cmatch matchResults;
				regex_search(args->commandData.optionsArgs.at(2).c_str(), matchResults, userIDRegExp);
				std::string targetUserIDOne = matchResults.str();
				targetUserID = targetUserIDOne;
			}

			uint32_t targetUserBalance = (uint32_t) std::stoll(args->commandData.optionsArgs.at(0));
			std::string balanceType = args->commandData.optionsArgs.at(1);

			GuildMember targetMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = targetUserID,.guildId = args->eventData->getGuildId() }).get();

			if (targetMember.user.userName == "") {
				std::string msgString = "------\n**Sorry, but the specified user could not be found!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**User Issue:**__");
				RespondToInputEventData dataPackage{ *args->eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}

			DiscordGuildMember discordGuildMember(targetMember);

			std::string msgString;
			auto botUser = args->discordCoreClient->getBotUser();
			DiscordUser discordUser(botUser.userName, botUser.id);
			if (balanceType == "bank") {
				discordGuildMember.data.currency.bank = targetUserBalance;
				discordGuildMember.writeDataToDB();

				uint32_t newBalance = discordGuildMember.data.currency.bank;

				msgString = "__You've set the user <@!" + targetUserID + "> 's bank balance to:__ " + std::to_string(newBalance) + " " + discordUser.data.currencyName;
			}
			else if (balanceType == "wallet") {
				discordGuildMember.data.currency.wallet = targetUserBalance;
				discordGuildMember.writeDataToDB();

				uint32_t newBalance = discordGuildMember.data.currency.wallet;

				msgString = "__You've set the user <@!" + targetUserID + ">'s wallet balance to:__ " + std::to_string(newBalance) + " " + discordUser.data.currencyName;
			}

			EmbedData messageEmbed;
			messageEmbed.setTimeStamp(getTimeAndDate());
			messageEmbed.setAuthor(args->eventData->getUserName(), args->eventData->getAvatarUrl());
			messageEmbed.setColor(discordGuild.data.borderColor);
			messageEmbed.setDescription(msgString);
			messageEmbed.setTimeStamp(getTimeAndDate());
			messageEmbed.setTitle("__**Set New Balance:**__");
			RespondToInputEventData dataPackage{ *args->eventData };
			dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
			dataPackage.addContent("<@!" + targetUserID + ">");
			dataPackage.addMessageEmbed(messageEmbed);
			auto newEvent = InputEvents::respondToEvent(dataPackage);
			return;
		}
		virtual ~SetBalance() {};
	};
}
#endif