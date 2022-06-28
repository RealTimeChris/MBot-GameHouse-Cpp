// RemoveShopRole.hpp - Header for the "remove shop role" command.
// Jun 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class RemoveShopRole : public BaseFunction {
	  public:
		RemoveShopRole() {
			this->commandName = "removeshoprole";
			this->helpDescription = "Remove a role from the server's shop.";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter /removeshoprole ROLENAME.\n------");
			msgEmbed.setTitle("__**Remove Shop Role Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<RemoveShopRole>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ argsNew.eventData.getChannelId() }).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);

				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({
																					  .guildMemberId = argsNew.eventData.getAuthorId(),
																					  .guildId = argsNew.eventData.getGuildId(),
																				  })
											  .get();

				bool doWeHaveAdminPermission = doWeHaveAdminPermissions(argsNew, argsNew.eventData, discordGuild, channel, guildMember);

				if (doWeHaveAdminPermission == false) {
					return;
				}

				std::regex roleNameRegExp{ ".{1,36}" };
				std::regex roleIdRegExp{ "\\d{1,18}" };

				std::string roleName = argsNew.commandData.optionsArgs.at(0);

				uint64_t roleID;
				std::cmatch matchResults;
				regex_search(argsNew.commandData.optionsArgs.at(0).c_str(), matchResults, roleIdRegExp);
				roleID = stoull(matchResults.str());
				std::string realRoleName;

				bool isRoleFound = false;
				for (uint32_t x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
					if (roleName == discordGuild.data.guildShop.roles[x].roleName || roleID == discordGuild.data.guildShop.roles[x].roleId) {
						roleID = discordGuild.data.guildShop.roles[x].roleId;
						realRoleName = discordGuild.data.guildShop.roles[x].roleName;
						isRoleFound = true;
						discordGuild.data.guildShop.roles.erase(discordGuild.data.guildShop.roles.begin() + x);
						discordGuild.writeDataToDB();
						break;
					}
				}

				std::string msgString;

				if (isRoleFound == false) {
					msgString = "------\n**Sorry, but that role was not found in the shop's inventory!**\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Role Issue:**__");
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					return;
				}

				Roles::removeGuildRoleAsync({ .guildId = argsNew.eventData.getGuildId(), .roleId = roleID }).get();

				msgString += "You've just deleted a role from the shop / server!\n------\n__**Role Name:**__ " + realRoleName + "\n------";

				EmbedData msgEmbed{};
				msgEmbed.setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Shop/Server Role Deleted:**__");
				msgEmbed.setColor(discordGuild.data.borderColor);
				RespondToInputEventData dataPackage(argsNew.eventData);
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();

				return;
			} catch (...) {
				reportException("RemoveShopRole::execute()");
			}
		}
		~RemoveShopRole(){};
	};
}
