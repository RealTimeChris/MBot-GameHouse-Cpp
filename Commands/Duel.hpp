// Duel.hpp - Header for the "duel" command.
// jun 22, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	void executeCheck(const base_function_arguments argsNew, discord_guild_member* discordFromGuildMember, discord_guild_member* discordToGuildMember, discord_guild* discordGuild,
		input_event_data newEvent, int32_t* betAmount, respond_to_input_event_data dataPackageNew, jsonifier::string_view msgEmbedString, jsonifier::string_view fromUserIDNew, jsonifier::string_view toUserIDNew) {
		discordFromGuildMember->getDataFromDB(managerAgent);
		int32_t fromUserCurrency = discordFromGuildMember->data.currency.wallet;
		discordToGuildMember->getDataFromDB(managerAgent);
		int32_t toUserCurrency = discordToGuildMember->data.currency.wallet;
		user_data currentUser   = users::getUserAsync({ .userId = newEvent.getUserData().id }).get();

		if (*betAmount > fromUserCurrency) {
			jsonifier::string msgString;
			msgString = msgEmbedString + "\n\n__**Sorry, but you have insufficient funds in your wallet for placing that wager!**__";

			embed_data messageEmbed3{};
			messageEmbed3.setAuthor(currentUser.userName, currentUser.getUserImageUrl(user_image_types::Avatar));
			messageEmbed3.setDescription(msgString);
			messageEmbed3.setTimeStamp(getTimeAndDate());
			messageEmbed3.setColor(discordGuild->data.borderColor);
			messageEmbed3.setTitle("__**Insufficient funds:**__");
			respond_to_input_event_data dataPackage(newEvent);
			dataPackage.setResponseType(input_event_response_type::Deferred_Response);
			auto newEvent02 = input_events::respondToInputEventAsync(dataPackage).get();
			respond_to_input_event_data dataPackage2(newEvent02);
			dataPackage2.setResponseType(input_event_response_type::Follow_Up_Message);
			dataPackageNew.addMessageEmbed(messageEmbed3);
			auto newEvent03 = input_events::respondToInputEventAsync(dataPackageNew).get();
			input_events::deleteInputEventResponseAsync(newEvent03, 20000);
			return;
		}
		if (*betAmount > toUserCurrency) {
			jsonifier::string msgString;
			msgString += msgEmbedString + "\n\n__**Sorry, but they have insufficient funds in their wallet for accepting that wager!**__";
			embed_data messageEmbed4{};
			messageEmbed4.setAuthor(currentUser.userName, currentUser.getUserImageUrl(user_image_types::Avatar));
			messageEmbed4.setDescription(msgString);
			messageEmbed4.setTimeStamp(getTimeAndDate());
			messageEmbed4.setColor(discordGuild->data.borderColor);
			messageEmbed4.setTitle("__**Insufficient funds:**__");
			respond_to_input_event_data dataPackage(newEvent);
			dataPackage.setResponseType(input_event_response_type::Deferred_Response);
			auto newEvent02 = input_events::respondToInputEventAsync(dataPackage).get();
			respond_to_input_event_data dataPackage02(newEvent02);
			dataPackage02.setResponseType(input_event_response_type::Follow_Up_Message);
			dataPackageNew.addMessageEmbed(messageEmbed4);
			auto newEvent03 = input_events::respondToInputEventAsync(dataPackageNew).get();
			input_events::deleteInputEventResponseAsync(newEvent03, 20000);
			return;
		}

		std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };

		int32_t fromUserRoll = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 100.0f);
		int32_t toUserRoll	 = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 100.0f);
		jsonifier::vector<embed_data> messageEmbeds{};
		jsonifier::vector<jsonifier::string> finalStrings;
		jsonifier::vector<jsonifier::string> fromUserGainStrings;
		jsonifier::vector<jsonifier::string> fromUserLossStrings;
		jsonifier::vector<jsonifier::string> toUserGainStrings;
		jsonifier::vector<jsonifier::string> toUserLossStrings;
		int32_t fromUserSelfMod	  = 0;
		int32_t fromUserOppMod	  = 0;
		int32_t toUserSelfMod	  = 0;
		int32_t toUserOppMod	  = 0;
		int32_t finalFromUserRoll = 0;
		int32_t finalToUserRoll	  = 0;

		for (auto& value : discordFromGuildMember->data.items) {
			if (value.selfMod > 0) {
				jsonifier::string currentString = "+" + jsonifier::toString(value.selfMod);
				currentString += " of base roll from <@!" + fromUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
				fromUserGainStrings.emplace_back(currentString);
				fromUserSelfMod += value.selfMod;
			}
			if (value.oppMod < 0) {
				jsonifier::string currentString = jsonifier::toString(value.oppMod);
				currentString += " of base roll from <@!" + fromUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
				toUserLossStrings.emplace_back(currentString);
				toUserOppMod += value.oppMod;
			}
		}

		for (inventory_item value : discordToGuildMember->data.items) {
			if (value.selfMod > 0) {
				jsonifier::string currentString = "+" + jsonifier::toString(value.selfMod);
				currentString += " of base roll from <@!" + toUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
				toUserGainStrings.emplace_back(currentString);
				toUserSelfMod += value.selfMod;
			}
			if (value.oppMod < 0) {
				jsonifier::string currentString = jsonifier::toString(value.oppMod);
				currentString += " of base roll from <@!" + toUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
				fromUserLossStrings.emplace_back(currentString);
				fromUserOppMod += value.oppMod;
			}
		}

		finalFromUserRoll = (int32_t)applyAsymptoticTransform((float)(fromUserRoll + fromUserSelfMod + fromUserOppMod), 2000, 100);
		finalToUserRoll	  = (int32_t)applyAsymptoticTransform((float)(toUserRoll + toUserSelfMod + toUserOppMod), 2000, 100);

		jsonifier::string fromUserFooterString;
		if (finalFromUserRoll != fromUserRoll || fromUserOppMod != 0 || fromUserSelfMod != 0) {
			fromUserFooterString = "__**For a final roll of:**__ " + jsonifier::toString(finalFromUserRoll) + "\n";
		}

		jsonifier::string toUserFooterString;
		if (finalToUserRoll != toUserRoll || toUserOppMod != 0 || toUserSelfMod != 0) {
			toUserFooterString = "__**For a final roll of:**__ " + jsonifier::toString(finalToUserRoll) + "\n";
		}

		if (finalFromUserRoll > finalToUserRoll) {
			discordFromGuildMember->data.currency.wallet += *betAmount;
			discordFromGuildMember->writeDataToDB(managerAgent);
			discordToGuildMember->getDataFromDB(managerAgent);
			discordToGuildMember->data.currency.wallet -= *betAmount;
			discordToGuildMember->writeDataToDB(managerAgent);

			uint32_t currentPage = 0;

			jsonifier::string fromUserVicHeaderString;
			fromUserVicHeaderString = "<@!" + fromUserIDNew + "> has defeated <@!" + toUserIDNew + ">!\n__Your rolls were__:\n";

			finalStrings.resize(1);
			finalStrings[currentPage] += fromUserVicHeaderString;

			auto midFooter1 = "__**<@!" + fromUserIDNew + ">:**__ " + jsonifier::toString(fromUserRoll) + "\n";
			auto midFooter2 = "__**<@!" + toUserIDNew + ">:**__ " + jsonifier::toString(toUserRoll) + "\n";

			finalStrings[currentPage] += midFooter1;

			jsonifier::string finalFooterString;
			finalFooterString = "------\n__Your new wallet balances are:__\n";
			auto botUser	  = discord_core_client::getInstance()->getBotUser();
			jsonifier::string theString{ botUser.userName };
			discord_user discordUser(managerAgent, theString, botUser.id);
			finalFooterString += "<@!" + fromUserIDNew + ">: " + jsonifier::toString(discordFromGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n" +
								 "<@!" + toUserIDNew + ">: " + jsonifier::toString(discordToGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

			jsonifier::vector<jsonifier::string> fromUserModStrings{};
			for (jsonifier::string& value : fromUserLossStrings) {
				fromUserGainStrings.emplace_back(value);
			}
			fromUserModStrings = fromUserGainStrings;

			for (jsonifier::string& value : fromUserModStrings) {
				if ((finalStrings[currentPage].size() + value.size() + midFooter1.size() + fromUserFooterString.size()) >= 2048) {
					finalStrings.resize(finalStrings.size() + 1);
					currentPage += 1;
					finalStrings[currentPage] = fromUserVicHeaderString;
				}
				finalStrings[currentPage] += value;
				if (value == fromUserModStrings.at(fromUserModStrings.size() - 1)) {
					finalStrings[currentPage] += fromUserFooterString;
				}
			}

			finalStrings[currentPage] += midFooter2;

			jsonifier::vector<jsonifier::string> toUserModStrings;
			for (jsonifier::string& value : toUserLossStrings) {
				toUserGainStrings.emplace_back(value);
			}
			toUserModStrings = toUserGainStrings;

			for (jsonifier::string& value : toUserModStrings) {
				if ((finalStrings[currentPage].size() + value.size() + midFooter1.size() + toUserFooterString.size()) >= 2048) {
					finalStrings.resize(finalStrings.size() + 1);
					currentPage += 1;
					finalStrings[currentPage] = fromUserVicHeaderString;
				}
				finalStrings[currentPage] += value;
				if (value == toUserModStrings.at(toUserModStrings.size() - 1)) {
					finalStrings[currentPage] += toUserFooterString;
				}
			}
			finalStrings[currentPage] += finalFooterString;

			messageEmbeds.resize((int64_t)currentPage + (int64_t)1);
			for (uint32_t x = 0; x < finalStrings.size(); x += 1) {
				messageEmbeds[x] = embed_data();
				messageEmbeds[x].setAuthor(currentUser.userName, currentUser.getUserImageUrl(user_image_types::Avatar));
				messageEmbeds[x].setColor("00FE00");
				messageEmbeds[x].setTimeStamp(getTimeAndDate());
				messageEmbeds[x].setTitle("__**Duel results: " + jsonifier::toString(x + 1) + " of " + jsonifier::toString(finalStrings.size()) + "**__");
				messageEmbeds[x].setDescription(finalStrings[x]);
			}
		} else if (finalToUserRoll > finalFromUserRoll) {
			discordToGuildMember->data.currency.wallet += *betAmount;
			discordToGuildMember->writeDataToDB(managerAgent);
			discordFromGuildMember->getDataFromDB(managerAgent);
			discordFromGuildMember->data.currency.wallet -= *betAmount;
			discordFromGuildMember->writeDataToDB(managerAgent);

			uint32_t currentPage = 0;

			jsonifier::string toUserVicHeaderString;
			toUserVicHeaderString = "<@!" + toUserIDNew + "> has defeated <@!" + fromUserIDNew + "> !!\n__Your rolls were__:\n";

			finalStrings.resize(1);
			finalStrings[currentPage] += toUserVicHeaderString;

			jsonifier::string midFooter1 = "__**<@!" + toUserIDNew + ">:**__ " + jsonifier::toString(toUserRoll) + "\n";
			jsonifier::string midFooter2 = "__**<@!" + fromUserIDNew + ">:**__ " + jsonifier::toString(fromUserRoll) + "\n";

			finalStrings[currentPage] += midFooter1;

			auto botUser = discord_core_client::getInstance()->getBotUser();
			jsonifier::string theString{ botUser.userName };
			discord_user discordUser(managerAgent, theString, botUser.id);

			jsonifier::string finalFooterString = "-----\n__Your new wallet balances are: __\n<@!" + toUserIDNew + ">: " + jsonifier::toString(discordToGuildMember->data.currency.wallet) +
											" " + discordUser.data.currencyName + "\n<@!" + fromUserIDNew + ">: " + jsonifier::toString(discordFromGuildMember->data.currency.wallet) +
											" " + discordUser.data.currencyName + "\n------";

			jsonifier::vector<jsonifier::string> toUserModStrings;
			for (auto& value : toUserLossStrings) {
				toUserGainStrings.emplace_back(value);
			}
			toUserModStrings = toUserGainStrings;

			for (auto& value : toUserModStrings) {
				if ((finalStrings[currentPage].size() + value.size() + midFooter1.size() + toUserFooterString.size()) >= 2048) {
					finalStrings.resize(finalStrings.size() + 1);
					currentPage += 1;
					finalStrings[currentPage] = toUserVicHeaderString;
				}
				finalStrings[currentPage] += value;
				if (value == toUserModStrings.at(toUserModStrings.size() - 1)) {
					finalStrings[currentPage] += toUserFooterString;
				}
			}

			finalStrings[currentPage] += midFooter2;

			jsonifier::vector<jsonifier::string> fromUserModStrings;
			for (auto& value : fromUserLossStrings) {
				fromUserGainStrings.emplace_back(value);
			}
			fromUserModStrings = fromUserGainStrings;

			for (auto& value : fromUserModStrings) {
				if ((finalStrings[currentPage].size() + value.size() + fromUserFooterString.size()) >= 2048) {
					finalStrings.resize(finalStrings.size() + 1);
					currentPage += 1;
					finalStrings[currentPage] = toUserVicHeaderString;
				}
				finalStrings[currentPage] += value;
				if (value == fromUserModStrings.at(fromUserModStrings.size() - 1)) {
					finalStrings[currentPage] += fromUserFooterString;
				}
			}
			finalStrings[currentPage] += finalFooterString;

			messageEmbeds.resize((int64_t)currentPage + (int64_t)1);
			for (uint32_t x = 0; x < finalStrings.size(); x += 1) {
				messageEmbeds[x] = embed_data();
				messageEmbeds[x].setAuthor(currentUser.userName, currentUser.getUserImageUrl(user_image_types::Avatar));
				messageEmbeds[x].setColor("fe0000");
				messageEmbeds[x].setTimeStamp(getTimeAndDate());
				messageEmbeds[x].setTitle("__**Duel results: " + jsonifier::toString(x + 1) + " of " + jsonifier::toString(finalStrings.size()) + "**__");
				messageEmbeds[x].setDescription(finalStrings[x]);
			}
		} else if (finalToUserRoll == finalFromUserRoll) {
			finalStrings.resize(1);
			messageEmbeds.emplace_back(embed_data());
			finalStrings[0] = "__**Looks like it was a draw! nicely done!**__";
			messageEmbeds[0].setAuthor(currentUser.userName, currentUser.getUserImageUrl(user_image_types::Avatar));
			messageEmbeds[0].setColor("fefefe");
			messageEmbeds[0].setTimeStamp(getTimeAndDate());
			messageEmbeds[0].setTitle("__**Duel results: " + jsonifier::toString(0 + 1) + " of " + jsonifier::toString(finalStrings.size()) + "**__");
			messageEmbeds[0].setDescription(finalStrings[0]);
			dataPackageNew.setResponseType(input_event_response_type::Edit_Interaction_Response);
			dataPackageNew.addMessageEmbed(messageEmbeds[0]);
			newEvent = input_events::respondToInputEventAsync(dataPackageNew).get();
		}
		dataPackageNew.setResponseType(input_event_response_type::Edit_Interaction_Response);
		dataPackageNew.addMessageEmbed(messageEmbeds[0]);
		auto newEvent02 = input_events::respondToInputEventAsync(dataPackageNew).get();

		uint32_t currentPageIndex = 0;
		moveThroughMessagePages(fromUserIDNew.operator jsonifier::string(), newEvent02, currentPageIndex, messageEmbeds, false, 120000, false);
	}

	void executeExit(jsonifier::string fromUserID, jsonifier::string toUserID, discord_guild discordGuild, input_event_data originalEvent) {
		jsonifier::string rejectedString;
		rejectedString		 = "sorry, <@!" + fromUserID + ">, but <@!" + toUserID + "> has rejected your duel offer! (timed out!)";
		user_data currentUser = users::getUserAsync({ originalEvent.getUserData().id }).get();
		embed_data messageEmbed2{};
		messageEmbed2.setAuthor(currentUser.userName, currentUser.getUserImageUrl(user_image_types::Avatar));
		messageEmbed2.setColor("fefefe");
		messageEmbed2.setTimeStamp(getTimeAndDate());
		messageEmbed2.setTitle("__**Duel rejected!**__");
		messageEmbed2.setDescription(rejectedString);
		respond_to_input_event_data dataPackage(originalEvent);
		dataPackage.setResponseType(input_event_response_type::Edit_Follow_Up_Message);
		dataPackage.addMessageEmbed(messageEmbed2);
		input_events::respondToInputEventAsync(dataPackage).get();
	}

	class duel : public base_function {
	  public:
		duel() {
			this->commandName	  = "duel";
			this->helpDescription = "challenge a fellow server member to a duel, for currency.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /duel betamount, @usermentiontargetusermention.\n------");
			msgEmbed.setTitle("__**Duel usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<duel>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };
				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				std::regex numberRegExp{ "\\d{1,18}" };
				std::regex idRegExp{ "\\d{18,20}" };

				if (argsNew.getCommandArguments().values.size() < 2 ||
					argsNew.getCommandArguments().values["betamount"].operator std::streamoff() < 0) {
					jsonifier::string msgString = "------\n**Please enter a valid bet amount! (!duel = betamount, @usermention)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.addMessageEmbed(msgEmbed);
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					auto newEvent02 = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				int32_t betAmount = argsNew.getCommandArguments().values["betamount"].operator std::streamoff();
				snowflake toUserID{ argsNew.getCommandArguments().values["challengetarget"].operator jsonifier::string() };
				snowflake fromUserID = argsNew.getUserData().id;

				guild_member_data fromGuildMember = guild_members::getCachedGuildMember({ .guildMemberId = fromUserID, .guildId = guild.id });
				discord_guild_member discordFromGuildMember(managerAgent, fromGuildMember);

				guild_member_data toGuildMember = guild_members::getCachedGuildMember({ .guildMemberId = toUserID, .guildId = guild.id });
				discord_guild_member discordToGuildMember(managerAgent, toGuildMember);

				if (toGuildMember.getUserData().userName == "") {
					jsonifier::string msgString = "------\n**sorry, but that user could not be found!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User_data issue:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent02 = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				int32_t fromUserCurrency = discordFromGuildMember.data.currency.wallet;
				int32_t toUserCurrency	 = discordToGuildMember.data.currency.wallet;

				if (betAmount > fromUserCurrency) {
					jsonifier::string msgString = "------\n**sorry, but you have insufficient funds in your wallet for placing that wager!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient funds:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent02 = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (betAmount > toUserCurrency) {
					jsonifier::string msgString = "------\n**sorry, but they have insufficient funds in their wallet for accepting that wager!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient funds:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent02 = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				jsonifier::string msgEmbedString = "you've been challenged to a duel! :crossed_swords: \nBy user: <@!" + fromUserID + ">\nFor a wager of: " + jsonifier::toString(betAmount) +
											 " " + discordUser.data.currencyName + "\nReact with :white_check_mark: to accept or :x: to reject!";
				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setDescription(msgEmbedString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**It's time to duel!**__");
				msgEmbed.setColor("fefefe");
				input_event_data newEvent02 = argsNew.getInputEventData();
				respond_to_input_event_data dataPackage2(argsNew.getInputEventData());
				dataPackage2.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage2.addMessageEmbed(msgEmbed);
				dataPackage2.addContent("<@!" + toUserID + ">");
				dataPackage2.addButton(false, "check", "accept", button_style::Success, "✅");
				dataPackage2.addButton(false, "cross", "reject", button_style::Success, "❌");
				newEvent02 = input_events::respondToInputEventAsync(dataPackage2).get();
				discord_core_api::unique_ptr<button_collector> button{ discord_core_api::makeUnique<button_collector>(newEvent02) };
				auto createResponseData = discord_core_api::makeUnique<create_interaction_response_data>();
				auto embedData			= discord_core_api::makeUnique<embed_data>();
				embedData->setColor("fefefe");
				embedData->setTitle("__**Permissions issue:**__");
				embedData->setTimeStamp(getTimeAndDate());
				embedData->setDescription("sorry, but that button can only be pressed by <@" + argsNew.getUserData().id + ">!");
				createResponseData->addMessageEmbed(*embedData);
				createResponseData->setResponseType(interaction_callback_type::Channel_Message_With_Source);
				createResponseData->setFlags(64);
				jsonifier::vector<button_response_data> buttonInteractionData = button->collectButtonData(false, 120000, 1, *createResponseData, toUserID).get();
				respond_to_input_event_data dataPackageNew(*buttonInteractionData.at(0).interactionData);
				if (buttonInteractionData.at(0).buttonId == "empty") {
					executeExit(fromUserID.operator jsonifier::string(), toUserID.operator jsonifier::string(), discordGuild, newEvent02);
				} else if (buttonInteractionData.at(0).buttonId == "check") {
					jsonifier::string theNewUserId{ toUserID };
					jsonifier::string theNewUserFromId{ fromUserID };
					executeCheck(argsNew, &discordFromGuildMember, &discordToGuildMember, &discordGuild, newEvent02, &betAmount, dataPackageNew, msgEmbedString, theNewUserFromId,
						theNewUserId);
				} else if (buttonInteractionData.at(0).buttonId == "cross") {
					jsonifier::string rejectedString = "sorry, <@!" + fromUserID + ">, but <@!" + toUserID + "> has rejected your duel offer!";
					embed_data messageEmbed5{};
					messageEmbed5 = embed_data();
					messageEmbed5.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					messageEmbed5.setColor("fe0000");
					messageEmbed5.setTimeStamp(getTimeAndDate());
					messageEmbed5.setTitle("__**Duel rejected!**__");
					messageEmbed5.setDescription(rejectedString);
					dataPackageNew.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackageNew.addMessageEmbed(messageEmbed5);
					dataPackageNew.addContent("<@!" + fromUserID + ">");
					input_events::respondToInputEventAsync(dataPackageNew).get();
				}
				return;
			} catch (const std::exception& error) {
				std::cout << "duel::execute()" << error.what() << std::endl;
			}
		}
		~duel(){};
	};
}// namespace discord_core_api
