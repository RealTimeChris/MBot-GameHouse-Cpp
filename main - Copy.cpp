// main.cpp - Main entry point.
// Jun 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#include "Commands/CommandsList.hpp"

void onBoot00(DiscordCoreAPI::DiscordCoreClient* args) {
	auto botUser = args->getBotUser();
	DiscordCoreAPI::DatabaseManagerAgent::initialize(std::to_string(botUser.id));
	DiscordCoreAPI::DiscordUser theUser{ botUser.userName, botUser.id };
}

void onGuildCreation(DiscordCoreAPI::OnGuildCreationData dataPackage) {
	DiscordCoreAPI::DiscordGuild discordGuild(dataPackage.guild);
	discordGuild.data.rouletteGame.currentlySpinning = false;
	discordGuild.writeDataToDB();
}

void onBoot01(DiscordCoreAPI::DiscordCoreClient* args) {
	std::vector<DiscordCoreAPI::ActivityData> activities;
	DiscordCoreAPI::ActivityData activity;
	activity.name = "/help for my commands!";
	activity.type = DiscordCoreAPI::ActivityType::Game;
	activities.push_back(activity);
	DiscordCoreAPI::UpdatePresenceData dataPackage{ .activities = activities, .status = "online", .afk = false };
	args->getBotUser().updatePresence(dataPackage);
}

int32_t main() {
	std::string botToken = "YOUR_BOT_TOKEN_HERE";
	std::vector<DiscordCoreAPI::RepeatedFunctionData> functionVector{};
	DiscordCoreAPI::RepeatedFunctionData function01{};
	function01.function = std::ref(onBoot00);
	function01.intervalInMs = 50;
	function01.repeated = false;
	functionVector.push_back(function01);
	DiscordCoreAPI::RepeatedFunctionData function02{};
	function02.function = std::ref(onBoot01);
	function02.intervalInMs = 150;
	function02.repeated = false;
	functionVector.push_back(function02);
	DiscordCoreAPI::ShardingOptions shardOptions{};
	shardOptions.numberOfShardsForThisProcess = 1;
	shardOptions.startingShard = 0;
	shardOptions.totalNumberOfShards = 1;
	DiscordCoreAPI::LoggingOptions logOptions{};
	logOptions.logFFMPEGErrorMessages = true;
	logOptions.logGeneralErrorMessages = true;
	logOptions.logHttpErrorMessages = true;
	logOptions.logWebSocketErrorMessages = true;
	DiscordCoreAPI::DiscordCoreClientConfig clientConfig{};
	//clientConfig.alternateConnectionAddress = "127.0.0.1";
	clientConfig.botToken = botToken;
	clientConfig.logOptions = logOptions;
	clientConfig.shardOptions = shardOptions;
	clientConfig.functionsToExecute = functionVector;
	auto thePtr = std::make_unique<DiscordCoreAPI::DiscordCoreClient>(clientConfig);
	thePtr->eventManager.onGuildCreation(&onGuildCreation);
	thePtr->registerFunction(std::vector<std::string>{ "addshopitem" }, std::make_unique<DiscordCoreAPI::AddShopItem>());
	thePtr->registerFunction(std::vector<std::string>{ "addshoprole" }, std::make_unique<DiscordCoreAPI::AddShopRole>());
	thePtr->registerFunction(std::vector<std::string>{ "balance" }, std::make_unique<DiscordCoreAPI::Balance>());
	thePtr->registerFunction(std::vector<std::string>{ "blackjack" }, std::make_unique<DiscordCoreAPI::Blackjack>());
	thePtr->registerFunction(std::vector<std::string>{ "botinfo" }, std::make_unique<DiscordCoreAPI::BotInfo>());
	thePtr->registerFunction(std::vector<std::string>{ "buy" }, std::make_unique<DiscordCoreAPI::Buy>());
	thePtr->registerFunction(std::vector<std::string>{ "casinostats" }, std::make_unique<DiscordCoreAPI::CasinoStatsFunction>());
	thePtr->registerFunction(std::vector<std::string>{ "coinflip" }, std::make_unique<DiscordCoreAPI::Coinflip>());
	thePtr->registerFunction(std::vector<std::string>{ "deposit" }, std::make_unique<DiscordCoreAPI::Deposit>());
	thePtr->registerFunction(std::vector<std::string>{ "displayguildsdata" }, std::make_unique<DiscordCoreAPI::DisplayGuildsData>());
	thePtr->registerFunction(std::vector<std::string>{ "duel" }, std::make_unique<DiscordCoreAPI::Duel>());
	thePtr->registerFunction(std::vector<std::string>{ "help" }, std::make_unique<DiscordCoreAPI::Help>());
	thePtr->registerFunction(std::vector<std::string>{ "inventory" }, std::make_unique<DiscordCoreAPI::Inventory>());
	thePtr->registerFunction(std::vector<std::string>{ "leaderboard" }, std::make_unique<DiscordCoreAPI::Leaderboard>());
	thePtr->registerFunction(std::vector<std::string>{ "registerapplicationcommands" }, std::make_unique<DiscordCoreAPI::RegisterApplicationCommands>());
	thePtr->registerFunction(std::vector<std::string>{ "removeobject" }, std::make_unique<DiscordCoreAPI::RemoveObject>());
	thePtr->registerFunction(std::vector<std::string>{ "removeshopitem" }, std::make_unique<DiscordCoreAPI::RemoveShopItem>());
	thePtr->registerFunction(std::vector<std::string>{ "removeshoprole" }, std::make_unique<DiscordCoreAPI::RemoveShopRole>());
	thePtr->registerFunction(std::vector<std::string>{ "rob" }, std::make_unique<DiscordCoreAPI::Rob>());
	thePtr->registerFunction(std::vector<std::string>{ "roulette" }, std::make_unique<DiscordCoreAPI::RouletteGame>());
	thePtr->registerFunction(std::vector<std::string>{ "selldrugs" }, std::make_unique<DiscordCoreAPI::SellDrugs>());
	thePtr->registerFunction(std::vector<std::string>{ "setbalance" }, std::make_unique<DiscordCoreAPI::SetBalance>());
	thePtr->registerFunction(std::vector<std::string>{ "setbordercolor" }, std::make_unique<DiscordCoreAPI::SetBorderColor>());
	thePtr->registerFunction(std::vector<std::string>{ "setgamechannel" }, std::make_unique<DiscordCoreAPI::SetGameChannel>());
	thePtr->registerFunction(std::vector<std::string>{ "shop" }, std::make_unique<DiscordCoreAPI::Shop>());
	thePtr->registerFunction(std::vector<std::string>{ "slots" }, std::make_unique<DiscordCoreAPI::Slots>());
	thePtr->registerFunction(std::vector<std::string>{ "test" }, std::make_unique<DiscordCoreAPI::Test>());
	thePtr->registerFunction(std::vector<std::string>{ "transfer" }, std::make_unique<DiscordCoreAPI::Transfer>());
	thePtr->registerFunction(std::vector<std::string>{ "withdraw" }, std::make_unique<DiscordCoreAPI::Withdraw>());
	thePtr->runBot();

	//thePtr->eventManager.onGuildCreation(onGuildCreation);
	return 0;
}
