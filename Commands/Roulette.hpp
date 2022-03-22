// Roulette.hpp - Header for the "roulette" command.
// Jun 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

std::string getNumberString(std::string inputString, std::vector<std::string> redNumbers, std::vector<std::string> blackNumbers) {
	std::string returnString;
	for (uint32_t x = 0; x < redNumbers.size(); x += 1) {
		if (redNumbers[x].find(inputString)!= std::string::npos) {
			if (redNumbers[x].substr(12) ==  inputString) {
				returnString = redNumbers[x];
			}
		}
	}
	for (uint32_t x = 0; x < blackNumbers.size(); x += 1) {
		if (blackNumbers[x].find(inputString) != std::string::npos) {
			if (blackNumbers[x].substr(20) == inputString) {
				returnString = blackNumbers[x];
			}
		}
	}
	if (inputString ==  "0") {
		returnString = ":green_square:0";
	}
	else if (inputString ==  "00") {
		returnString = ":green_square:00";
	}
	return returnString;
}

void calculateResults(std::string finalRoll, DiscordCoreAPI::InputEventData newEvent, DiscordCoreAPI::DiscordUser discordUser, std::vector<std::string>redNumbers, std::vector<std::string> blackNumbers) {
	std::string msgStringFinal;
	std::string finalRollString = getNumberString(finalRoll, redNumbers, blackNumbers);
	msgStringFinal += "------\n__**Final Roll:**__ " + finalRollString + "\n------\n";
	DiscordCoreAPI::Guild guild = DiscordCoreAPI::Guilds::getCachedGuildAsync({ .guildId = newEvent.getGuildId() }).get();
	DiscordCoreAPI::DiscordGuild discordGuild(guild);
	for (uint32_t x = 0; x < discordGuild.data.rouletteGame.rouletteBets.size(); x += 1) {
		bool isItAWinner = false;
		DiscordCoreAPI::GuildMember guildMember = DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = discordGuild.data.rouletteGame.rouletteBets.at(x).userId,.guildId = newEvent.getGuildId() }).get();
		DiscordCoreAPI::DiscordGuildMember discordGuildMember(guildMember);
		msgStringFinal += "__**<@!" + guildMember.user.id + ">**__: ";
		int32_t betAmount = discordGuild.data.rouletteGame.rouletteBets.at(x).betAmount;
		int32_t payoutAmount = discordGuild.data.rouletteGame.rouletteBets.at(x).payoutAmount;
		std::vector<std::string> winningNumbers = discordGuild.data.rouletteGame.rouletteBets.at(x).winningNumbers;
		for (uint32_t y = 0;y < winningNumbers.size(); y += 1) {
			if (finalRoll == "37") {
				finalRoll = "00";
			}
			if (getNumberString(finalRoll, redNumbers,
				blackNumbers) == winningNumbers[y]) {
				isItAWinner = true;
				break;
			}
		}
		if (isItAWinner == false) {
			payoutAmount = (-1 * betAmount);
		}
		if ((uint32_t)betAmount > discordGuildMember.data.currency.wallet) {
			if (discordGuild.data.rouletteGame.rouletteBets[x].betOptions != "") {
				msgStringFinal += "__**NSF:**__ Non-sufficient funds! __**Bet:**__ " + std::to_string(discordGuild.data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + "__**On:**__ " + discordGuild.data.rouletteGame.rouletteBets[x].betType +
					", " + discordGuild.data.rouletteGame.rouletteBets[x].betOptions + "\n";
			}
			else {
				msgStringFinal += "__**NSF:**__ Non-sufficient funds! __ **Bet:**__ " + std::to_string(discordGuild.data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + "__**On:**__ " +
					discordGuild.data.rouletteGame.rouletteBets[x].betType + "\n";
			}
		}
		else {
			if ((int32_t)payoutAmount > discordGuild.data.casinoStats.largestRoulettePayout.amount) {
				discordGuild.data.casinoStats.largestRoulettePayout.amount = payoutAmount;
				discordGuild.data.casinoStats.largestRoulettePayout.timeStamp = DiscordCoreAPI::getTimeAndDate();
				discordGuild.data.casinoStats.largestRoulettePayout.userId = discordGuildMember.data.guildMemberId;
				discordGuild.data.casinoStats.largestRoulettePayout.userName = discordGuildMember.data.userName;
			}
			discordGuild.data.casinoStats.totalRoulettePayout += payoutAmount;
			discordGuild.data.casinoStats.totalPayout += payoutAmount;
			discordGuildMember.data.currency.wallet += payoutAmount;
			discordGuildMember.writeDataToDB();

			if (discordGuild.data.rouletteGame.rouletteBets[x].betOptions != "") {
				msgStringFinal += std::to_string(payoutAmount) + " " + discordUser.data.currencyName + " __**Bet:**__ " + std::to_string(discordGuild.data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + " __**On:**__ " +
					discordGuild.data.rouletteGame.rouletteBets[x].betType + ", " + discordGuild.data.rouletteGame.rouletteBets[x].betOptions + "\n";
			}
			else {
				msgStringFinal += std::to_string(payoutAmount) + " " + discordUser.data.currencyName + " __**Bet:**__ " + std::to_string(discordGuild.data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + " __**On:**__ " +
					discordGuild.data.rouletteGame.rouletteBets[x].betType + "\n";
			}
		}
	}
	discordGuild.data.rouletteGame.currentlySpinning = false;
	discordGuild.data.rouletteGame.rouletteBets = std::vector<DiscordCoreAPI::RouletteBet>();
	discordGuild.writeDataToDB();
	msgStringFinal += "------";
	DiscordCoreAPI::EmbedData msgEmbed;
	msgEmbed.setAuthor(newEvent.getUserName(), newEvent.getAvatarUrl());
	msgEmbed.setDescription(msgStringFinal);
	msgEmbed.setTitle("__**Roulette Results:**__");
	msgEmbed.setTimeStamp(DiscordCoreAPI::getTimeAndDate());
	msgEmbed.setColor(discordGuild.data.borderColor);
	DiscordCoreAPI::RespondToInputEventData dataPackage(newEvent);
	dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Follow_Up_Message);
	dataPackage.addMessageEmbed(msgEmbed);
	DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
	return;
}

namespace DiscordCoreAPI {

	constexpr uint32_t stringToInt(std::string str, int32_t h = 0)
	{
		return !str[h] ? 5381 : (stringToInt(str, h + 1) * 33) ^ str[h];
	}

	class RouletteGame :public BaseFunction {
	public:
		RouletteGame() {
			this->commandName = "roulette";
			this->helpDescription = "Play a game of roulette.";
			std::string newString;
			newString = "\n__**Roulette Usage:**__ /roulette start to begin a game, and then /roulette bet BETAMOUNT, BETTYPE, BETOPTIONS to bet on it!\n__Where BETTYPE and BETOPTIONS are as follows:__\n```isbl\n";
			newString += "Bet Type/Payout:     Bet Options:\n------------------------------------------------------------";
			newString += "\n0        / 35:1  |";
			newString += "\n00       / 35:1  |";
			newString += "\nstraight / 35:1  | A single number to choose.";
			newString += "\nrow      / 17:1  |";
			newString += "\nsplit    / 17:1  | The first of two consecutive numbers.";
			newString += "\nstreet   / 11:1  | The first of threee consecutive numbers.";
			newString += "\nbasket   / 6:1   |";
			newString += "\nsixline  / 5:1   | The first of six consecutive numbers.";
			newString += "\n1stcolumn/ 2:1   |";
			newString += "\n2ndcolumn/ 2:1   |";
			newString += "\n3rdcolumn/ 2:1   |";
			newString += "\n1stdozen / 2:1   |";
			newString += "\n2nddozen / 2:1   |";
			newString += "\n3rddozen / 2:1   |";
			newString += "\nodd      / 1:1   |";
			newString += "\neven     / 1:1   |";
			newString += "\nred      / 1:1   |";
			newString += "\nblack    / 1:1   |";
			newString += "\n1to18    / 1:1   |";
			newString += "\n19to36   / 1:1   |```";
			EmbedData msgEmbed;
			msgEmbed.setDescription(newString);
			msgEmbed.setTitle("__**Roulette Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<RouletteGame>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();
			bool areWeInADm = areWeInADM(*args.eventData, channel);

			if (areWeInADm ==  true) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			bool areWeAllowed = checkIfAllowedGamingInChannel(*args.eventData, discordGuild);

			if (areWeAllowed == false) {
				return;
			}
			std::string whatAreWeDoing;
			uint32_t betAmount = 0;
			std::string betType;
			std::string betOptions;
			std::regex betRegex("bet");
			if (args.commandData.subCommandName == "start") {
				if (discordGuild.data.rouletteGame.currentlySpinning == true) {
					std::string msgString = "------\n**Please, wait until the current game is over, before starting another one!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Game Issue:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					return;
				}
				whatAreWeDoing = "start";
			}
			else if (regex_search(args.commandData.subCommandName, betRegex)) {
				if (discordGuild.data.rouletteGame.currentlySpinning ==  false) {
					std::string msgString = "------\n**Please, start a roulette game before placing any bets!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Game Issue:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					return;
				}
				whatAreWeDoing = "bet";
			}
			if (args.commandData.optionsArgs.size() > 0) {
				if (std::stoll(args.commandData.optionsArgs[0]) <= 0) {
					std::string msgString = "------\n**Please, enter a valid betting amount! (!roulette = bet, BETAMOUNT, BETTYPE, BETOPTIONS)** \n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					return;
				}
				else {
					betAmount = (uint32_t) std::stoll(args.commandData.optionsArgs[0]);
				}
			}
			 std::regex digitRegExp("\\d{1,18}");
			std::vector<std::string> betTypes{ "0", "00", "straight", "row", "split", "street", "basket", "sixline", "1stcolumn", "2ndcolumn", "3rdcolumn", "1stdozen", "2nddozen", "3rddozen", "odd", "even", "red", "black", "1to18", "19to36" };
			std::vector<std::string> redNumbers{ ":red_square:32", ":red_square:19", ":red_square:21", ":red_square:25", ":red_square:34", ":red_square:27", ":red_square:36", ":red_square:30", ":red_square:23", ":red_square:5",
				":red_square:16", ":red_square:1", ":red_square:14", ":red_square:9", ":red_square:18", ":red_square:7", ":red_square:12", ":red_square:3" };
			std::vector<std::string> blackNumbers{ ":black_large_square:15", ":black_large_square:4", ":black_large_square:2", ":black_large_square:17", ":black_large_square:6", ":black_large_square:13", ":black_large_square:11", ":black_large_square:8", ":black_large_square:10",
				":black_large_square:24", ":black_large_square:33", ":black_large_square:20", ":black_large_square:31", ":black_large_square:22", ":black_large_square:29", ":black_large_square:28", ":black_large_square:35", ":black_large_square:26" };

			if (whatAreWeDoing == "bet") {
				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId(),.guildId = args.eventData->getGuildId() }).get();
				DiscordGuildMember discordGuildMember(guildMember);

				uint32_t currentBetAmount = 0;

				for (uint32_t x = 0; x < discordGuild.data.rouletteGame.rouletteBets.size(); x += 1) {
					if (discordGuildMember.data.guildMemberId == discordGuild.data.rouletteGame.rouletteBets[x].userId) {
						uint32_t number = discordGuild.data.rouletteGame.rouletteBets[x].betAmount;
						currentBetAmount += number;
					}
				}
				if ((currentBetAmount + betAmount) > discordGuildMember.data.currency.wallet) {
					std::string msgString = "------\n**Sorry, but you have insufficient funds in your wallet for placing that bet!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					return;
				}

				bool isValidType = false;
				for (uint32_t x = 0; x < betTypes.size(); x += 1) {
					if (args.commandData.optionsArgs[1] != "" && args.commandData.optionsArgs[1] == betTypes[x]) {
						isValidType = true;
						break;
					}
				}

				if (isValidType == false) {
					std::string msgString = "------\n**Please enter a valid bet type!Enter '!help = roulette' for more info!(!roulette = BETAMOUNT, BETTYPE, BETOPTIONS)** \n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					return;
				}
				else {
					betType = args.commandData.optionsArgs[1];
				}
				std::cout << "OPTION 00: " << args.commandData.optionsArgs[0] << std::endl;
				std::cout << "OPTION 01: " << args.commandData.optionsArgs[1] << std::endl;
				if (args.commandData.optionsArgs.size() > 2) {
					betOptions = args.commandData.optionsArgs[2];
				}
				uint32_t payoutAmount = 0;
				std::vector<std::string> winningNumbers;
				switch (stringToInt(betType)) {
				case stringToInt("0"): 
				{
					winningNumbers.push_back(":green_square:0");
					payoutAmount = betAmount * 35;
					break;
				}
				case stringToInt("00"):
				{
					winningNumbers.push_back(":green_square:00");
					payoutAmount = betAmount * 35;
					break;
				}
				case stringToInt("red"):
				{
					payoutAmount = betAmount;
					winningNumbers = redNumbers;
					break;
				}
				case stringToInt("black"):
				{
					payoutAmount = betAmount;
					winningNumbers = blackNumbers;
					break;
				}
				case stringToInt("straight"):
				{
					payoutAmount = betAmount * 35;
					if (args.commandData.optionsArgs.size() < 3 || args.commandData.optionsArgs[2] == "" || !regex_search(args.commandData.optionsArgs[2], digitRegExp)) {
						std::string msgString = "------\n**Please enter a valid value from the roulette wheel!(1 - 36)**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						return;
					}

					if ( std::stoll(args.commandData.optionsArgs[2]) < 1 ||  std::stoll(args.commandData.optionsArgs[2]) > 36) {
						std::string msgString = "------\n**Please enter a value between 1 and 36!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						return;
					}

					winningNumbers.push_back(getNumberString(args.commandData.optionsArgs[2], redNumbers, blackNumbers));

					break;
				}
				case stringToInt("row"):
				{
					payoutAmount = betAmount * 17;
					winningNumbers.push_back(":green_square:0");
					winningNumbers.push_back(":green_square:00");
					break;
				}
				case stringToInt("split"):
				{
					payoutAmount = betAmount * 17;
					if (args.commandData.optionsArgs.size() < 4 || args.commandData.optionsArgs[2] == "" || !regex_search(args.commandData.optionsArgs[2], digitRegExp)) {
						std::string msgString = "------\n**Please enter a valid starting value for your split!(1 - 35)** \n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						return;
					}

					if ( std::stoll(args.commandData.optionsArgs[2]) < 1 ||  std::stoll(args.commandData.optionsArgs[2]) > 35) {
						std::string msgString = "-------\n**Please enter a value between 1 and 35!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						return;
					}

					winningNumbers.push_back(getNumberString(args.commandData.optionsArgs[2], redNumbers, blackNumbers));
					std::stringstream sstream{};
					sstream <<  std::stoll(args.commandData.optionsArgs[2]) + 1;
					winningNumbers.push_back(getNumberString(sstream.str(), redNumbers, blackNumbers));

					break;
				}
				case stringToInt("street"):
				{
					payoutAmount = betAmount * 11;
					if (args.commandData.optionsArgs.size() < 4 || args.commandData.optionsArgs[2] == "" || !regex_search(args.commandData.optionsArgs[2], digitRegExp)) {
						std::string msgString = "------\n**Please enter a valid starting value for your street!(1 - 34)** \n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						return;
					}

					if ( std::stoll(args.commandData.optionsArgs[2]) < 1 ||  std::stoll(args.commandData.optionsArgs[2]) > 34) {
						std::string msgString = "-------\n**Please enter a value between 1 and 34!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
						return;
					}

					winningNumbers.push_back(getNumberString(args.commandData.optionsArgs[2], redNumbers, blackNumbers));
					std::stringstream stream01{};
					stream01 <<  std::stoll(args.commandData.optionsArgs[2]) + 1;
					winningNumbers.push_back(getNumberString(stream01.str(), redNumbers, blackNumbers));
					std::stringstream stream02{};
					stream02 <<  std::stoll(args.commandData.optionsArgs[2]) + 2;
					winningNumbers.push_back(getNumberString(stream02.str(), redNumbers, blackNumbers));

					break;
				}
				case stringToInt("basket"):
				{
					payoutAmount = betAmount * 6;
					winningNumbers.push_back(getNumberString("0", redNumbers, blackNumbers));
					winningNumbers.push_back(getNumberString("1", redNumbers, blackNumbers));
					winningNumbers.push_back(getNumberString("2", redNumbers, blackNumbers));
					winningNumbers.push_back(getNumberString("3", redNumbers, blackNumbers));
					winningNumbers.push_back(":green_square:00");
					break;
				}
				case stringToInt("sixline"):
				{
					payoutAmount = betAmount * 5;
					if (args.commandData.optionsArgs.size() < 4 || args.commandData.optionsArgs[2] == "" || !regex_search(args.commandData.optionsArgs[2], digitRegExp)) {
						std::string msgString = "------\n**Please enter a valid starting value for your sixline!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
							dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(msgEmbed);
							auto newEvent = InputEvents::respondToEvent(dataPackage);
						return;
					}

					if ( std::stoll(args.commandData.optionsArgs[2]) < 1 ||  std::stoll(args.commandData.optionsArgs[2]) > 31) {
						std::string msgString = "------\n * *Please enter a value between 1 and 31!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
							dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(msgEmbed);
							auto newEvent = InputEvents::respondToEvent(dataPackage);
						return;
					}

					winningNumbers.push_back(getNumberString(args.commandData.optionsArgs[2], redNumbers, blackNumbers));
					std::stringstream stream01{};
					stream01 <<  std::stoll(args.commandData.optionsArgs[2]) + 1;
					winningNumbers.push_back(getNumberString(stream01.str(), redNumbers, blackNumbers));
					std::stringstream stream02{};
					stream02 <<  std::stoll(args.commandData.optionsArgs[2]) + 2;
					winningNumbers.push_back(getNumberString(stream02.str(), redNumbers, blackNumbers));
					std::stringstream stream03{};
					stream03 <<  std::stoll(args.commandData.optionsArgs[2]) + 3;
					winningNumbers.push_back(getNumberString(stream03.str(), redNumbers, blackNumbers));
					std::stringstream stream04{};
					stream04 <<  std::stoll(args.commandData.optionsArgs[2]) + 4;
					winningNumbers.push_back(getNumberString(stream04.str(), redNumbers, blackNumbers));
					std::stringstream stream05{};
					stream05 <<  std::stoll(args.commandData.optionsArgs[2]) + 5;
					winningNumbers.push_back(getNumberString(stream05.str(), redNumbers, blackNumbers));

					break;
				}
				case stringToInt("1stcolumn"):
				{
					payoutAmount = betAmount * 2;
					winningNumbers = { getNumberString("1", redNumbers, blackNumbers), getNumberString("4", redNumbers, blackNumbers), getNumberString("7", redNumbers, blackNumbers),  getNumberString("10", redNumbers, blackNumbers),
						 getNumberString("13", redNumbers, blackNumbers),  getNumberString("16", redNumbers, blackNumbers),  getNumberString("19", redNumbers, blackNumbers),  getNumberString("22", redNumbers, blackNumbers),
						 getNumberString("25", redNumbers, blackNumbers),  getNumberString("28", redNumbers, blackNumbers),  getNumberString("31", redNumbers, blackNumbers),  getNumberString("34", redNumbers, blackNumbers) };
					break;
				}
				case stringToInt("2ndcolumn"):
				{
					payoutAmount = betAmount * 2;
					winningNumbers = { getNumberString("2", redNumbers, blackNumbers),  getNumberString("5", redNumbers, blackNumbers),  getNumberString("8", redNumbers, blackNumbers),  getNumberString("11", redNumbers, blackNumbers),
						 getNumberString("14", redNumbers, blackNumbers),  getNumberString("17", redNumbers, blackNumbers),  getNumberString("20", redNumbers, blackNumbers),  getNumberString("23", redNumbers, blackNumbers),
						 getNumberString("26", redNumbers, blackNumbers),  getNumberString("29", redNumbers, blackNumbers),  getNumberString("32", redNumbers, blackNumbers),  getNumberString("35", redNumbers, blackNumbers) };
					break;
				}
				case stringToInt("3rdcolumn"):
				{
					payoutAmount = betAmount * 2;
					winningNumbers = { getNumberString("3", redNumbers, blackNumbers),  getNumberString("6", redNumbers, blackNumbers),  getNumberString("9", redNumbers, blackNumbers),  getNumberString("12", redNumbers, blackNumbers),
						 getNumberString("15", redNumbers, blackNumbers),  getNumberString("18", redNumbers, blackNumbers),  getNumberString("21", redNumbers, blackNumbers),  getNumberString("24", redNumbers, blackNumbers),
						 getNumberString("27", redNumbers, blackNumbers),  getNumberString("30", redNumbers, blackNumbers),  getNumberString("33", redNumbers, blackNumbers),  getNumberString("36", redNumbers, blackNumbers) };
					break;
				}
				case stringToInt("1stdozen"):
				{
					payoutAmount = betAmount * 2;
					for (uint32_t x = 1; x <= 12; x += 1) {
						std::stringstream stream{};
						stream << x;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}
				case stringToInt("2nddozen"):
				{
					payoutAmount = betAmount * 2;
					for (uint32_t x = 13; x <= 24; x += 1) {
						std::stringstream stream{};
						stream << x;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}	
				case stringToInt("3rddozen"):
				{
					payoutAmount = betAmount * 2;
					for (uint32_t x = 25; x <= 36; x += 1) {
						std::stringstream stream{};
						stream << x;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}					
				case stringToInt("odd"):
				{
					payoutAmount = betAmount;
					for (uint32_t x = 0; x < (36 / 2); x += 1) {
						std::stringstream stream{};
						stream << (x + 1) * 2 - 1;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}					
				case stringToInt("even"):
				{
					payoutAmount = betAmount;
					for (uint32_t x = 0; x < (36 / 2); x += 1) {
						std::stringstream stream{};
						stream << (x + 1) * 2;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}					
				case stringToInt("1to18"):
				{
					payoutAmount = betAmount;
					for (uint32_t x = 0; x < 18; x += 1) {
						std::stringstream stream{};
						stream << x + 1;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break;
				}
				case stringToInt("19to36"):
				{
					payoutAmount = betAmount;
					for (uint32_t x = 0; x < 18; x += 1) {
						std::stringstream stream{};
						stream << x + 19;
						winningNumbers.push_back(getNumberString(stream.str(), redNumbers, blackNumbers));
					}
					break; 
				}
				default: {
					break;
				}					
				}
				RouletteBet newRouletteBet{ winningNumbers, payoutAmount,  betAmount, betOptions,  betType, args.eventData->getAuthorId(), };

				discordGuild.data.rouletteGame.rouletteBets.push_back(newRouletteBet);
				discordGuild.writeDataToDB();

				DiscordGuild discordGuild2(guild);
				auto botUser = args.discordCoreClient->getBotUser();
				DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				std::string winningNumbersNew;
				for (uint32_t x = 0; x < winningNumbers.size(); x+=1){
					winningNumbersNew += winningNumbers[x].c_str();
					if (x < winningNumbers.size() - 1) {
						winningNumbersNew += ", ";
					}					
				}
				msgEmbed.setDescription("------\n__**Bet Type:**__ " + betType + "\n__**Your winning numbers are:**__\n" + winningNumbersNew + "\n__**Your winning payout would be:**__\n" + std::to_string(payoutAmount) + " " + discordUser.data.currencyName + "\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Roulette Bet Placed:**__");
				DiscordCoreAPI::RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				return;
			}
			else if (whatAreWeDoing == "start") {
				discordGuild.data.rouletteGame.currentlySpinning = true;
				discordGuild.writeDataToDB();

				int32_t currentIndex = 3;
				currentIndex = 3;
				std::unique_ptr<InputEventData> newEvent = std::make_unique<InputEventData>(*args.eventData);
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("------\n__**" + std::to_string(currentIndex * 10) + " seconds remaining to place your roulette bets!**__\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Roulette Ball Rolling:**__");
				if (currentIndex == 3) {
					DiscordCoreAPI::RespondToInputEventData dataPackage(*newEvent);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					newEvent = InputEvents::respondToEvent(dataPackage);
				}
				auto botUser = args.discordCoreClient->getBotUser();
				DiscordCoreAPI::DiscordUser discordUser(botUser.userName, botUser.id);
				currentIndex -= 1;
				std::function<void()> function01 = [&]() {
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription("------\n__**" + std::to_string(currentIndex * 10) + " seconds remaining to place your roulette bets!**__\n------");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Roulette Ball Rolling:**__");
					DiscordCoreAPI::RespondToInputEventData dataPackage(*newEvent);
					dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEvents::respondToEvent(dataPackage);
					currentIndex -= 1;
					if (currentIndex == -1) {
						DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(std::move(newEvent));
						std::stringstream stream;
						std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
						stream << static_cast<uint32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 38.0f);
						std::string finalRoll = stream.str();

						calculateResults(finalRoll, *args.eventData, discordUser, redNumbers, blackNumbers);
					}
				};

				ThreadPoolTimer::executeFunctionAfterTimePeriod(function01, 10000).get();
				ThreadPoolTimer::executeFunctionAfterTimePeriod(function01, 10000).get();
				ThreadPoolTimer::executeFunctionAfterTimePeriod(function01, 10000).get();
			}
			return;
		}
		virtual ~RouletteGame() {};
	};
}
