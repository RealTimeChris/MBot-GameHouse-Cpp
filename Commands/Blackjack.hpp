// Blackjack.hpp - Header for the blackjack command.
// may 31, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"
namespace discord_core_api {

	/**
	refreshes a blackjack stack of cards.
*/
	jsonifier::vector<card> refreshBlackjackStack() {
		jsonifier::vector<card> newCardStack;
		if (newCardStack.size() == 0) {
			newCardStack.resize(312);
			for (uint32_t x = 0; x < 6; x += 1) {
				deck newDeck;
				for (uint32_t y = 0; y < 52; y += 1) {
					newCardStack[(int64_t)x * (int64_t)52 + (int64_t)y] = newDeck.drawRandomcard();
				}
			}
		}
		return newCardStack;
	}

	/**
	draws the next card from a stack of blackjack cards.
 */
	card drawNextBlackjackCard(jsonifier::vector<card>* cardStack) {
		if (cardStack->size() == 0) {
			*cardStack		 = refreshBlackjackStack();
			card currentCard = cardStack->at(0);
			cardStack->erase(cardStack->begin());
			return currentCard;
		}

		card currentCard = cardStack->at(0);
		cardStack->erase(cardStack->begin());
		return currentCard;
	}

	void checkAndSetAceValues(jsonifier::vector<card>* playerHand, jsonifier::vector<uint32_t>* playerAceIndices) {
		auto newPlayerAceIndices = playerAceIndices;
		if (playerHand->at(playerHand->size() - 1).type == "ace") {
			newPlayerAceIndices->resize(newPlayerAceIndices->size() + 1);
			newPlayerAceIndices->at((int32_t)newPlayerAceIndices->size() - 1) = (int32_t)playerHand->size() - 1;
		}
		auto playerNonAceValue = 0;
		for (auto x = 0; x < playerHand->size(); x += 1) {
			if (playerHand->at(x).type != "ace") {
				playerNonAceValue += playerHand->at(x).value;
			}
		}
		auto newPlayerHand = playerHand;
		if (playerNonAceValue + 11 > 21 && newPlayerAceIndices->size() > 0) {
			newPlayerHand->at(newPlayerAceIndices->at(0)).value = 1;
		} else if (playerNonAceValue + 11 <= 21 && playerAceIndices->size() > 0) {
			newPlayerHand->at(playerAceIndices->at(0)).value = 11;
		}
		for (auto x = 0; x < playerAceIndices->size(); x += 1) {
			if (x > 0) {
				newPlayerHand->at(playerAceIndices->at(x)).value = 1;
			}
		}
	}

