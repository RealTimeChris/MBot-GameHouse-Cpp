// Blackjack.hpp - Header for the blackjack command.
// May 31, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

/**
	Refreshes a blackjack stack of Cards.
*/
std::vector<DiscordCoreAPI::Card> refreshBlackjackStack() {
	std::vector<DiscordCoreAPI::Card> newCardStack;
	if (newCardStack.size() == 0) {
		newCardStack.resize(312);
		for (uint32_t x = 0; x < 6; x += 1) {
			DiscordCoreAPI::Deck newDeck;
			for (uint32_t y = 0; y < 52; y += 1) {
				newCardStack[( int64_t )x * ( int64_t )52 + ( int64_t )y] = newDeck.drawRandomcard();
			}
		}
	}
	return newCardStack;
}

/**
	Draws the next card from a stack of blackjack cards.
 */
DiscordCoreAPI::Card drawNextBlackjackCard(std::vector<DiscordCoreAPI::Card>* cardStack) {
	if (cardStack->size() == 0) {
		*cardStack = refreshBlackjackStack();
		DiscordCoreAPI::Card currentCard = cardStack->at(0);
		cardStack->erase(cardStack->begin());
		return currentCard;
	}

	DiscordCoreAPI::Card currentCard = cardStack->at(0);
	cardStack->erase(cardStack->begin());
	return currentCard;
}

