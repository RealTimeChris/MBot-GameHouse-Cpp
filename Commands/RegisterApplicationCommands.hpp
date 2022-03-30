// RegisterApplicationCommands.hpp - Registers the slash commands of this bot.
// May 27, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class RegisterApplicationCommands: public BaseFunction {
	  public:
		RegisterApplicationCommands() {
			this->commandName = "registerapplicationcommands";
			this->helpDescription = "Registers some application commands.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /RegisterApplicationCommands.\n------");
			msgEmbed.setTitle("__**Register Slash Commands Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<RegisterApplicationCommands>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

				std::unique_ptr<InputEventData> newEvent = std::make_unique<InputEventData>(*args.eventData);

				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
				if (args.eventData->eventType == InteractionType::Application_Command) {
					newEvent = InputEvents::respondToEvent(dataPackage);
				}

				CreateGlobalApplicationCommandData RegisterApplicationCommandsCommandData;
				RegisterApplicationCommandsCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				RegisterApplicationCommandsCommandData.defaultPermission = true;
				RegisterApplicationCommandsCommandData.description = "Register the programmatically designated slash commands.";
				RegisterApplicationCommandsCommandData.name = "registerapplicationcommands";
				RegisterApplicationCommandsCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(RegisterApplicationCommandsCommandData).get();

				CreateGlobalApplicationCommandData createSellDrugsCommandData;
				createSellDrugsCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createSellDrugsCommandData.defaultPermission = true;
				createSellDrugsCommandData.description = "Sell drugs in exchange for some currency!";
				createSellDrugsCommandData.name = "selldrugs";
				createSellDrugsCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createSellDrugsCommandData).get();

				CreateGlobalApplicationCommandData createBalanceCommandData;
				createBalanceCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createBalanceCommandData.defaultPermission = true;
				createBalanceCommandData.description = "Check your or another person's currency balances.";
				createBalanceCommandData.name = "balance";
				ApplicationCommandOptionData applicationCommandOptionOne;
				applicationCommandOptionOne.name = "person";
				applicationCommandOptionOne.required = false;
				applicationCommandOptionOne.type = ApplicationCommandOptionType::User;
				applicationCommandOptionOne.description = "The person who's balances you would like to check.";
				createBalanceCommandData.options.push_back(applicationCommandOptionOne);
				createBalanceCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createBalanceCommandData).get();

				CreateGlobalApplicationCommandData createAddShopItemCommandData;
				createAddShopItemCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createAddShopItemCommandData.defaultPermission = true;
				createAddShopItemCommandData.description = "Add an item to the shop's inventory.";
				createAddShopItemCommandData.name = "addshopitem";
				ApplicationCommandOptionData addShopItemCommandOptionOne;
				addShopItemCommandOptionOne.name = "itemname";
				addShopItemCommandOptionOne.required = true;
				addShopItemCommandOptionOne.type = ApplicationCommandOptionType::String;
				addShopItemCommandOptionOne.description = "The name of the item.";
				createAddShopItemCommandData.options.push_back(addShopItemCommandOptionOne);
				ApplicationCommandOptionData addShopItemCommandOptionTwo;
				addShopItemCommandOptionTwo.name = "selfmod";
				addShopItemCommandOptionTwo.required = true;
				addShopItemCommandOptionTwo.type = ApplicationCommandOptionType::Integer;
				addShopItemCommandOptionTwo.description = "The self-mod value of the item.";
				createAddShopItemCommandData.options.push_back(addShopItemCommandOptionTwo);
				ApplicationCommandOptionData addShopItemCommandOptionThree;
				addShopItemCommandOptionThree.name = "oppmod";
				addShopItemCommandOptionThree.required = true;
				addShopItemCommandOptionThree.type = ApplicationCommandOptionType::Integer;
				addShopItemCommandOptionThree.description = "The opp-mod value of the item.";
				createAddShopItemCommandData.options.push_back(addShopItemCommandOptionThree);
				ApplicationCommandOptionData addShopItemCommandOptionFour;
				addShopItemCommandOptionFour.name = "itemcost";
				addShopItemCommandOptionFour.required = true;
				addShopItemCommandOptionFour.type = ApplicationCommandOptionType::Integer;
				addShopItemCommandOptionFour.description = "The value/cost of the item.";
				createAddShopItemCommandData.options.push_back(addShopItemCommandOptionFour);
				ApplicationCommandOptionData addShopItemCommandOptionFive;
				addShopItemCommandOptionFive.name = "emoji";
				addShopItemCommandOptionFive.required = true;
				addShopItemCommandOptionFive.type = ApplicationCommandOptionType::String;
				addShopItemCommandOptionFive.description = "The emoji/icon to use for the item.";
				createAddShopItemCommandData.options.push_back(addShopItemCommandOptionFive);
				createAddShopItemCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createAddShopItemCommandData).get();

				CreateGlobalApplicationCommandData createAddShopRoleCommandData;
				createAddShopRoleCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createAddShopRoleCommandData.defaultPermission = true;
				createAddShopRoleCommandData.description = "Add a role to the shop's inventory.";
				createAddShopRoleCommandData.name = "addshoprole";
				ApplicationCommandOptionData addShopRoleCommandOptionOne;
				addShopRoleCommandOptionOne.name = "rolename";
				addShopRoleCommandOptionOne.required = true;
				addShopRoleCommandOptionOne.type = ApplicationCommandOptionType::String;
				addShopRoleCommandOptionOne.description = "The name of the role.";
				createAddShopRoleCommandData.options.push_back(addShopRoleCommandOptionOne);
				ApplicationCommandOptionData addShopRoleCommandOptionTwo;
				addShopRoleCommandOptionTwo.name = "hexcolorvalue";
				addShopRoleCommandOptionTwo.required = true;
				addShopRoleCommandOptionTwo.type = ApplicationCommandOptionType::String;
				addShopRoleCommandOptionTwo.description = "The hex-color value of the role.";
				createAddShopRoleCommandData.options.push_back(addShopRoleCommandOptionTwo);
				ApplicationCommandOptionData addShopRoleCommandOptionThree;
				addShopRoleCommandOptionThree.name = "rolecost";
				addShopRoleCommandOptionThree.required = true;
				addShopRoleCommandOptionThree.type = ApplicationCommandOptionType::Integer;
				addShopRoleCommandOptionThree.description = "The value/cost of the role.";
				createAddShopRoleCommandData.options.push_back(addShopRoleCommandOptionThree);
				createAddShopRoleCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createAddShopRoleCommandData).get();

				CreateGlobalApplicationCommandData createCoinflipRoleCommandData;
				createCoinflipRoleCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createCoinflipRoleCommandData.defaultPermission = true;
				createCoinflipRoleCommandData.description = "Play heads or tails.";
				createCoinflipRoleCommandData.name = "coinflip";
				ApplicationCommandOptionData coinflipCommandOptionOne;
				coinflipCommandOptionOne.name = "betamount";
				coinflipCommandOptionOne.required = true;
				coinflipCommandOptionOne.type = ApplicationCommandOptionType::Integer;
				coinflipCommandOptionOne.description = "The wager you would like to place.";
				createCoinflipRoleCommandData.options.push_back(coinflipCommandOptionOne);
				createCoinflipRoleCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createCoinflipRoleCommandData).get();

				CreateGlobalApplicationCommandData createBlackjackCommandData;
				createBlackjackCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createBlackjackCommandData.defaultPermission = true;
				createBlackjackCommandData.description = "Play a round of blackjack.";
				createBlackjackCommandData.name = "blackjack";
				ApplicationCommandOptionData blackJackCommandOptionOne;
				blackJackCommandOptionOne.name = "betamount";
				blackJackCommandOptionOne.required = true;
				blackJackCommandOptionOne.type = ApplicationCommandOptionType::Integer;
				blackJackCommandOptionOne.description = "The amount which you would like to wager.";
				createBlackjackCommandData.options.push_back(blackJackCommandOptionOne);
				createBlackjackCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createBlackjackCommandData).get();

				CreateGlobalApplicationCommandData createButtonsCommandData;
				createButtonsCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createButtonsCommandData.defaultPermission = true;
				createButtonsCommandData.description = "Test the buttons.";
				createButtonsCommandData.name = "buttons";
				createButtonsCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createButtonsCommandData).get();

				CreateGlobalApplicationCommandData createShopCommandData;
				createShopCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createShopCommandData.defaultPermission = true;
				createShopCommandData.description = "Check out the server's shop!";
				createShopCommandData.name = "shop";
				createShopCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createShopCommandData).get();

				CreateGlobalApplicationCommandData createBotInfoCommandData;
				createBotInfoCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createBotInfoCommandData.defaultPermission = true;
				createBotInfoCommandData.description = "Displays info about the current bot.";
				createBotInfoCommandData.name = "botinfo";
				createBotInfoCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createBotInfoCommandData).get();

				CreateGlobalApplicationCommandData createCasinoStatsCommandData;
				createCasinoStatsCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createCasinoStatsCommandData.defaultPermission = true;
				createCasinoStatsCommandData.description = "View the server's casino stats.";
				createCasinoStatsCommandData.name = "casinostats";
				createCasinoStatsCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createCasinoStatsCommandData).get();

				CreateGlobalApplicationCommandData createDisplayGuildsDataCommandData;
				createDisplayGuildsDataCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createDisplayGuildsDataCommandData.defaultPermission = true;
				createDisplayGuildsDataCommandData.description = "View the list of servers that this bot is in.";
				createDisplayGuildsDataCommandData.name = "displayguildsdata";
				createDisplayGuildsDataCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createDisplayGuildsDataCommandData).get();

				CreateGlobalApplicationCommandData createDuelCommandData;
				createDuelCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createDuelCommandData.defaultPermission = true;
				createDuelCommandData.description = "Challenge another server member do a duel.";
				createDuelCommandData.name = "duel";
				ApplicationCommandOptionData duelCommandOptionOne;
				duelCommandOptionOne.name = "challengetarget";
				duelCommandOptionOne.required = true;
				duelCommandOptionOne.type = ApplicationCommandOptionType::User;
				duelCommandOptionOne.description = "The individual you are challenging to a duel.";
				createDuelCommandData.options.push_back(duelCommandOptionOne);
				ApplicationCommandOptionData duelCommandOptionTwo;
				duelCommandOptionTwo.name = "betamount";
				duelCommandOptionTwo.required = true;
				duelCommandOptionTwo.type = ApplicationCommandOptionType::Integer;
				duelCommandOptionTwo.description = "The amount of the wager you are putting up.";
				createDuelCommandData.options.push_back(duelCommandOptionTwo);
				createDuelCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createDuelCommandData).get();

				CreateGlobalApplicationCommandData createGamehouseOptionsCommandData;
				createGamehouseOptionsCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createGamehouseOptionsCommandData.defaultPermission = true;
				createGamehouseOptionsCommandData.description = "View the options of this bot.";
				createGamehouseOptionsCommandData.name = "gamehouseoptions";
				createGamehouseOptionsCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createGamehouseOptionsCommandData).get();

				CreateGlobalApplicationCommandData createInventoryCommandData;
				createInventoryCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createInventoryCommandData.defaultPermission = true;
				createInventoryCommandData.description = "Observe your inventory of items and roles.";
				createInventoryCommandData.name = "inventory";
				ApplicationCommandOptionData inventoryCommandOptionOne;
				inventoryCommandOptionOne.name = "user";
				inventoryCommandOptionOne.required = false;
				inventoryCommandOptionOne.type = ApplicationCommandOptionType::User;
				inventoryCommandOptionOne.description = "The user who's inventory you would like to view.";
				createInventoryCommandData.options.push_back(inventoryCommandOptionOne);
				createInventoryCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createInventoryCommandData).get();

				CreateGlobalApplicationCommandData createLeaderboardCommandData;
				createLeaderboardCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createLeaderboardCommandData.defaultPermission = true;
				createLeaderboardCommandData.description = "View the server's currency leaderboard.";
				createLeaderboardCommandData.name = "leaderboard";
				createLeaderboardCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createLeaderboardCommandData).get();

				CreateGlobalApplicationCommandData createRemoveObjectCommandData;
				createRemoveObjectCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createRemoveObjectCommandData.defaultPermission = true;
				createRemoveObjectCommandData.description = "Remove an object or role from a user's inventory.";
				createRemoveObjectCommandData.name = "removeobject";
				ApplicationCommandOptionData testCommandOptionOne;
				testCommandOptionOne.name = "objectname";
				testCommandOptionOne.required = false;
				testCommandOptionOne.type = ApplicationCommandOptionType::String;
				testCommandOptionOne.description = "The object to be removed.";
				createRemoveObjectCommandData.options.push_back(testCommandOptionOne);
				ApplicationCommandOptionData testCommandOptionTwo;
				testCommandOptionTwo.name = "role";
				testCommandOptionTwo.required = false;
				testCommandOptionTwo.type = ApplicationCommandOptionType::Role;
				testCommandOptionTwo.description = "The role to be removed.";
				createRemoveObjectCommandData.options.push_back(testCommandOptionTwo);
				ApplicationCommandOptionData testCommandOptionThree;
				testCommandOptionThree.name = "user";
				testCommandOptionThree.required = false;
				testCommandOptionThree.type = ApplicationCommandOptionType::User;
				testCommandOptionThree.description = "The user to remove the object/role from.";
				createRemoveObjectCommandData.options.push_back(testCommandOptionThree);
				createRemoveObjectCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createRemoveObjectCommandData).get();

				CreateGlobalApplicationCommandData createRemoveShopItemCommandData;
				createRemoveShopItemCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createRemoveShopItemCommandData.defaultPermission = true;
				createRemoveShopItemCommandData.description = "Remove an item from the server's shop.";
				createRemoveShopItemCommandData.name = "removeshopitem";
				ApplicationCommandOptionData removeShopItemOptionOne;
				removeShopItemOptionOne.name = "item";
				removeShopItemOptionOne.required = true;
				removeShopItemOptionOne.type = ApplicationCommandOptionType::String;
				removeShopItemOptionOne.description = "The item which you would like to remove.";
				createRemoveShopItemCommandData.options.push_back(removeShopItemOptionOne);
				createRemoveShopItemCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createRemoveShopItemCommandData).get();

				CreateGlobalApplicationCommandData createRemoveShopRoleCommandData;
				createRemoveShopRoleCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createRemoveShopRoleCommandData.defaultPermission = true;
				createRemoveShopRoleCommandData.description = "Remove a role from the server's shop.";
				createRemoveShopRoleCommandData.name = "removeshoprole";
				ApplicationCommandOptionData removeShopRoleOptionOne;
				removeShopRoleOptionOne.name = "role";
				removeShopRoleOptionOne.required = true;
				removeShopRoleOptionOne.type = ApplicationCommandOptionType::Role;
				removeShopRoleOptionOne.description = "The role which you would like to remove.";
				createRemoveShopRoleCommandData.options.push_back(removeShopRoleOptionOne);
				createRemoveShopRoleCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createRemoveShopRoleCommandData).get();

				CreateGlobalApplicationCommandData createRobCommandData;
				createRobCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createRobCommandData.defaultPermission = true;
				createRobCommandData.description = "Rob a user for currency.";
				createRobCommandData.name = "rob";
				ApplicationCommandOptionData robOptionOne;
				robOptionOne.name = "user";
				robOptionOne.required = true;
				robOptionOne.type = ApplicationCommandOptionType::User;
				robOptionOne.description = "The user which you would like to rob.";
				createRobCommandData.options.push_back(robOptionOne);
				createRobCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createRobCommandData).get();

				CreateGlobalApplicationCommandData createRouletteCommandData;
				createRouletteCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createRouletteCommandData.defaultPermission = true;
				createRouletteCommandData.description = "Start or bet on a game of roulette.";
				createRouletteCommandData.name = "roulette";
				ApplicationCommandOptionData rouletteOptionOne;
				rouletteOptionOne.name = "start";
				rouletteOptionOne.type = ApplicationCommandOptionType::Sub_Command;
				rouletteOptionOne.description = "Begins the roulette round.";
				createRouletteCommandData.options.push_back(rouletteOptionOne);
				ApplicationCommandOptionData rouletteOptionTwo;
				rouletteOptionTwo.name = "bet1";
				rouletteOptionTwo.type = ApplicationCommandOptionType::Sub_Command;
				rouletteOptionTwo.description = "Bet on an active game of roulette.";
				ApplicationCommandOptionData rouletteOptionTwoOh;
				rouletteOptionTwoOh.name = "amount";
				rouletteOptionTwoOh.description = "Enter the amount which you would like to bet";
				rouletteOptionTwoOh.required = true;
				rouletteOptionTwoOh.type = ApplicationCommandOptionType::Integer;
				rouletteOptionTwo.options.push_back(rouletteOptionTwoOh);
				ApplicationCommandOptionData rouletteOptionTwoOne;
				rouletteOptionTwoOne.name = "type";
				rouletteOptionTwoOne.type = ApplicationCommandOptionType::String;
				rouletteOptionTwoOne.required = true;
				rouletteOptionTwoOne.description = "Choose the type of bet you would like to make.";
				ApplicationCommandOptionChoiceData choice01;
				choice01.name = "0";
				choice01.valueString = "0";
				rouletteOptionTwoOne.choices.push_back(choice01);
				ApplicationCommandOptionChoiceData choice02;
				choice02.name = "00";
				choice02.valueString = "00";
				rouletteOptionTwoOne.choices.push_back(choice02);
				ApplicationCommandOptionChoiceData choice03;
				choice03.name = "straight";
				choice03.valueString = "straight";
				rouletteOptionTwoOne.choices.push_back(choice03);
				ApplicationCommandOptionChoiceData choice04;
				choice04.name = "row";
				choice04.valueString = "row";
				rouletteOptionTwoOne.choices.push_back(choice04);
				ApplicationCommandOptionChoiceData choice05;
				choice05.name = "split";
				choice05.valueString = "split";
				rouletteOptionTwoOne.choices.push_back(choice05);
				ApplicationCommandOptionChoiceData choice06;
				choice06.name = "street";
				choice06.valueString = "street";
				rouletteOptionTwoOne.choices.push_back(choice06);
				ApplicationCommandOptionChoiceData choice07;
				choice07.name = "basket";
				choice07.valueString = "basket";
				rouletteOptionTwoOne.choices.push_back(choice07);
				ApplicationCommandOptionChoiceData choice08;
				choice07.name = "sixline";
				choice07.valueString = "sixline";
				rouletteOptionTwoOne.choices.push_back(choice08);
				ApplicationCommandOptionChoiceData choice09;
				choice09.name = "1stcolumn";
				choice09.valueString = "1stcolumn";
				rouletteOptionTwoOne.choices.push_back(choice09);
				ApplicationCommandOptionChoiceData choice10;
				choice10.name = "2ndcolumn";
				choice10.valueString = "2ndcolumn";
				rouletteOptionTwoOne.choices.push_back(choice10);
				rouletteOptionTwo.options.push_back(rouletteOptionTwoOne);
				ApplicationCommandOptionData rouletteOptionTwoTwo;
				rouletteOptionTwoTwo.name = "betoptions";
				rouletteOptionTwoTwo.description = "Bet options for select bet-types.";
				rouletteOptionTwoOne.required = false;
				rouletteOptionTwoTwo.type = ApplicationCommandOptionType::Integer;
				rouletteOptionTwo.options.push_back(rouletteOptionTwoTwo);
				createRouletteCommandData.options.push_back(rouletteOptionTwo);
				ApplicationCommandOptionData rouletteOptionThree;
				rouletteOptionThree.name = "bet2";
				rouletteOptionThree.type = ApplicationCommandOptionType::Sub_Command;
				rouletteOptionThree.description = "Bet on an active game of roulette.";
				ApplicationCommandOptionData rouletteOptionThreeOh;
				rouletteOptionThreeOh.name = "amount";
				rouletteOptionThreeOh.description = "Enter the amount which you would like to bet";
				rouletteOptionThreeOh.required = true;
				rouletteOptionThreeOh.type = ApplicationCommandOptionType::Integer;
				rouletteOptionThree.options.push_back(rouletteOptionThreeOh);
				ApplicationCommandOptionData rouletteOptionThreeOne;
				rouletteOptionThreeOne.name = "type";
				rouletteOptionThreeOne.type = ApplicationCommandOptionType::String;
				rouletteOptionThreeOne.required = true;
				rouletteOptionThreeOne.description = "Choose the type of bet you would like to make.";
				ApplicationCommandOptionChoiceData choice11;
				choice11.name = "3rdcolumn";
				choice11.valueString = "3rdcolumn";
				rouletteOptionThreeOne.choices.push_back(choice11);
				ApplicationCommandOptionChoiceData choice12;
				choice12.name = "1stdozen";
				choice12.valueString = "1stdozen";
				rouletteOptionThreeOne.choices.push_back(choice12);
				ApplicationCommandOptionChoiceData choice13;
				choice13.name = "2nddozen";
				choice13.valueString = "2nddozen";
				rouletteOptionThreeOne.choices.push_back(choice13);
				ApplicationCommandOptionChoiceData choice14;
				choice14.name = "3rddozen";
				choice14.valueString = "3rddozen";
				rouletteOptionThreeOne.choices.push_back(choice14);
				ApplicationCommandOptionChoiceData choice15;
				choice15.name = "odd";
				choice15.valueString = "odd";
				rouletteOptionThreeOne.choices.push_back(choice15);
				ApplicationCommandOptionChoiceData choice16;
				choice16.name = "even";
				choice16.valueString = "even";
				rouletteOptionThreeOne.choices.push_back(choice16);
				ApplicationCommandOptionChoiceData choice17;
				choice17.name = "red";
				choice17.valueString = "red";
				rouletteOptionThreeOne.choices.push_back(choice17);
				ApplicationCommandOptionChoiceData choice18;
				choice18.name = "black";
				choice18.valueString = "black";
				rouletteOptionThreeOne.choices.push_back(choice18);
				ApplicationCommandOptionChoiceData choice19;
				choice19.name = "1to18";
				choice19.valueString = "1to18";
				rouletteOptionThreeOne.choices.push_back(choice19);
				ApplicationCommandOptionChoiceData choice20;
				choice20.name = "19to36";
				choice20.valueString = "19to36";
				rouletteOptionThreeOne.choices.push_back(choice20);
				rouletteOptionThree.options.push_back(rouletteOptionThreeOne);
				ApplicationCommandOptionData rouletteOptionThreeTwo;
				rouletteOptionThreeTwo.name = "betoptions";
				rouletteOptionThreeTwo.description = "Bet options for select bet-types.";
				rouletteOptionThreeTwo.required = false;
				rouletteOptionThreeTwo.type = ApplicationCommandOptionType::Integer;
				rouletteOptionThree.options.push_back(rouletteOptionThreeTwo);
				createRouletteCommandData.options.push_back(rouletteOptionThree);
				createRouletteCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createRouletteCommandData).get();

				CreateGlobalApplicationCommandData createSetGameChannelCommandData;
				createSetGameChannelCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createSetGameChannelCommandData.defaultPermission = true;
				createSetGameChannelCommandData.description = "Sets the channels which you can play games in.";
				createSetGameChannelCommandData.name = "setgamechannel";
				ApplicationCommandOptionData createSetGameChannelOptionOne;
				createSetGameChannelOptionOne.type = ApplicationCommandOptionType::Sub_Command;
				createSetGameChannelOptionOne.name = "view";
				createSetGameChannelOptionOne.description = "View the currently enabled channels.";
				createSetGameChannelCommandData.options.push_back(createSetGameChannelOptionOne);
				ApplicationCommandOptionData createSetGameChannelOptionTwo;
				createSetGameChannelOptionTwo.type = ApplicationCommandOptionType::Sub_Command;
				createSetGameChannelOptionTwo.name = "add";
				createSetGameChannelOptionTwo.description = "Add a channel to the list of enabled channels.";
				createSetGameChannelCommandData.options.push_back(createSetGameChannelOptionTwo);
				ApplicationCommandOptionData createSetGameChannelOptionThree;
				createSetGameChannelOptionThree.type = ApplicationCommandOptionType::Sub_Command;
				createSetGameChannelOptionThree.name = "remove";
				createSetGameChannelOptionThree.description = "Remove a channel to the list of enabled channels.";
				createSetGameChannelCommandData.options.push_back(createSetGameChannelOptionThree);
				ApplicationCommandOptionData createSetGameChannelOptionFour;
				createSetGameChannelOptionFour.type = ApplicationCommandOptionType::Sub_Command;
				createSetGameChannelOptionFour.name = "purge";
				createSetGameChannelOptionFour.description = "Purges the list of channels for the server.";
				createSetGameChannelCommandData.options.push_back(createSetGameChannelOptionFour);
				createSetGameChannelCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createSetGameChannelCommandData).get();

				CreateGlobalApplicationCommandData createSetBalanceCommandData;
				createSetBalanceCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createSetBalanceCommandData.defaultPermission = true;
				createSetBalanceCommandData.description = "Set the currency balance of yourself or another server member.";
				createSetBalanceCommandData.name = "setbalance";
				ApplicationCommandOptionData createSetBalanceOptionOne;
				createSetBalanceOptionOne.type = ApplicationCommandOptionType::Integer;
				createSetBalanceOptionOne.name = "amount";
				createSetBalanceOptionOne.required = true;
				createSetBalanceOptionOne.description = "The amount to set the balance to.";
				createSetBalanceCommandData.options.push_back(createSetBalanceOptionOne);
				ApplicationCommandOptionData createBalanceOptionOne;
				createBalanceOptionOne.type = ApplicationCommandOptionType::String;
				createBalanceOptionOne.name = "balancetype";
				createBalanceOptionOne.description = "Which of the two balances to set.";
				createBalanceOptionOne.required = true;
				ApplicationCommandOptionChoiceData setBalancechoiceOne;
				setBalancechoiceOne.name = "wallet";
				setBalancechoiceOne.valueString = "wallet";
				ApplicationCommandOptionChoiceData setBalancechoiceTwo;
				setBalancechoiceTwo.name = "bank";
				setBalancechoiceTwo.valueString = "bank";
				createBalanceOptionOne.choices.push_back(setBalancechoiceOne);
				createBalanceOptionOne.choices.push_back(setBalancechoiceTwo);
				createSetBalanceCommandData.options.push_back(createBalanceOptionOne);
				ApplicationCommandOptionData createSetBalanceOptionThree;
				createSetBalanceOptionThree.name = "targetuser";
				createSetBalanceOptionThree.description = "The user who's balance you would like to set.";
				createSetBalanceOptionThree.required = false;
				createSetBalanceOptionThree.type = ApplicationCommandOptionType::User;
				createSetBalanceCommandData.options.push_back(createSetBalanceOptionThree);
				createSetBalanceCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createSetBalanceCommandData).get();

				CreateGlobalApplicationCommandData createSetBorderColorCommandData;
				createSetBorderColorCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createSetBorderColorCommandData.defaultPermission = true;
				createSetBorderColorCommandData.description = "Set the defaul color of borders.";
				createSetBorderColorCommandData.name = "setbordercolor";
				ApplicationCommandOptionData createSetBorderColoreOptionOne;
				createSetBorderColoreOptionOne.type = ApplicationCommandOptionType::String;
				createSetBorderColoreOptionOne.name = "botname";
				createSetBorderColoreOptionOne.description = "Which of the bots to change the setting on.";
				createSetBorderColoreOptionOne.required = true;
				ApplicationCommandOptionChoiceData setBorderColorchoiceOne;
				setBorderColorchoiceOne.name = "gamehouse";
				setBorderColorchoiceOne.valueString = "gamehouse";
				createSetBorderColoreOptionOne.choices.push_back(setBorderColorchoiceOne);
				createSetBorderColorCommandData.options.push_back(createSetBorderColoreOptionOne);
				ApplicationCommandOptionData createSetBorderColorOptionTwo;
				createSetBorderColorOptionTwo.type = ApplicationCommandOptionType::String;
				createSetBorderColorOptionTwo.name = "hexcolorvalue";
				createSetBorderColorOptionTwo.required = true;
				createSetBorderColorOptionTwo.description = "The hex-color-value to set the borders to.";
				createSetBorderColorCommandData.options.push_back(createSetBorderColorOptionTwo);
				createSetBorderColorCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createSetBorderColorCommandData).get();

				CreateGlobalApplicationCommandData createSlotsCommandData;
				createSlotsCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createSlotsCommandData.defaultPermission = true;
				createSlotsCommandData.description = "Play a game of slots.";
				createSlotsCommandData.name = "slots";
				ApplicationCommandOptionData createSlotsOptionOne;
				createSlotsOptionOne.type = ApplicationCommandOptionType::Integer;
				createSlotsOptionOne.name = "betamount";
				createSlotsOptionOne.description = "The amount of currency which you will wager.";
				createSlotsOptionOne.required = true;
				createSlotsCommandData.options.push_back(createSlotsOptionOne);
				createSlotsCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createSlotsCommandData).get();

				CreateGlobalApplicationCommandData createTransferCommandData;
				createTransferCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createTransferCommandData.defaultPermission = true;
				createTransferCommandData.description = "Transfer currency from yourself to another server member.";
				createTransferCommandData.name = "transfer";
				ApplicationCommandOptionData createTransferOptionOne;
				createTransferOptionOne.type = ApplicationCommandOptionType::Integer;
				createTransferOptionOne.name = "amount";
				createTransferOptionOne.description = "The amount of currency which you would like to transfer.";
				createTransferOptionOne.required = true;
				createTransferCommandData.options.push_back(createTransferOptionOne);
				ApplicationCommandOptionData createTransferOptionTwo;
				createTransferOptionTwo.type = ApplicationCommandOptionType::User;
				createTransferOptionTwo.name = "user";
				createTransferOptionTwo.description = "The target user for the transfer.";
				createTransferOptionTwo.required = true;
				createTransferCommandData.options.push_back(createTransferOptionTwo);
				createTransferCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createTransferCommandData).get();

				CreateGlobalApplicationCommandData createWithdrawCommandData;
				createWithdrawCommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createWithdrawCommandData.defaultPermission = true;
				createWithdrawCommandData.description = "Withdraw currency from your bank account to your wallet.";
				createWithdrawCommandData.name = "withdraw";
				ApplicationCommandOptionData createWithdrawOptionOne;
				createWithdrawOptionOne.type = ApplicationCommandOptionType::Integer;
				createWithdrawOptionOne.name = "amount";
				createWithdrawOptionOne.description = "The amount of currency which you would like to withdraw.";
				createWithdrawOptionOne.required = true;
				createWithdrawCommandData.options.push_back(createWithdrawOptionOne);
				createWithdrawCommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createWithdrawCommandData).get();

				CreateGlobalApplicationCommandData createDepositommandData;
				createDepositommandData.applicationId = args.discordCoreClient->getBotUser().id;
				createDepositommandData.defaultPermission = true;
				createDepositommandData.description = "Deposit currency from your wallet into your bank.";
				createDepositommandData.name = "deposit";
				ApplicationCommandOptionData createDepositOptionOne;
				createDepositOptionOne.type = ApplicationCommandOptionType::Integer;
				createDepositOptionOne.name = "amount";
				createDepositOptionOne.description = "The amount of currency which you would like to deposit.";
				createDepositOptionOne.required = true;
				createDepositommandData.options.push_back(createDepositOptionOne);
				createDepositommandData.type = ApplicationCommandType::Chat_Input;
				ApplicationCommands::createGlobalApplicationCommandAsync(createDepositommandData).get();

				CreateGlobalApplicationCommandData createHelpData;
				createHelpData.applicationId = args.discordCoreClient->getBotUser().id;
				createHelpData.defaultPermission = true;
				createHelpData.type = ApplicationCommandType::Chat_Input;
				createHelpData.description = "Get help with this bot's commands.";
				createHelpData.name = "help";
				ApplicationCommands::createGlobalApplicationCommandAsync(createHelpData).get();

				CreateGlobalApplicationCommandData createTestData;
				createTestData.applicationId = args.discordCoreClient->getBotUser().id;
				createTestData.type = ApplicationCommandType::Chat_Input;
				createTestData.name = "test";
				createTestData.defaultPermission = true;
				createTestData.description = "Test command.";
				ApplicationCommands::createGlobalApplicationCommandAsync(createTestData).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
				DiscordGuild discordGuild(guild);
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription("------\nNicely done, you've registered some commands!\n------");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Register Application Commands Complete:**__");
				RespondToInputEventData dataPackage02(*args.eventData);
				dataPackage02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				dataPackage02.addMessageEmbed(msgEmbed);
				auto event = InputEvents::respondToEvent(dataPackage02);
				return;
			} catch (...) {
				reportException("RegisterApplicationCommands::execute()");
			}
		}
		virtual ~RegisterApplicationCommands(){};
	};
}
