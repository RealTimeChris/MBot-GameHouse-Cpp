// Deposit.hpp - Header for the "Deposit" command.
// jun 21, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class deposit : public base_function {
	  public:
		deposit() {
			this->commandName	  = "deposit";
			this->helpDescription = "deposit some currency to your bank account for safe keeping.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /deposit depositamount.\n------");
			msgEmbed.setTitle("__**Deposit usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<deposit>();
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

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				discord_guild_member discordGuildMember(managerAgent, guildMember);

				std::regex depositAmountRegExp{ "\\d{1,18}" };
				std::cmatch matchResults;
				uint32_t depositAmount = 0;
				if (argsNew.getCommandArguments().values.size() == 0 || argsNew.getCommandArguments().values["amount"].operator jsonifier::string() == "all") {
					depositAmount = discordGuildMember.data.currency.wallet;
				} else if (argsNew.getCommandArguments().values.size() == 0 || argsNew.getCommandArguments().values["amount"].operator jsonifier::string() == "" ||
					argsNew.getCommandArguments().values["amount"].operator std::streamoff() <= 0) {
					jsonifier::string msgString = "------\n**Please enter a valid deposit amount!(!deposit = amount)**\n------";
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
					depositAmount = argsNew.getCommandArguments().values["amount"].operator std::streamoff();
				}

				if (depositAmount > discordGuildMember.data.currency.wallet) {
					jsonifier::string msgString = "------\n**sorry, but you do not have sufficient funds to deposit that much!**\n------";
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
				}
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				uint32_t msPerSecond	   = 1000;
				uint32_t seconds_per_minute  = 60;
				uint32_t msPerMinute	   = msPerSecond * seconds_per_minute;
				uint32_t minutes_per_hour	   = 60;
				uint32_t msPerHour		   = msPerMinute * minutes_per_hour;
				uint32_t msPerDepositCycle = msPerHour * discordUser.data.hoursOfDepositCooldown;
				uint32_t currentTime =
					static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

				uint32_t timeSinceLastDeposit = currentTime - discordGuildMember.data.currency.timeOfLastDeposit;

				jsonifier::string msgString = "";
				if (timeSinceLastDeposit >= msPerDepositCycle) {
					discordGuildMember.data.currency.bank += depositAmount;
					discordGuildMember.data.currency.wallet -= depositAmount;
					discordGuildMember.data.currency.timeOfLastDeposit = currentTime;
					discordGuildMember.writeDataToDB(managerAgent);

					msgString = "congratulations! you've deposited " + jsonifier::toString(depositAmount) + " " + discordUser.data.currencyName +
								" from your wallet into your bank!\n------\n__**Your new balances are:**__\n__Bank:__ " + jsonifier::toString(discordGuildMember.data.currency.bank) +
								" ";
					msgString += discordUser.data.currencyName + "\n" + "__Wallet:__ " + jsonifier::toString(discordGuildMember.data.currency.wallet) + " " +
								 discordUser.data.currencyName + "\n------";
				} else {
					uint32_t timeRemaining = msPerDepositCycle - timeSinceLastDeposit;
					uint32_t hoursRemain   = (uint32_t)trunc(timeRemaining / msPerHour);
					uint32_t minutesRemain = (uint32_t)trunc((timeRemaining % msPerHour) / msPerMinute);
					uint32_t secondsRemain = (uint32_t)trunc(((timeRemaining % msPerHour) % msPerMinute) / msPerSecond);

					if (hoursRemain > 0) {
						msgString = "sorry, but you need to wait " + jsonifier::toString(hoursRemain) + " hours, " + jsonifier::toString(minutesRemain) + " minutes, and " +
									jsonifier::toString(secondsRemain) + " seconds before you can make another deposit!";
					} else if (minutesRemain > 0) {
						msgString = "sorry, but you need to wait " + jsonifier::toString(minutesRemain) + " minutes, and " + jsonifier::toString(secondsRemain) +
									" seconds before you can make another deposit!";
					} else {
						msgString = "sorry, but you need to wait " + jsonifier::toString(secondsRemain) + " seconds before you can make another deposit!";
					}
				}

				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setColor("fefefe");
				msgEmbed.setTitle("__**Bank deposit:**__");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setDescription(msgString);
				respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
				dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const std::exception& error) {
				std::cout << "deposit::execute()" << error.what() << std::endl;
			}
		}
		~deposit(){};
	};
}// namespace discord_core_api
