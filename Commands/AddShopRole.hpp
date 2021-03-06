// AddShopRole.hpp - Header for the "add shop role" command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class AddShopRole : public BaseFunction {
	  public:
		AddShopRole() {
			this->commandName = "addshoprole";
			this->helpDescription = "Add a role to the server's shop";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter /addshoprole NAME, HEXCOLORVALUE, COST.\n------");
			msgEmbed.setTitle("__**Add Shop Role Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<AddShopRole>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = argsNew.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				GuildMember guildMember = GuildMembers::getGuildMemberAsync({ .guildMemberId = argsNew.eventData.getAuthorId(), .guildId = argsNew.eventData.getGuildId() }).get();

				bool doWeHaveAdmin = doWeHaveAdminPermissions(argsNew, argsNew.eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdmin == false) {
					return;
				}

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.eventData, discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				std::regex hexColorRegExp{ ".{1,24}" };
				std::regex costRegExp{ "\\d{1,8}" };
				if (argsNew.optionsArgs.size() < 2 || !regex_search(argsNew.optionsArgs.at(1), hexColorRegExp)) {
					std::string msgString = "------\n**Please enter a valid hex color value! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto event = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.optionsArgs.size() < 3 || !regex_search(argsNew.optionsArgs.at(2), costRegExp) || std::stoll(argsNew.optionsArgs.at(2)) <= 0) {
					std::string msgString = "------\n**Please enter a valid cost value! (!addshoprole = NAME, HEXCOLORVALIE, COST)**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
					RespondToInputEventData dataPackage{ argsNew.eventData };
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto event = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				std::cmatch matchResults;
				std::string roleName = argsNew.optionsArgs.at(0).c_str();
				regex_search(argsNew.optionsArgs.at(1).c_str(), matchResults, hexColorRegExp);
				std::string roleColor = matchResults.str();
				regex_search(argsNew.optionsArgs.at(2).c_str(), matchResults, costRegExp);
				uint32_t roleCost = ( uint32_t )std::stoll(matchResults.str());

				for (auto& value: discordGuild.data.guildShop.roles) {
					if (roleName == value.roleName) {
						std::string msgString = "------\n**Sorry, but a role by that name already exists!**\n------";
						EmbedData msgEmbed{};
						msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
						msgEmbed.setColor(discordGuild.data.borderColor);
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
						RespondToInputEventData dataPackage{ argsNew.eventData };
						dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto event = InputEvents::respondToInputEventAsync(dataPackage).get();
						return;
					}
				}

				Permissions rolePermsString{ "" };
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
				createRoleData.guildId = argsNew.eventData.getGuildId();
				createRoleData.hoist = true;
				createRoleData.mentionable = true;
				createRoleData.name = roleName;
				createRoleData.permissions = rolePermsString;
				Role role = Roles::createGuildRoleAsync(createRoleData).get();
				if (role.id == 0) {
					throw std::exception("Role not initialized!");
				}
				InventoryRole currentRole;
				currentRole.roleCost = roleCost;
				currentRole.roleId = role.id;
				currentRole.roleName = role.name;

				discordGuild.data.guildShop.roles.push_back(currentRole);
				discordGuild.writeDataToDB();
				std::string msgString = "";
				auto botUser = argsNew.discordCoreClient->getBotUser();
				DiscordUser discordUser(botUser.userName, botUser.id);
				msgString = "Nicely done! You've added a new role to the store's inventory, giving the server access to it!\nIt is as "
							"follows:\n------\n__**Role:**__ <@&" +
					std::to_string(currentRole.roleId) + "> __**Cost**__ : " + std::to_string(roleCost) + " " + discordUser.data.currencyName + "\n------";
				EmbedData msgEmbed{};
				msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed.setColor(discordGuild.data.borderColor);
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**New Role Added:**__");
				RespondToInputEventData dataPackage{ argsNew.eventData };
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				InputEvents::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (...) {
				reportException("AddShopRole::execute()");
			}
		}
		~AddShopRole(){};
	};
}