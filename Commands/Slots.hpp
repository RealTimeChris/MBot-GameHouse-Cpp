// Slots.hpp - Header for the "slots" command.
// jun 26, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class slots : public base_function {
	  public:
		slots() {
			this->commandName	  = "slots";
			this->helpDescription = "gamble some currency on a game of slots!";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /slots betamount.\n------");
			msgEmbed.setTitle("__**Slots usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<slots>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (!areWeAllowed) {
					return;
				}


				int32_t betAmountOld;
				std::regex digitRegExp{ "\\d{1,18}" };
				if (argsNew.getCommandArguments().values["betamount"].operator std::streamoff() <= 0) {
					jsonifier::string msgString = "------\n**Please, enter a valid bet amount as the first argument of the command! (!slots = betamount)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				} else {
					betAmountOld = argsNew.getCommandArguments().values["betamount"].operator std::streamoff();
				}

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				discord_guild_member discordGuildMember(managerAgent, guildMember);

				if ((uint32_t)betAmountOld > discordGuildMember.data.currency.wallet) {
					jsonifier::string msgString = "------\n**sorry, but you don't have sufficient funds in your wallet for placing that bet!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient funds:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				int32_t payoutAmount{};
				jsonifier::string gameResultType{};
				jsonifier::vector<jsonifier::string> slotReel{
					":crossed_swords:", ":apple:", ":ring:", ":gun:", ":swan:", ":rocket:", ":coin:", ":star:", ":jack_o_lantern:", ":christmas_tree:"
				};

				std::mt19937_64 randomEngine{ static_cast<uint64_t>(
					std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
				int32_t reelStartIndex1 = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 10.0f);
				jsonifier::vector<int32_t> reelIndices1{};
				reelIndices1.resize(10);
				for (uint32_t x = 0; x < 10; x += 1) {
					reelIndices1[x] = (reelStartIndex1 + x) % 10;
				}
				int32_t reelStartIndex2 = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 10.0f);
				jsonifier::vector<int32_t> reelIndices2{};
				reelIndices2.resize(10);
				for (uint32_t x = 0; x < 10; x += 1) {
					reelIndices2[x] = (reelStartIndex2 + x) % 10;
				}
				int32_t reelStartIndex3 = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 10.0f);
				jsonifier::vector<int32_t> reelIndices3{};
				reelIndices3.resize(10);
				for (uint32_t x = 0; x < 10; x += 1) {
					reelIndices3[x] = (reelStartIndex3 + x) % 10;
				}

				jsonifier::string msgString0 = "__**Slot "
											   "results:**__\n[:question:][:question:][:question:]\n[:question:][:question:][:question:]\n[:question:][:question:][:"
											   "question:]\n\n__**Your wager:**__ " +
					jsonifier::toString(betAmountOld) + " " + discordUser.data.currencyName + "\n__**Maximum payout:**__ " + jsonifier::toString(15 * betAmountOld) + " " +
					discordUser.data.currencyName;

				embed_data msgEmbed0;
				msgEmbed0.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed0.setColor("0000FE");
				msgEmbed0.setDescription(msgString0);
				msgEmbed0.setTimeStamp(getTimeAndDate());
				msgEmbed0.setTitle("__**Slots game:**__");

				respond_to_input_event_data dataPackage(argsNew.getInputEventData());
				dataPackage.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed0);
				input_events::respondToInputEventAsync(dataPackage).get();
				std::function<void()> theFunction00 = [=]() mutable -> void {
					jsonifier::string msgString1 = "__**Slot results:**__\n[" + slotReel[reelIndices1[7]] + "][:question:][:question:]\n[" + slotReel[reelIndices1[8]] +
											 "][:question:][:question:]\n[" + slotReel[reelIndices1[9]] + "][:question:][:question:]\n\n__**Your wager:**__ " +
											 jsonifier::toString(betAmountOld) + " " + discordUser.data.currencyName + "\n__**Maximum payout:**__ " + jsonifier::toString(15 * betAmountOld) +
											 " " + discordUser.data.currencyName;
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("0000FE");
					msgEmbed.setDescription(msgString1);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Slots game:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				};
				std::function<void()> theFunction01 = [=]() mutable -> void {
					jsonifier::string msgString2 = "__**Slot results:**__\n[" + slotReel[reelIndices1[7]] + "][" + slotReel[reelIndices2[7]] + "][:question:]\n[" +
											 slotReel[reelIndices1[8]] + "][" + slotReel[reelIndices2[8]] + "][:question:]\n" + "[" + slotReel[reelIndices1[9]] + "][" +
											 slotReel[reelIndices2[9]] + "][:question:]\n\n__**Your wager:**__ " + jsonifier::toString(betAmountOld) + " " +
											 discordUser.data.currencyName + "\n__**Maximum payout:**__ " + jsonifier::toString(15 * betAmountOld) + " " + discordUser.data.currencyName;
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("0000FE");
					msgEmbed.setDescription(msgString2);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Slots game:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				};
				std::function<void()> theFunction = [=]() mutable -> void {
					jsonifier::string gameResultTypeNew;
					int32_t payoutAmountNew		   = payoutAmount;
					guild_member_data guildMemberNew = guild_members::getCachedGuildMember({ .guildMemberId = guildMember.user.id, .guildId = guild.id });
					discord_guild_member discordGuildMember(managerAgent, guildMemberNew);
					discord_guild discordGuild{ managerAgent, guild };
					if (slotReel[reelIndices1[8]] == slotReel[reelIndices2[8]] && slotReel[reelIndices2[8]] == slotReel[reelIndices3[8]]) {
						gameResultTypeNew = "triple straight";
						payoutAmountNew	  = betAmountOld * 15;
					} else if ((slotReel[reelIndices1[9]] == slotReel[reelIndices2[8]] && slotReel[reelIndices2[8]] == slotReel[reelIndices3[7]]) ||
							   (slotReel[reelIndices1[7]] == slotReel[reelIndices2[8]] && slotReel[reelIndices2[8]] == slotReel[reelIndices3[9]])) {
						gameResultTypeNew = "triple diagonal";
						payoutAmountNew	  = betAmountOld * 7;
					} else if (slotReel[reelIndices1[8]] == slotReel[reelIndices2[8]] || slotReel[reelIndices3[8]] == slotReel[reelIndices2[8]]) {
						gameResultTypeNew = "double straight";
						payoutAmountNew	  = betAmountOld * 1;
					} else if ((slotReel[reelIndices1[9]] == slotReel[reelIndices2[8]]) || (slotReel[reelIndices1[7]] == slotReel[reelIndices2[8]]) ||
							   (slotReel[reelIndices3[9]] == slotReel[reelIndices2[8]]) || (slotReel[reelIndices3[7]] == slotReel[reelIndices2[8]])) {
						gameResultTypeNew = "double diagonal";
						payoutAmountNew	  = betAmountOld * 1;
					} else {
						gameResultTypeNew = "loss";
						payoutAmountNew	  = -1 * betAmountOld;
					}

					discordGuild.data.casinoStats.totalPayout += payoutAmountNew;
					discordGuild.data.casinoStats.totalSlotsPayout += payoutAmountNew;
					if (payoutAmountNew > discordGuild.data.casinoStats.largestSlotsPayout.amount) {
						discordGuild.data.casinoStats.largestSlotsPayout.amount	   = payoutAmountNew;
						discordGuild.data.casinoStats.largestSlotsPayout.timeStamp = getTimeAndDate();
						discordGuild.data.casinoStats.largestSlotsPayout.userId	   = guildMember.user.id;
						discordGuild.data.casinoStats.largestSlotsPayout.userName  = guildMember.getUserData().userName;
					}
					discordGuild.writeDataToDB(managerAgent);
					input_event_data newEvent02 = argsNew.getInputEventData();
					if (betAmountOld > (int32_t)discordGuildMember.data.currency.wallet) {
						jsonifier::string msgString3 = "__**Slot results:**__\n[:x:][:x:][:x:]\n[:x:][:x:][:x:]\n[:x:][:x:][:x:]\n------\n__**Your wager:**__ " +
												 jsonifier::toString(betAmountOld) + "\n__**Maximum payout:**__ " + jsonifier::toString(15 * betAmountOld) + " " +
												 discordUser.data.currencyName +
												 "\n__**Game results:**__\n__**Payout:**__ NSF__** result type:**__ non-sufficient funds\n__**Your new wallet "
												 "balance:**__ " +
												 jsonifier::toString(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

						embed_data msgEmbed{};
						msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed.setColor("0000FE");
						msgEmbed.setDescription(msgString3);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Slots game:**__");
						respond_to_input_event_data dataPackage(newEvent02);
						dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						newEvent02 = input_events::respondToInputEventAsync(dataPackage).get();
						return;
					}

					discordGuildMember.getDataFromDB(managerAgent);
					discordGuildMember.data.currency.wallet += payoutAmountNew;
					discordGuildMember.writeDataToDB(managerAgent);
					jsonifier::string msgString4 = "__**Slot results:**__\n[" + slotReel[reelIndices1[7]] + "][" + slotReel[reelIndices2[7]] + "][" + slotReel[reelIndices3[7]] + "]\n[" +
											 slotReel[reelIndices1[8]] + "][" + slotReel[reelIndices2[8]] + "][" + slotReel[reelIndices3[8]] + "]\n" + "[" +
											 slotReel[reelIndices1[9]] + "][" + slotReel[reelIndices2[9]] + "][" + slotReel[reelIndices3[9]] + "]\n------\n__**Your wager:**__ " +
											 jsonifier::toString(betAmountOld) + "\n__**Maximum payout:**__ " + jsonifier::toString(15 * betAmountOld) + " " + discordUser.data.currencyName +
											 "\n__**Game results:**__\n__**Payout:**__ " + jsonifier::toString(payoutAmountNew) + " " + discordUser.data.currencyName +
											 " __**Result type:**__ " + gameResultTypeNew + "\n__**Your new wallet balance:**__ " +
											 jsonifier::toString(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName + "\n------";

					embed_data msgEmbed{};
					if (gameResultTypeNew == "loss") {
						msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed.setColor("fe0000");
						msgEmbed.setDescription(msgString4);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Slots game:**__");
					} else {
						msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed.setColor("00FE00");
						msgEmbed.setDescription(msgString4);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Slots game:**__");
					}
					respond_to_input_event_data dataPackage(newEvent02);
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					newEvent02 = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				};
				executeFunctionAfterTimePeriod(theFunction00, 3000, false, true);
				executeFunctionAfterTimePeriod(theFunction01, 3000, false, true);
				executeFunctionAfterTimePeriod(theFunction, 3000, false, true);
				return;
			} catch (const std::exception& error) {
				std::cout << "slots::execute()" << error.what() << std::endl;
			}
		}
		~slots(){};
	};
}// namespace discord_core_api
