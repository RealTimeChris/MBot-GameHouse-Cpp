// Leaderboard.hpp - Header for the "leaderboard" command.
// Jun 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _LEADERBOARD_
#define _LEADERBOARD_

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Leaderboard :public BaseFunction {
	public:
		Leaderboard() {
			this->commandName = "leaderboard";
			this->helpDescription = "Displays the leaderboard for currency on the current server.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /leaderboard.\n------");
			msgEmbed.setTitle("__**Leaderboard Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<Leaderboard>();
		}

		virtual void execute(BaseFunctionArguments& args) {
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

			std::unique_ptr<InputEventData> newEvent = std::make_unique<InputEventData>(*args.eventData);
			if (args.eventData->eventType == InputEventType::Application_Command_Interaction) {
				RespondToInputEventData dataPackage(*newEvent);
				dataPackage.setResponseType(InputEventResponseType::Deferred_Response);
				newEvent = InputEvents::respondToEvent(dataPackage);
			}

			std::vector<DiscordGuildMember> membersArray;
			for (auto [key, value] : guild.members) {
				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = value.user.id,.guildId = args.eventData->getGuildId() }).get();
				DiscordGuildMember guildMemberNew(guildMember);
				membersArray.push_back(guildMemberNew);
			}
			
			uint32_t minIdx = 0;
			uint32_t len = (uint32_t)membersArray.size();
			for (uint32_t x = 0; x < len; x += 1) {
				minIdx = x;
				for (uint32_t y = x + 1; y < len; y += 1) {
					if (membersArray[y].data.currency.wallet > membersArray[minIdx].data.currency.wallet) {
						minIdx = y;
					}
				}
				DiscordGuildMember temp = membersArray[x];
				membersArray[x] = membersArray[minIdx];
				membersArray[minIdx] = temp;
			}
			
			uint32_t membersPerPage = 20;
			uint32_t totalPageCount = 0;
			if (membersArray.size() % membersPerPage > 0) {
				totalPageCount = (uint32_t)trunc(membersArray.size() / membersPerPage) + 1;
			}
			else {
				totalPageCount = (uint32_t)trunc(membersArray.size() / membersPerPage);
			}
			uint32_t currentPage = 0;
			std::vector<EmbedData> pageEmbeds;
			std::vector<std::string> pageStrings;
			auto botUser = args.discordCoreClient->getBotUser();
			DiscordUser discordUser(botUser.userName, botUser.id);
			for (uint32_t x = 0; x < (uint32_t)membersArray.size(); x += 1) {
				if (x% membersPerPage ==  0) {
					pageEmbeds.push_back(EmbedData());
					pageStrings.push_back("");
				}			

				std::string msgString = "";
				msgString += "__**#" + std::to_string(currentPage * membersPerPage + ((x % membersPerPage) + 1)) + " | Name:**__ " + membersArray[x].data.guildMemberMention + "** | __" + discordUser.data.currencyName +
					":__** " + std::to_string(membersArray[x].data.currency.wallet) + "\n";				 

					pageStrings[currentPage] += msgString;
				if (x% membersPerPage ==  membersPerPage - 1 || x ==  (uint32_t)membersArray.size() - 1) {
					pageEmbeds[currentPage].setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					pageEmbeds[currentPage].setDescription(pageStrings[currentPage]);
					pageEmbeds[currentPage].setTimeStamp(getTimeAndDate());
					pageEmbeds[currentPage].setTitle("__**Leaderboard (Wallet) (Page " + std::to_string(currentPage + 1) + " of " + std::to_string(totalPageCount) + ")**__");
					pageEmbeds[currentPage].setColor(discordGuild.data.borderColor);
					currentPage += 1;
				}
			}

			uint32_t currentPageIndex = 0;
			std::string userID = args.eventData->getAuthorId();
			moveThroughMessagePages(userID, std::make_unique<InputEventData>(*newEvent), currentPageIndex, pageEmbeds, true, 120000);
			discordGuild.writeDataToDB();
			return;

		}
		virtual ~Leaderboard() {};
	};
}
#endif