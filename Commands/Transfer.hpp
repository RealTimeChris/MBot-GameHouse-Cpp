// Transfer.hpp - Header for the "transfer" command.
// Jun 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class Transfer : public BaseFunction {
	  public:
		Transfer() {
			this->commandName = "transfer";
			this->helpDescription = "Transfers currency from yourself to another server member.";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /transfer AMOUNT, @USERMENTION.\n------");
			msgEmbed.setTitle("__**Transfer Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Transfer>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ argsNew.eventData.getChannelId() }).get();

				Guild guild = Guilds::getGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.eventData, discordGuild);

				if (!areWeAllowed) {
					return;
				}

				std::regex userMentionRegExp("\\d{18}");
				std::regex amountRegExp("\\d{1,18}");
				if (!regex_search(argsNew.commandData.optionsArgs[0], amountRegExp) || std::stoll(argsNew.commandData.optionsArgs[0]) <= 0) {
					std::string msgString = "------\n**Please enter a valid number for amount! (!transfer = AMOUNT, @USERMENTION)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				std::cmatch matchResults{};
				regex_search(argsNew.commandData.optionsArgs[1].c_str(), matchResults, userMentionRegExp);
				uint64_t toUserID = stoull(matchResults.str());
				uint64_t fromUserID = argsNew.eventData.getAuthorId();
				uint32_t amount = ( uint32_t )std::stoll(argsNew.commandData.optionsArgs[0]);
				GuildMember toUserMember = GuildMembers::getCachedGuildMemberAsync({
																					   .guildMemberId = toUserID,
																					   .guildId = argsNew.eventData.getGuildId(),
																				   })
											   .get();

				if (toUserID == fromUserID) {
					std::string msgString = "------\n**Sorry, but you cannot transfer to yourself!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Transfer Issue:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				DiscordGuildMember discordToGuildMember(toUserMember);
				if (toUserMember.userName == "" || discordToGuildMember.data.userName == "") {
					std::string msgString = "------\n**Sorry, but that user could not be found!**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**User Issue:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				GuildMember fromGuildMember = GuildMembers::getCachedGuildMemberAsync({
																						  .guildMemberId = fromUserID,
																						  .guildId = argsNew.eventData.getGuildId(),
																					  })
												  .get();
				DiscordGuildMember discordFromGuildMember(fromGuildMember);

				if (amount > discordFromGuildMember.data.currency.wallet) {
					std::string msgString = "------\n**Sorry, but you don't have sufficient funds in your wallet for that transfer!**\n-------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Insufficient Funds:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				discordFromGuildMember.data.currency.wallet -= amount;
				discordFromGuildMember.writeDataToDB();
				discordToGuildMember.data.currency.wallet += amount;
				discordToGuildMember.writeDataToDB();
				auto botUser = argsNew.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				std::string msgString;
				msgString += "<@!" + std::to_string(fromUserID) + "> succesfully transferred " + std::to_string(amount) + " " + discordUser.data.currencyName + " to <@!" +
					std::to_string(toUserID) + ">.";
				msgString += "\n__Your new wallet balances are:__ \n<@!" + std::to_string(fromUserID) + ">: " + std::to_string(discordFromGuildMember.data.currency.wallet) + " " +
					discordUser.data.currencyName;
				msgString += "\n<@!" + std::to_string(toUserID) + ">: " + std::to_string(discordToGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName;
				EmbedData msgEmbed;
				msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Balance Transfer:**__");
				RespondToInputEventData dataPackage(argsNew.eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				dataPackage.addContent("<@!" + std::to_string(toUserID) + ">");
				InputEvents::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (...) {
				reportException("Transfer::execute()");
			}
		}
		~Transfer(){};
	};
}
