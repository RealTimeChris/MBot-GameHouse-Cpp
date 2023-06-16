// SetBalance.hpp - Header for the "set balance" command.
// jun 26, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class set_balance : public base_function {
	  public:
		set_balance() {
			this->commandName	  = "setbalance";
			this->helpDescription = "sets your own or another server member's currency balances.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /setbalance newbalance, balancetype, @usermention or to set your own balance it's simply /setbalance = "
									"newbalance, balancetype.\n------");
			msgEmbed.setTitle("__**Set balance usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<set_balance>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				guild_member_data guildMember{ argsNew.getGuildMemberData() };


				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (!areWeAllowed) {
					return;
				}

				auto inputEventData			 = argsNew.getInputEventData();
				bool doWeHaveAdminPermission = doWeHaveAdminPermissions(argsNew, inputEventData, discordGuild, channel, guildMember);

				if (!doWeHaveAdminPermission) {
					return;
				}

				std::regex balanceRegExp{ "\\d{1,18}" };
				std::regex userMentionRegExp{ "<@!\\d{18,20}>" };
				std::regex userIDRegExp{ "\\d{18,20}" };
				snowflake targetUserID{};

				if (argsNew.getCommandArguments().values.size() == 0 || argsNew.getCommandArguments().values["amount"].operator std::streamoff() < 0) {
					jsonifier::string msgString = "------\n**Please enter a valid desired balance! (!setbalance = newbalance, balancetype, @usermention, or just "
											"!setbalance = newbalance, balancetype)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.getCommandArguments().values.size() < 2 ||
					(argsNew.getCommandArguments().values.at("balancetype").operator jsonifier::string() != "bank" &&
						argsNew.getCommandArguments().values.at("balancetype").operator jsonifier::string() != "wallet")) {
					jsonifier::string msgString = "------\n**Please enter a valid balance type! bank or wallet! (!setbalance = newbalance, balancetype, "
											"@usermention, or just !setbalance = newbalance, balancetype)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.getCommandArguments().values.size() < 3) {
					targetUserID = argsNew.getUserData().id;
				} else if (argsNew.getCommandArguments().values.size() == 3) {
					jsonifier::string msgString = "------\n**Please enter a valid target user mention, or leave it blank to select yourself as the target! "
											"(!setbalance = newbalance, balancetype, @usermention, or just "
											"!setbalance = newbalance, balancetype)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				} else if (argsNew.getCommandArguments().values.at("targetuser").operator jsonifier::string() != "") {
					targetUserID = argsNew.getCommandArguments().values["targetuser"].operator size_t();
				}

				uint32_t targetUserBalance	  = argsNew.getCommandArguments().values["amount"].operator std::streamoff();
				jsonifier::string balanceType = argsNew.getCommandArguments().values.at("balancetype").operator jsonifier::string();

				guild_member_data targetMember = guild_members::getCachedGuildMember({ .guildMemberId = targetUserID, .guildId = guild.id });

				if (targetMember.getUserData().userName == "") {
					jsonifier::string msgString = "------\n**sorry, but the specified user could not be found!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User_data issue:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				discord_guild_member discordGuildMember(managerAgent, targetMember);

				jsonifier::string msgString;
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				if (balanceType == "bank") {
					discordGuildMember.data.currency.bank = targetUserBalance;
					discordGuildMember.writeDataToDB(managerAgent);

					uint32_t newBalance = discordGuildMember.data.currency.bank;

					msgString = "__You've set the user <@!" + targetUserID + "> 's bank balance to:__ " + jsonifier::toString(newBalance) + " " + discordUser.data.currencyName;
				} else if (balanceType == "wallet") {
					discordGuildMember.data.currency.wallet = targetUserBalance;
					discordGuildMember.writeDataToDB(managerAgent);

					uint32_t newBalance = discordGuildMember.data.currency.wallet;

					msgString = "__You've set the user <@!" + targetUserID + ">'s wallet balance to:__ " + jsonifier::toString(newBalance) + " " + discordUser.data.currencyName;
				}

				embed_data msgEmbed{};
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Set new balance:**__");
				respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
				dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
				dataPackage.addContent("<@!" + targetUserID + ">");
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const std::exception& error) {
				std::cout << "set_balance::execute()" << error.what() << std::endl;
			}
		}
		~set_balance(){};
	};
}// namespace discord_core_api