void checkAndSetAceValues(std::vector<DiscordCoreAPI::Card>* playerHand, std::vector<uint32_t>* playerAceIndices) {
	auto newPlayerAceIndices = playerAceIndices;
	if (playerHand->at(playerHand->size() - 1).type == "Ace") {
		newPlayerAceIndices->resize(newPlayerAceIndices->size() + 1);
		newPlayerAceIndices->at(( int32_t )newPlayerAceIndices->size() - 1) = ( int32_t )playerHand->size() - 1;
	}
	auto playerNonAceValue = 0;
	for (auto x = 0; x < playerHand->size(); x += 1) {
		if (playerHand->at(x).type != "Ace") {
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

void executeCrossResponse(DiscordCoreAPI::BaseFunctionArguments args, DiscordCoreAPI::DiscordGuildMember* discordGuildMember, uint32_t* betAmount,
	DiscordCoreAPI::GuildMember* guildMember, DiscordCoreAPI::DiscordGuild* discordGuild, DiscordCoreAPI::RespondToInputEventData* buttonInteraction,
	std::vector<DiscordCoreAPI::Card>* userHand, std::vector<uint32_t>* dealerAceIndices, std::string* userID, std::vector<DiscordCoreAPI::Card>* dealerHand,
	DiscordCoreAPI::EmbedData finalEmbed) {
	discordGuildMember->getDataFromDB();
	uint32_t fineAmount = 0;
	fineAmount = 1 * *betAmount;
	if (fineAmount > discordGuildMember->data.currency.wallet) {
		std::string inPlayFooterString = "------\n__***Sorry, but you have insufficient funds for placing that wager now!***__\n------";
		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->user.userName, guildMember->user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setColor("FF0000");
		msgEmbed.setTitle("__**Blackjack Fail:**__");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("__**Game Status: Failed Wager**__", inPlayFooterString, false);
		buttonInteraction->addMessageEmbed(msgEmbed);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
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
		discordGuild->getDataFromDB();
		dealerHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
		discordGuild->writeDataToDB();

		checkAndSetAceValues(dealerHand, dealerAceIndices);
	}

	newDealerHandScore = 0;
	for (auto x = 0; x < dealerHand->size(); x += 1) {
		newDealerHandScore += dealerHand->at(x).value;
	}

	std::string dealerHandString;
	for (auto x = 0; x < dealerHand->size(); x += 1) {
		dealerHandString += dealerHand->at(x).suit + dealerHand->at(x).type;
	}

	std::string userHandString;
	for (auto x = 0; x < userHand->size(); x += 1) {
		userHandString += userHand->at(x).suit + userHand->at(x).type;
	}

	if ((newUserHandScore == 21 && newDealerHandScore != 21) || (newUserHandScore < 21 && newUserHandScore > newDealerHandScore) ||
		(newUserHandScore < 21 && newDealerHandScore > 21)) {
		int32_t payAmount = *betAmount;
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->user.userName;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();

		discordGuildMember->getDataFromDB();
		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();
		auto botUser = args.discordCoreClient->getBotUser();
		DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
		std::string winFooterString = "------\n__**Payout Amount:**__ " + std::to_string(payAmount) + " " + discordUser.data.currencyName +
			"\n__**Your New Wallet Balance:**__ " + std::to_string(discordGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed{};
		msgEmbed.setAuthor(guildMember->user.userName, guildMember->user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Win:**__");
		msgEmbed.setColor("00FF00");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Wins**__", winFooterString, false);
		buttonInteraction->addMessageEmbed(msgEmbed);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);

		return;
	} else if (newUserHandScore == newDealerHandScore) {
		auto botUser = args.discordCoreClient->getBotUser();
		DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
		std::string tieFooterString = "------\n__**Your Wallet Balance:**__ " + std::to_string(discordGuildMember->data.currency.wallet) + " " +
			discordUser.data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->user.userName, guildMember->user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Tie:**__");
		msgEmbed.setColor("0000FF");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Tie**__", tieFooterString, false);
		msgEmbed.setDescription(finalEmbed.description);
		buttonInteraction->addMessageEmbed(msgEmbed);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
		return;
	} else {
		int32_t payAmount = -1 * *betAmount;
		discordGuildMember->getDataFromDB();
		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->user.userName;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();
		auto botUser = args.discordCoreClient->getBotUser();
		DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
		std::string bustFooterString = "------\n__**Your New Wallet Balance:**__ " + std::to_string(discordGuildMember->data.currency.wallet) + " " +
			discordUser.data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->user.userName, guildMember->user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Loss:**__");
		msgEmbed.setColor("FF0000");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Bust**__", bustFooterString, false);
		msgEmbed.setDescription(finalEmbed.description);
		buttonInteraction->addMessageEmbed(msgEmbed);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
		return;
	}
}

void executeCheckResponse(DiscordCoreAPI::BaseFunctionArguments args, DiscordCoreAPI::DiscordGuildMember* discordGuildMember, uint32_t* betAmount,
	DiscordCoreAPI::GuildMember* guildMember, DiscordCoreAPI::DiscordGuild* discordGuild, DiscordCoreAPI::InputEventData* newEvent,
	DiscordCoreAPI::RespondToInputEventData* buttonInteraction, uint32_t* newCardCount, std::vector<DiscordCoreAPI::Card>* userHand,
	std::vector<uint32_t>* userAceIndices, std::vector<uint32_t>* dealerAceIndices, std::string* userID, std::vector<DiscordCoreAPI::Card>* dealerHand,
	DiscordCoreAPI::EmbedData finalEmbed, DiscordCoreAPI::ActionRowData component) {
	discordGuildMember->getDataFromDB();
	DiscordCoreAPI::User currentUser = DiscordCoreAPI::Users::getUserAsync({ newEvent->getRequesterId() }).get();

	uint32_t fineAmount = 0;
	fineAmount = 1 * *betAmount;
	if (fineAmount > discordGuildMember->data.currency.wallet) {
		std::string inPlayFooterString;
		inPlayFooterString = "------\n__***Sorry, but you have insufficient funds for placing that wager now!***__\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setColor("FF0000");
		msgEmbed.setTitle("__**Blackjack Fail:**__");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("__**Game Status: Failed Wager**__", inPlayFooterString, false);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		buttonInteraction->addMessageEmbed(msgEmbed);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
		return;
	}

	newCardCount += 1;
	discordGuild->getDataFromDB();
	userHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
	discordGuild->writeDataToDB();

	checkAndSetAceValues(userHand, userAceIndices);

	uint32_t newUserHandScore = 0;
	for (auto x = 0; x < userHand->size(); x += 1) {
		newUserHandScore += (*userHand).at(x).value;
	}

	if (newUserHandScore > 21) {
		int32_t payAmount = (int32_t)(( float )*betAmount * -1.0);
		discordGuildMember->getDataFromDB();
		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->user.userName;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();

		uint32_t newDealerHandScore = 0;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			newDealerHandScore += (*dealerHand).at(x).value;
		}

		std::string dealerHandString;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			dealerHandString += (*dealerHand).at(x).suit + (*dealerHand).at(x).type;
		}

		std::string userHandString;
		for (auto x = 0; x < userHand->size(); x += 1) {
			userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
		}
		auto botUser = args.discordCoreClient->getBotUser();
		DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
		std::string bustFooterString = "------\n__**Your New Wallet Balance:**__ " + std::to_string(discordGuildMember->data.currency.wallet) + " " +
			discordUser.data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Loss:**__");
		msgEmbed.setColor("FF0000");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Bust**__", bustFooterString, false);
		msgEmbed.setDescription(finalEmbed.description);
		buttonInteraction->addMessageEmbed(msgEmbed);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
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
			discordGuild->getDataFromDB();
			dealerHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
			discordGuild->writeDataToDB();

			checkAndSetAceValues(dealerHand, dealerAceIndices);
		}

		newDealerHandScore = 0;
		for (auto x = 0; x < dealerHand->size(); x += 1) {
			newDealerHandScore += (*dealerHand).at(x).value;
		}

		if (newDealerHandScore == 21) {
			std::string dealerHandString;
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				dealerHandString += (*dealerHand).at(x).suit + (*dealerHand).at(x).type;
			}

			std::string userHandString;
			for (auto x = 0; x < userHand->size(); x += 1) {
				userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
			}
			auto botUser = args.discordCoreClient->getBotUser();
			DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
			std::string tieFooterString = "------\n__**Your Wallet Balance:**__ " + std::to_string(discordGuildMember->data.currency.wallet) + " " +
				discordUser.data.currencyName + "\n------";

			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
			msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed.setTitle("__**Blackjack Tie:**__");
			msgEmbed.setColor("0000FF");
			msgEmbed.setFooter("Cards Remaining: " + discordGuild->data.blackjackStack.size());
			msgEmbed.setDescription(finalEmbed.description);
			msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
			msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
			msgEmbed.addField("__**Game Status: Tie**__", tieFooterString, false);
			msgEmbed.setDescription(finalEmbed.description);
			buttonInteraction->addMessageEmbed(msgEmbed);
			buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
			DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
			return;
		} else {
			int32_t payAmount = *betAmount;
			discordGuildMember->getDataFromDB();
			discordGuildMember->data.currency.wallet += payAmount;
			discordGuildMember->writeDataToDB();
			discordGuild->getDataFromDB();
			if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
				discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
				discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
				discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
				discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->user.userName;
			}
			discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
			discordGuild->data.casinoStats.totalPayout += payAmount;
			discordGuild->writeDataToDB();

			std::string dealerHandString;
			for (auto x = 0; x < dealerHand->size(); x += 1) {
				dealerHandString += (*dealerHand).at(x).suit + (*dealerHand).at(x).type;
			}

			std::string userHandString;
			for (auto x = 0; x < userHand->size(); x += 1) {
				userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
			}

			auto botUser = args.discordCoreClient->getBotUser();
			DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
			std::string winFooterString;
			winFooterString = "------\n__**Payout Amount:**__ " + std::to_string(payAmount) + " " + discordUser.data.currencyName +
				"\n__**Your New Wallet Balance:**__ " + std::to_string(discordGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName +
				"\n------";

			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
			msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed.setTitle("__**Blackjack Win:**__");
			msgEmbed.setColor("00FF00");
			msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
			msgEmbed.setDescription(finalEmbed.description);
			msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
			msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
			msgEmbed.addField("__**Game Status: Player Wins**__", winFooterString, false);
			msgEmbed.setDescription(finalEmbed.description);
			buttonInteraction->addMessageEmbed(msgEmbed);
			buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
			DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
			return;
		}
	} else if (newUserHandScore < 21) {
		uint32_t newDealerHandScore = (*dealerHand).at(0).value;

		std::string dealerHandString;
		dealerHandString += (*dealerHand).at(0).suit + (*dealerHand).at(0).type;

		std::string userHandString;
		for (auto x = 0; x < userHand->size(); x += 1) {
			userHandString += (*userHand).at(x).suit + (*userHand).at(x).type;
		}

		std::string inPlayFooterString = "------\n------";
		DiscordCoreAPI::EmbedData msgEmbed;
		DiscordCoreAPI::InputEventData newData{};
		msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack:**__");
		msgEmbed.setColor("00FF00");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.setDescription(finalEmbed.description);
		msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: In Play**__", inPlayFooterString, false);
		msgEmbed.setDescription(finalEmbed.description);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		buttonInteraction->addComponentRow(component);
		buttonInteraction->addMessageEmbed(msgEmbed);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
		std::unique_ptr<DiscordCoreAPI::ButtonCollector> button = std::make_unique<DiscordCoreAPI::ButtonCollector>(*newEvent);
		std::unique_ptr<std::vector<DiscordCoreAPI::ButtonResponseData>> buttonInteractionData =
			std::make_unique<std::vector<DiscordCoreAPI::ButtonResponseData>>(button->collectButtonData(false, 120000, 1, newEvent->getRequesterId()).get());
		DiscordCoreAPI::RespondToInputEventData inputData{ buttonInteractionData->at(0).interactionData };
		if (buttonInteractionData->at(0).buttonId == "") {
			std::string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
			DiscordCoreAPI::EmbedData msgEmbed2;
			msgEmbed2.setColor("FF0000");
			msgEmbed2.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed2.setTitle("__**Blackjack Game:**__");
			msgEmbed.setAuthor(currentUser.userName, currentUser.avatar);
			msgEmbed2.setDescription(timeOutString);
			std::vector<DiscordCoreAPI::EmbedData> embeds;
			embeds.push_back(msgEmbed2);
			DiscordCoreAPI::RespondToInputEventData dataPackage{ *newEvent };
			dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
			inputData.addMessageEmbed(msgEmbed2);
			dataPackage.addMessageEmbed(msgEmbed2);
			DiscordCoreAPI::InputEvents::respondToEvent(inputData);
			return;
		} else {
		}
		if (buttonInteractionData->at(0).buttonId == "check") {
			executeCheckResponse(args, discordGuildMember, betAmount, guildMember, discordGuild, newEvent, &inputData, newCardCount, userHand, userAceIndices,
				dealerAceIndices, userID, dealerHand, finalEmbed, component);
		} else if (buttonInteractionData->at(0).buttonId == "cross") {
			executeCrossResponse(
				args, discordGuildMember, betAmount, guildMember, discordGuild, &inputData, userHand, dealerAceIndices, userID, dealerHand, finalEmbed);
		};
	};
	return;
};

