// SellDrugs.hpp - Header for the "sell drugs" command.
// may 28, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class sell_drugs : public base_function {
	  public:
		sell_drugs() {
			this->commandName	  = "selldrugs";
			this->helpDescription = "gains you some currency!";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /selldrugs to gain currency.\n------");
			msgEmbed.setTitle("__**Sell drugs usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<sell_drugs>();
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
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				int64_t currentTime		= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				uint32_t msPerWorkCycle = discordUser.data.hoursOfDrugSaleCooldown * 60 * 60 * 1000;
				uint32_t msPerSecond	= 1000;
				uint32_t msPerMinute	= 60 * msPerSecond;
				uint32_t msPerHour		= 60 * msPerMinute;

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				discord_guild_member discordGuildMember(managerAgent, guildMember);
				user_data currentUser	= users::getUserAsync({ .userId = argsNew.getUserData().id }).get();
				uint32_t lastTimeWorked = discordGuildMember.data.lastTimeWorked;

				uint32_t timeDifference = (uint32_t)currentTime - lastTimeWorked;

				if (timeDifference >= msPerWorkCycle) {
					std::mt19937_64 randomEngine{ static_cast<uint64_t>(
						std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
					uint32_t amountEarned = static_cast<uint32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 5000.0f);

					discordGuildMember.data.currency.wallet += amountEarned;
					discordGuildMember.writeDataToDB(managerAgent);

					jsonifier::string msgString = "";
					msgString += "you've been busy dealing drugs... and you've earned " + jsonifier::toString(amountEarned) + " " + discordUser.data.currencyName +
								 "\nNice job and watch out for cops!\nYour new wallet balance is: ";
					msgString += jsonifier::toString(discordGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName;
					embed_data msgEmbed{};
					msgEmbed.setAuthor(currentUser.userName, currentUser.getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setDescription(msgString);
					msgEmbed.setTitle("__**Drug dealing:**__");
					msgEmbed.setColor("fefefe");
					msgEmbed.setTimeStamp(getTimeAndDate());
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_event_data event01 = input_events::respondToInputEventAsync(dataPackage).get();

					discordGuildMember.data.lastTimeWorked =
						(uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					discordGuildMember.writeDataToDB(managerAgent);
				} else {
					uint32_t timeLeft	 = msPerWorkCycle - timeDifference;
					uint32_t hoursLeft	 = (uint32_t)trunc(timeLeft / msPerHour);
					uint32_t minutesLeft = (uint32_t)trunc((timeLeft % msPerHour) / msPerMinute);
					uint32_t secondsLeft = (uint32_t)trunc(((timeLeft % msPerHour) % msPerMinute) / msPerSecond);

					jsonifier::string msgString = "";
					if (hoursLeft > 0) {
						msgString += "sorry, but you need to wait " + jsonifier::toString(hoursLeft) + " hours, " + jsonifier::toString(minutesLeft) + " minutes, and " +
									 jsonifier::toString(secondsLeft) + " seconds before you can get paid again!";
					} else if (minutesLeft > 0) {
						msgString += "sorry, but you need to wait " + jsonifier::toString(minutesLeft) + " minutes, and " + jsonifier::toString(secondsLeft) +
									 " seconds before you can get paid again!";
					} else {
						msgString += "sorry, but you need to wait " + jsonifier::toString(secondsLeft) + " seconds before you can get paid again!";
					}
					embed_data msgEmbed{};
					msgEmbed.setAuthor(currentUser.userName, currentUser.getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setDescription(msgString);
					msgEmbed.setTitle("__**Drug dealing:**__");
					msgEmbed.setColor("fefefe");
					msgEmbed.setTimeStamp(getTimeAndDate());
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_event_data event01 = input_events::respondToInputEventAsync(dataPackage).get();
				}
			} catch (const std::exception& error) {
				std::cout << "sell_drugs::execute()" << error.what() << std::endl;
			}
		}
		~sell_drugs(){};
	};
}// namespace discord_core_api
