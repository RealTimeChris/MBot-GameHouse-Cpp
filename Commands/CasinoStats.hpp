// CasinoStats.hpp - Header for the "Casino Stats" command.
// jun 22, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class casino_stats_function : public base_function {
	  public:
		casino_stats_function() {
			this->commandName	  = "casinostats";
			this->helpDescription = "view stats about this server's casino.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /casinostats.\n------");
			msgEmbed.setTitle("__**Casino stats usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<casino_stats_function>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				bool areWeAllowedHere = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (!areWeAllowedHere) {
					return;
				}

				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				guild_member_data guildMember{ argsNew.getGuildMemberData() };
				jsonifier::vector<embed_field_data> fields;
				embed_field_data field1;
				field1.name	 = "__**Largest coinflip payout:**__";
				field1.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestCoinFlipPayout.userId + "> " + discordGuild.data.casinoStats.largestCoinFlipPayout.userName +
							   "\n__Amount:__ " + jsonifier::toString(discordGuild.data.casinoStats.largestCoinFlipPayout.amount) + " " + discordUser.data.currencyName +
							   "\n__Date:__ " + discordGuild.data.casinoStats.largestCoinFlipPayout.timeStamp;
				field1.Inline = true;
				fields.emplace_back(field1);
				embed_field_data field2;
				field2.name	 = "__**Largest roulette payout:**__";
				field2.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestRoulettePayout.userId + "> " + discordGuild.data.casinoStats.largestRoulettePayout.userName +
							   "\n__Amount:__ " + jsonifier::toString(discordGuild.data.casinoStats.largestRoulettePayout.amount) + " " + discordUser.data.currencyName +
							   "\n__Date:__ " + discordGuild.data.casinoStats.largestRoulettePayout.timeStamp;
				field2.Inline = true;
				fields.emplace_back(field2);
				embed_field_data field3;
				field3.name	 = "__**Largest blackjack payout:**__";
				field3.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestBlackjackPayout.userId + "> " +
							   discordGuild.data.casinoStats.largestBlackjackPayout.userName + "\n__Amount:__ " +
							   jsonifier::toString(discordGuild.data.casinoStats.largestBlackjackPayout.amount) + " " + discordUser.data.currencyName + "\n__Date:__ " +
							   discordGuild.data.casinoStats.largestBlackjackPayout.timeStamp;
				field3.Inline = true;
				fields.emplace_back(field3);
				embed_field_data field4;
				field4.name	 = "__**Largest slots payout:**__";
				field4.value = "__User:__ <@!" + discordGuild.data.casinoStats.largestSlotsPayout.userId + "> " + discordGuild.data.casinoStats.largestSlotsPayout.userName +
							   "\n__Amount:__ " + jsonifier::toString(discordGuild.data.casinoStats.largestSlotsPayout.amount) + " " + discordUser.data.currencyName + "\n__Date:__ " +
							   discordGuild.data.casinoStats.largestSlotsPayout.timeStamp;
				field4.Inline = true;
				fields.emplace_back(field4);
				embed_field_data field5;
				field5.name	  = "__ **net coinflip payout:**__";
				field5.value  = "__Amount:__ " + jsonifier::toString(discordGuild.data.casinoStats.totalCoinFlipPayout) + " " + discordUser.data.currencyName;
				field5.Inline = true;
				fields.emplace_back(field5);
				embed_field_data field6;
				field6.name	  = "__**Net roulette payout:**__";
				field6.value  = "__Amount:__ " + jsonifier::toString(discordGuild.data.casinoStats.totalRoulettePayout) + " " + discordUser.data.currencyName;
				field6.Inline = true;
				fields.emplace_back(field6);
				embed_field_data field7;
				field7.name	  = "__**Net blackjack payout:**__";
				field7.value  = "__Amount:__ " + jsonifier::toString(discordGuild.data.casinoStats.totalBlackjackPayout) + " " + discordUser.data.currencyName;
				field7.Inline = true;
				fields.emplace_back(field7);
				embed_field_data field8;
				field8.name	  = "__**Net slots payout:**__";
				field8.value  = "__Amount:__ " + jsonifier::toString(discordGuild.data.casinoStats.totalSlotsPayout) + " " + discordUser.data.currencyName;
				field8.Inline = true;
				fields.emplace_back(field8);

				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription(
					"__**Net casino payout:**__\n__Amount:__ " + jsonifier::toString(discordGuild.data.casinoStats.totalPayout) + " " + discordUser.data.currencyName);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Server casino stats:**__");
				msgEmbed.fields = fields;
				respond_to_input_event_data dataPackage(argsNew.getInputEventData());
				dataPackage.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				input_events::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const std::exception& error) {
				std::cout << "casino_stats::execute()" << error.what() << std::endl;
			}
		};

		~casino_stats_function(){};
	};
}// namespace discord_core_api