void executeDoubleResponse(DiscordCoreAPI::BaseFunctionArguments args, DiscordCoreAPI::DiscordGuildMember* discordGuildMember, uint32_t* betAmount,
	DiscordCoreAPI::GuildMember* guildMember, DiscordCoreAPI::DiscordGuild* discordGuild, DiscordCoreAPI::InputEventData* newEvent,
	DiscordCoreAPI::RespondToInputEventData* buttonInteraction, uint32_t* newCardCount, std::vector<DiscordCoreAPI::Card>* userHand,
	std::vector<uint32_t>* userAceIndices, std::vector<uint32_t>* dealerAceIndices, std::string* userID, std::vector<DiscordCoreAPI::Card>* dealerHand,
	DiscordCoreAPI::EmbedData finalEmbed, DiscordCoreAPI::ActionRowData component) {
	uint32_t fineAmount = 2 * *betAmount;
	if (fineAmount > discordGuildMember->data.currency.wallet || *newCardCount > 2) {
		std::string failedFooterString;
		discordGuildMember->getDataFromDB();
		if ((finalEmbed.fields[2].value.find("⏬") == std::string::npos) || *newCardCount > 2) {
			failedFooterString = "__***Sorry, but you do not have the option to double down!***__\n------\n✅ to Hit, ❎ to Stand.\n------";
		} else if (fineAmount > discordGuildMember->data.currency.wallet) {
			failedFooterString = "__***Sorry, but you have insufficient funds for placing that wager now!***__\n------\n✅ to Hit, ❎ to Stand.\n------";
		}

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->user.userName, guildMember->user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setColor("00FF00");
		msgEmbed.setTitle("__**Blackjack:**__");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		std::unique_ptr<DiscordCoreAPI::InputEventData> eventData002 = std::make_unique<DiscordCoreAPI::InputEventData>(*newEvent);
		msgEmbed.setDescription(finalEmbed.description);
		msgEmbed.addField(finalEmbed.fields[0].name, finalEmbed.fields[0].value, finalEmbed.fields[0].Inline);
		msgEmbed.addField(finalEmbed.fields[1].name, finalEmbed.fields[1].value, finalEmbed.fields[1].Inline);
		msgEmbed.addField("__**Game Status: In Play**__", failedFooterString, false);
		for (auto& value: newEvent->getComponents()) {
			buttonInteraction->addComponentRow(value);
		}
		buttonInteraction->addMessageEmbed(msgEmbed);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		eventData002 = DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
		DiscordCoreAPI::ButtonCollector button(*newEvent);
		std::vector<DiscordCoreAPI::ButtonResponseData> buttonIntData = button.collectButtonData(false, 120000, 1, newEvent->getRequesterId()).get();
		DiscordCoreAPI::RespondToInputEventData inputData{ buttonIntData.at(0).interactionData };
		if (buttonIntData.at(0).buttonId == "check") {
			executeCheckResponse(args, discordGuildMember, betAmount, guildMember, discordGuild, newEvent, &inputData, newCardCount, userHand, userAceIndices,
				dealerAceIndices, userID, dealerHand, finalEmbed, component);
		} else if (buttonIntData.at(0).buttonId == "cross") {
			executeCrossResponse(
				args, discordGuildMember, betAmount, guildMember, discordGuild, &inputData, userHand, dealerAceIndices, userID, dealerHand, finalEmbed);
		} else if (buttonIntData.at(0).buttonId == "exit" || buttonIntData.at(0).buttonId == "empty") {
			std::string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
			DiscordCoreAPI::EmbedData msgEmbed2;
			msgEmbed2.setColor("FF0000");
			msgEmbed2.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			msgEmbed2.setTitle("__**Blackjack Game:**__");
			msgEmbed2.setAuthor(guildMember->user.userName, guildMember->user.avatar);
			msgEmbed2.setDescription(timeOutString);
			DiscordCoreAPI::RespondToInputEventData dataPackage{ *newEvent };
			dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
			dataPackage.addMessageEmbed(msgEmbed2);
			buttonInteraction->addMessageEmbed(msgEmbed2);
			buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
			DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
		}
		return;
	};

	newCardCount += 1;
	discordGuild->getDataFromDB();
	userHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
	discordGuild->writeDataToDB();

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
		discordGuild->getDataFromDB();
		dealerHand->push_back(drawNextBlackjackCard(&discordGuild->data.blackjackStack));
		discordGuild->writeDataToDB();

		checkAndSetAceValues(dealerHand, dealerAceIndices);
	}

	newDealerHandScore = 0;
	for (auto x = 0; x < dealerHand->size(); x += 1) {
		newDealerHandScore += dealerHand->at(x).value;
	}

	std::string dealerHandString;
	for (auto x = 0; x < dealerHand->size(); x += 1) {
		dealerHandString += dealerHand->at(x).suit + dealerHand->at(x).type;
	}

	std::string userHandString;
	for (auto x = 0; x < userHand->size(); x += 1) {
		userHandString += userHand->at(x).suit + userHand->at(x).type;
	}

	if ((newUserHandScore == 21 && newDealerHandScore != 21) || (newUserHandScore < 21 && newUserHandScore > newDealerHandScore) ||
		(newUserHandScore < 21 && newDealerHandScore > 21)) {
		int32_t payAmount = 2 * *betAmount;

		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->user.userName;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();

		auto botUser = args.discordCoreClient->getBotUser();
		DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
		std::string winFooterString = "------\n__**Payout Amount:**__ " + std::to_string(payAmount) + " " + discordUser.data.currencyName +
			"\n__**Your New Wallet Balance:**__ " + std::to_string(discordGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->user.userName, guildMember->user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Win:**__");
		msgEmbed.setColor("00FF00");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Wins**__", winFooterString, false);
		msgEmbed.setDescription(finalEmbed.description);
		buttonInteraction->addMessageEmbed(msgEmbed);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
		return;
	} else if (newUserHandScore == newDealerHandScore) {
		auto botUser = args.discordCoreClient->getBotUser();
		DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
		std::string tieFooterString = "------\n__**Your Wallet Balance:**__ " + std::to_string(discordGuildMember->data.currency.wallet) + " " +
			discordUser.data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->user.userName, guildMember->user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Tie:**__");
		msgEmbed.setColor("0000FF");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Tie**__", tieFooterString, false);
		msgEmbed.setDescription(finalEmbed.description);
		buttonInteraction->addMessageEmbed(msgEmbed);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
		return;
	} else {
		int32_t payAmount = -2 * *betAmount;

		discordGuildMember->data.currency.wallet += payAmount;
		discordGuildMember->writeDataToDB();
		discordGuild->getDataFromDB();
		if (payAmount > discordGuild->data.casinoStats.largestBlackjackPayout.amount) {
			discordGuild->data.casinoStats.largestBlackjackPayout.amount = payAmount;
			discordGuild->data.casinoStats.largestBlackjackPayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
			discordGuild->data.casinoStats.largestBlackjackPayout.userId = *userID;
			discordGuild->data.casinoStats.largestBlackjackPayout.userName = guildMember->user.userName;
		}
		discordGuild->data.casinoStats.totalBlackjackPayout += payAmount;
		discordGuild->data.casinoStats.totalPayout += payAmount;
		discordGuild->writeDataToDB();

		auto botUser = args.discordCoreClient->getBotUser();
		DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
		std::string bustFooterString = "------\n__**Your New Wallet Balance:**__ " + std::to_string(discordGuildMember->data.currency.wallet) + " " +
			discordUser.data.currencyName + "\n------";

		DiscordCoreAPI::EmbedData msgEmbed;
		msgEmbed.setAuthor(guildMember->user.userName, guildMember->user.avatar);
		msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		msgEmbed.setTitle("__**Blackjack Loss:**__");
		msgEmbed.setColor("FF0000");
		msgEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild->data.blackjackStack.size()));
		msgEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHandString, true);
		msgEmbed.addField("Player's Hand: " + std::to_string(newUserHandScore), userHandString, true);
		msgEmbed.addField("__**Game Status: Player Bust**__", bustFooterString, false);
		msgEmbed.setDescription(finalEmbed.description);
		buttonInteraction->addMessageEmbed(msgEmbed);
		buttonInteraction->setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		DiscordCoreAPI::InputEvents::respondToEvent(*buttonInteraction);
		return;
	}
}

