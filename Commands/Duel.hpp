// Duel.hpp - Header for the "duel" command.
// Jun 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

void executeCheck(DiscordCoreAPI::BaseFunctionArguments args, DiscordCoreAPI::DiscordGuildMember* discordFromGuildMember,
				  DiscordCoreAPI::DiscordGuildMember* discordToGuildMember, DiscordCoreAPI::DiscordGuild* discordGuild,
				  DiscordCoreAPI::InputEventData* newEvent, int32_t* betAmount, DiscordCoreAPI::RespondToInputEventData dataPackageNew,
				  std::string* msgEmbedString, std::string* fromUserIDNew, std::string* toUserIDNew) {
	discordFromGuildMember->getDataFromDB();
	int32_t fromUserCurrency = discordFromGuildMember->data.currency.wallet;
	discordToGuildMember->getDataFromDB();
	int32_t toUserCurrency = discordToGuildMember->data.currency.wallet;
	DiscordCoreAPI::User currentUser = DiscordCoreAPI::Users::getUserAsync({ .userId = newEvent->getRequesterId() }).get();
	*fromUserIDNew;

	if (*betAmount > fromUserCurrency) {
		std::string msgString;
		msgString = *msgEmbedString + "\n\n__**Sorry, but you have insufficient funds in your wallet for placing that wager!**__";

		DiscordCoreAPI::EmbedData messageEmbed3;
		messageEmbed3.setAuthor(currentUser.userName, currentUser.avatar);
		messageEmbed3.setDescription(msgString);
		messageEmbed3.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		messageEmbed3.setColor(discordGuild->data.borderColor);
		messageEmbed3.setTitle("__**Insufficient Funds:**__");
		DiscordCoreAPI::RespondToInputEventData dataPackage(*newEvent);
		dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Deferred_Response);
		auto newEvent02 = DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
		DiscordCoreAPI::RespondToInputEventData dataPackage2(*newEvent02);
		dataPackage2.setResponseType(DiscordCoreAPI::InputEventResponseType::Follow_Up_Message);
		dataPackageNew.addMessageEmbed(messageEmbed3);
		auto newEvent03 = DiscordCoreAPI::InputEvents::respondToEvent(dataPackageNew);
		DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(std::move(newEvent03), 20000);
		return;
	}
	if (*betAmount > toUserCurrency) {
		std::string msgString;
		msgString += *msgEmbedString + "\n\n__**Sorry, but they have insufficient funds in their wallet for accepting that wager!**__";
		DiscordCoreAPI::EmbedData messageEmbed4;
		messageEmbed4.setAuthor(currentUser.userName, currentUser.avatar);
		messageEmbed4.setDescription(msgString);
		messageEmbed4.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		messageEmbed4.setColor(discordGuild->data.borderColor);
		messageEmbed4.setTitle("__**Insufficient Funds:**__");
		DiscordCoreAPI::RespondToInputEventData dataPackage(*newEvent);
		dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Deferred_Response);
		auto newEvent02 = DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
		DiscordCoreAPI::RespondToInputEventData dataPackage02(*newEvent02);
		dataPackage02.setResponseType(DiscordCoreAPI::InputEventResponseType::Follow_Up_Message);
		dataPackageNew.addMessageEmbed(messageEmbed4);
		auto newEvent03 = DiscordCoreAPI::InputEvents::respondToEvent(dataPackageNew);
		DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(std::move(newEvent03), 20000);
		return;
	}

	std::mt19937_64 randomEngine{ static_cast<uint64_t>(
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };

	int32_t fromUserRoll = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 100.0f);
	int32_t toUserRoll = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 100.0f);
	std::vector<DiscordCoreAPI::EmbedData> messageEmbeds;
	std::vector<std::string> finalStrings;
	std::vector<std::string> fromUserGainStrings;
	std::vector<std::string> fromUserLossStrings;
	std::vector<std::string> toUserGainStrings;
	std::vector<std::string> toUserLossStrings;
	int32_t fromUserSelfMod = 0;
	int32_t fromUserOppMod = 0;
	int32_t toUserSelfMod = 0;
	int32_t toUserOppMod = 0;
	int32_t finalFromUserRoll = 0;
	int32_t finalToUserRoll = 0;

	for (auto value: discordFromGuildMember->data.items) {
		if (value.selfMod > 0) {
			std::string currentString = "+" + std::to_string(value.selfMod);
			currentString += " of base roll from <@!" + *fromUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
			fromUserGainStrings.push_back(currentString);
			fromUserSelfMod += value.selfMod;
		}
		if (value.oppMod < 0) {
			std::string currentString = std::to_string(value.oppMod);
			currentString += " of base roll from <@!" + *fromUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
			toUserLossStrings.push_back(currentString);
			toUserOppMod += value.oppMod;
		}
	}

	for (auto value: discordToGuildMember->data.items) {
		if (value.selfMod > 0) {
			std::string currentString = "+" + std::to_string(value.selfMod);
			currentString += " of base roll from <@!" + *toUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
			toUserGainStrings.push_back(currentString);
			toUserSelfMod += value.selfMod;
		}
		if (value.oppMod < 0) {
			std::string currentString = std::to_string(value.oppMod);
			currentString += " of base roll from <@!" + *toUserIDNew + ">'s " + value.emoji + value.itemName + "\n";
			fromUserLossStrings.push_back(currentString);
			fromUserOppMod += value.oppMod;
		}
	}

	finalFromUserRoll = ( int32_t )DiscordCoreAPI::applyAsymptoticTransform(( float )(fromUserRoll + fromUserSelfMod + fromUserOppMod), 2000, 100);
	finalToUserRoll = ( int32_t )DiscordCoreAPI::applyAsymptoticTransform(( float )(toUserRoll + toUserSelfMod + toUserOppMod), 2000, 100);

	std::string fromUserFooterString;
	if (finalFromUserRoll != fromUserRoll || fromUserOppMod != 0 || fromUserSelfMod != 0) {
		fromUserFooterString = "__**For a final roll of:**__ " + std::to_string(finalFromUserRoll) + "\n";
	}

	std::string toUserFooterString;
	if (finalToUserRoll != toUserRoll || toUserOppMod != 0 || toUserSelfMod != 0) {
		toUserFooterString = "__**For a final roll of:**__ " + std::to_string(finalToUserRoll) + "\n";
	}

	if (finalFromUserRoll > finalToUserRoll) {
		discordFromGuildMember->data.currency.wallet += *betAmount;
		discordFromGuildMember->writeDataToDB();
		discordToGuildMember->getDataFromDB();
		discordToGuildMember->data.currency.wallet -= *betAmount;
		discordToGuildMember->writeDataToDB();

		uint32_t currentPage = 0;

		std::string fromUserVicHeaderString;
		fromUserVicHeaderString = "<@!" + *fromUserIDNew + "> has defeated <@!" + *toUserIDNew + ">!\n__Your rolls were__:\n";

		finalStrings.resize(1);
		finalStrings[currentPage] += fromUserVicHeaderString;

		std::string midFooter1 = "__**<@!" + *fromUserIDNew + ">:**__ " + std::to_string(fromUserRoll) + "\n";
		std::string midFooter2 = "__**<@!" + *toUserIDNew + ">:**__ " + std::to_string(toUserRoll) + "\n";

		finalStrings[currentPage] += midFooter1;

		std::string finalFooterString;
		finalFooterString = "------\n__Your new wallet balances are:__\n";
		auto botUser = args.discordCoreClient->getBotUser();
		DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
		finalFooterString += "<@!" + *fromUserIDNew + ">: " + std::to_string(discordFromGuildMember->data.currency.wallet) + " " +
			discordUser.data.currencyName + "\n" + "<@!" + *toUserIDNew + ">: " + std::to_string(discordToGuildMember->data.currency.wallet) + " " +
			discordUser.data.currencyName + "\n------";

		std::vector<std::string> fromUserModStrings;
		for (auto value: fromUserLossStrings) {
			fromUserGainStrings.push_back(value);
		}
		fromUserModStrings = fromUserGainStrings;

		for (auto value: fromUserModStrings) {
			if ((finalStrings[currentPage].length() + value.length() + midFooter1.length() + fromUserFooterString.length()) >= 2048) {
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

		std::vector<std::string> toUserModStrings;
		for (auto value: toUserLossStrings) {
			toUserGainStrings.push_back(value);
		}
		toUserModStrings = toUserGainStrings;

		for (auto value: toUserModStrings) {
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

		messageEmbeds.resize(( int64_t )currentPage + ( int64_t )1);
		for (uint32_t x = 0; x < finalStrings.size(); x += 1) {
			messageEmbeds[x] = DiscordCoreAPI::EmbedData();
			messageEmbeds[x].setAuthor(currentUser.userName, currentUser.avatar);
			messageEmbeds[x].setColor("00FE00");
			messageEmbeds[x].setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			messageEmbeds[x].setTitle("__**Duel Results: " + std::to_string(x + 1) + " of " + std::to_string(finalStrings.size()) + "**__");
			messageEmbeds[x].setDescription(finalStrings[x]);
		}
	} else if (finalToUserRoll > finalFromUserRoll) {
		discordToGuildMember->data.currency.wallet += *betAmount;
		discordToGuildMember->writeDataToDB();
		discordFromGuildMember->getDataFromDB();
		discordFromGuildMember->data.currency.wallet -= *betAmount;
		discordFromGuildMember->writeDataToDB();

		uint32_t currentPage = 0;

		std::string toUserVicHeaderString;
		toUserVicHeaderString = "<@!" + *toUserIDNew + "> has defeated <@!" + *fromUserIDNew + "> !!\n__Your rolls were__:\n";

		finalStrings.resize(1);
		finalStrings[currentPage] += toUserVicHeaderString;

		std::string midFooter1 = "__**<@!" + *toUserIDNew + ">:**__ " + std::to_string(toUserRoll) + "\n";
		std::string midFooter2 = "__**<@!" + *fromUserIDNew + ">:**__ " + std::to_string(fromUserRoll) + "\n";

		finalStrings[currentPage] += midFooter1;

		auto botUser = args.discordCoreClient->getBotUser();
		DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);

		std::string finalFooterString = "-----\n__Your new wallet balances are: __\n<@!" + *toUserIDNew +
			">: " + std::to_string(discordToGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n<@!" + *fromUserIDNew +
			">: " + std::to_string(discordFromGuildMember->data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

		std::vector<std::string> toUserModStrings;
		for (auto value: toUserLossStrings) {
			toUserGainStrings.push_back(value);
		}
		toUserModStrings = toUserGainStrings;

		for (auto value: toUserModStrings) {
			if ((finalStrings[currentPage].length() + value.length() + midFooter1.length() + toUserFooterString.length()) >= 2048) {
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

		std::vector<std::string> fromUserModStrings;
		for (auto value: fromUserLossStrings) {
			fromUserGainStrings.push_back(value);
		}
		fromUserModStrings = fromUserGainStrings;

		for (auto value: fromUserModStrings) {
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

		messageEmbeds.resize(( int64_t )currentPage + ( int64_t )1);
		for (uint32_t x = 0; x < finalStrings.size(); x += 1) {
			messageEmbeds[x] = DiscordCoreAPI::EmbedData();
			messageEmbeds[x].setAuthor(currentUser.userName, currentUser.avatar);
			messageEmbeds[x].setColor("FE0000");
			messageEmbeds[x].setTimeStamp(DiscordCoreAPI::getTimeAndDate());
			messageEmbeds[x].setTitle("__**Duel Results: " + std::to_string(x + 1) + " of " + std::to_string(finalStrings.size()) + "**__");
			messageEmbeds[x].setDescription(finalStrings[x]);
		}
	} else if (finalToUserRoll == finalFromUserRoll) {
		finalStrings.resize(1);
		messageEmbeds.push_back(DiscordCoreAPI::EmbedData());
		finalStrings[0] = "__**Looks like it was a draw! Nicely done!**__";
		messageEmbeds[0].setAuthor(currentUser.userName, currentUser.avatar);
		messageEmbeds[0].setColor("FEFEFE");
		messageEmbeds[0].setTimeStamp(DiscordCoreAPI::getTimeAndDate());
		messageEmbeds[0].setTitle("__**Duel Results: " + std::to_string(0 + 1) + " of " + std::to_string(finalStrings.size()) + "**__");
		messageEmbeds[0].setDescription(finalStrings[0]);
		dataPackageNew.setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
		dataPackageNew.addMessageEmbed(messageEmbeds[0]);
		*newEvent = *DiscordCoreAPI::InputEvents::respondToEvent(dataPackageNew);
	}
	dataPackageNew.setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
	dataPackageNew.addMessageEmbed(messageEmbeds[0]);
	auto newEvent02 = DiscordCoreAPI::InputEvents::respondToEvent(dataPackageNew);

	uint32_t currentPageIndex = 0;
	moveThroughMessagePages(*fromUserIDNew, std::make_unique<DiscordCoreAPI::InputEventData>(*newEvent), currentPageIndex, messageEmbeds, false, 120000);
}

void executeExit(std::string fromUserID, std::string toUserID, DiscordCoreAPI::DiscordGuild discordGuild, DiscordCoreAPI::InputEventData originalEvent) {
	std::string rejectedString;
	rejectedString = "Sorry, <@!" + fromUserID + ">, but <@!" + toUserID + "> has rejected your duel offer! (Timed Out!)";
	DiscordCoreAPI::User currentUser = DiscordCoreAPI::Users::getUserAsync({ originalEvent.getRequesterId() }).get();
	DiscordCoreAPI::EmbedData messageEmbed2;
	messageEmbed2.setAuthor(currentUser.userName, currentUser.avatar);
	messageEmbed2.setColor(discordGuild.data.borderColor);
	messageEmbed2.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
	messageEmbed2.setTitle("__**DUEL REJECTED!**__");
	messageEmbed2.setDescription(rejectedString);
	DiscordCoreAPI::RespondToInputEventData dataPackage(originalEvent);
	dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Follow_Up_Message);
	dataPackage.addMessageEmbed(messageEmbed2);
	DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
}

namespace DiscordCoreAPI {
	class Duel : public BaseFunction {
	  public:
		Duel() {
			this->commandName = "duel";
			this->helpDescription = "Challenge a fellow server member to a duel, for currency.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /duel BETAMOUNT, @USERMENTIONTARGETUSERMENTION.\n------");
			msgEmbed.setTitle("__**Duel Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Duel>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();
				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm == true) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

				Guild guild = Guilds::getCachedGuildAsync({ args.eventData->getGuildId() }).get();
				DiscordGuild discordGuild(guild);
				bool areWeAllowed = checkIfAllowedGamingInChannel(*args.eventData, discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				std::regex numberRegExp("\\d{1,18}");
				std::regex idRegExp("\\d{18}");

				if (args.commandData.optionsArgs.size() < 2 || !std::regex_search(args.commandData.optionsArgs.at(1), numberRegExp) ||
					std::stoll(args.commandData.optionsArgs.at(1)) < 0) {
					std::string msgString = "------\n**Please enter a valid bet amount! (!duel = BETAMOUNT, @USERMENTION)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.addMessageEmbed(msgEmbed);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					auto newEvent02 = DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
					return;
				}

				std::cmatch matchResults;
				std::regex_search(args.commandData.optionsArgs.at(1).c_str(), matchResults, numberRegExp);
				int32_t betAmount = ( int32_t )std::stoll(matchResults.str());
				std::cmatch matchResults02;
				std::regex_search(args.commandData.optionsArgs.at(0).c_str(), matchResults02, idRegExp);
				std::string toUserID = matchResults02.str();
				std::string fromUserID = args.eventData->getAuthorId();

				GuildMember fromGuildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = fromUserID, .guildId = args.eventData->getGuildId() }).get();
				DiscordGuildMember discordFromGuildMember(fromGuildMember);

				GuildMember toGuildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = toUserID, .guildId = args.eventData->getGuildId() }).get();
				DiscordGuildMember discordToGuildMember(toGuildMember);

				if (toGuildMember.user.userName == "") {
					std::string msgString = "------\n**Sorry, but that user could not be found!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User Issue:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent02 = DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
					return;
				}

				int32_t fromUserCurrency = discordFromGuildMember.data.currency.wallet;
				int32_t toUserCurrency = discordToGuildMember.data.currency.wallet;

				if (betAmount > fromUserCurrency) {
					std::string msgString = "------\n**Sorry, but you have insufficient funds in your wallet for placing that wager!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent02 = DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
					return;
				}
				if (betAmount > toUserCurrency) {
					std::string msgString = "------\n**Sorry, but they have insufficient funds in their wallet for accepting that wager!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent02 = DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
					return;
				}
				auto botUser = args.discordCoreClient->getBotUser();
				DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
				std::string msgEmbedString = "You've been challenged to a duel! :crossed_swords: \nBy user: <@!" + fromUserID +
					">\nFor a wager of: " + std::to_string(betAmount) + " " + discordUser.data.currencyName +
					"\nReact with :white_check_mark: to accept or :x: to reject!";
				EmbedData messageEmbed;
				messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				messageEmbed.setDescription(msgEmbedString);
				messageEmbed.setTimeStamp(getTimeAndDate());
				messageEmbed.setTitle("__**IT'S TIME TO DUEL!**__");
				messageEmbed.setColor(discordGuild.data.borderColor);
				std::unique_ptr<InputEventData> newEvent02 = std::make_unique<InputEventData>(*args.eventData);
				RespondToInputEventData dataPackage2(*args.eventData);
				dataPackage2.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage2.addMessageEmbed(messageEmbed);
				dataPackage2.addContent("<@!" + toUserID + ">");
				dataPackage2.addButton(false, "check", "Accept", ButtonStyle::Success, "✅");
				dataPackage2.addButton(false, "cross", "Reject", ButtonStyle::Success, "❌");
				newEvent02 = InputEvents::respondToEvent(dataPackage2);
				ButtonCollector button(*newEvent02);
				std::vector<ButtonResponseData> buttonInteractionData = button.collectButtonData(false, 120000, 1, toUserID).get();
				RespondToInputEventData dataPackageNew(buttonInteractionData.at(0).interactionData);
				if (buttonInteractionData.at(0).buttonId == "empty") {
					executeExit(fromUserID, toUserID, discordGuild, *newEvent02);
				} else if (buttonInteractionData.at(0).buttonId == "check") {
					executeCheck(args, &discordFromGuildMember, &discordToGuildMember, &discordGuild, newEvent02.get(), &betAmount, dataPackageNew,
								 &msgEmbedString, &fromUserID, &toUserID);
				} else if (buttonInteractionData.at(0).buttonId == "cross") {
					std::string rejectedString = "Sorry, <@!" + fromUserID + ">, but <@!" + toUserID + "> has rejected your duel offer!";
					EmbedData messageEmbed5;
					messageEmbed5 = EmbedData();
					messageEmbed5.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					messageEmbed5.setColor("FE0000");
					messageEmbed5.setTimeStamp(getTimeAndDate());
					messageEmbed5.setTitle("__**DUEL REJECTED!**__");
					messageEmbed5.setDescription(rejectedString);
					dataPackageNew.setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
					dataPackageNew.addMessageEmbed(messageEmbed5);
					dataPackageNew.addContent("<@!" + fromUserID + ">");
					InputEvents::respondToEvent(dataPackageNew);
				}
				return;
			} catch (...) {
				reportException("Duel::execute()");
			}
		}
		virtual ~Duel(){};
	};
}
