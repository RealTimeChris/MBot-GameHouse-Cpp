// Balance.hpp - Header for the "balance" command.
// May 28, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Balance : public BaseFunction {
	public:
		Balance() {
			this->commandName = "balance";
			this->helpDescription = "Display yours or another server member's currency balance.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /balance to view your own balance, or /balance @USERMENTION to view someone else's balances.\n------");
			msgEmbed.setTitle("__**Balance Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<Balance>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = args.eventData->getChannelId() }).get();

				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm == true) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedGamingInChannel(*args.eventData, discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				std::string userID = "";
				uint32_t bankAmount = 0;
				uint32_t walletAmount = 0;

				 std::regex mentionRegExp("<@!\\d{18,}>");
				 std::regex idRegExp("\\d{18,}");
				if (args.commandData.optionsArgs.size() == 0) {
					userID = args.eventData->getRequesterId();
				}
				else if (args.commandData.optionsArgs.at(0) != "") {
					if (!regex_search(args.commandData.optionsArgs.at(0), mentionRegExp, std::regex_constants::match_flag_type::format_first_only) && !regex_search(args.commandData.optionsArgs.at(0), idRegExp, std::regex_constants::match_flag_type::format_first_only)) {
						std::string msgString = "------\n* *Please, enter a valid user mention, or enter none at all!(!balance = @USERMENTION)**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						RespondToInputEventData dataPackage{ *args.eventData };
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						std::unique_ptr<InputEventData> event = InputEvents::respondToEvent(dataPackage);
						return;
					}
					 std::cmatch matchResults;
					regex_search(args.commandData.optionsArgs.at(0).c_str(), matchResults, idRegExp);
					userID = matchResults.str();
				}

				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = userID,.guildId = args.eventData->getGuildId() }).get();

				if (guildMember.user.id == "") {
					std::string msgString = "------\n**Sorry, but that user could not be found!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User Issue:**__");
					RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event = InputEvents::respondToEvent(dataPackage);
					return;
				}

				DiscordGuildMember discordGuildMember(guildMember);

				std::string msgString = "";
				bankAmount = discordGuildMember.data.currency.bank;
				walletAmount = discordGuildMember.data.currency.wallet;
				auto botUser = args.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				msgString = "<@!" + guildMember.user.id + "> 's balances are:\n------\n__**Bank Balance:**__ " + std::to_string(bankAmount) + " " + discordUser.data.currencyName +
					"\n__**Wallet Balance:**__ " + std::to_string(walletAmount) + " " + discordUser.data.currencyName + "\n------";

				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setDescription(msgString);
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Current Balances:**__");
				RespondToInputEventData dataPackage{ *args.eventData };
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				std::unique_ptr<InputEventData> event = InputEvents::respondToEvent(dataPackage);
			}
			catch (...) {
				reportException("Balance::execute()");
			}			
		}

		virtual ~Balance() {};

	};

}