namespace DiscordCoreAPI {

	class Blackjack : public BaseFunction {
	  public:
		Blackjack() {
			this->commandName = "blackjack";
			this->helpDescription = "Bet some currency on a game of blackjack.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /blackjack BETAMOUNT.\n------");
			msgEmbed.setTitle("__**Blackjack Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			std::unique_ptr<BaseFunction> newPtr = std::make_unique<Blackjack>();
			return newPtr;
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();

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
				auto botUser = args.discordCoreClient->getBotUser();
				DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
				discordUser.writeDataToDB();
				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getRequesterId(), .guildId = args.eventData->getGuildId() })
						.get();
				GuildMember botMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = discordUser.data.userId, .guildId = args.eventData->getGuildId() }).get();
				if (!botMember.permissions.checkForPermission(botMember, channel, Permission::Manage_Messages)) {
					std::string msgString = "------\n**I need the Manage Messages permission in this channel, for this game!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Permissions Issue:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
					return;
				}

				std::regex betRegExp("\\d{1,18}");
				if (args.commandData.optionsArgs.size() == 0 || !std::regex_search(args.commandData.optionsArgs.at(0), betRegExp) ||
					std::stoll(args.commandData.optionsArgs.at(0)) < 1) {
					std::string msgString = "------\n**Please enter a valid bet amount!(!blackjack = BETAMOUNT)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
					return;
				}

				std::cmatch matchResults;
				std::regex_search(args.commandData.optionsArgs.at(0).c_str(), matchResults, betRegExp);
				uint32_t betAmount = ( uint32_t )std::stoll(matchResults.str());

				std::srand(( uint32_t )std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

				std::string userID = guildMember.user.id;
				DiscordCoreAPI::DiscordGuildMember discordGuildMember(guildMember);

				if (betAmount > discordGuildMember.data.currency.wallet) {
					std::string msgString = "------\n**Sorry, but you have insufficient funds for placing that wager!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Funds:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					std::unique_ptr<InputEventData> event01 = InputEvents::respondToEvent(dataPackage);
					return;
				}
				std::string finalMsgString;
				finalMsgString = "__**Your Bet Amount:**__ " + std::to_string(betAmount) + " " + discordUser.data.currencyName + "\n";

				std::string footerMsgStringOld = "------\n------";

				discordGuild.getDataFromDB();
				std::vector<Card> userHand;
				std::vector<uint32_t> userAceIndices;
				userHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&userHand, &userAceIndices);
				userHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&userHand, &userAceIndices);
				uint32_t userHandScore = userHand.at(0).value + userHand.at(1).value;
				discordGuild.writeDataToDB();

				std::vector<Card> dealerHand;
				std::vector<uint32_t> dealerAceIndices;
				dealerHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&dealerHand, &dealerAceIndices);
				dealerHand.push_back(drawNextBlackjackCard(&discordGuild.data.blackjackStack));
				checkAndSetAceValues(&dealerHand, &dealerAceIndices);
				uint32_t newDealerHandScore = dealerHand[0].value;
				discordGuild.writeDataToDB();
				std::string footerMsgString = footerMsgStringOld;

				if (userHandScore == 21) {
					if (dealerHand[0].value == 10 && dealerHand[1].type == "Ace") {
						dealerHand[1].value = 11;
					} else if (dealerHand[1].value == 10 && dealerHand[0].type == "Ace") {
						dealerHand[0].value = 11;
					}
					newDealerHandScore = dealerHand[0].value + dealerHand[1].value;
					std::string footerMsgString2;
					if (newDealerHandScore == 21) {
						footerMsgString2 = "\n------\n__**Your Wallet Balance:**__ " + std::to_string(discordGuildMember.data.currency.wallet) + " " +
							discordUser.data.currencyName + "\n------";

						EmbedData finalMessageEmbed;
						finalMessageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						finalMessageEmbed.setTimeStamp(getTimeAndDate());
						finalMessageEmbed.setColor("0000FF");
						finalMessageEmbed.setDescription(finalMsgString);
						finalMessageEmbed.setTitle("__**Blackjack Tie:**__");
						finalMessageEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild.data.blackjackStack.size()));
						finalMessageEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore),
							dealerHand[0].suit + dealerHand[0].type + dealerHand[1].suit + dealerHand[1].type, true);
						finalMessageEmbed.addField(
							"Player's Hand: " + std::to_string(userHandScore), userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type, true);
						finalMessageEmbed.addField("__**Game Status: Tie**__", footerMsgString2, false);

						DiscordCoreAPI::RespondToInputEventData dataPackage{ *args.eventData };
						dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Interaction_Response);
						dataPackage.addMessageEmbed(finalMessageEmbed);
						InputEvents::respondToEvent(dataPackage);

						return;
					}

					discordGuildMember.getDataFromDB();
					int32_t payAmount = ( uint32_t )trunc(1.5 * betAmount);
					discordGuildMember.data.currency.wallet += payAmount;
					discordGuildMember.writeDataToDB();
					discordGuild.getDataFromDB();
					if (payAmount > discordGuild.data.casinoStats.largestBlackjackPayout.amount) {
						discordGuild.data.casinoStats.largestBlackjackPayout.amount = payAmount;
						discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp = getTimeAndDate();
						discordGuild.data.casinoStats.largestBlackjackPayout.userId = userID;
						discordGuild.data.casinoStats.largestBlackjackPayout.userName = guildMember.user.userName;
					}
					discordGuild.data.casinoStats.totalBlackjackPayout += payAmount;
					discordGuild.data.casinoStats.totalPayout += payAmount;
					discordGuild.writeDataToDB();

					footerMsgString2 = "\n------\n__**Payout Amount:**__ " + std::to_string(payAmount) + " " + discordUser.data.currencyName +
						"\n__**Your New Wallet Balance:**__ " + std::to_string(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName +
						"\n------";

					DiscordCoreAPI::EmbedData finalMessageEmbed;
					finalMessageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					finalMessageEmbed.setTimeStamp(getTimeAndDate());
					finalMessageEmbed.setColor("00FF00");
					finalMessageEmbed.setDescription(finalMsgString);
					finalMessageEmbed.setTitle("__**Blackjack Win:**__");
					finalMessageEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild.data.blackjackStack.size()));
					finalMessageEmbed.addField("Dealer's Hand: ", dealerHand[0].suit + dealerHand[0].type + dealerHand[1].suit + dealerHand[1].type, true);
					finalMessageEmbed.addField("Player's Hand: ", userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type, true);
					finalMessageEmbed.addField("__**Game Status: Player Wins**__", footerMsgString2, false);
					DiscordCoreAPI::RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(finalMessageEmbed);
					InputEvents::respondToEvent(dataPackage);
					return;
				}
				bool canWeDoubleDown = false;
				if ((userHandScore == 9) || (userHandScore == 10) || (userHandScore == 11)) {
					canWeDoubleDown = true;
				}

				std::unique_ptr<DiscordCoreAPI::InputEventData> event001 = std::make_unique<InputEventData>(*args.eventData);

				EmbedData finalMessageEmbed;
				finalMessageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				finalMessageEmbed.setTimeStamp(getTimeAndDate());
				finalMessageEmbed.setColor("00FF00");
				finalMessageEmbed.setDescription(finalMsgString);
				finalMessageEmbed.setTitle("__**Blackjack:**__");
				finalMessageEmbed.setFooter("Cards Remaining: " + std::to_string(discordGuild.data.blackjackStack.size()));
				finalMessageEmbed.addField("Dealer's Hand: " + std::to_string(newDealerHandScore), dealerHand[0].suit + dealerHand[0].type, true);
				finalMessageEmbed.addField(
					"Player's Hand: " + std::to_string(userHandScore), userHand[0].suit + userHand[0].type + userHand[1].suit + userHand[1].type, true);
				finalMessageEmbed.addField("__**Game Status: In Play**__", footerMsgString, false);
				if (args.eventData->eventType == InteractionType::Application_Command) {
					RespondToInputEventData replyInteractionData(*args.eventData);
					replyInteractionData.setResponseType(InputEventResponseType::Interaction_Response);
					replyInteractionData.addMessageEmbed(finalMessageEmbed);
					replyInteractionData.addButton(false, "check", "Hit", ButtonStyle::Success, "✅");
					replyInteractionData.addButton(false, "cross", "Stand", ButtonStyle::Success, "❎");
					if (canWeDoubleDown) {
						replyInteractionData.addButton(false, "double", "Double-Down", ButtonStyle::Primary, "⏬");
					}
					event001 = InputEvents::respondToEvent(replyInteractionData);
				}
				DiscordCoreAPI::ButtonCollector button(*event001);
				std::vector<ButtonResponseData> buttonIntData = button.collectButtonData(false, 120000, 1, args.eventData->getRequesterId()).get();
				if (buttonIntData.at(0).buttonId == "exit" || buttonIntData.at(0).buttonId == "empty") {
					std::string timeOutString = "------\nSorry, but you ran out of time to select an option.\n------";
					EmbedData msgEmbed2;
					msgEmbed2.setColor("FF0000");
					msgEmbed2.setTimeStamp(getTimeAndDate());
					msgEmbed2.setTitle("__**Blackjack Game:**__");
					msgEmbed2.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed2.setDescription(timeOutString);
					DiscordCoreAPI::RespondToInputEventData dataPackage{ buttonIntData.at(0).interactionData };
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed2);
					event001 = InputEvents::respondToEvent(dataPackage);
					return;
				} else {
				}
				auto components = event001->getComponents();
				if (canWeDoubleDown) {
					components.at(0).components.erase(components.at(0).components.begin() + 2);
				}

				DiscordCoreAPI::RespondToInputEventData buttonInteraction{ buttonIntData.at(0).interactionData };
				uint32_t newCardCount = 0;
				if (buttonIntData.at(0).buttonId == "check") {
					executeCheckResponse(args, &discordGuildMember, &betAmount, &guildMember, &discordGuild, event001.get(), &buttonInteraction, &newCardCount,
						&userHand, &userAceIndices, &dealerAceIndices, &userID, &dealerHand, finalMessageEmbed, components.at(0));
				} else if (buttonIntData.at(0).buttonId == "cross") {
					executeCrossResponse(args, &discordGuildMember, &betAmount, &guildMember, &discordGuild, &buttonInteraction, &userHand, &dealerAceIndices,
						&userID, &dealerHand, finalMessageEmbed);
				} else if (buttonIntData.at(0).buttonId == "double") {
					executeDoubleResponse(args, &discordGuildMember, &betAmount, &guildMember, &discordGuild, event001.get(), &buttonInteraction, &newCardCount,
						&userHand, &userAceIndices, &dealerAceIndices, &userID, &dealerHand, finalMessageEmbed, components.at(0));
				};

				return;
			} catch (...) {
				reportException("Blackjack::execute()");
			}
		};

		virtual ~Blackjack(){};
	};
};
