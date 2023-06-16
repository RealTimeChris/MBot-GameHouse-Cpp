// RemoveShopRole.hpp - Header for the "remove shop role" command.
// jun 25, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class remove_shop_role : public base_function {
	  public:
		remove_shop_role() {
			this->commandName	  = "removeshoprole";
			this->helpDescription = "remove a role from the server's shop.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /removeshoprole rolename.\n------");
			msgEmbed.setTitle("__**Remove shop role_data usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<remove_shop_role>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				auto inputEventData			 = argsNew.getInputEventData();
				bool doWeHaveAdminPermission = doWeHaveAdminPermissions(argsNew, inputEventData, discordGuild, channel, guildMember);

				if (doWeHaveAdminPermission == false) {
					return;
				}

				std::regex roleNameRegExp{ ".{1,36}" };
				std::regex roleIdRegExp{ "\\d{18,20}" };

				jsonifier::string roleName = argsNew.getCommandArguments().values.at("role").operator jsonifier::string();

				snowflake roleID{ argsNew.getCommandArguments().values["role"].operator size_t() };
				jsonifier::string realRoleName;

				bool isRoleFound = false;
				for (uint32_t x = 0; x < discordGuild.data.guildShop.roles.size(); x += 1) {
					if (roleName == discordGuild.data.guildShop.roles[x].roleName || roleID == discordGuild.data.guildShop.roles[x].roleId) {
						roleID		 = discordGuild.data.guildShop.roles[x].roleId;
						realRoleName = discordGuild.data.guildShop.roles[x].roleName;
						isRoleFound	 = true;
						discordGuild.data.guildShop.roles.erase(discordGuild.data.guildShop.roles.begin() + x);
						discordGuild.writeDataToDB(managerAgent);
						break;
					}
				}

				jsonifier::string msgString;

				if (isRoleFound == false) {
					msgString = "------\n**sorry, but that role was not found in the shop's inventory!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Role_data issue:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				roles::removeGuildRoleAsync({ .guildId = guild.id, .roleId = roleID }).get();

				msgString += "you've just deleted a role from the shop / server!\n------\n__**Role_data name:**__ " + realRoleName + "\n------";

				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Shop/server role_data deleted:**__");
				msgEmbed.setColor("fefefe");
				respond_to_input_event_data dataPackage(argsNew.getInputEventData());
				dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();

				return;
			} catch (const std::exception& error) {
				std::cout << "remove_shop_role::execute()" << error.what() << std::endl;
			}
		}
		~remove_shop_role(){};
	};
}// namespace discord_core_api
