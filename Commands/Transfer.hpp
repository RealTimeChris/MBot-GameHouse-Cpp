// Transfer.hpp - Header for the "transfer" command.
// Jun 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {
	class Transfer :public BaseFunction {
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
			return  std::make_unique<Transfer>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();
			bool  areWeInADm = areWeInADM(*args.eventData, channel);

			if (areWeInADm == true) {
				return;
			}

			InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

			Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
			DiscordGuild discordGuild(guild);

			bool areWeAllowed = checkIfAllowedGamingInChannel(*args.eventData, discordGuild);

			if (!areWeAllowed) {
				return;
			}

			std::regex userMentionRegExp("\\d{18}");
			std::regex amountRegExp("\\d{1,18}");
			if (!regex_search(args.commandData.optionsArgs[0], amountRegExp) ||  std::stoll(args.commandData.optionsArgs[0]) <= 0) {
				std::string msgString = "------\n**Please enter a valid number for amount! (!transfer = AMOUNT, @USERMENTION)**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				InputEvents::respondToEvent(dataPackage);
				return;
			}

			std::cmatch matchResults{};
			regex_search(args.commandData.optionsArgs[1].c_str(), matchResults, userMentionRegExp);
			std::string toUserID = matchResults.str();
			std::string fromUserID = args.eventData->getAuthorId();
			uint32_t amount = (uint32_t)std::stoll(args.commandData.optionsArgs[0]);
			GuildMember toUserMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = toUserID ,.guildId = args.eventData->getGuildId(), }).get();

			if (toUserID == fromUserID) {
				std::string msgString = "------\n**Sorry, but you cannot transfer to yourself!**\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Transfer Issue:**__");
				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				InputEvents::respondToEvent(dataPackage);
				return;
			}
			DiscordGuildMember discordToGuildMember(toUserMember);
			if (toUserMember.user.userName == "" || discordToGuildMember.data.userName == "") {
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
				InputEvents::respondToEvent(dataPackage);
				return;
			}

			GuildMember fromGuildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = fromUserID , .guildId = args.eventData->getGuildId(), }).get();
			DiscordGuildMember discordFromGuildMember(fromGuildMember);

			if (amount > discordFromGuildMember.data.currency.wallet) {
				std::string msgString = "------\n**Sorry, but you don't have sufficient funds in your wallet for that transfer!**\n-------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Insufficient Funds:**__");
				RespondToInputEventData dataPackage(*args.eventData);
				dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				InputEvents::respondToEvent(dataPackage);
				return;
			}
			discordFromGuildMember.data.currency.wallet -= amount;
			discordFromGuildMember.writeDataToDB();
			discordToGuildMember.data.currency.wallet += amount;
			discordToGuildMember.writeDataToDB();
			auto botUser = args.discordCoreClient->getBotUser();
			DiscordUser discordUser(botUser.userName, botUser.id);
			std::string msgString;
			msgString += "<@!" + fromUserID + "> succesfully transferred " + std::to_string(amount) + " " + discordUser.data.currencyName + " to <@!" + toUserID + ">.";
			msgString += "\n__Your new wallet balances are:__ \n<@!" + fromUserID + ">: " + std::to_string(discordFromGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName;
			msgString += "\n<@!" + toUserID + ">: " + std::to_string(discordToGuildMember.data.currency.wallet) + " " + discordUser.data.currencyName;
			EmbedData msgEmbed;
			msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
			msgEmbed.setColor(discordGuild.data.borderColor);
			msgEmbed.setDescription(msgString);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Balance Transfer:**__");
			RespondToInputEventData dataPackage(*args.eventData);
			dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
			dataPackage.addMessageEmbed(msgEmbed);
			dataPackage.addContent("<@!" + toUserID + ">");
			InputEvents::respondToEvent(dataPackage);
			return;
		}
		virtual ~Transfer() {};
	};
}
