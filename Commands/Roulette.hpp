// Roulette.hpp - Header for the "roulette" command.
// jun 25, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	jsonifier::string getNumberString(jsonifier::string inputString, jsonifier::vector<jsonifier::string> redNumbers, jsonifier::vector<jsonifier::string> blackNumbers) {
		jsonifier::string returnString;
		for (uint32_t x = 0; x < redNumbers.size(); x += 1) {
			if (redNumbers[x].find(inputString) != jsonifier::string::npos) {
				if (redNumbers[x].substr(12) == inputString) {
					returnString = redNumbers[x];
				}
			}
		}
		for (uint32_t x = 0; x < blackNumbers.size(); x += 1) {
			if (blackNumbers[x].find(inputString) != jsonifier::string::npos) {
				if (blackNumbers[x].substr(20) == inputString) {
					returnString = blackNumbers[x];
				}
			}
		}
		if (inputString == "0") {
			returnString = ":green_square:0";
		} else if (inputString == "37") {
			returnString = ":green_square:00";
		}
		return returnString;
	}

	void calculateResults(
		jsonifier::string finalRoll, input_event_data newEvent,guild_data guild, discord_user discordUser, jsonifier::vector<jsonifier::string> redNumbers, jsonifier::vector<jsonifier::string> blackNumbers) {
		jsonifier::string msgStringFinal{};
		jsonifier::string finalRollString = getNumberString(finalRoll, redNumbers, blackNumbers);
		msgStringFinal += "------\n__**Final roll:**__ " + finalRollString + "\n------\n";
		discord_core_api::unique_ptr<discord_guild> discordGuild(discord_core_api::makeUnique<discord_guild>(managerAgent, guild));
		for (uint32_t x = 0; x < discordGuild->data.rouletteGame.rouletteBets.size(); x += 1) {
			bool isItAWinner = false;
			get_guild_member_data theData{};
			theData.guildMemberId		= discordGuild->data.rouletteGame.rouletteBets.at(x).userId;
			guild_member_data guildMember = guild_members::getCachedGuildMember(theData);
			discord_guild_member discordGuildMember(managerAgent, guildMember);
			msgStringFinal += "__**<@!" + guildMember.user.id + ">**__: ";
			int32_t betAmount							  = discordGuild->data.rouletteGame.rouletteBets.at(x).betAmount;
			int32_t payoutAmount						  = discordGuild->data.rouletteGame.rouletteBets.at(x).payoutAmount;
			jsonifier::vector<jsonifier::string> winningNumbers = discordGuild->data.rouletteGame.rouletteBets.at(x).winningNumbers;
			for (uint32_t y = 0; y < winningNumbers.size(); y += 1) {
				if (finalRollString == winningNumbers[y]) {
					isItAWinner = true;
					break;
				}
			}
			if (isItAWinner == false) {
				payoutAmount = (-1 * betAmount);
			}
			if ((uint32_t)betAmount > discordGuildMember.data.currency.wallet) {
				if (discordGuild->data.rouletteGame.rouletteBets[x].betOptions != "") {
					msgStringFinal += "__**Nsf:**__ non-sufficient funds! __**Bet:**__ " + jsonifier::toString(discordGuild->data.rouletteGame.rouletteBets[x].betAmount) + " " +
									  discordUser.data.currencyName + "__**On:**__ " + discordGuild->data.rouletteGame.rouletteBets[x].betType + ", " +
									  discordGuild->data.rouletteGame.rouletteBets[x].betOptions + "\n";
				} else {
					msgStringFinal += "__**Nsf:**__ non-sufficient funds! __ **bet:**__ " + jsonifier::toString(discordGuild->data.rouletteGame.rouletteBets[x].betAmount) + " " +
									  discordUser.data.currencyName + "__**On:**__ " + discordGuild->data.rouletteGame.rouletteBets[x].betType + "\n";
				}
			} else {
				if ((int32_t)payoutAmount > discordGuild->data.casinoStats.largestRoulettePayout.amount) {
					discordGuild->data.casinoStats.largestRoulettePayout.amount	   = payoutAmount;
					discordGuild->data.casinoStats.largestRoulettePayout.timeStamp = getTimeAndDate();
					discordGuild->data.casinoStats.largestRoulettePayout.userId	   = discordGuildMember.data.guildMemberId;
					discordGuild->data.casinoStats.largestRoulettePayout.userName  = discordGuildMember.data.userName;
				}
				discordGuild->data.casinoStats.totalRoulettePayout += payoutAmount;
				discordGuild->data.casinoStats.totalPayout += payoutAmount;
				discordGuildMember.data.currency.wallet += payoutAmount;
				discordGuildMember.writeDataToDB(managerAgent);

				if (discordGuild->data.rouletteGame.rouletteBets[x].betOptions != "") {
					msgStringFinal += jsonifier::toString(payoutAmount) + " " + discordUser.data.currencyName + " __**Bet:**__ " +
									  jsonifier::toString(discordGuild->data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + " __**On:**__ " +
									  discordGuild->data.rouletteGame.rouletteBets[x].betType + ", " + discordGuild->data.rouletteGame.rouletteBets[x].betOptions + "\n";
				} else {
					msgStringFinal += jsonifier::toString(payoutAmount) + " " + discordUser.data.currencyName + " __**Bet:**__ " +
									  jsonifier::toString(discordGuild->data.rouletteGame.rouletteBets[x].betAmount) + " " + discordUser.data.currencyName + " __**On:**__ " +
									  discordGuild->data.rouletteGame.rouletteBets[x].betType + "\n";
				}
			}
		}
		discordGuild->data.rouletteGame.currentlySpinning = false;
		discordGuild->data.rouletteGame.rouletteBets	  = jsonifier::vector<roulette_bet>();
		discordGuild->writeDataToDB(managerAgent);
		msgStringFinal += "------";
		discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
		msgEmbed->setAuthor(newEvent.getUserData().userName, newEvent.getUserData().getUserImageUrl(user_image_types::Avatar));
		msgEmbed->setDescription(msgStringFinal);
		msgEmbed->setTitle("__**Roulette results:**__");
		msgEmbed->setTimeStamp(getTimeAndDate());
		msgEmbed->setColor(discordGuild->data.borderColor);
		respond_to_input_event_data dataPackage(newEvent);
		dataPackage.setResponseType(input_event_response_type::Follow_Up_Message);
		dataPackage.addMessageEmbed(*msgEmbed);
		input_events::respondToInputEventAsync(dataPackage).get();
		return;
	}

	constexpr uint32_t stringToInt(const char* str, int32_t h = 0) {
		return !str[h] ? 5381 : (stringToInt(str, h + 1) * 33) ^ str[h];
	}

	class roulette_game : public base_function {
	  public:
		roulette_game() {
			this->commandName	  = "roulette";
			this->helpDescription = "play a game of roulette.";
			jsonifier::string newString;
			newString = "\n__**Roulette usage:**__ /roulette start to begin a game, and then /roulette bet betamount, bettype, betoptions to bet on "
						"it!\n__Where bettype and betoptions are as follows:__\n```isbl\n";
			newString += "bet type/payout:     bet options:\n------------------------------------------------------------";
			newString += "\n0        / 35:1  |";
			newString += "\n00       / 35:1  |";
			newString += "\nstraight / 35:1  | a single number to choose.";
			newString += "\nrow      / 17:1  |";
			newString += "\nsplit    / 17:1  | the first of two consecutive numbers.";
			newString += "\nstreet   / 11:1  | the first of threee consecutive numbers.";
			newString += "\nbasket   / 6:1   |";
			newString += "\nsixline  / 5:1   | the first of six consecutive numbers.";
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
			discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
			msgEmbed->setDescription(newString);
			msgEmbed->setTitle("__**Roulette usage:**__");
			msgEmbed->setTimeStamp(getTimeAndDate());
			msgEmbed->setColor("fefefe");
			this->helpEmbed = *msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<roulette_game>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				discord_core_api::unique_ptr<channel_data> channel{ discord_core_api::makeUnique<channel_data>(argsNew.getChannelData()) };

				unique_ptr<guild_cache_data> guild{ makeUnique<guild_cache_data>(argsNew.getInteractionData().guildId) };
				discord_core_api::unique_ptr<discord_guild> discordGuild(discord_core_api::makeUnique<discord_guild>(managerAgent, *guild));

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), *discordGuild);

				if (areWeAllowed == false) {
					return;
				}
				jsonifier::string whatAreWeDoing;
				uint32_t betAmount = 0;
				jsonifier::string betType;
				jsonifier::string betOptions;
				if (argsNew.getSubCommandName() == "start") {
					if (discordGuild->data.rouletteGame.currentlySpinning == true) {
						jsonifier::string msgString = "------\n**Please, wait until the current game is over, before starting another one!**\n------";
						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed->setColor(discordGuild->data.borderColor);
						msgEmbed->setDescription(msgString);
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Game issue:**__");
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
						return;
					}
					whatAreWeDoing = "start";
				} else if (argsNew.getSubCommandName() == "bet") {
					if (discordGuild->data.rouletteGame.currentlySpinning == false) {
						jsonifier::string msgString = "------\n**Please, start a roulette game before placing any bets!**\n------";
						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed->setColor(discordGuild->data.borderColor);
						msgEmbed->setDescription(msgString);
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Game issue:**__");
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
						return;
					}
					whatAreWeDoing = "bet";
				}
				if (argsNew.getCommandArguments().values.size() > 1) {
					if (argsNew.getCommandArguments().values["amount"].operator std::streamoff() <= 0) {
						jsonifier::string msgString = "------\n**Please, enter a valid betting amount! (!roulette = bet, betamount, bettype, betoptions)** \n------";
						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed->setColor(discordGuild->data.borderColor);
						msgEmbed->setDescription(msgString);
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Missing or invalid arguments:**__");
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
						return;
					} else {
						betAmount = ( uint32_t )argsNew.getCommandArguments().values["amount"].operator std::streamoff();
					}
				}
				jsonifier::vector<jsonifier::string> betTypes{ "0",
					"00",
					"straight",
					"row",
					"split",
					"street",
					"basket",
					"sixline",
					"1stcolumn",
					"2ndcolumn",
					"3rdcolumn",
					"1stdozen",
					"2nddozen",
					"3rddozen",
					"odd",
					"even",
					"red",
					"black",
					"1to18",
					"19to36" };
				jsonifier::vector<jsonifier::string> redNumbers{ ":red_square:32",
					":red_square:19",
					":red_square:21",
					":red_square:25",
					":red_square:34",
					":red_square:27",
					":red_square:36",
					":red_square:30",
					":red_square:23",
					":red_square:5",
					":red_square:16",
					":red_square:1",
					":red_square:14",
					":red_square:9",
					":red_square:18",
					":red_square:7",
					":red_square:12",
					":red_square:3" };
				jsonifier::vector<jsonifier::string> blackNumbers{ ":black_large_square:15",
					":black_large_square:4",
					":black_large_square:2",
					":black_large_square:17",
					":black_large_square:6",
					":black_large_square:13",
					":black_large_square:11",
					":black_large_square:8",
					":black_large_square:10",
					":black_large_square:24",
					":black_large_square:33",
					":black_large_square:20",
					":black_large_square:31",
					":black_large_square:22",
					":black_large_square:29",
					":black_large_square:28",
					":black_large_square:35",
					":black_large_square:26" };

				if (whatAreWeDoing == "bet") {
					guild_member_data guildMember{ argsNew.getGuildMemberData() };
					discord_guild_member discordGuildMember(managerAgent, guildMember);

					uint32_t currentBetAmount = 0;

					for (uint32_t x = 0; x < discordGuild->data.rouletteGame.rouletteBets.size(); x += 1) {
						if (discordGuildMember.data.guildMemberId == discordGuild->data.rouletteGame.rouletteBets[x].userId) {
							uint32_t number = discordGuild->data.rouletteGame.rouletteBets[x].betAmount;
							currentBetAmount += number;
						}
					}
					if ((currentBetAmount + betAmount) > discordGuildMember.data.currency.wallet) {
						jsonifier::string msgString = "------\n**sorry, but you have insufficient funds in your wallet for placing that bet!**\n------";
						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed->setColor(discordGuild->data.borderColor);
						msgEmbed->setDescription(msgString);
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Insufficient funds:**__");
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
						return;
					}

					bool isValidType = false;
					for (uint32_t x = 0; x < betTypes.size(); x += 1) {
						if (argsNew.getCommandArguments().values["type"].operator jsonifier::string() != "" &&
							argsNew.getCommandArguments().values["type"].operator jsonifier::string() == betTypes[x]) {
							isValidType = true;
							break;
						}
					}

					if (isValidType == false) {
						jsonifier::string msgString = "------\n**Please enter a valid bet type!enter '!help = roulette' for more info!(!roulette = betamount, "
												"bettype, betoptions)** \n------";
						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed->setColor(discordGuild->data.borderColor);
						msgEmbed->setDescription(msgString);
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Missing or invalid arguments:**__");
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
						return;
					} else {
						betType = argsNew.getCommandArguments().values["type"].operator jsonifier::string();
					}
					if (argsNew.getCommandArguments().values.size() > 2) {
						betOptions = argsNew.getCommandArguments().values["betOptions"].operator jsonifier::string();
					}
					uint32_t payoutAmount = 0;
					jsonifier::vector<jsonifier::string> winningNumbers;
					switch (stringToInt(betType.data())) {
					case stringToInt("0"): {
						winningNumbers.emplace_back(":green_square:0");
						payoutAmount = betAmount * 35;
						break;
					}
					case stringToInt("00"): {
						winningNumbers.emplace_back(":green_square:00");
						payoutAmount = betAmount * 35;
						break;
					}
					case stringToInt("red"): {
						payoutAmount   = betAmount;
						winningNumbers = redNumbers;
						break;
					}
					case stringToInt("black"): {
						payoutAmount   = betAmount;
						winningNumbers = blackNumbers;
						break;
					}
					case stringToInt("straight"): {
						payoutAmount = betAmount * 35;
						if (argsNew.getCommandArguments().values.size() < 3 || !argsNew.getCommandArguments().values.contains("betOptions")) {
							jsonifier::string msgString = "------\n**Please enter a valid value from the roulette wheel!(1 - 36)**\n------";
							discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
							msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed->setColor(discordGuild->data.borderColor);
							msgEmbed->setDescription(msgString);
							msgEmbed->setTimeStamp(getTimeAndDate());
							msgEmbed->setTitle("__**Missing or invalid arguments:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(*msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}

						if (argsNew.getCommandArguments().values["betoptions"].operator size_t() < 1 ||
							argsNew.getCommandArguments().values["betoptions"].operator size_t() > 36) {
							jsonifier::string msgString = "------\n**Please enter a value between 1 and 36!**\n------";
							discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
							msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed->setColor(discordGuild->data.borderColor);
							msgEmbed->setDescription(msgString);
							msgEmbed->setTimeStamp(getTimeAndDate());
							msgEmbed->setTitle("__**Missing or invalid arguments:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(*msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}

						winningNumbers.emplace_back(
							getNumberString(argsNew.getCommandArguments().values["betOptions"].operator jsonifier::string(), redNumbers, blackNumbers));

						break;
					}
					case stringToInt("row"): {
						payoutAmount = betAmount * 17;
						winningNumbers.emplace_back(":green_square:0");
						winningNumbers.emplace_back(":green_square:00");
						break;
					}
					case stringToInt("split"): {
						payoutAmount = betAmount * 17;
						if (argsNew.getCommandArguments().values.size() < 3 || !argsNew.getCommandArguments().values.contains("betOptions")) {
							jsonifier::string msgString = "------\n**Please enter a valid starting value for your split!(1 - 35)** \n------";
							discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
							msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed->setColor(discordGuild->data.borderColor);
							msgEmbed->setDescription(msgString);
							msgEmbed->setTimeStamp(getTimeAndDate());
							msgEmbed->setTitle("__**Missing or invalid arguments:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(*msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}

						if (argsNew.getCommandArguments().values["betOptions"].operator size_t() < 1 ||
							argsNew.getCommandArguments().values["betOptions"].operator size_t() > 35) {
							jsonifier::string msgString = "-------\n**please enter a value between 1 and 35!**\n------";
							discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
							msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed->setColor(discordGuild->data.borderColor);
							msgEmbed->setDescription(msgString);
							msgEmbed->setTimeStamp(getTimeAndDate());
							msgEmbed->setTitle("__**Missing or invalid arguments:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(*msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}

						winningNumbers.emplace_back(
							getNumberString(argsNew.getCommandArguments().values["betOptions"].operator jsonifier::string(), redNumbers, blackNumbers));
						std::stringstream sstream{};
						sstream << argsNew.getCommandArguments().values["betOptions"].operator size_t() + 1;
						winningNumbers.emplace_back(getNumberString(jsonifier::string{ sstream.str() }, redNumbers, blackNumbers));

						break;
					}
					case stringToInt("street"): {
						payoutAmount = betAmount * 11;
						if (argsNew.getCommandArguments().values.size() < 3 || !argsNew.getCommandArguments().values.contains("betOptions")) {
							jsonifier::string msgString = "------\n**Please enter a valid starting value for your street!(1 - 34)** \n------";
							discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
							msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed->setColor(discordGuild->data.borderColor);
							msgEmbed->setDescription(msgString);
							msgEmbed->setTimeStamp(getTimeAndDate());
							msgEmbed->setTitle("__**Missing or invalid arguments:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(*msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}

						if (argsNew.getCommandArguments().values["betOptions"].operator size_t() < 1 ||
							argsNew.getCommandArguments().values["betOptions"].operator size_t() > 34) {
							jsonifier::string msgString = "-------\n**please enter a value between 1 and 34!**\n------";
							discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
							msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed->setColor(discordGuild->data.borderColor);
							msgEmbed->setDescription(msgString);
							msgEmbed->setTimeStamp(getTimeAndDate());
							msgEmbed->setTitle("__**Missing or invalid arguments:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(*msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}

						winningNumbers.emplace_back(
							getNumberString(argsNew.getCommandArguments().values["betOptions"].operator jsonifier::string(), redNumbers, blackNumbers));
						std::stringstream stream01{};
						stream01 << argsNew.getCommandArguments().values["betOptions"].operator size_t() + 1;
						winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream01.str() }, redNumbers, blackNumbers));
						std::stringstream stream02{};
						stream02 << argsNew.getCommandArguments().values["betOptions"].operator size_t() + 2;
						winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream02.str() }, redNumbers, blackNumbers));

						break;
					}
					case stringToInt("basket"): {
						payoutAmount = betAmount * 6;
						winningNumbers.emplace_back(getNumberString("0", redNumbers, blackNumbers));
						winningNumbers.emplace_back(getNumberString("1", redNumbers, blackNumbers));
						winningNumbers.emplace_back(getNumberString("2", redNumbers, blackNumbers));
						winningNumbers.emplace_back(getNumberString("3", redNumbers, blackNumbers));
						winningNumbers.emplace_back(":green_square:00");
						break;
					}
					case stringToInt("sixline"): {
						payoutAmount = betAmount * 5;
						if (argsNew.getCommandArguments().values.size() < 3 || !argsNew.getCommandArguments().values.contains("betOptions")) {
							jsonifier::string msgString = "------\n**Please enter a valid starting value for your sixline!**\n------";
							discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
							msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed->setColor(discordGuild->data.borderColor);
							msgEmbed->setDescription(msgString);
							msgEmbed->setTimeStamp(getTimeAndDate());
							msgEmbed->setTitle("__**Missing or invalid arguments:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(*msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}

						if (argsNew.getCommandArguments().values["betOptions"].operator size_t() < 1 ||
							argsNew.getCommandArguments().values["betOptions"].operator size_t() > 31) {
							jsonifier::string msgString = "------\n * *please enter a value between 1 and 31!**\n------";
							discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
							msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed->setColor(discordGuild->data.borderColor);
							msgEmbed->setDescription(msgString);
							msgEmbed->setTimeStamp(getTimeAndDate());
							msgEmbed->setTitle("__**Missing or invalid arguments:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(*msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}

						winningNumbers.emplace_back(
							getNumberString(argsNew.getCommandArguments().values["betOptions"].operator jsonifier::string(), redNumbers, blackNumbers));
						std::stringstream stream01{};
						stream01 << argsNew.getCommandArguments().values["betOptions"].operator size_t() + 1;
						winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream01.str() }, redNumbers, blackNumbers));
						stream01 << argsNew.getCommandArguments().values["betOptions"].operator size_t() + 2;
						winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream01.str() }, redNumbers, blackNumbers));
						stream01 << argsNew.getCommandArguments().values["betOptions"].operator size_t() + 3;
						winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream01.str() }, redNumbers, blackNumbers));
						stream01 << argsNew.getCommandArguments().values["betOptions"].operator size_t() + 4;
						winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream01.str() }, redNumbers, blackNumbers));
						stream01 << argsNew.getCommandArguments().values["betOptions"].operator size_t() + 5;
						winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream01.str() }, redNumbers, blackNumbers));

						break;
					}
					case stringToInt("1stcolumn"): {
						payoutAmount   = betAmount * 2;
						winningNumbers = { getNumberString("1", redNumbers, blackNumbers),
							getNumberString("4", redNumbers, blackNumbers),
							getNumberString("7", redNumbers, blackNumbers),
							getNumberString("10", redNumbers, blackNumbers),
							getNumberString("13", redNumbers, blackNumbers),
							getNumberString("16", redNumbers, blackNumbers),
							getNumberString("19", redNumbers, blackNumbers),
							getNumberString("22", redNumbers, blackNumbers),
							getNumberString("25", redNumbers, blackNumbers),
							getNumberString("28", redNumbers, blackNumbers),
							getNumberString("31", redNumbers, blackNumbers),
							getNumberString("34", redNumbers, blackNumbers) };
						break;
					}
					case stringToInt("2ndcolumn"): {
						payoutAmount   = betAmount * 2;
						winningNumbers = { getNumberString("2", redNumbers, blackNumbers),
							getNumberString("5", redNumbers, blackNumbers),
							getNumberString("8", redNumbers, blackNumbers),
							getNumberString("11", redNumbers, blackNumbers),
							getNumberString("14", redNumbers, blackNumbers),
							getNumberString("17", redNumbers, blackNumbers),
							getNumberString("20", redNumbers, blackNumbers),
							getNumberString("23", redNumbers, blackNumbers),
							getNumberString("26", redNumbers, blackNumbers),
							getNumberString("29", redNumbers, blackNumbers),
							getNumberString("32", redNumbers, blackNumbers),
							getNumberString("35", redNumbers, blackNumbers) };
						break;
					}
					case stringToInt("3rdcolumn"): {
						payoutAmount   = betAmount * 2;
						winningNumbers = { getNumberString("3", redNumbers, blackNumbers),
							getNumberString("6", redNumbers, blackNumbers),
							getNumberString("9", redNumbers, blackNumbers),
							getNumberString("12", redNumbers, blackNumbers),
							getNumberString("15", redNumbers, blackNumbers),
							getNumberString("18", redNumbers, blackNumbers),
							getNumberString("21", redNumbers, blackNumbers),
							getNumberString("24", redNumbers, blackNumbers),
							getNumberString("27", redNumbers, blackNumbers),
							getNumberString("30", redNumbers, blackNumbers),
							getNumberString("33", redNumbers, blackNumbers),
							getNumberString("36", redNumbers, blackNumbers) };
						break;
					}
					case stringToInt("1stdozen"): {
						payoutAmount = betAmount * 2;
						for (uint32_t x = 1; x <= 12; x += 1) {
							std::stringstream stream{};
							stream << x;
							winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream.str() }, redNumbers, blackNumbers));
						}
						break;
					}
					case stringToInt("2nddozen"): {
						payoutAmount = betAmount * 2;
						for (uint32_t x = 13; x <= 24; x += 1) {
							std::stringstream stream{};
							stream << x;
							winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream.str() }, redNumbers, blackNumbers));
						}
						break;
					}
					case stringToInt("3rddozen"): {
						payoutAmount = betAmount * 2;
						for (uint32_t x = 25; x <= 36; x += 1) {
							std::stringstream stream{};
							stream << x;
							winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream.str() }, redNumbers, blackNumbers));
						}
						break;
					}
					case stringToInt("odd"): {
						payoutAmount = betAmount;
						for (uint32_t x = 0; x < (36 / 2); x += 1) {
							std::stringstream stream{};
							stream << (x + 1) * 2 - 1;
							winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream.str() }, redNumbers, blackNumbers));
						}
						break;
					}
					case stringToInt("even"): {
						payoutAmount = betAmount;
						for (uint32_t x = 0; x < (36 / 2); x += 1) {
							std::stringstream stream{};
							stream << (x + 1) * 2;
							winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream.str() }, redNumbers, blackNumbers));
						}
						break;
					}
					case stringToInt("1to18"): {
						payoutAmount = betAmount;
						for (uint32_t x = 0; x < 18; x += 1) {
							std::stringstream stream{};
							stream << x + 1;
							winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream.str() }, redNumbers, blackNumbers));
						}
						break;
					}
					case stringToInt("19to36"): {
						payoutAmount = betAmount;
						for (uint32_t x = 0; x < 18; x += 1) {
							std::stringstream stream{};
							stream << x + 19;
							winningNumbers.emplace_back(getNumberString(jsonifier::string{ stream.str() }, redNumbers, blackNumbers));
						}
						break;
					}
					default: {
						break;
					}
					}
					roulette_bet newRouletteBet{
						winningNumbers,
						betOptions,
						payoutAmount,
						betType,
						betAmount,
						argsNew.getUserData().id,
					};

					discordGuild->data.rouletteGame.rouletteBets.emplace_back(newRouletteBet);
					discordGuild->writeDataToDB(managerAgent);

					auto botUser = discord_core_client::getInstance()->getBotUser();
					jsonifier::string theString{ botUser.userName };
					discord_user discordUser(managerAgent, theString, botUser.id);
					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setColor(discordGuild->data.borderColor);
					jsonifier::string winningNumbersNew;
					for (uint32_t x = 0; x < winningNumbers.size(); x += 1) {
						winningNumbersNew += winningNumbers[x];
						if (x < winningNumbers.size() - 1) {
							winningNumbersNew += ", ";
						}
					}
					msgEmbed->setDescription("------\n__**Bet type:**__ " + betType + "\n__**Your winning numbers are:**__\n" + winningNumbersNew +
											 "\n__**Your winning payout would be:**__\n" + jsonifier::toString(payoutAmount) + " " + discordUser.data.currencyName + "\n------");
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Roulette bet placed:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				} else if (whatAreWeDoing == "start") {
					discordGuild->data.rouletteGame.currentlySpinning = true;
					discordGuild->writeDataToDB(managerAgent);

					int32_t currentIndex	= 3;
					input_event_data newEvent = argsNew.getInputEventData();
					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setColor(discordGuild->data.borderColor);
					msgEmbed->setDescription("------\n__**" + jsonifier::toString(currentIndex * 10) + " seconds remaining to place your roulette bets!**__\n------");
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Roulette ball rolling:**__");
					if (currentIndex == 3) {
						respond_to_input_event_data dataPackage(newEvent);
						dataPackage.setResponseType(input_event_response_type::Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					}
					auto botUser = discord_core_client::getInstance()->getBotUser();
					jsonifier::string theString{ botUser.userName };
					discord_user discordUser(managerAgent, theString, botUser.id);
					currentIndex -= 1;
					jsonifier::string borderColor			 = discordGuild->data.borderColor;
					std::function<void()> function01 = [&]() mutable {
						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed->setColor(borderColor);
						msgEmbed->setDescription("------\n__**" + jsonifier::toString(currentIndex * 10) + " seconds remaining to place your roulette bets!**__\n------");
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Roulette ball rolling:**__");
						respond_to_input_event_data dataPackage(newEvent);
						dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						input_events::respondToInputEventAsync(dataPackage).get();
						currentIndex -= 1;
						if (currentIndex == -1) {
							input_events::deleteInputEventResponseAsync(newEvent);
							std::mt19937_64 randomEngine{ static_cast<uint64_t>(
								std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
							jsonifier::string finalRoll = jsonifier::toString(static_cast<uint32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 38.0f));

							calculateResults(finalRoll, argsNew.getInputEventData(), *guild, discordUser, redNumbers, blackNumbers);
						}
					};

					executeFunctionAfterTimePeriod(function01, 10000, false, true);
					executeFunctionAfterTimePeriod(function01, 10000, false, true);
					executeFunctionAfterTimePeriod(function01, 10000, false, true);
				}
				return;
			} catch (const std::exception& error) {
				std::cout << "roulette_game::execute()" << error.what() << std::endl;
			}
		}
		~roulette_game(){};
	};
}// namespace discord_core_api
