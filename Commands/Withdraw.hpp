// Withdraw.hpp - Header for the "withdraw" command.
// jun 29, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {
	class withdraw : public base_function {
	  public:
		withdraw() {
			this->commandName	  = "withdraw";
			this->helpDescription = "withdraws currency from your bank account to your wallet.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /withdraw amount.\n------");
			msgEmbed.setTitle("__**Withdraw usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<withdraw>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (!areWeAllowed) {
					return;
				}

				uint32_t withdrawAmount = 0;

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				discord_guild_member discordGuildMember(managerAgent, guildMember);

				std::regex amountRegExp{ "\\d{1,18}" };
				if (argsNew.getCommandArguments().values.size() == 0 ||
					!regex_search(argsNew.getCommandArguments().values["amount"].operator jsonifier::string().data(), amountRegExp) ||
					argsNew.getCommandArguments().values["amount"].operator std::streamoff() <= 0) {
					jsonifier::string msgString = "------\n**Please enter a valid withdrawl amount! (!withdraw = amount)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				} else {
					withdrawAmount = ( uint32_t )argsNew.getCommandArguments().values["amount"].operator std::streamoff();
				}

				if (withdrawAmount > discordGuildMember.data.currency.bank) {
					jsonifier::string msgString = "-------\n**sorry, but you do not have sufficient funds to withdraw that much!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient funds:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				discordGuildMember.data.currency.wallet += withdrawAmount;
				discordGuildMember.data.currency.bank -= withdrawAmount;
				discordGuildMember.writeDataToDB(managerAgent);
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				jsonifier::string msgString = "congratulations! you've withdrawn " + jsonifier::toString(withdrawAmount) + " " + discordUser.data.currencyName +
										" from your bank account to your wallet!\n------\n__**Your new balances are:**__\n" + "__Bank:__ " +
										jsonifier::toString(discordGuildMember.data.currency.bank) + " " + discordUser.data.currencyName + "\n" + "__Wallet:__ " +
										jsonifier::toString(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Bank withdrawal:**__");
				respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
				dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const std::exception& error) {
				std::cout << "withdraw::execute()" << error.what() << std::endl;
			}
		}
		~withdraw(){};
	};
}// namespace discord_core_api
