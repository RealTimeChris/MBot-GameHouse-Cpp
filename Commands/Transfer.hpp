// Transfer.hpp - Header for the "transfer" command.
// jun 29, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {
	class transfer : public base_function {
	  public:
		transfer() {
			this->commandName	  = "transfer";
			this->helpDescription = "transfers currency from yourself to another server member.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /transfer amount, @usermention.\n------");
			msgEmbed.setTitle("__**Transfer usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<transfer>();
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

				std::regex userMentionRegExp{ "\\d{18,20}" };
				std::regex amountRegExp{ "\\d{1,18}" };
				if (argsNew.getCommandArguments().values["amount"].operator std::streamoff() <= 0) {
					jsonifier::string msgString = "------\n**Please enter a valid number for amount! (!transfer = amount, @usermention)**\n------";
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
				}

				snowflake toUserID{ argsNew.getCommandArguments().values["user"].operator size_t() };
				snowflake fromUserID{ argsNew.getUserData().id };
				uint32_t amount				 = argsNew.getCommandArguments().values["amount"].operator std::streamoff();
				guild_member_data toUserMember = guild_members::getCachedGuildMember({
					.guildMemberId = toUserID,
					.guildId	   = guild.id,
				});

				if (toUserID == fromUserID) {
					jsonifier::string msgString = "------\n**sorry, but you cannot transfer to yourself!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Transfer issue:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				discord_guild_member discordToGuildMember(managerAgent, toUserMember);
				if (toUserMember.getUserData().userName == "" || discordToGuildMember.data.userName == "") {
					jsonifier::string msgString = "------\n**sorry, but that user could not be found!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User_data issue:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				guild_member_data fromGuildMember = guild_members::getCachedGuildMember({
					.guildMemberId = fromUserID,
					.guildId	   = guild.id,
				});
				discord_guild_member discordFromGuildMember(managerAgent, fromGuildMember);

				if (amount > discordFromGuildMember.data.currency.wallet) {
					jsonifier::string msgString = "------\n**sorry, but you don't have sufficient funds in your wallet for that transfer!**\n-------";
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
				discordFromGuildMember.data.currency.wallet -= amount;
				discordFromGuildMember.writeDataToDB(managerAgent);
				discordToGuildMember.data.currency.wallet += amount;
				discordToGuildMember.writeDataToDB(managerAgent);
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				jsonifier::string msgString;
				msgString += "<@!" + fromUserID + "> succesfully transferred " + jsonifier::toString(amount) + " " + discordUser.data.currencyName + " to <@!" + toUserID + ">.";
				msgString += "\n__Your new wallet balances are:__ \n<@!" + fromUserID + ">: " + jsonifier::toString(discordFromGuildMember.data.currency.wallet) + " " +
							 discordUser.data.currencyName;
				msgString += "\n<@!" + toUserID + ">: " + jsonifier::toString(discordToGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName;
				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Balance transfer:**__");
				respond_to_input_event_data dataPackage(argsNew.getInputEventData());
				dataPackage.setResponseType(input_event_response_type::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				dataPackage.addContent("<@!" + toUserID + ">");
				input_events::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const std::exception& error) {
				std::cout << "transfer::execute()" << error.what() << std::endl;
			}
		}
		~transfer(){};
	};
}// namespace discord_core_api
