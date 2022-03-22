// AddShopRole.hpp - Header for the "add shop role" command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _ADD_SHOP_ROLE_
#define _ADD_SHOP_ROLE_

#include "Index.hpp"

namespace DiscordCoreAPI {

	class AddShopRole :public BaseFunction {
	public:
		AddShopRole() {
			this->commandName = "addshoprole";
			this->helpDescription = "Add a role to the server's shop";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nEnter /addshoprole NAME, HEXCOLORVALUE, COST.\n------");
			msgEmbed.setTitle("__**Add Shop Role Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<AddShopRole>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = args.eventData->getChannelId() }).get();

				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm == true) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData));

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId(),.guildId = args.eventData->getGuildId()  }).get();

				bool doWeHaveAdmin = doWeHaveAdminPermissions(args, *args.eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdmin == false) {
					return;
				}
				
				bool areWeAllowed = checkIfAllowedGamingInChannel(*args.eventData,  discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				std::regex hexColorRegExp(".{1,24}");
				std::regex costRegExp("\\d{1,8}");
				if (args.commandData.optionsArgs.size() < 2 || !regex_search(args.commandData.optionsArgs.at(1), hexColorRegExp)) {
					std::string msgString = "------\n**Please enter a valid hex color value! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto event = InputEvents::respondToEvent(dataPackage);
					return;
				}
				if (args.commandData.optionsArgs.size() < 3 || !regex_search(args.commandData.optionsArgs.at(2), costRegExp) ||  std::stoll(args.commandData.optionsArgs.at(2)) <= 0) {
					std::string msgString = "------\n**Please enter a valid cost value! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ *args.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto event = InputEvents::respondToEvent(dataPackage);
					return;
				}

				std::cmatch matchResults;
				std::string roleName = args.commandData.optionsArgs.at(0).c_str();
				regex_search(args.commandData.optionsArgs.at(1).c_str(), matchResults, hexColorRegExp);
				std::string roleColor = matchResults.str();
				regex_search(args.commandData.optionsArgs.at(2).c_str(), matchResults, costRegExp);
				uint32_t roleCost = (uint32_t) std::stoll(matchResults.str());

				for (auto& value : discordGuild.data.guildShop.roles) {
					if (roleName == value.roleName) {
						std::string msgString = "------\n**Sorry, but a role by that name already exists!**\n------";
						EmbedData msgEmbed;
						msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						RespondToInputEventData dataPackage{ *args.eventData };
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto event = InputEvents::respondToEvent(dataPackage);
						return;
					}
				}

				Permissions  rolePermsString{ "" };
				std::vector<Permission> permissions;
				permissions.push_back(Permission::Create_Instant_Invite);
				permissions.push_back(Permission::Add_Reactions);
				permissions.push_back(Permission::View_Channel);
				permissions.push_back(Permission::Send_Messages);
				permissions.push_back(Permission::Change_Nickname);
				permissions.push_back(Permission::Use_External_Emojis);
				permissions.push_back(Permission::Connect);
				permissions.push_back(Permission::Embed_Links);
				permissions.push_back(Permission::Attach_Files);
				permissions.push_back(Permission::Speak);
				permissions.push_back(Permission::Use_Application_Commands);
				permissions.push_back(Permission::Read_Message_History);
				permissions.push_back(Permission::Stream);
				permissions.push_back(Permission::Request_To_Speak);
				permissions.push_back(Permission::Send_Tts_Messages);
				permissions.push_back(Permission::Create_Private_Threads);
				permissions.push_back(Permission::Use_Vad);

				rolePermsString.addPermissions(permissions);

				CreateGuildRoleData createRoleData{};
				createRoleData.hexColorValue = roleColor;
				createRoleData.guildId = args.eventData->getGuildId();
				createRoleData.hoist = true;
				createRoleData.mentionable = true;
				createRoleData.name = roleName;
				createRoleData.permissions = rolePermsString;
				Role role = Roles::createGuildRoleAsync(createRoleData).get();
				if (role.id == "") {
					throw std::exception("Role not initialized!");
				}
				InventoryRole currentRole;
				currentRole.roleCost = roleCost;
				currentRole.roleId = role.id;
				currentRole.roleName = role.name;

				discordGuild.data.guildShop.roles.push_back(currentRole);
				discordGuild.writeDataToDB();
				std::string msgString = "";
				auto botUser = args.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				msgString = "Nicely done! You've added a new role to the store's inventory, giving the server access to it!\nIt is as follows:\n------\n__**Role:**__ <@&" + currentRole.roleId + "> __**Cost**__ : " + std::to_string(roleCost) + " "
					+ discordUser.data.currencyName + "\n------";
				EmbedData msgEmbed;
				msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**New Role Added:**__");
				RespondToInputEventData dataPackage{ *args.eventData };
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				InputEvents::respondToEvent(dataPackage);
				return;
			}
			catch (...) {
				reportException("AddShopRole::executeAsync()");
			}
		}
		virtual ~AddShopRole() {};
	};
}
#endif
