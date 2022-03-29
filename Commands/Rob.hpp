// Rob.hpp - Header for the "rob" command.
// Jun 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class Rob: public BaseFunction {
	public:
		Rob() {
			this->commandName = "rob";
			this->helpDescription = "Attempt to rob a fellow server-mate, for currency.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /rob @USERMENTION.\n------");
			msgEmbed.setTitle("__**Rob Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<Rob>();
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

				std::regex userMentionRegExp(".{2,3}\\d{18}>");
				std::regex userIDRegExp("\\d{18}");


				std::string userID = args.eventData->getAuthorId();

				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId(),.guildId = args.eventData->getGuildId() }).get();
				DiscordGuildMember discordGuildMember(guildMember);

				std::cmatch matchResults;
				regex_search(args.commandData.optionsArgs.at(0).c_str(), matchResults, userIDRegExp);
				std::string targetUserID = matchResults.str();
				GuildMember targetMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = targetUserID,.guildId = args.eventData->getGuildId() }).get();
				DiscordGuildMember targetGuildMember(targetMember);

				if (targetMember.user.userName == "") {
					std::string msgString = "------\n**Sorry, but that user could not be found!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User Issue:**__");
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					return;
				}

				if (userID == targetUserID) {
					std::string msgString = "------\n**You can't rob yourself, dumbass!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Robbery Issue:**__");
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
					return;
				}
				auto botUser = args.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				uint32_t msPerSecond = 1000;
				uint32_t secondsPerMinute = 60;
				uint32_t msPerMinute = msPerSecond * secondsPerMinute;
				uint32_t minutesPerHour = 60;
				uint32_t msPerHour = msPerMinute * minutesPerHour;
				uint32_t timeBetweenRobberies = (uint32_t)(discordUser.data.hoursOfRobberyCooldown * (float)msPerHour);
				uint32_t currentTime = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				uint32_t currentTimeDifference = currentTime - discordGuildMember.data.lastTimeRobbed;

				if (currentTimeDifference >= timeBetweenRobberies) {
					std::string userGainString;
					std::string userLossString;

					int32_t userLossAmount = 0;
					int32_t userGainAmount = 0;

					for (uint32_t x = 0; x < discordGuildMember.data.items.size(); x += 1) {
						if (discordGuildMember.data.items[x].selfMod > 0) {
							userGainAmount += discordGuildMember.data.items[x].selfMod;
							userGainString += "+" + std::to_string(discordGuildMember.data.items[x].selfMod) + " of base roll from <@!" + discordGuildMember.data.guildMemberId + "> 's " + discordGuildMember.data.items[x].emoji + discordGuildMember.data.items[x].itemName + "\n";
						}
					}

					for (uint32_t x = 0; x < targetGuildMember.data.items.size(); x += 1) {
						if (targetGuildMember.data.items[x].oppMod < 0) {
							userLossAmount += targetGuildMember.data.items[x].oppMod;
							userLossString += std::to_string(targetGuildMember.data.items[x].oppMod) + " of base roll from <@!" + targetGuildMember.data.guildMemberId + "> 's " + targetGuildMember.data.items[x].emoji + targetGuildMember.data.items[x].itemName + "\n";
						}
					}

					int32_t userRollMod = userGainAmount + userLossAmount;

					int32_t userRollModTotal = (uint32_t)applyAsymptoticTransform((float)userRollMod, 2000.0f, 40.0f);

					int32_t baseProbabilityOfSuccess = 40;
					int32_t totalProbabilityOfSuccess = baseProbabilityOfSuccess + userRollModTotal;

					std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
					int32_t currentProbabilityValue = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 100.0f);
					bool currentSuccessValue = currentProbabilityValue > (100 - totalProbabilityOfSuccess);

					std::string msgString;

					if (currentSuccessValue == true) {
						msgString = "Nicely done! You robbed that motherfucker, <@!" + targetUserID + ">, good!\n\n__Base probability of success: __ " + std::to_string(baseProbabilityOfSuccess) + "% \n";

						if (userGainAmount > 0 || userLossAmount < 0) {
							msgString += userGainString + userLossString + "\n__Resulting in a net probability of success gain of:__ " + std::to_string(userRollModTotal) + "% \n__For a final probability of success of:__ " + std::to_string(totalProbabilityOfSuccess) + "% \n";
						}

						uint32_t currencyRobPercentage = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 60.0f);

						uint32_t currencyRobAmount = (uint32_t)trunc((float)targetGuildMember.data.currency.wallet * ((float)currencyRobPercentage / 100.0f));

						if (currencyRobAmount < 0) {
							std::string msgStringNew = "------\n**Cannot rob for debt!**\n------";
							EmbedData msgEmbed;
							msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
							msgEmbed.setColor(discordGuild.data.borderColor);
							msgEmbed.setDescription(msgStringNew);
							msgEmbed.setTimeStamp(getTimeAndDate());
							msgEmbed.setTitle("__**Target Issue:**__");
							RespondToInputEventData dataPackage(*args.eventData);
							dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
							dataPackage.addMessageEmbed(msgEmbed);
							auto newEvent = InputEvents::respondToEvent(dataPackage);
							return;
						}

						targetGuildMember.data.currency.wallet -= currencyRobAmount;
						targetGuildMember.writeDataToDB();
						discordGuildMember.data.currency.wallet += currencyRobAmount;
						discordGuildMember.writeDataToDB();
						uint32_t targetUserNewBalance = targetGuildMember.data.currency.wallet;
						uint32_t userNewBalance = discordGuildMember.data.currency.wallet;

						msgString = msgString + "------\n**You've robbed <@!" + targetUserID + "> for " + std::to_string(currencyRobPercentage) + "% of their wallet, which is " + std::to_string(currencyRobAmount) + " " + discordUser.data.currencyName + ".\n"
							+ "**\n__Your new wallet balances are:__\n<@!" + userID + ">: " + std::to_string(userNewBalance) + " " + discordUser.data.currencyName + "\n<@!" + targetUserID + ">: " + std::to_string(targetUserNewBalance) + " " +
							discordUser.data.currencyName;

						discordGuildMember.data.lastTimeRobbed = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						discordGuildMember.writeDataToDB();
						EmbedData messageEmbed;
						messageEmbed.setColor(discordGuild.data.borderColor);
						messageEmbed.setDescription(msgString);
						messageEmbed.setTimeStamp(getTimeAndDate());
						messageEmbed.setTitle("__**Succesful Robbery:**__");
						messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
						dataPackage.addContent("<@!" + targetUserID + ">");
						dataPackage.addMessageEmbed(messageEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);
					}
					else if (currentSuccessValue == false) {
						int32_t finedPercentage = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 30.0f);
						uint32_t finedAmount = (uint32_t)trunc(((float)discordGuildMember.data.currency.wallet * ((float)finedPercentage / 100.0f)));
						discordGuildMember.data.currency.wallet -= finedAmount;
						discordGuildMember.writeDataToDB();
						uint32_t userNewBalance = discordGuildMember.data.currency.wallet;

						int32_t repaidPercentage = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 50.0f);
						int32_t repaidAmount = static_cast<int32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 100.0f);
						targetGuildMember.data.currency.wallet += repaidAmount;
						targetGuildMember.writeDataToDB();
						uint32_t targetUserNewBalance = targetGuildMember.data.currency.wallet;

						msgString = "Oof! You've been caught while attempting to rob <@!" + targetUserID + ">!\n\n__Base probability of success:__ " + std::to_string(baseProbabilityOfSuccess) + "% \n";

						if (userGainAmount > 0 || userLossAmount < 0) {
							msgString += userGainString + userLossString + "\n__Resulting in a net probability of success gain of:__ " + std::to_string(userRollModTotal) + "%\n__For a final probability of success of:__ " + std::to_string(totalProbabilityOfSuccess) + "%\n";
						}

						msgString += "------\n**You've been fined " + std::to_string(finedPercentage) + "% of your wallet balance, which is " + std::to_string(finedAmount) + " " + discordUser.data.currencyName + "!\n<@!" + targetUserID + "> has been reimbursed " +
							std::to_string(repaidAmount) + " " + discordUser.data.currencyName + " (" + std::to_string(repaidPercentage) + "%).**";

						msgString += "\n\n__Your new wallet balances are:__\n<@!" + userID + ">: " + std::to_string(userNewBalance) + " " + discordUser.data.currencyName + "\n<@!" + targetUserID
							+ ">: " + std::to_string(targetUserNewBalance) + " " + discordUser.data.currencyName;

						discordGuildMember.data.lastTimeRobbed = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						discordGuildMember.writeDataToDB();

						EmbedData messageEmbed;
						messageEmbed.setColor(discordGuild.data.borderColor);
						messageEmbed.setDescription(msgString);
						messageEmbed.setTimeStamp(getTimeAndDate());
						messageEmbed.setTitle("__**Failed Robbery:**__");
						messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						RespondToInputEventData dataPackage(*args.eventData);
						dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
						dataPackage.addMessageEmbed(messageEmbed);
						auto newEvent = InputEvents::respondToEvent(dataPackage);

					}
				}
				else {
					std::string msgString;
					uint32_t timeLeft = timeBetweenRobberies - currentTimeDifference;
					uint32_t hoursRemain = (uint32_t)trunc(timeLeft / msPerHour);
					uint32_t minutesRemain = (uint32_t)trunc((timeLeft % msPerHour) / msPerMinute);
					uint32_t secondsRemain = (uint32_t)trunc(((timeLeft % msPerHour) % msPerMinute) / msPerSecond);

					if (hoursRemain > 0) {
						msgString = "Sorry, but you need to wait " + std::to_string(hoursRemain) + " hours, " + std::to_string(minutesRemain) + " minutes, and " + std::to_string(secondsRemain) + " seconds before you can rob someone again!";
					}
					else if (minutesRemain > 0) {
						msgString = "Sorry, but you need to wait " + std::to_string(minutesRemain) + " minutes and " + std::to_string(secondsRemain) + " seconds before you can rob someone again!";
					}
					else {
						msgString = "Sorry, but you need to wait " + std::to_string(secondsRemain) + " seconds before you can rob someone again!";
					}

					EmbedData messageEmbed;
					messageEmbed.setColor(discordGuild.data.borderColor);
					messageEmbed.setDescription(msgString);
					messageEmbed.setTimeStamp(getTimeAndDate());
					messageEmbed.setTitle("__**Failed Robbery:**__");
					messageEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					RespondToInputEventData dataPackage(*args.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(messageEmbed);
					auto newEvent = InputEvents::respondToEvent(dataPackage);
				}
				return;
			}
			catch (...) {
				reportException("Rob::execute()");
			}
		}
		virtual ~Rob() {};
	};
}
