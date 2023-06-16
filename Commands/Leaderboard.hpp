// Leaderboard.hpp - Header for the "leaderboard" command.
// jun 24, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class leaderboard : public base_function {
	  public:
		leaderboard() {
			this->commandName	  = "leaderboard";
			this->helpDescription = "displays the leaderboard for currency on the current server.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /leaderboard.\n------");
			msgEmbed.setTitle("__**Leaderboard usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<leaderboard>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ guilds::getGuildAsync({ .guildId = argsNew.getInteractionData().guildId }).get() };

				discord_guild discordGuild{ managerAgent, guild };

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (areWeAllowed == false) {
					return;
				}
				input_event_data newEvent = argsNew.getInputEventData();
				respond_to_input_event_data dataPackage(newEvent);
				dataPackage.setResponseType(input_event_response_type::Deferred_Response);
				newEvent = input_events::respondToInputEventAsync(dataPackage).get();
				jsonifier::vector<discord_guild_member> membersArray;
				for (auto value : guild.members) {
					guild_member_data guildMember = guild_members::getCachedGuildMember({ .guildMemberId = value.user.id, .guildId = guild.id });
					discord_guild_member guildMemberNew(managerAgent, guildMember);
					membersArray.emplace_back(guildMemberNew);
				}
				uint32_t minIdx = 0;
				uint32_t len	= (uint32_t)membersArray.size();
				for (uint32_t x = 0; x < len; x += 1) {
					minIdx = x;
					for (uint32_t y = x + 1; y < len; y += 1) {
						if (membersArray[y].data.currency.wallet > membersArray[minIdx].data.currency.wallet) {
							minIdx = y;
						}
					}
					discord_guild_member temp = membersArray[x];
					membersArray[x]			= membersArray[minIdx];
					membersArray[minIdx]	= temp;
				}

				uint32_t membersPerPage = 20;
				uint32_t totalPageCount = 0;
				if (membersArray.size() % membersPerPage > 0) {
					totalPageCount = (uint32_t)trunc(membersArray.size() / membersPerPage) + 1;
				} else {
					totalPageCount = (uint32_t)trunc(membersArray.size() / membersPerPage);
				}
				uint32_t currentPage = 0;
				jsonifier::vector<embed_data> pageEmbeds{};
				jsonifier::vector<jsonifier::string> pageStrings;
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				for (uint32_t x = 0; x < (uint32_t)membersArray.size(); x += 1) {
					if (x % membersPerPage == 0) {
						pageEmbeds.emplace_back(embed_data());
						pageStrings.emplace_back("");
					}

					jsonifier::string msgString = "";
					msgString += "__**#" + jsonifier::toString(currentPage * membersPerPage + ((x % membersPerPage) + 1)) + " | name:**__ " + membersArray[x].data.guildMemberMention +
								 "** | __" + discordUser.data.currencyName + ":__** " + jsonifier::toString(membersArray[x].data.currency.wallet) + "\n";

					pageStrings[currentPage] += msgString;
					if (x % membersPerPage == membersPerPage - 1 || x == (uint32_t)membersArray.size() - 1) {
						pageEmbeds[currentPage].setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						pageEmbeds[currentPage].setDescription(pageStrings[currentPage]);
						pageEmbeds[currentPage].setTimeStamp(getTimeAndDate());
						pageEmbeds[currentPage].setTitle("__**Leaderboard (wallet) (page " + jsonifier::toString(currentPage + 1) + " of " + jsonifier::toString(totalPageCount) + ")**__");
						pageEmbeds[currentPage].setColor("fefefe");
						currentPage += 1;
					}
				}
				uint32_t currentPageIndex = 0;
				snowflake userID		  = argsNew.getUserData().id;
				if (pageEmbeds.size() > 0) {
					moveThroughMessagePages(userID, newEvent, currentPageIndex, pageEmbeds, true, 120000, false);
				}
				discordGuild.writeDataToDB(managerAgent);
				return;
			} catch (const std::exception& error) {
				std::cout << "leaderboard::execute()" << error.what() << std::endl;
			}
		}
		~leaderboard(){};
	};
}// namespace discord_core_api
