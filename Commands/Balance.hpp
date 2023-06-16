// Balance.hpp - Header for the "balance" command.
// may 28, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class balance : public base_function {
	  public:
		balance() {
			this->commandName	  = "balance";
			this->helpDescription = "display yours or another server member's currency balance.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /balance to view your own balance, or /balance "
									"@usermention to view someone else's balances.\n------");
			msgEmbed.setTitle("__**Balance usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<balance>();
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

				snowflake userID{};
				uint32_t bankAmount	  = 0;
				uint32_t walletAmount = 0;

				std::regex mentionRegExp{ "<@!\\d{18,20}>" };
				std::regex idRegExp{ "\\d{18,20}" };
				if (argsNew.getCommandArguments().values["person"].operator jsonifier::string() == "0") {
					userID = argsNew.getUserData().id;
				}

				guild_member_data guildMember = guild_members::getCachedGuildMember({ .guildMemberId = userID, .guildId = guild.id });

				if (guildMember.user.id == 0) {
					jsonifier::string msgString = "------\n**sorry, but that user could not be found!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User_data issue:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_event_data eventNew = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				discord_guild_member discordGuildMember(managerAgent, guildMember);

				jsonifier::string msgString = "";
				bankAmount			  = discordGuildMember.data.currency.bank;
				walletAmount		  = discordGuildMember.data.currency.wallet;
				auto botUser		  = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				msgString = "<@!" + guildMember.user.id + "> 's balances are:\n------\n__**Bank balance:**__ " + jsonifier::toString(bankAmount) + " " + discordUser.data.currencyName +
							"\n__**Wallet balance:**__ " + jsonifier::toString(walletAmount) + " " + discordUser.data.currencyName + "\n------";

				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setDescription(msgString);
				msgEmbed.setColor("fefefe");
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Current balances:**__");
				respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
				dataPackage.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				input_event_data eventNew = input_events::respondToInputEventAsync(dataPackage).get();
			} catch (const std::exception& error) {
				std::cout << "balance::execute()" << error.what() << std::endl;
			}
		}

		~balance(){};
	};

}// namespace discord_core_api