	void executeCrossResponse(const base_function_arguments& argsNew, discord_guild_member* discordGuildMember, uint32_t* betAmount, guild_member_data* guildMember, discord_guild* discordGuild,
		respond_to_input_event_data* buttonInteraction, jsonifier::vector<card>* userHand, jsonifier::vector<uint32_t>* dealerAceIndices, snowflake* userID,
		jsonifier::vector<card>* dealerHand, embed_data& finalEmbed) {
		discordGuildMember->getDataFromDB(managerAgent);
		uint32_t fineAmount = 0;
		fineAmount			= 1 * *betAmount;
		if (fineAmount > discordGuildMember->data.currency.wallet) {
			jsonifier::string inPlayFooterString = "------\n__***sorry, but you have insufficient funds for placing that wager now!***__\n------";
			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setColor("ff0000");
			msgEmbed->setTitle("__**Blackjack fail:**__");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->addField("__**Game status: failed wager**__", inPlayFooterString, false);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			input_events::respondToInputEventAsync(*buttonInteraction).get();
			return;
		}
		uint32_t newUserHandScore = 0;
		for (auto x = 0; x < userHand->size(); x += 1) {
			newUserHandScore += userHand->at(x).value;
		}
		uint32_t newDealerHandScore = 0;
		while (newDealerHandScore < 17) {
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				newDealerHandScore += dealerHand->at(x).value;
			}
			if (newDealerHandScore >= 17) {
				break;
			}
			discordGuild->getDataFromDB(managerAgent);
			dealerHand->emplace_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
			discordGuild->writeDataToDB(managerAgent);

			checkAndSetAceValues(dealerHand, dealerAceIndices);
		}
		newDealerHandScore = 0;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			newDealerHandScore += dealerHand->at(x).value;
		}

		jsonifier::string dealerHandString;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			dealerHandString += dealerHand->at(x).suit + dealerHand->at(x).type;
		}

		jsonifier::string userHandString;
		for (auto x = 0; x < userHand->size(); x += 1) {
			userHandString += userHand->at(x).suit + userHand->at(x).type;
		}

		if ((newUserHandScore == 21 && newDealerHandScore != 21) || (newUserHandScore < 21 && newUserHandScore > newDealerHandScore) ||
			(newUserHandScore < 21 && newDealerHandScore > 21)) {
			int32_t payAmount = *betAmount;
			discordGuild->getDataFromDB(managerAgent);
			if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
				discordGuild->data.casinoStats.largestBlackjackPayout.amount	= payAmount;
				discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
				discordGuild->data.casinoStats.largestBlackjackPayout.userId	= *userID;
				discordGuild->data.casinoStats.largestBlackjackPayout.userName	= guildMember->getUserData().userName;
			}
			discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
			discordGuild->data.casinoStats.totalPayout += payAmount;
			discordGuild->writeDataToDB(managerAgent);

			discordGuildMember->getDataFromDB(managerAgent);
			discordGuildMember->data.currency.wallet += payAmount;
			discordGuildMember->writeDataToDB(managerAgent);
			auto botUser = discord_core_client::getInstance()->getBotUser();
			jsonifier::string theString{ botUser.userName };
			discord_core_api::unique_ptr<discord_user> discordUser{ discord_core_api::makeUnique<discord_user>(managerAgent, theString, botUser.id) };
			jsonifier::string winFooterString = "------\n__**Payout amount:**__ " + jsonifier::toString(payAmount) + " " + discordUser->data.currencyName +
										  "\n__**Your new wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser->data.currencyName +
										  "\n------";

			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**Blackjack win:**__");
			msgEmbed->setColor("00FF00");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
			msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
			msgEmbed->addField("__**Game status: player wins**__", winFooterString, false);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			input_events::respondToInputEventAsync(*buttonInteraction).get();
			return;
		} else if (newUserHandScore == newDealerHandScore) {
			auto botUser = discord_core_client::getInstance()->getBotUser();
			jsonifier::string theString{ botUser.userName };
			discord_core_api::unique_ptr<discord_user> discordUser{ discord_core_api::makeUnique<discord_user>(managerAgent, theString, botUser.id) };
			jsonifier::string tieFooterString =
				"------\n__**Your wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser->data.currencyName + "\n------";

			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**Blackjack tie:**__");
			msgEmbed->setColor("0000FF");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
			msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
			msgEmbed->addField("__**Game status: tie**__", tieFooterString, false);
			msgEmbed->setDescription(finalEmbed.description);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			input_events::respondToInputEventAsync(*buttonInteraction).get();
			return;
		} else {
			int32_t payAmount = -1 * *betAmount;
			discordGuildMember->getDataFromDB(managerAgent);
			discordGuildMember->data.currency.wallet += payAmount;
			discordGuildMember->writeDataToDB(managerAgent);
			discordGuild->getDataFromDB(managerAgent);
			if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
				discordGuild->data.casinoStats.largestBlackjackPayout.amount	= payAmount;
				discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
				discordGuild->data.casinoStats.largestBlackjackPayout.userId	= *userID;
				discordGuild->data.casinoStats.largestBlackjackPayout.userName	= guildMember->getUserData().userName;
			}
			discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
			discordGuild->data.casinoStats.totalPayout += payAmount;
			discordGuild->writeDataToDB(managerAgent);
			auto botUser = discord_core_client::getInstance()->getBotUser();
			jsonifier::string theString{ botUser.userName };
			discord_core_api::unique_ptr<discord_user> discordUser{ discord_core_api::makeUnique<discord_user>(managerAgent, theString, botUser.id) };
			jsonifier::string bustFooterString =
				"------\n__**Your new wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser->data.currencyName + "\n------";

			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**Blackjack loss:**__");
			msgEmbed->setColor("ff0000");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
			msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
			msgEmbed->addField("__**Game status: player bust**__", bustFooterString, false);
			msgEmbed->setDescription(finalEmbed.description);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			input_events::respondToInputEventAsync(*buttonInteraction).get();
			return;
		}
	}

	void executeCheckResponse(const base_function_arguments& argsNew, discord_guild_member* discordGuildMember, uint32_t* betAmount, guild_member_data* guildMember, discord_guild* discordGuild,
		input_event_data newEvent, respond_to_input_event_data* buttonInteraction, uint32_t* newCardCount, jsonifier::vector<card>* userHand, jsonifier::vector<uint32_t>* userAceIndices,
		jsonifier::vector<uint32_t>* dealerAceIndices, snowflake* userID, jsonifier::vector<card>* dealerHand, embed_data& finalEmbed, action_row_data& component) {
		discordGuildMember->getDataFromDB(managerAgent);
		user_data currentUser = users::getUserAsync({ newEvent.getUserData().id }).get();

		uint32_t fineAmount = 0;
		fineAmount			= 1 * *betAmount;
		if (fineAmount > discordGuildMember->data.currency.wallet) {
			jsonifier::string inPlayFooterString;
			inPlayFooterString = "------\n__***sorry, but you have insufficient funds for placing that wager now!***__\n------";

			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setColor("ff0000");
			msgEmbed->setTitle("__**Blackjack fail:**__");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->addField("__**Game status: failed wager**__", inPlayFooterString, false);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			input_events::respondToInputEventAsync(*buttonInteraction).get();
			return;
		}

		newCardCount += 1;
		discordGuild->getDataFromDB(managerAgent);
		userHand->emplace_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
		discordGuild->writeDataToDB(managerAgent);

		checkAndSetAceValues(userHand, userAceIndices);

		uint32_t newUserHandScore = 0;
		for (auto x = 0; x < userHand->size(); x += 1) {
			newUserHandScore += (*userHand).at(x).value;
		}

		if (newUserHandScore > 21) {
			int32_t payAmount = (int32_t)((float)*betAmount * -1.0);
			discordGuildMember->getDataFromDB(managerAgent);
			discordGuildMember->data.currency.wallet += payAmount;
			discordGuildMember->writeDataToDB(managerAgent);
			discordGuild->getDataFromDB(managerAgent);
			if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
				discordGuild->data.casinoStats.largestBlackjackPayout.amount	= payAmount;
				discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
				discordGuild->data.casinoStats.largestBlackjackPayout.userId	= *userID;
				discordGuild->data.casinoStats.largestBlackjackPayout.userName	= guildMember->getUserData().userName;
			}
			discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
			discordGuild->data.casinoStats.totalPayout += payAmount;
			discordGuild->writeDataToDB(managerAgent);

			uint32_t newDealerHandScore = 0;
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				newDealerHandScore += (*dealerHand).at(x).value;
			}

			jsonifier::string dealerHandString;
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				dealerHandString += (*dealerHand).at(x).suit + (*dealerHand).at(x).type;
			}

			jsonifier::string userHandString;
			for (auto x = 0; x < userHand->size(); x += 1) {
				userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
			}
			auto botUser = discord_core_client::getInstance()->getBotUser();
			jsonifier::string theString{ botUser.userName };
			discord_core_api::unique_ptr<discord_user> discordUser{ discord_core_api::makeUnique<discord_user>(managerAgent, theString, botUser.id) };
			jsonifier::string bustFooterString =
				"------\n__**Your new wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser->data.currencyName + "\n------";

			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**Blackjack loss:**__");
			msgEmbed->setColor("ff0000");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
			msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
			msgEmbed->addField("__**Game status: player bust**__", bustFooterString, false);
			msgEmbed->setDescription(finalEmbed.description);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			input_events::respondToInputEventAsync(*buttonInteraction).get();
			return;
		} else if (newUserHandScore == 21) {
			uint32_t newDealerHandScore = 0;
			while (newDealerHandScore < 17) {
				for (auto x = 0; x < dealerHand->size(); x += 1) {
					newDealerHandScore += (*dealerHand).at(x).value;
				}
				if (newDealerHandScore >= 17) {
					break;
				}
				discordGuild->getDataFromDB(managerAgent);
				dealerHand->emplace_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
				discordGuild->writeDataToDB(managerAgent);

				checkAndSetAceValues(dealerHand, dealerAceIndices);
			}

			newDealerHandScore = 0;
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				newDealerHandScore += (*dealerHand).at(x).value;
			}

			if (newDealerHandScore == 21) {
				jsonifier::string dealerHandString;
				for (auto x = 0; x < dealerHand->size(); x += 1) {
					dealerHandString += (*dealerHand).at(x).suit + (*dealerHand).at(x).type;
				}

				jsonifier::string userHandString;
				for (auto x = 0; x < userHand->size(); x += 1) {
					userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
				}
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				jsonifier::string tieFooterString =
					"------\n__**Your wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

				discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
				msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
				msgEmbed->setTimeStamp(getTimeAndDate());
				msgEmbed->setTitle("__**Blackjack tie:**__");
				msgEmbed->setColor("0000FF");
				msgEmbed->setFooter("cards remaining: " + discordGuild->data.blackjackStack.size());
				msgEmbed->setDescription(finalEmbed.description);
				msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
				msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
				msgEmbed->addField("__**Game status: tie**__", tieFooterString, false);
				msgEmbed->setDescription(finalEmbed.description);
				buttonInteraction->addMessageEmbed(*msgEmbed);
				buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
				input_events::respondToInputEventAsync(*buttonInteraction).get();
				return;
			} else {
				int32_t payAmount = *betAmount;
				discordGuildMember->getDataFromDB(managerAgent);
				discordGuildMember->data.currency.wallet += payAmount;
				discordGuildMember->writeDataToDB(managerAgent);
				discordGuild->getDataFromDB(managerAgent);
				if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
					discordGuild->data.casinoStats.largestBlackjackPayout.amount	= payAmount;
					discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
					discordGuild->data.casinoStats.largestBlackjackPayout.userId	= *userID;
					discordGuild->data.casinoStats.largestBlackjackPayout.userName	= guildMember->getUserData().userName;
				}
				discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
				discordGuild->data.casinoStats.totalPayout += payAmount;
				discordGuild->writeDataToDB(managerAgent);

				jsonifier::string dealerHandString;
				for (auto x = 0; x < dealerHand->size(); x += 1) {
					dealerHandString += (*dealerHand).at(x).suit + (*dealerHand).at(x).type;
				}

				jsonifier::string userHandString;
				for (auto x = 0; x < userHand->size(); x += 1) {
					userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
				}

				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_core_api::unique_ptr<discord_user> discordUser{ discord_core_api::makeUnique<discord_user>(managerAgent, theString, botUser.id) };
				jsonifier::string winFooterString;
				winFooterString = "------\n__**Payout amount:**__ " + jsonifier::toString(payAmount) + " " + discordUser->data.currencyName + "\n__**Your new wallet balance:**__ " +
								  jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser->data.currencyName + "\n------";

				discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
				msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
				msgEmbed->setTimeStamp(getTimeAndDate());
				msgEmbed->setTitle("__**Blackjack win:**__");
				msgEmbed->setColor("00FF00");
				msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
				msgEmbed->setDescription(finalEmbed.description);
				msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
				msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
				msgEmbed->addField("__**Game status: player wins**__", winFooterString, false);
				msgEmbed->setDescription(finalEmbed.description);
				buttonInteraction->addMessageEmbed(*msgEmbed);
				buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
				input_events::respondToInputEventAsync(*buttonInteraction).get();
				return;
			}
		} else if (newUserHandScore < 21) {
			uint32_t newDealerHandScore = (*dealerHand).at(0).value;

			jsonifier::string dealerHandString;
			dealerHandString += (*dealerHand).at(0).suit + (*dealerHand).at(0).type;

			jsonifier::string userHandString;
			for (auto x = 0; x < userHand->size(); x += 1) {
				userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
			}

			jsonifier::string inPlayFooterString = "------\n------";
			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			input_event_data newData{};
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**Blackjack:**__");
			msgEmbed->setColor("00FF00");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->setDescription(finalEmbed.description);
			msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
			msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
			msgEmbed->addField("__**Game status: in play**__", inPlayFooterString, false);
			msgEmbed->setDescription(finalEmbed.description);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			buttonInteraction->addComponentRow(component);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			input_events::respondToInputEventAsync(*buttonInteraction);
			discord_core_api::unique_ptr<button_collector> button = discord_core_api::makeUnique<button_collector>(newEvent);
			auto createResponseData							  = discord_core_api::makeUnique<create_interaction_response_data>();
			auto embedData									  = discord_core_api::makeUnique<embed_data>();
			embedData->setColor("fefefe");
			embedData->setTitle("__**Permissions issue:**__");
			embedData->setTimeStamp(getTimeAndDate());
			embedData->setDescription("sorry, but that button can only be pressed by <@" + argsNew.getUserData().id + ">!");
			createResponseData->addMessageEmbed(*embedData);
			createResponseData->setResponseType(interaction_callback_type::Channel_Message_With_Source);
			createResponseData->setFlags(64);
			discord_core_api::unique_ptr<jsonifier::vector<button_response_data>> buttonInteractionData =
				discord_core_api::makeUnique<jsonifier::vector<button_response_data>>(button->collectButtonData(false, 120000, 1, *createResponseData, guildMember->user.id).get());
			respond_to_input_event_data inputData{ *buttonInteractionData->at(0).interactionData };
			if (buttonInteractionData->at(0).buttonId == "") {
				jsonifier::string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
				embed_data msgEmbed2;
				msgEmbed2.setColor("ff0000");
				msgEmbed2.setTimeStamp(getTimeAndDate());
				msgEmbed2.setTitle("__**Blackjack game:**__");
				msgEmbed->setAuthor(currentUser.userName, currentUser.getUserImageUrl(user_image_types::Avatar));
				msgEmbed2.setDescription(timeOutString);
				jsonifier::vector<embed_data> embeds;
				embeds.emplace_back(msgEmbed2);
				respond_to_input_event_data dataPackage{ newEvent };
				dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
				inputData.addMessageEmbed(msgEmbed2);
				dataPackage.addMessageEmbed(msgEmbed2);
				input_events::respondToInputEventAsync(inputData).get();
				return;
			} else {
			}
			if (buttonInteractionData->at(0).buttonId == "check") {
				executeCheckResponse(argsNew,
					discordGuildMember,
					betAmount,
					guildMember,
					discordGuild,
					newEvent,
					&inputData,
					newCardCount,
					userHand,
					userAceIndices,
					dealerAceIndices,
					userID,
					dealerHand,
					finalEmbed,
					component);
			} else if (buttonInteractionData->at(0).buttonId == "cross") {
				executeCrossResponse(argsNew, discordGuildMember, betAmount, guildMember, discordGuild, &inputData, userHand, dealerAceIndices, userID, dealerHand, finalEmbed);
			};
		};
		return;
	};

	void executeDoubleResponse(const base_function_arguments& argsNew, discord_guild_member* discordGuildMember, uint32_t* betAmount, guild_member_data* guildMember,
		discord_guild* discordGuild, input_event_data& newEvent, respond_to_input_event_data* buttonInteraction, uint32_t* newCardCount, jsonifier::vector<card>* userHand,
		jsonifier::vector<uint32_t>* userAceIndices, jsonifier::vector<uint32_t>* dealerAceIndices, snowflake* userID, jsonifier::vector<card>* dealerHand, embed_data& finalEmbed,
		action_row_data& component) {
		uint32_t fineAmount = 2 * *betAmount;
		if (fineAmount > discordGuildMember->data.currency.wallet || *newCardCount > 2) {
			jsonifier::string failedFooterString;
			discordGuildMember->getDataFromDB(managerAgent);
			if ((finalEmbed.fields[2].value.find("⏬") == jsonifier::string::npos) || *newCardCount > 2) {
				failedFooterString = "__***sorry, but you do not have the option to double down!***__\n------\n✅ to hit, ❎ to stand.\n------";
			} else if (fineAmount > discordGuildMember->data.currency.wallet) {
				failedFooterString = "__***sorry, but you have insufficient funds for placing that wager now!***__\n------\n✅ to hit, ❎ to stand.\n------";
			}

			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setColor("00FF00");
			msgEmbed->setTitle("__**Blackjack:**__");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			input_event_data eventData002 = newEvent;
			msgEmbed->setDescription(finalEmbed.description);
			msgEmbed->addField(finalEmbed.fields[0].name, finalEmbed.fields[0].value, finalEmbed.fields[0].Inline);
			msgEmbed->addField(finalEmbed.fields[1].name, finalEmbed.fields[1].value, finalEmbed.fields[1].Inline);
			msgEmbed->addField("__**Game status: in play**__", failedFooterString, false);
			for (auto& value : newEvent.getMessageData().components) {
				buttonInteraction->addComponentRow(value);
			}
			buttonInteraction->addMessageEmbed(*msgEmbed);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			eventData002 = input_events::respondToInputEventAsync(*buttonInteraction).get();
			discord_core_api::unique_ptr<button_collector> button{ discord_core_api::makeUnique<button_collector>(newEvent) };
			auto createResponseData = discord_core_api::makeUnique<create_interaction_response_data>();
			auto embedData			= discord_core_api::makeUnique<embed_data>();
			embedData->setColor("fefefe");
			embedData->setTitle("__**Permissions issue:**__");
			embedData->setTimeStamp(getTimeAndDate());
			embedData->setDescription("sorry, but that button can only be pressed by <@" + argsNew.getUserData().id + ">!");
			createResponseData->addMessageEmbed(*embedData);
			createResponseData->setResponseType(interaction_callback_type::Channel_Message_With_Source);
			createResponseData->setFlags(64);
			jsonifier::vector<button_response_data> buttonIntData = button->collectButtonData(false, 120000, 1, *createResponseData, guildMember->user.id).get();
			respond_to_input_event_data inputData{ *buttonIntData.at(0).interactionData };
			if (buttonIntData.at(0).buttonId == "check") {
				executeCheckResponse(argsNew,
					discordGuildMember,
					betAmount,
					guildMember,
					discordGuild,
					newEvent,
					&inputData,
					newCardCount,
					userHand,
					userAceIndices,
					dealerAceIndices,
					userID,
					dealerHand,
					finalEmbed,
					component);
			} else if (buttonIntData.at(0).buttonId == "cross") {
				executeCrossResponse(argsNew, discordGuildMember, betAmount, guildMember, discordGuild, &inputData, userHand, dealerAceIndices, userID, dealerHand, finalEmbed);
			} else if (buttonIntData.at(0).buttonId == "exit" || buttonIntData.at(0).buttonId == "empty") {
				jsonifier::string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
				embed_data msgEmbed2;
				msgEmbed2.setColor("ff0000");
				msgEmbed2.setTimeStamp(getTimeAndDate());
				msgEmbed2.setTitle("__**Blackjack game:**__");
				msgEmbed2.setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
				msgEmbed2.setDescription(timeOutString);
				respond_to_input_event_data dataPackage{ newEvent };
				dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed2);
				buttonInteraction->addMessageEmbed(msgEmbed2);
				buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
				input_events::respondToInputEventAsync(dataPackage).get();
			}
			return;
		};

		newCardCount += 1;
		discordGuild->getDataFromDB(managerAgent);
		userHand->emplace_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
		discordGuild->writeDataToDB(managerAgent);

		checkAndSetAceValues(userHand, userAceIndices);

		uint32_t newUserHandScore = 0;
		for (auto x = 0; x < userHand->size(); x += 1) {
			newUserHandScore += userHand->at(x).value;
		}
		uint32_t newDealerHandScore = 0;
		while (newDealerHandScore < 17) {
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				newDealerHandScore += dealerHand->at(x).value;
			}
			if (newDealerHandScore >= 17) {
				break;
			}
			discordGuild->getDataFromDB(managerAgent);
			dealerHand->emplace_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
			discordGuild->writeDataToDB(managerAgent);

			checkAndSetAceValues(dealerHand, dealerAceIndices);
		}

		newDealerHandScore = 0;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			newDealerHandScore += dealerHand->at(x).value;
		}

		jsonifier::string dealerHandString;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			dealerHandString += dealerHand->at(x).suit + dealerHand->at(x).type;
		}

		jsonifier::string userHandString;
		for (auto x = 0; x < userHand->size(); x += 1) {
			userHandString += userHand->at(x).suit + userHand->at(x).type;
		}

		if ((newUserHandScore == 21 && newDealerHandScore != 21) || (newUserHandScore < 21 && newUserHandScore > newDealerHandScore) ||
			(newUserHandScore < 21 && newDealerHandScore > 21)) {
			int32_t payAmount = 2 * *betAmount;

			discordGuildMember->data.currency.wallet += payAmount;
			discordGuildMember->writeDataToDB(managerAgent);
			discordGuild->getDataFromDB(managerAgent);
			if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
				discordGuild->data.casinoStats.largestBlackjackPayout.amount	= payAmount;
				discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
				discordGuild->data.casinoStats.largestBlackjackPayout.userId	= *userID;
				discordGuild->data.casinoStats.largestBlackjackPayout.userName	= guildMember->getUserData().userName;
			}
			discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
			discordGuild->data.casinoStats.totalPayout += payAmount;
			discordGuild->writeDataToDB(managerAgent);

			auto botUser = discord_core_client::getInstance()->getBotUser();
			jsonifier::string theString{ botUser.userName };
			discord_user discordUser(managerAgent, theString, botUser.id);
			jsonifier::string winFooterString = "------\n__**Payout amount:**__ " + jsonifier::toString(payAmount) + " " + discordUser.data.currencyName +
										  "\n__**Your new wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName +
										  "\n------";

			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**Blackjack win:**__");
			msgEmbed->setColor("00FF00");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
			msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
			msgEmbed->addField("__**Game status: player wins**__", winFooterString, false);
			msgEmbed->setDescription(finalEmbed.description);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			input_events::respondToInputEventAsync(*buttonInteraction).get();
			return;
		} else if (newUserHandScore == newDealerHandScore) {
			auto botUser = discord_core_client::getInstance()->getBotUser();
			jsonifier::string theString{ botUser.userName };
			discord_user discordUser(managerAgent, theString, botUser.id);
			jsonifier::string tieFooterString =
				"------\n__**Your wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**Blackjack tie:**__");
			msgEmbed->setColor("0000FF");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
			msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
			msgEmbed->addField("__**Game status: tie**__", tieFooterString, false);
			msgEmbed->setDescription(finalEmbed.description);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			input_events::respondToInputEventAsync(*buttonInteraction).get();
			return;
		} else {
			int32_t payAmount = -2 * *betAmount;

			discordGuildMember->data.currency.wallet += payAmount;
			discordGuildMember->writeDataToDB(managerAgent);
			discordGuild->getDataFromDB(managerAgent);
			if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
				discordGuild->data.casinoStats.largestBlackjackPayout.amount	= payAmount;
				discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
				discordGuild->data.casinoStats.largestBlackjackPayout.userId	= *userID;
				discordGuild->data.casinoStats.largestBlackjackPayout.userName	= guildMember->getUserData().userName;
			}
			discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
			discordGuild->data.casinoStats.totalPayout += payAmount;
			discordGuild->writeDataToDB(managerAgent);

			auto botUser = discord_core_client::getInstance()->getBotUser();
			jsonifier::string theString{ botUser.userName };
			discord_user discordUser(managerAgent, theString, botUser.id);
			jsonifier::string bustFooterString =
				"------\n__**Your new wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setAuthor(guildMember->getUserData().userName, guildMember->getGuildMemberImageUrl(guild_member_image_types::Avatar));
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setTitle("__**Blackjack loss:**__");
			msgEmbed->setColor("ff0000");
			msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
			msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHandString, true);
			msgEmbed->addField("player's hand: " + jsonifier::toString(newUserHandScore), userHandString, true);
			msgEmbed->addField("__**Game status: player bust**__", bustFooterString, false);
			msgEmbed->setDescription(finalEmbed.description);
			buttonInteraction->addMessageEmbed(*msgEmbed);
			buttonInteraction->setResponseType(input_event_response_type::Edit_Interaction_Response);
			input_events::respondToInputEventAsync(*buttonInteraction).get();
			return;
		}
	}


	class blackjack : public base_function {
	  public:
		blackjack() {
			this->commandName	  = "blackjack";
			this->helpDescription = "bet some currency on a game of blackjack.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /blackjack betamount.\n------");
			msgEmbed.setTitle("__**Blackjack usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<blackjack>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				unique_ptr<guild_cache_data> guild{ makeUnique<guild_cache_data>(argsNew.getInteractionData().guildId) };
				discord_core_api::unique_ptr<discord_guild> discordGuild(discord_core_api::makeUnique<discord_guild>(managerAgent, *guild));

				std::regex betRegExp{ "\\d{1,18}" };
				uint32_t betAmount = argsNew.getCommandArguments().values["betamount"].operator size_t();

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				snowflake userID = guildMember.user.id;
				discord_core_api::unique_ptr<discord_guild_member> discordGuildMember{ discord_core_api::makeUnique<discord_guild_member>(managerAgent, guildMember) };
				if (betAmount > discordGuildMember->data.currency.wallet) {
					jsonifier::string msgString = "------\n**sorry, but you have insufficient funds for placing that wager!**\n------";
					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setColor(discordGuild->data.borderColor);
					msgEmbed->setDescription(msgString);
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Missing funds:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				discord_core_api::unique_ptr<channel_data> channel{ discord_core_api::makeUnique<channel_data>(argsNew.getChannelData()) };

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), *discordGuild);
				if (areWeAllowed == false) {
					return;
				}

				respond_to_input_event_data dataPackage00{ argsNew.getInputEventData() };
				dataPackage00.setResponseType(input_event_response_type ::Deferred_Response);
				auto newEvent01 = input_events::respondToInputEventAsync(dataPackage00).get();


				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				discordUser.writeDataToDB(managerAgent);
				guild_member_data botMember{ guild_members::getCachedGuildMember({ .guildMemberId = discordUser.data.userId, .guildId = guild->id }) };

				std::srand((uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

				jsonifier::string finalMsgString;
				finalMsgString = "__**Your bet amount:**__ " + jsonifier::toString(betAmount) + " " + discordUser.data.currencyName + "\n";

				jsonifier::string footerMsgStringOld = "------\n------";

				discordGuild->getDataFromDB(managerAgent);
				jsonifier::vector<card> userHand;
				jsonifier::vector<uint32_t> userAceIndices;
				userHand.emplace_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
				checkAndSetAceValues(&userHand, &userAceIndices);
				userHand.emplace_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
				checkAndSetAceValues(&userHand, &userAceIndices);
				uint32_t userHandScore = userHand.at(0).value + userHand.at(1).value;
				discordGuild->writeDataToDB(managerAgent);

				jsonifier::vector<card> dealerHand;
				jsonifier::vector<uint32_t> dealerAceIndices;
				dealerHand.emplace_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
				checkAndSetAceValues(&dealerHand, &dealerAceIndices);
				dealerHand.emplace_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
				checkAndSetAceValues(&dealerHand, &dealerAceIndices);
				uint32_t newDealerHandScore = dealerHand[0].value;
				discordGuild->writeDataToDB(managerAgent);
				jsonifier::string footerMsgString = footerMsgStringOld;

				if (userHandScore == 21) {
					if (dealerHand[0].value == 10 && dealerHand[1].type == "ace") {
						dealerHand[1].value = 11;
					} else if (dealerHand[1].value == 10 && dealerHand[0].type == "ace") {
						dealerHand[0].value = 11;
					}
					newDealerHandScore = dealerHand[0].value + dealerHand[1].value;
					jsonifier::string footerMsgString2;
					if (newDealerHandScore == 21) {
						footerMsgString2 =
							"\n------\n__**Your wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setColor("0000FF");
						msgEmbed->setDescription(finalMsgString);
						msgEmbed->setTitle("__**Blackjack tie:**__");
						msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
						msgEmbed->addField(
							"dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHand[0].suit + dealerHand[0].type + dealerHand[1].suit + dealerHand[1].type, true);
						msgEmbed->addField("player's hand: " + jsonifier::toString(userHandScore), userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type, true);
						msgEmbed->addField("__**Game status: tie**__", footerMsgString2, false);

						respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
						dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						input_events::respondToInputEventAsync(dataPackage).get();

						return;
					}

					discordGuildMember->getDataFromDB(managerAgent);
					int32_t payAmount = (uint32_t)trunc(1.5 * betAmount);
					discordGuildMember->data.currency.wallet += payAmount;
					discordGuildMember->writeDataToDB(managerAgent);
					discordGuild->getDataFromDB(managerAgent);
					if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
						discordGuild->data.casinoStats.largestBlackjackPayout.amount	= payAmount;
						discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
						discordGuild->data.casinoStats.largestBlackjackPayout.userId	= userID;
						discordGuild->data.casinoStats.largestBlackjackPayout.userName	= guildMember.getUserData().userName;
					}
					discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
					discordGuild->data.casinoStats.totalPayout += payAmount;
					discordGuild->writeDataToDB(managerAgent);

					footerMsgString2 = "\n------\n__**Payout amount:**__ " + jsonifier::toString(payAmount) + " " + discordUser.data.currencyName +
									   "\n__**Your new wallet balance:**__ " + jsonifier::toString(discordGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName +
									   "\n------";

					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setColor("00FF00");
					msgEmbed->setDescription(finalMsgString);
					msgEmbed->setTitle("__**Blackjack win:**__");
					msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
					msgEmbed->addField("dealer's hand: ", dealerHand[0].suit + dealerHand[0].type + dealerHand[1].suit + dealerHand[1].type, true);
					msgEmbed->addField("player's hand: ", userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type, true);
					msgEmbed->addField("__**Game status: player wins**__", footerMsgString2, false);
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				bool canWeDoubleDown = false;
				if ((userHandScore == 9) || (userHandScore == 10) || (userHandScore == 11)) {
					canWeDoubleDown = true;
				}

				input_event_data event001 = argsNew.getInputEventData();

				discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
				msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed->setTimeStamp(getTimeAndDate());
				msgEmbed->setColor("00FF00");
				msgEmbed->setDescription(finalMsgString);
				msgEmbed->setTitle("__**Blackjack:**__");
				msgEmbed->setFooter("cards remaining: " + jsonifier::toString(discordGuild->data.blackjackStack.size()));
				msgEmbed->addField("dealer's hand: " + jsonifier::toString(newDealerHandScore), dealerHand[0].suit + dealerHand[0].type, true);
				msgEmbed->addField("player's hand: " + jsonifier::toString(userHandScore), userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type, true);
				msgEmbed->addField("__**Game status: in play**__", footerMsgString, false);
				respond_to_input_event_data replyInteractionData(argsNew.getInputEventData());
				replyInteractionData.setResponseType(input_event_response_type::Edit_Interaction_Response);
				replyInteractionData.addMessageEmbed(*msgEmbed);
				replyInteractionData.addButton(false, "check", "hit", button_style::Success, "✅");
				replyInteractionData.addButton(false, "cross", "stand", button_style::Success, "❎");
				if (canWeDoubleDown) {
					replyInteractionData.addButton(false, "double", "double-down", button_style::Primary, "⏬");
				}
				event001 = input_events::respondToInputEventAsync(replyInteractionData).get();
				discord_core_api::unique_ptr<button_collector> button{ discord_core_api::makeUnique<button_collector>(event001) };
				auto createResponseData = discord_core_api::makeUnique<create_interaction_response_data>();
				auto embedData			= discord_core_api::makeUnique<embed_data>();
				embedData->setColor("fefefe");
				embedData->setTitle("__**Permissions issue:**__");
				embedData->setTimeStamp(getTimeAndDate());
				embedData->setDescription("sorry, but that button can only be pressed by <@" + argsNew.getUserData().id + ">!");
				createResponseData->addMessageEmbed(*embedData);
				createResponseData->setResponseType(interaction_callback_type::Channel_Message_With_Source);
				createResponseData->setFlags(64);
				jsonifier::vector<button_response_data> buttonIntData = button->collectButtonData(false, 120000, 1, *createResponseData, argsNew.getUserData().id).get();
				if (buttonIntData.at(0).buttonId == "exit" || buttonIntData.at(0).buttonId == "empty") {
					jsonifier::string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
					discord_core_api::unique_ptr<embed_data> msgEmbed02{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed02->setColor("ff0000");
					msgEmbed02->setTimeStamp(getTimeAndDate());
					msgEmbed02->setTitle("__**Blackjack game:**__");
					msgEmbed02->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed02->setDescription(timeOutString);
					respond_to_input_event_data dataPackage{ *buttonIntData.at(0).interactionData };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed02);
					event001 = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				} else {
				}
				auto components = event001.getMessageData().components;
				if (canWeDoubleDown) {
					components.at(0).components.erase(components.at(0).components.begin() + 2);
				}

				respond_to_input_event_data buttonInteraction{ *buttonIntData.at(0).interactionData };
				uint32_t newCardCount = 0;
				if (buttonIntData.at(0).buttonId == "check") {
					executeCheckResponse(argsNew,
						discordGuildMember.get(),
						&betAmount,
						&guildMember,
						discordGuild.get(),
						event001,
						&buttonInteraction,
						&newCardCount,
						&userHand,
						&userAceIndices,
						&dealerAceIndices,
						&userID,
						&dealerHand,
						*msgEmbed,
						components.at(0));
				} else if (buttonIntData.at(0).buttonId == "cross") {
					executeCrossResponse(argsNew,
						discordGuildMember.get(),
						&betAmount,
						&guildMember,
						discordGuild.get(),
						&buttonInteraction,
						&userHand,
						&dealerAceIndices,
						&userID,
						&dealerHand,
						*msgEmbed);
				} else if (buttonIntData.at(0).buttonId == "double") {
					executeDoubleResponse(argsNew,
						discordGuildMember.get(),
						&betAmount,
						&guildMember,
						discordGuild.get(),
						event001,
						&buttonInteraction,
						&newCardCount,
						&userHand,
						&userAceIndices,
						&dealerAceIndices,
						&userID,
						&dealerHand,
						*msgEmbed,
						components.at(0));
				};

				return;
			} catch (const std::exception& error) {
				std::cout << "blackjack::execute()" << error.what() << std::endl;
			}
		};

		~blackjack(){};
	};
};// namespace discord_core_api
