// Slots.hpp - Header for the "slots" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class Slots : public BaseFunction {
	  public:
		Slots() {
			this->commandName = "slots";
			this->helpDescription = "Gamble some currency on a game of slots!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /slots BETAMOUNT.\n------");
			msgEmbed.setTitle("__**Slots Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Slots>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ argsNew.eventData.getChannelId() }).get();

				InputEvents::deleteInputEventResponseAsync(argsNew.eventData).get();

				Guild guild = Guilds::getGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}


				int32_t betAmountOld;
				std::regex digitRegExp("\\d{1,18}");
				if (std::stoll(argsNew.commandData.optionsArgs[0]) <= 0 || !regex_search(argsNew.commandData.optionsArgs.at(0), digitRegExp)) {
					std::string msgString = "------\n**Please, enter a valid bet amount as the first argument of the command! (!slots = BETAMOUNT)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				} else {
					std::cmatch matchResults;
					regex_search(argsNew.commandData.optionsArgs.at(0).c_str(), matchResults, digitRegExp);
					betAmountOld = ( int32_t )std::stoll(matchResults.str());
				}

				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = argsNew.eventData.getAuthorId(), .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuildMember discordGuildMember(guildMember);

				if (( uint32_t )betAmountOld > discordGuildMember.data.currency.wallet) {
					std::string msgString = "------\n**Sorry, but you don't have sufficient funds in your wallet for placing that bet!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				auto botUser = argsNew.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				int32_t payoutAmount{};
				std::string gameResultType{};
				std::vector<std::string> slotReel = {
					":crossed_swords:", ":apple:", ":ring:", ":gun:", ":swan:", ":rocket:", ":coin:", ":star:", ":jack_o_lantern:", ":christmas_tree:"
				};

				std::mt19937_64 randomEngine{ static_cast<uint64_t>(
					std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
				int32_t reelStartIndex1 = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 10.0f);
				std::vector<int32_t> reelIndices1{};
				reelIndices1.resize(10);
				for (uint32_t x = 0; x < 10; x += 1) {
					reelIndices1[x] = (reelStartIndex1 + x) % 10;
				}
				int32_t reelStartIndex2 = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 10.0f);
				std::vector<int32_t> reelIndices2{};
				reelIndices2.resize(10);
				for (uint32_t x = 0; x < 10; x += 1) {
					reelIndices2[x] = (reelStartIndex2 + x) % 10;
				}
				int32_t reelStartIndex3 = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 10.0f);
				std::vector<int32_t> reelIndices3{};
				reelIndices3.resize(10);
				for (uint32_t x = 0; x < 10; x += 1) {
					reelIndices3[x] = (reelStartIndex3 + x) % 10;
				}

				std::string msgString0 = "__**Slot "
										 "Results:**__\n[:question:][:question:][:question:]\n[:question:][:question:][:question:]\n[:question:][:question:][:"
										 "question:]\n\n__**Your Wager:**__ " +
					std::to_string(betAmountOld) + " " + discordUser.data.currencyName + +"\n__**Maximum Payout:**__ " + std::to_string(15 * betAmountOld) + " " +
					discordUser.data.currencyName;

				EmbedData msgEmbed0;
				msgEmbed0.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed0.setColor("0000FE");
				msgEmbed0.setDescription(msgString0);
				msgEmbed0.setTimeStamp(getTimeAndDate());
				msgEmbed0.setTitle("__**Slots Game:**__");

				RespondToInputEventData dataPackage(argsNew.eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed0);
				InputEvents::respondToInputEventAsync(dataPackage).get();
				std::function<void()> theFunction00 = [=]() mutable -> void {
					std::string msgString1 = "__**Slot Results:**__\n[" + slotReel[reelIndices1[7]] + "][:question:][:question:]\n[" + slotReel[reelIndices1[8]] +
						"][:question:][:question:]\n[" + slotReel[reelIndices1[9]] + "][:question:][:question:]\n\n__**Your Wager:**__ " + std::to_string(betAmountOld) + " " +
						discordUser.data.currencyName + "\n__**Maximum Payout:**__ " + std::to_string(15 * betAmountOld) + " " + discordUser.data.currencyName;
					EmbedData msgEmbed;
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor("0000FE");
					msgEmbed.setDescription(msgString1);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Slots Game:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				};
				std::function<void()> theFunction01 = [=]() mutable -> void {
					std::string msgString2 = "__**Slot Results:**__\n[" + slotReel[reelIndices1[7]] + "][" + slotReel[reelIndices2[7]] + "][:question:]\n[" +
						slotReel[reelIndices1[8]] + "][" + slotReel[reelIndices2[8]] + "][:question:]\n" + "[" + slotReel[reelIndices1[9]] + "][" + slotReel[reelIndices2[9]] +
						"][:question:]\n\n__**Your Wager:**__ " + std::to_string(betAmountOld) + " " + discordUser.data.currencyName + "\n__**Maximum Payout:**__ " +
						std::to_string(15 * betAmountOld) + " " + discordUser.data.currencyName;
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor("0000FE");
					msgEmbed.setDescription(msgString2);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Slots Game:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				};
				std::function<void()> theFunction = [=]() mutable -> void {
					std::string gameResultTypeNew;
					int32_t payoutAmountNew = payoutAmount;
					GuildMember guildMemberNew = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = guildMember.user.id, .guildId = argsNew.eventData.getGuildId() }).get();
					DiscordGuildMember discordGuildMember(guildMemberNew);
					DiscordGuild discordGuild(guild);
					if (slotReel[reelIndices1[8]] == slotReel[reelIndices2[8]] && slotReel[reelIndices2[8]] == slotReel[reelIndices3[8]]) {
						gameResultTypeNew = "Triple Straight";
						payoutAmountNew = betAmountOld * 15;
					} else if ((slotReel[reelIndices1[9]] == slotReel[reelIndices2[8]] && slotReel[reelIndices2[8]] == slotReel[reelIndices3[7]]) ||
						(slotReel[reelIndices1[7]] == slotReel[reelIndices2[8]] && slotReel[reelIndices2[8]] == slotReel[reelIndices3[9]])) {
						gameResultTypeNew = "Triple Diagonal";
						payoutAmountNew = betAmountOld * 7;
					} else if (slotReel[reelIndices1[8]] == slotReel[reelIndices2[8]] || slotReel[reelIndices3[8]] == slotReel[reelIndices2[8]]) {
						gameResultTypeNew = "Double Straight";
						payoutAmountNew = betAmountOld * 1;
					} else if ((slotReel[reelIndices1[9]] == slotReel[reelIndices2[8]]) || (slotReel[reelIndices1[7]] == slotReel[reelIndices2[8]]) ||
						(slotReel[reelIndices3[9]] == slotReel[reelIndices2[8]]) || (slotReel[reelIndices3[7]] == slotReel[reelIndices2[8]])) {
						gameResultTypeNew = "Double Diagonal";
						payoutAmountNew = betAmountOld * 1;
					} else {
						gameResultTypeNew = "Loss";
						payoutAmountNew = -1 * betAmountOld;
					}

					discordGuild.data.casinoStats.totalPayout += payoutAmountNew;
					discordGuild.data.casinoStats.totalSlotsPayout += payoutAmountNew;
					if (payoutAmountNew > discordGuild.data.casinoStats.largestSlotsPayout.amount) {
						discordGuild.data.casinoStats.largestSlotsPayout.amount = payoutAmountNew;
						discordGuild.data.casinoStats.largestSlotsPayout.timeStamp = getTimeAndDate();
						discordGuild.data.casinoStats.largestSlotsPayout.userId = guildMember.user.id;
						discordGuild.data.casinoStats.largestSlotsPayout.userName = guildMember.user.userName;
					}
					discordGuild.writeDataToDB();
					InputEventData newEvent02 = argsNew.eventData;
					if (betAmountOld > ( int32_t )discordGuildMember.data.currency.wallet) {
						std::string msgString3 = "__**Slot Results:**__\n[:x:][:x:][:x:]\n[:x:][:x:][:x:]\n[:x:][:x:][:x:]\n------\n__**Your Wager:**__ " +
							std::to_string(betAmountOld) + "\n__**Maximum Payout:**__ " + std::to_string(15 * betAmountOld) + " " + discordUser.data.currencyName +
							"\n__**Game Results:**__\n__**Payout:**__ NSF__** Result Type:**__ Non-Sufficient Funds\n__**Your New Wallet Balance:**__ " +
							std::to_string(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

						EmbedData msgEmbed{};
						msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
						msgEmbed.setColor("0000FE");
						msgEmbed.setDescription(msgString3);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Slots Game:**__");
						RespondToInputEventData dataPackage(newEvent02);
						dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						newEvent02 = InputEvents::respondToInputEventAsync(dataPackage).get();
						return;
					}

					discordGuildMember.getDataFromDB();
					discordGuildMember.data.currency.wallet += payoutAmountNew;
					discordGuildMember.writeDataToDB();
					std::string msgString4 = "__**Slot Results:**__\n[" + slotReel[reelIndices1[7]] + "][" + slotReel[reelIndices2[7]] + "][" + slotReel[reelIndices3[7]] + "]\n[" +
						slotReel[reelIndices1[8]] + "][" + slotReel[reelIndices2[8]] + "][" + slotReel[reelIndices3[8]] + "]\n" + "[" + slotReel[reelIndices1[9]] + "][" +
						slotReel[reelIndices2[9]] + "][" + slotReel[reelIndices3[9]] + "]\n------\n__**Your Wager:**__ " + std::to_string(betAmountOld) +
						"\n__**Maximum Payout:**__ " + std::to_string(15 * betAmountOld) + " " + discordUser.data.currencyName + "\n__**Game Results:**__\n__**Payout:**__ " +
						std::to_string(payoutAmountNew) + " " + discordUser.data.currencyName + " __**Result Type:**__ " + gameResultTypeNew +
						"\n__**Your New Wallet Balance:**__ " + std::to_string(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

					EmbedData msgEmbed{};
					if (gameResultTypeNew == "Loss") {
						msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
						msgEmbed.setColor("FE0000");
						msgEmbed.setDescription(msgString4);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Slots Game:**__");
					} else {
						msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
						msgEmbed.setColor("00FE00");
						msgEmbed.setDescription(msgString4);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Slots Game:**__");
					}
					RespondToInputEventData dataPackage(newEvent02);
					dataPackage.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					newEvent02 = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				};
				ThreadPool::executeFunctionAfterTimePeriod(theFunction00, 3000, true);
				ThreadPool::executeFunctionAfterTimePeriod(theFunction01, 3000, true);
				ThreadPool::executeFunctionAfterTimePeriod(theFunction, 3000, true);
				return;
			} catch (...) {
				reportException("Slots::execute()");
			}
		}
		~Slots(){};
	};
}
