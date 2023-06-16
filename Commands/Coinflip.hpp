// Coinflip.hpp - Header for the "coinflip" game/command.
// may 29, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"
#include <regex>

namespace discord_core_api {

	class coinflip : public base_function {
	  public:
		coinflip() {
			this->commandName	  = "coinflip";
			this->helpDescription = "gamble some currency on a coinflip!";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /coinflip betamount.\n------");
			msgEmbed.setTitle("__**Coinflip usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<coinflip>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_core_api::unique_ptr<discord_guild> discordGuild{ discord_core_api::makeUnique<discord_guild>(managerAgent, guild) };
				bool areWeAllowed{ checkIfAllowedGamingInChannel(argsNew.getInputEventData(), *discordGuild) };

				if (areWeAllowed == false) {
					return;
				}
				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				guild_member_data botMember{ guild_members::getCachedGuildMember({ .guildMemberId = discord_core_client::getInstance()->getBotUser().id, .guildId = guild.id }) };
				if (!permissions{ botMember.permissions }.checkForPermission(botMember, channel, permission::Manage_Messages)) {
					jsonifier::string msgString = "------\n**I need the manage messages permission in this channel, for this game!**\n------";
					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setColor(discordGuild->data.borderColor);
					msgEmbed->setDescription(msgString);
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Permissions issue:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				std::regex betAmountRegExp{ "\\d{1,18}" };
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				if (argsNew.getCommandArguments().values["betamount"].operator std::streamoff() <= 0) {
					jsonifier::string msgString = "------\n**Please enter a valid amount to bet! 1 " + discordUser.data.currencyName + " or more! (!coinflip = betamount)**\n------";
					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setColor(discordGuild->data.borderColor);
					msgEmbed->setDescription(msgString);
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				discord_guild_member discordGuildMember(managerAgent, guildMember);
				int32_t betAmount	   = argsNew.getCommandArguments().values["betamount"].operator std::streamoff();
				int32_t currencyAmount = discordGuildMember.data.currency.wallet;
				if (betAmount > currencyAmount) {
					jsonifier::string msgString = "------\n**sorry, but you have insufficient funds in your wallet to place that wager!**\n------";
					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setColor(discordGuild->data.borderColor);
					msgEmbed->setDescription(msgString);
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Insufficient funds:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				jsonifier::string newBetString =
					"welcome, <@!" + guildMember.user.id + "> , you have placed a bet of **" + jsonifier::toString(betAmount) + " " + discordUser.data.currencyName + "**.\n";
				newBetString += "react with :exploding_head: to choose heads, or with :snake: to choose tails!";
				discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
				msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed->setColor("0000FF");
				msgEmbed->setDescription(newBetString);
				msgEmbed->setTimeStamp(getTimeAndDate());
				msgEmbed->setTitle("__**Heads, or tails!?**__");
				discord_core_api::unique_ptr<input_event_data> inputEvent = discord_core_api::makeUnique<input_event_data>(argsNew.getInputEventData());
				respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
				dataPackage.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage.addButton(false, "heads", "heads", button_style::Success, "🤯");
				dataPackage.addButton(false, "tails", "tails", button_style::Success, "🐍");
				dataPackage.addMessageEmbed(*msgEmbed);
				input_event_data inputData = input_events::respondToInputEventAsync(dataPackage).get();
				discord_core_api::unique_ptr<button_collector> button2{ discord_core_api::makeUnique<button_collector>(inputData) };
				auto createResponseData = discord_core_api::makeUnique<create_interaction_response_data>();
				auto embedData			= discord_core_api::makeUnique<embed_data>();
				embedData->setColor("fefefe");
				embedData->setTitle("__**Permissions issue:**__");
				embedData->setTimeStamp(getTimeAndDate());
				embedData->setDescription("sorry, but that button can only be pressed by <@" + argsNew.getUserData().id + ">!");
				createResponseData->addMessageEmbed(*embedData);
				createResponseData->setResponseType(interaction_callback_type::Channel_Message_With_Source);
				createResponseData->setFlags(64);
				jsonifier::vector<button_response_data> buttonInteractionData = button2->collectButtonData(false, 120000, 1, *createResponseData, argsNew.getUserData().id).get();
				if (buttonInteractionData.at(0).buttonId == "empty") {
					jsonifier::string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
					embed_data msgEmbed2;
					msgEmbed2.setColor("ff0000");
					msgEmbed2.setTimeStamp(getTimeAndDate());
					msgEmbed2.setTitle("__**Heads, or tails?:**__");
					msgEmbed2.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed2.setDescription(timeOutString);
					respond_to_input_event_data dataPackage02{ *buttonInteractionData.at(0).interactionData };
					dataPackage02.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed2);
					input_events::respondToInputEventAsync(dataPackage02).get();
					return;
				}
				std::mt19937_64 randomEngine{ static_cast<uint64_t>(
					std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
				float number		= static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max());
				uint32_t newBalance = 0;

				discordGuildMember.getDataFromDB(managerAgent);
				currencyAmount = discordGuildMember.data.currency.wallet;

				if (betAmount > currencyAmount) {
					jsonifier::string completionString = "------\nSorry, but you have insufficient funds in your wallet to place that wager.\n------";
					embed_data msgEmbed3;
					msgEmbed3.setColor("ff0000");
					msgEmbed3.setDescription(completionString);
					msgEmbed3.setTimeStamp(getTimeAndDate());
					msgEmbed3.setTitle("__**Heads, or tails**__");
					msgEmbed3.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					respond_to_input_event_data dataPackage02{ inputData };
					dataPackage02.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed3);
					input_events::respondToInputEventAsync(dataPackage02).get();
					return;
				}

				embed_data msgEmbed4;
				if (buttonInteractionData.at(0).buttonId == "heads" && number > 0.50 || buttonInteractionData.at(0).buttonId == "tails" && number < 0.50) {
					discordGuildMember.data.currency.wallet += betAmount;
					discordGuildMember.writeDataToDB(managerAgent);
					discordGuild->data.casinoStats.totalCoinFlipPayout += betAmount;
					discordGuild->data.casinoStats.totalPayout += betAmount;
					if (betAmount > discordGuild->data.casinoStats.largestCoinFlipPayout.amount) {
						discordGuild->data.casinoStats.largestCoinFlipPayout.amount	   = betAmount;
						discordGuild->data.casinoStats.largestCoinFlipPayout.timeStamp = getTimeAndDate();
						discordGuild->data.casinoStats.largestCoinFlipPayout.userId	   = argsNew.getUserData().id;
						discordGuild->data.casinoStats.largestCoinFlipPayout.userName  = argsNew.getUserData().userName;
						discordGuild->writeDataToDB(managerAgent);
					}
					newBalance = discordGuildMember.data.currency.wallet;
					jsonifier::string completionString =
						"------\nNICELY done faggot! you won!\nYour new wallet balance is: " + jsonifier::toString(newBalance) + " " + discordUser.data.currencyName + ".\n------";
					msgEmbed4.setColor("00FF00");
					msgEmbed4.setDescription(completionString);
					msgEmbed4.setTimeStamp(getTimeAndDate());
					msgEmbed4.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed4.setTitle("__**Heads, or tails?**__");
					respond_to_input_event_data dataPackage02{ *buttonInteractionData.at(0).interactionData };
					dataPackage02.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed4);
					input_events::respondToInputEventAsync(dataPackage02).get();
				} else if (buttonInteractionData.at(0).buttonId == "heads" && number <= 0.50 || buttonInteractionData.at(0).buttonId == "tails" && number >= 0.50) {
					discordGuildMember.data.currency.wallet -= betAmount;
					discordGuildMember.writeDataToDB(managerAgent);
					discordGuild->data.casinoStats.totalCoinFlipPayout -= betAmount;
					discordGuild->data.casinoStats.totalPayout -= betAmount;
					discordGuild->writeDataToDB(managerAgent);
					newBalance = discordGuildMember.data.currency.wallet;
					jsonifier::string completionString =
						"------\nOWNED fuck face! you lost!\nYour new wallet balance is: " + jsonifier::toString(newBalance) + " " + discordUser.data.currencyName + ".\n------";
					msgEmbed4.setColor("ff0000");
					msgEmbed4.setDescription(completionString);
					msgEmbed4.setTimeStamp(getTimeAndDate());
					msgEmbed4.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed4.setTitle("__**Heads, or tails?**__");
					respond_to_input_event_data dataPackage02{ *buttonInteractionData.at(0).interactionData };
					dataPackage02.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage02.addMessageEmbed(msgEmbed4);
					input_events::respondToInputEventAsync(dataPackage02).get();
				}
				return;
			} catch (const std::exception& error) {
				std::cout << "coinflip::execute()" << error.what() << std::endl;
			}
		}
		~coinflip(){};
	};
};// namespace discord_core_api
