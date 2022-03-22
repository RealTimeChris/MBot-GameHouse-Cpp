// CasinoStats.hpp - Header for the "Casino Stats" command.
// Jun 22, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _CASINO_STATS_
#define _CASINO_STATS_

#include "Index.hpp"

namespace DiscordCoreAPI {

	class CasinoStatsFunction :public BaseFunction {
	public:
		CasinoStatsFunction() {
			this->commandName = "casinostats";
			this->helpDescription = "View stats about this server's casino.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /casinostats.\n------");
			msgEmbed.setTitle("__**Casino Stats Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<CasinoStatsFunction>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();

			bool areWeInADm = areWeInADM(*args.eventData, channel);

			if (areWeInADm) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			bool areWeAllowedHere = checkIfAllowedGamingInChannel(*args.eventData, discordGuild);

			if (!areWeAllowedHere) {
				return;
			}

			auto botUser = args.discordCoreClient->getBotUser();
			DiscordUser discordUser(botUser.userName, botUser.id);
			GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId(),.guildId = args.eventData->getGuildId() }).get();
			std::vector<EmbedFieldData> fields;
			EmbedFieldData field1;
			field1.name = "__**Largest Coinflip Payout:**__";
			field1.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestCoinFlipPayout.userId + "> " + discordGuild.data.casinoStats.largestCoinFlipPayout.userName + "\n__Amount:__ " +
				std::to_string(discordGuild.data.casinoStats.largestCoinFlipPayout.amount) + " " + discordUser.data.currencyName + "\n__Date:__ " + discordGuild.data.casinoStats.largestCoinFlipPayout.timeStamp;
			field1.Inline = true;
			fields.push_back(field1);
			EmbedFieldData field2;
			field2.name = "__**Largest Roulette Payout:**__";
			field2.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestRoulettePayout.userId + "> " + discordGuild.data.casinoStats.largestRoulettePayout.userName + "\n__Amount:__ " +
				std::to_string(discordGuild.data.casinoStats.largestRoulettePayout.amount) + " " + discordUser.data.currencyName + "\n__Date:__ " + discordGuild.data.casinoStats.largestRoulettePayout.timeStamp;
			field2.Inline = true;
			fields.push_back(field2);
			EmbedFieldData field3;
			field3.name = "__**Largest Blackjack Payout:**__";
			field3.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestBlackjackPayout.userId + "> " + discordGuild.data.casinoStats.largestBlackjackPayout.userName + "\n__Amount:__ " +
				std::to_string(discordGuild.data.casinoStats.largestBlackjackPayout.amount) + " " + discordUser.data.currencyName + "\n__Date:__ " + discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp;
			field3.Inline = true;
			fields.push_back(field3);
			EmbedFieldData field4;
			field4.name = "__**Largest Slots Payout:**__";
			field4.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestSlotsPayout.userId + "> " + discordGuild.data.casinoStats.largestSlotsPayout.userName + "\n__Amount:__ " +
				std::to_string(discordGuild.data.casinoStats.largestSlotsPayout.amount) + " " + discordUser.data.currencyName + "\n__Date:__ " + discordGuild.data.casinoStats.largestSlotsPayout.timeStamp;
			field4.Inline = true;
			fields.push_back(field4);
			EmbedFieldData field5;
			field5.name = "__ **Net Coinflip Payout:**__";
			field5.value = "__Amount:__ " + std::to_string(discordGuild.data.casinoStats.totalCoinFlipPayout) + " " + discordUser.data.currencyName;
			field5.Inline = true;
			fields.push_back(field5);
			EmbedFieldData field6;
			field6.name = "__**Net Roulette Payout:**__";
			field6.value = "__Amount:__ " + std::to_string(discordGuild.data.casinoStats.totalRoulettePayout) + " " + discordUser.data.currencyName;
			field6.Inline = true;
			fields.push_back(field6);
			EmbedFieldData field7;
			field7.name = "__**Net Blackjack Payout:**__";
			field7.value = "__Amount:__ " + std::to_string(discordGuild.data.casinoStats.totalBlackjackPayout) + " " + discordUser.data.currencyName;
			field7.Inline = true;
			fields.push_back(field7);
			EmbedFieldData field8;
			field8.name = "__**Net Slots Payout:**__";
			field8.value = "__Amount:__ " + std::to_string(discordGuild.data.casinoStats.totalSlotsPayout) + " " + discordUser.data.currencyName;
			field8.Inline = true;
			fields.push_back(field8);

			EmbedData msgEmbed;
			msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription("__**Net Casino Payout:**__\n__Amount:__ " + std::to_string(discordGuild.data.casinoStats.totalPayout) + " " + discordUser.data.currencyName);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Server Casino Stats:**__");
			msgEmbed.fields = fields;
			RespondToInputEventData dataPackage(*args.eventData);
			dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
			dataPackage.addMessageEmbed(msgEmbed);
			InputEvents::respondToEvent(dataPackage);
			return;
		};

		virtual ~CasinoStatsFunction() {};

	};
}
#endif
