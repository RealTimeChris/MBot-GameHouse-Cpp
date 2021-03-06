// Withdraw.hpp - Header for the "withdraw" command.
// Jun 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class Withdraw : public BaseFunction {
	  public:
		Withdraw() {
			this->commandName = "withdraw";
			this->helpDescription = "Withdraws currency from your bank account to your wallet.";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter /withdraw AMOUNT.\n------");
			msgEmbed.setTitle("__**Withdraw Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Withdraw>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ argsNew.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				uint32_t withdrawAmount = 0;

				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({
																					  .guildMemberId = argsNew.eventData.getAuthorId(),
																					  .guildId = argsNew.eventData.getGuildId(),
																				  })
											  .get();
				DiscordGuildMember discordGuildMember(guildMember);

				std::regex amountRegExp{ "\\d{1,18}" };
				if (argsNew.optionsArgs.size() == 0 || !regex_search(argsNew.optionsArgs[0], amountRegExp) || std::stoll(argsNew.optionsArgs[0]) <= 0) {
					std::string msgString = "------\n**Please enter a valid withdrawl amount! (!withdraw = AMOUNT)**\n------";
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
				} else {
					std::cmatch matchResults;
					regex_search(argsNew.optionsArgs[0].c_str(), matchResults, amountRegExp);
					withdrawAmount = ( uint32_t )std::stoll(matchResults.str());
				}

				if (withdrawAmount > discordGuildMember.data.currency.bank) {
					std::string msgString = "-------\n**Sorry, but you do not have sufficient funds to withdraw that much!**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				discordGuildMember.data.currency.wallet += withdrawAmount;
				discordGuildMember.data.currency.bank -= withdrawAmount;
				discordGuildMember.writeDataToDB();
				auto botUser = argsNew.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				std::string msgString = "Congratulations! You've withdrawn " + std::to_string(withdrawAmount) + " " + discordUser.data.currencyName +
					" from your bank account to your wallet!\n------\n__**Your new balances are:**__\n" + "__Bank:__ " + std::to_string(discordGuildMember.data.currency.bank) +
					" " + discordUser.data.currencyName + "\n" + "__Wallet:__ " + std::to_string(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName +
					"\n------";

				EmbedData msgEmbed{};
				msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Bank Withdrawal:**__");
				RespondToInputEventData dataPackage{ argsNew.eventData };
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (...) {
				reportException("Withdraw::execute()");
			}
		}
		~Withdraw(){};
	};
}
