// main.cpp - Main entry point.
// jun 17, 2021
// chris m.
// https://github.com/real_time_chris

#include "commands/CommandsList.hpp"

discord_core_api::co_routine<void> onGuildCreation(discord_core_api::on_guild_creation_data dataPackage) {
	co_await discord_core_api::newThreadAwaitable<void>();
	discord_core_api::discord_guild discordGuild{ discord_core_api::managerAgent, dataPackage.value };
	discordGuild.getDataFromDB(discord_core_api::managerAgent);
	discordGuild.data.rouletteGame.currentlySpinning = false;
	discordGuild.writeDataToDB(discord_core_api::managerAgent);
	co_return;
}

void onBoot00(discord_core_api::discord_core_client* args) {
	auto botUser = args->getBotUser();
	discord_core_api::managerAgent.initialize(botUser.id);
	jsonifier::string theString{ botUser.userName };
	discord_core_api::discord_user theUser{ discord_core_api::managerAgent, theString, botUser.id };
	theUser.writeDataToDB(discord_core_api::managerAgent);
}

int32_t main() {
	jsonifier::string botToken = "";
	jsonifier::vector<discord_core_api::repeated_function_data> functionVector{};
	functionVector.reserve(5);
	discord_core_api::repeated_function_data function01{};
	function01.function		= onBoot00;
	function01.repeated		= false;
	function01.intervalInMs = 1500;
	functionVector.emplace_back(function01);
	discord_core_api::sharding_options shardOptions{};
	shardOptions.numberOfShardsForThisProcess = 1;
	shardOptions.startingShard				  = 0;
	shardOptions.totalNumberOfShards		  = 1;
	discord_core_api::logging_options logOptions{};
	logOptions.logWebSocketErrorMessages   = true;
	logOptions.logGeneralErrorMessages	 = true;
	logOptions.logHttpsErrorMessages	   = true;
	discord_core_api::discord_core_client_config clientConfig{};
	//clientConfig.connectionAddress = "127.0.0.1";
	clientConfig.botToken						= botToken;
	clientConfig.logOptions						= logOptions;
	clientConfig.shardOptions					= shardOptions;
	clientConfig.cacheOptions.cacheChannels		= true;
	clientConfig.cacheOptions.cacheGuilds		= true;
	clientConfig.cacheOptions.cacheUsers		= true;
	clientConfig.cacheOptions.cacheGuildMembers = true;
	clientConfig.cacheOptions.cacheRoles		= true;
	clientConfig.functionsToExecute				= functionVector;
	jsonifier::vector<discord_core_api::activity_data> activities{};
	discord_core_api::activity_data activity{};
	activity.name = "/help for my commands!";
	activity.type = discord_core_api::activity_type::game;
	activities.emplace_back(activity);
	clientConfig.presenceData.activities = activities;
	clientConfig.presenceData.afk		 = false;
	clientConfig.textFormat				 = discord_core_api::text_format::etf;
	clientConfig.presenceData.since		 = 0;
	clientConfig.presenceData.status	 = discord_core_api::presence_update_state::online;
	discord_core_api::register_application_commands data{};
	auto thePtr = discord_core_api::makeUnique<discord_core_api::discord_core_client>(clientConfig);
	// thePtr->getEventManager().onGuildCreation(&onGuildCreation);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "botinfo" }, discord_core_api::makeUnique<discord_core_api::bot_info>(), data.createBotInfoCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "addshopitem" }, discord_core_api::makeUnique<discord_core_api::add_shop_item>(), data.createAddShopItemCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "addshoprole" }, discord_core_api::makeUnique<discord_core_api::add_shop_role>(), data.createAddShopRoleCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "balance" }, discord_core_api::makeUnique<discord_core_api::balance>(), data.createBalanceCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "blackjack" }, discord_core_api::makeUnique<discord_core_api::blackjack>(), data.createBlackjackCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "botinfo" }, discord_core_api::makeUnique<discord_core_api::bot_info>(), data.createBotInfoCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "casinostats" }, discord_core_api::makeUnique<discord_core_api::casino_stats_function>(), data.createCasinoStatsCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "coinflip" }, discord_core_api::makeUnique<discord_core_api::coinflip>(), data.createCoinflipRoleCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "deposit" }, discord_core_api::makeUnique<discord_core_api::deposit>(), data.createDepositommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "disconnect" }, discord_core_api::makeUnique<discord_core_api::disconnect>(), data.createDisconnectData);
	thePtr->registerFunction(
		jsonifier::vector<jsonifier::string>{ "displayguildsdata" }, discord_core_api::makeUnique<discord_core_api::display_guilds_data>(), data.createDisplayGuildsDataCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "duel" }, discord_core_api::makeUnique<discord_core_api::duel>(), data.createDuelCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "help" }, discord_core_api::makeUnique<discord_core_api::help>(), data.createHelpData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "inventory" }, discord_core_api::makeUnique<discord_core_api::inventory>(), data.createInventoryCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "leaderboard" }, discord_core_api::makeUnique<discord_core_api::leaderboard>(), data.createLeaderboardCommandData);
	thePtr->registerFunction(
		jsonifier::vector<jsonifier::string>{ "removeshopitem" }, discord_core_api::makeUnique<discord_core_api::remove_shop_item>(), data.createRemoveShopItemCommandData);
	thePtr->registerFunction(
		jsonifier::vector<jsonifier::string>{ "removeshoprole" }, discord_core_api::makeUnique<discord_core_api::remove_shop_role>(), data.createRemoveShopRoleCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "rob" }, discord_core_api::makeUnique<discord_core_api::rob>(), data.createRobCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "roulette" }, discord_core_api::makeUnique<discord_core_api::roulette_game>(), data.createRouletteCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "selldrugs" }, discord_core_api::makeUnique<discord_core_api::sell_drugs>(), data.createSellDrugsCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "setbalance" }, discord_core_api::makeUnique<discord_core_api::set_balance>(), data.createSetBalanceCommandData);
	thePtr->registerFunction(
		jsonifier::vector<jsonifier::string>{ "setbordercolor" }, discord_core_api::makeUnique<discord_core_api::set_border_color>(), data.createSetBorderColorCommandData);
	thePtr->registerFunction(
		jsonifier::vector<jsonifier::string>{ "setgamechannel" }, discord_core_api::makeUnique<discord_core_api::set_game_channel>(), data.createSetGameChannelCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "streamaudio" }, discord_core_api::makeUnique<discord_core_api::stream_audio>(), data.createStreamAudioData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "shop" }, discord_core_api::makeUnique<discord_core_api::shop>(), data.createShopCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "slots" }, discord_core_api::makeUnique<discord_core_api::slots>(), data.createSlotsCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "test" }, discord_core_api::makeUnique<discord_core_api::test>(), data.createTestData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "transfer" }, discord_core_api::makeUnique<discord_core_api::transfer>(), data.createTransferCommandData);
	thePtr->registerFunction(jsonifier::vector<jsonifier::string>{ "withdraw" }, discord_core_api::makeUnique<discord_core_api::withdraw>(), data.createWithdrawCommandData);
	thePtr->runBot();
	return 0;
}
