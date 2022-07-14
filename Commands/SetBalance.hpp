// SetBalance.hpp - Header for the "set balance" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class SetBalance : public BaseFunction {
	  public:
		SetBalance() {
			this->commandName = "setbalance";
			this->helpDescription = "Sets your own or another server member's currency balances.";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter /setbalance NEWBALANCE, BALANCETYPE, @USERMENTION or to set your own balance it's simply /setbalance = "
									"NEWBALANCE, BALANCETYPE.\n------");
			msgEmbed.setTitle("__**Set Balance Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<SetBalance>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ argsNew.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = argsNew.eventData.getAuthorId(), .guildId = argsNew.eventData.getGuildId() }).get();

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				bool areTheyACommander = doWeHaveAdminPermissions(argsNew, argsNew.eventData, discordGuild, channel, guildMember);

				if (!areTheyACommander) {
					return;
				}

				std::regex balanceRegExp{ "\\d{1,18}" };
				std::regex userMentionRegExp{ "<@!\\d{18}>" };
				std::regex userIDRegExp{ "\\d{18}" };
				uint64_t targetUserID{};

				if (argsNew.optionsArgs.size() == 0 || !regex_search(argsNew.optionsArgs.at(0), balanceRegExp) ||
					std::stoll(argsNew.optionsArgs.at(0)) < 0) {
					std::string msgString = "------\n**Please enter a valid desired balance! (!setbalance = NEWBALANCE, BALANCETYPE, @USERMENTION, or just "
											"!setbalance = NEWBALANCE, BALANCETYPE)**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.optionsArgs.size() < 2 || (argsNew.optionsArgs.at(1) != "bank" && argsNew.optionsArgs.at(1) != "wallet")) {
					std::string msgString = "------\n**Please enter a valid balance type! Bank or Wallet! (!setbalance = NEWBALANCE, BALANCETYPE, "
											"@USERMENTION, or just !setbalance = NEWBALANCE, BALANCETYPE)**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.optionsArgs.size() < 3) {
					targetUserID = argsNew.eventData.getAuthorId();
				} else if (argsNew.optionsArgs.size() == 3 && !regex_search(argsNew.optionsArgs.at(2), userMentionRegExp) &&
					!regex_search(argsNew.optionsArgs.at(2), userIDRegExp)) {
					std::string msgString = "------\n**Please enter a valid target user mention, or leave it blank to select yourself as the target! "
											"(!setbalance = NEWBALANCE, BALANCETYPE, @USERMENTION, or just "
											"!setbalance = NEWBALANCE, BALANCETYPE)**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				} else if (argsNew.optionsArgs.at(2) != "") {
					std::cmatch matchResults;
					regex_search(argsNew.optionsArgs.at(2).c_str(), matchResults, userIDRegExp);
					std::string targetUserIDOne = matchResults.str();
					targetUserID = stoull(targetUserIDOne);
				}

				uint32_t targetUserBalance = ( uint32_t )std::stoll(argsNew.optionsArgs.at(0));
				std::string balanceType = argsNew.optionsArgs.at(1);

				GuildMember targetMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = targetUserID, .guildId = argsNew.eventData.getGuildId() }).get();

				if (targetMember.userName == "") {
					std::string msgString = "------\n**Sorry, but the specified user could not be found!**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User Issue:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				DiscordGuildMember discordGuildMember(targetMember);

				std::string msgString;
				auto botUser = argsNew.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				if (balanceType == "bank") {
					discordGuildMember.data.currency.bank = targetUserBalance;
					discordGuildMember.writeDataToDB();

					uint32_t newBalance = discordGuildMember.data.currency.bank;

					msgString =
						"__You've set the user <@!" + std::to_string(targetUserID) + "> 's bank balance to:__ " + std::to_string(newBalance) + " " + discordUser.data.currencyName;
				} else if (balanceType == "wallet") {
					discordGuildMember.data.currency.wallet = targetUserBalance;
					discordGuildMember.writeDataToDB();

					uint32_t newBalance = discordGuildMember.data.currency.wallet;

					msgString =
						"__You've set the user <@!" + std::to_string(targetUserID) + ">'s wallet balance to:__ " + std::to_string(newBalance) + " " + discordUser.data.currencyName;
				}

				EmbedData msgEmbed{};
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Set New Balance:**__");
				RespondToInputEventData dataPackage{ argsNew.eventData };
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addContent("<@!" + std::to_string(targetUserID) + ">");
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (...) {
				reportException("SetBalance::execute");
			}
		}
		~SetBalance(){};
	};
}
