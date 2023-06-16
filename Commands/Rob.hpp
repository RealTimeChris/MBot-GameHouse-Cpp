// Rob.hpp - Header for the "rob" command.
// jun 25, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {
	class rob : public base_function {
	  public:
		rob() {
			this->commandName	  = "rob";
			this->helpDescription = "attempt to rob a fellow server-mate, for currency.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /rob @usermention.\n------");
			msgEmbed.setTitle("__**Rob usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<rob>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				snowflake userID{ argsNew.getUserData().id };

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				discord_guild_member discordGuildMember(managerAgent, guildMember);

				snowflake targetUserID{ argsNew.getCommandArguments().values["user"].operator jsonifier::string() };
				guild_member_data targetMember = guild_members::getCachedGuildMember({ .guildMemberId = targetUserID, .guildId = guild.id });
				discord_guild_member targetGuildMember(managerAgent, targetMember);

				if (targetMember.getUserData().userName == "") {
					jsonifier::string msgString = "------\n**sorry, but that user could not be found!**\n------";
					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setColor("fefefe");
					msgEmbed->setDescription(msgString);
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**User_data issue:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				if (userID == targetUserID) {
					jsonifier::string msgString = "------\n**You can't rob yourself, dumbass!**\n------";
					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed->setColor("fefefe");
					msgEmbed->setDescription(msgString);
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Robbery issue:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				uint32_t msPerSecond		   = 1000;
				uint32_t secondsPerMinute	   = 60;
				uint32_t msPerMinute		   = msPerSecond * secondsPerMinute;
				uint32_t minutesPerHour		   = 60;
				uint32_t msPerHour			   = msPerMinute * minutesPerHour;
				uint32_t timeBetweenRobberies  = (uint32_t)(discordUser.data.hoursOfRobberyCooldown * (float)msPerHour);
				uint32_t currentTime		   = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				uint32_t currentTimeDifference = currentTime - discordGuildMember.data.lastTimeRobbed;

				if (currentTimeDifference >= timeBetweenRobberies) {
					jsonifier::string userGainString;
					jsonifier::string userLossString;

					int32_t userLossAmount = 0;
					int32_t userGainAmount = 0;

					for (uint32_t x = 0; x < discordGuildMember.data.items.size(); x += 1) {
						if (discordGuildMember.data.items[x].selfMod > 0) {
							userGainAmount += discordGuildMember.data.items[x].selfMod;
							userGainString += "+" + jsonifier::toString(discordGuildMember.data.items[x].selfMod) + jsonifier::string{ " of base roll from <@!" } +
								discordGuildMember.data.guildMemberId.operator jsonifier::string() + "> 's " + discordGuildMember.data.items[x].emoji +
								discordGuildMember.data.items[x].itemName + "\n";
						}
					}

					for (uint32_t x = 0; x < targetGuildMember.data.items.size(); x += 1) {
						if (targetGuildMember.data.items[x].oppMod < 0) {
							userLossAmount += targetGuildMember.data.items[x].oppMod;
							userLossString += jsonifier::toString(targetGuildMember.data.items[x].oppMod) + " of base roll from <@!" + targetGuildMember.data.guildMemberId + "> 's " +
											  targetGuildMember.data.items[x].emoji + targetGuildMember.data.items[x].itemName + "\n";
						}
					}

					int32_t userRollMod = userGainAmount + userLossAmount;

					int32_t userRollModTotal = (uint32_t)applyAsymptoticTransform((float)userRollMod, 2000.0f, 40.0f);

					int32_t baseProbabilityOfSuccess  = 40;
					int32_t totalProbabilityOfSuccess = baseProbabilityOfSuccess + userRollModTotal;

					std::mt19937_64 randomEngine{ static_cast<uint64_t>(
						std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
					int32_t currentProbabilityValue = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 100.0f);
					bool currentSuccessValue		= currentProbabilityValue > (100 - totalProbabilityOfSuccess);

					jsonifier::string msgString;

					if (currentSuccessValue == true) {
						msgString = "nicely done! you robbed that motherfucker, <@!" + targetUserID + ">, good!\n\n__Base probability of success: __ " +
									jsonifier::toString(baseProbabilityOfSuccess) + "% \n";

						if (userGainAmount > 0 || userLossAmount < 0) {
							msgString += userGainString + userLossString + "\n__Resulting in a net probability of success gain of:__ " + jsonifier::toString(userRollModTotal) +
										 "% \n__For a final probability of success of:__ " + jsonifier::toString(totalProbabilityOfSuccess) + "% \n";
						}

						uint32_t currencyRobPercentage = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 60.0f);

						uint32_t currencyRobAmount = (uint32_t)trunc((float)targetGuildMember.data.currency.wallet * ((float)currencyRobPercentage / 100.0f));

						if (currencyRobAmount < 0) {
							jsonifier::string msgStringNew = "------\n**Cannot rob for debt!**\n------";
							discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
							msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed->setColor("fefefe");
							msgEmbed->setDescription(msgStringNew);
							msgEmbed->setTimeStamp(getTimeAndDate());
							msgEmbed->setTitle("__**Target Issue:**__");
							respond_to_input_event_data dataPackage(argsNew.getInputEventData());
							dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(*msgEmbed);
							auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
							return;
						}

						targetGuildMember.data.currency.wallet -= currencyRobAmount;
						targetGuildMember.writeDataToDB(managerAgent);
						discordGuildMember.data.currency.wallet += currencyRobAmount;
						discordGuildMember.writeDataToDB(managerAgent);
						uint32_t targetUserNewBalance = targetGuildMember.data.currency.wallet;
						uint32_t userNewBalance		  = discordGuildMember.data.currency.wallet;

						msgString = msgString + "------\n**You've robbed <@!" + targetUserID + "> for " + jsonifier::toString(currencyRobPercentage) + "% of their wallet, which is " +
									jsonifier::toString(currencyRobAmount) + " " + discordUser.data.currencyName + ".\n" + "**\n__Your new wallet balances are:__\n<@!" + userID +
									">: " + jsonifier::toString(userNewBalance) + " " + discordUser.data.currencyName + "\n<@!" + targetUserID +
									">: " + jsonifier::toString(targetUserNewBalance) + " " + discordUser.data.currencyName;

						discordGuildMember.data.lastTimeRobbed =
							(uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						discordGuildMember.writeDataToDB(managerAgent);
						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setColor("fefefe");
						msgEmbed->setDescription(msgString);
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Succesful Robbery:**__");
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Interaction_Response);
						dataPackage.addContent("<@!" + targetUserID + ">");
						dataPackage.addMessageEmbed(*msgEmbed);
						auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					} else if (currentSuccessValue == false) {
						int32_t finedPercentage = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 30.0f);
						uint32_t finedAmount	= (uint32_t)trunc(((float)discordGuildMember.data.currency.wallet * ((float)finedPercentage / 100.0f)));
						discordGuildMember.data.currency.wallet -= finedAmount;
						discordGuildMember.writeDataToDB(managerAgent);
						uint32_t userNewBalance = discordGuildMember.data.currency.wallet;

						int32_t repaidPercentage = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 50.0f);
						int32_t repaidAmount	 = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 100.0f);
						targetGuildMember.data.currency.wallet += repaidAmount;
						targetGuildMember.writeDataToDB(managerAgent);
						uint32_t targetUserNewBalance = targetGuildMember.data.currency.wallet;

						msgString = "Oof! you've been caught while attempting to rob <@!" + targetUserID + ">!\n\n__Base probability of success:__ " +
							jsonifier::toString(baseProbabilityOfSuccess) + "% \n";

						if (userGainAmount > 0 || userLossAmount < 0) {
							msgString += userGainString + userLossString + "\n__Resulting in a net probability of success gain of:__ " + jsonifier::toString(userRollModTotal) +
										 "%\n__For a final probability of success of:__ " + jsonifier::toString(totalProbabilityOfSuccess) + "%\n";
						}

						msgString += "------\n**You've been fined " + jsonifier::toString(finedPercentage) + "% of your wallet balance, which is " + jsonifier::toString(finedAmount) + " " +
									 discordUser.data.currencyName + "!\n<@!" + targetUserID + "> has been reimbursed " + jsonifier::toString(repaidAmount) + " " +
									 discordUser.data.currencyName + " (" + jsonifier::toString(repaidPercentage) + "%).**";

						msgString += "\n\n__Your new wallet balances are:__\n<@!" + userID + ">: " + jsonifier::toString(userNewBalance) + " " + discordUser.data.currencyName +
									 "\n<@!" + targetUserID + ">: " + jsonifier::toString(targetUserNewBalance) + " " + discordUser.data.currencyName;

						discordGuildMember.data.lastTimeRobbed =
							(uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						discordGuildMember.writeDataToDB(managerAgent);

						discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
						msgEmbed->setColor("fefefe");
						msgEmbed->setDescription(msgString);
						msgEmbed->setTimeStamp(getTimeAndDate());
						msgEmbed->setTitle("__**Failed Robbery:**__");
						msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						respond_to_input_event_data dataPackage(argsNew.getInputEventData());
						dataPackage.setResponseType(input_event_response_type::Interaction_Response);
						dataPackage.addMessageEmbed(*msgEmbed);
						auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					}
				} else {
					jsonifier::string msgString;
					uint32_t timeLeft	   = timeBetweenRobberies - currentTimeDifference;
					uint32_t hoursRemain   = (uint32_t)trunc(timeLeft / msPerHour);
					uint32_t minutesRemain = (uint32_t)trunc((timeLeft % msPerHour) / msPerMinute);
					uint32_t secondsRemain = (uint32_t)trunc(((timeLeft % msPerHour) % msPerMinute) / msPerSecond);

					if (hoursRemain > 0) {
						msgString = "Sorry, but you need to wait " + jsonifier::toString(hoursRemain) + " hours, " + jsonifier::toString(minutesRemain) + " minutes, and " +
									jsonifier::toString(secondsRemain) + " seconds before you can rob someone again!";
					} else if (minutesRemain > 0) {
						msgString = "Sorry, but you need to wait " + jsonifier::toString(minutesRemain) + " minutes and " + jsonifier::toString(secondsRemain) +
									" seconds before you can rob someone again!";
					} else {
						msgString = "Sorry, but you need to wait " + jsonifier::toString(secondsRemain) + " seconds before you can rob someone again!";
					}

					discord_core_api::unique_ptr<embed_data> msgEmbed{ discord_core_api::makeUnique<embed_data>() };
					msgEmbed->setColor("fefefe");
					msgEmbed->setDescription(msgString);
					msgEmbed->setTimeStamp(getTimeAndDate());
					msgEmbed->setTitle("__**Failed Robbery:**__");
					msgEmbed->setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
				}
				return;
			} catch (const std::exception& error) {
				std::cout << "rob::execute()" << error.what() << std::endl;
			}
		}
		~rob(){};
	};
}// namespace discord_core_api
