// AddShopRole.hpp - Header for the "add shop role" command.
// may 29, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class add_shop_role : public base_function {
	  public:
		add_shop_role() {
			this->commandName	  = "addshoprole";
			this->helpDescription = "add a role to the server's shop";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /addshoprole name, hexcolorvalue, cost.\n------");
			msgEmbed.setTitle("__**Add shop role_data usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<add_shop_role>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };
				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };
				guild_member_data guildMember{ argsNew.getGuildMemberData() };
				auto inputEventData = argsNew.getInputEventData();
				bool doWeHaveAdmin	= doWeHaveAdminPermissions(argsNew, inputEventData, discordGuild, channel, guildMember);

				if (doWeHaveAdmin == false) {
					return;
				}

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (areWeAllowed == false) {
					return;
				}

				std::regex hexColorRegExp{ ".{1,24}" };
				std::regex costRegExp{ "\\d{1,8}" };
				if (argsNew.getCommandArguments().values.size() < 2) {
					jsonifier::string msgString = "------\n**Please enter a valid hex color value! (!addshoprole = name, hexcolorvalie, cost)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto event = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.getCommandArguments().values.size() < 3 || argsNew.getCommandArguments().values["cost"].operator std::streamoff() <= 0) {
					jsonifier::string msgString = "------\n**Please enter a valid cost value! (!addshoprole = name, hexcolorvalie, cost)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto event = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				jsonifier::string roleName  = argsNew.getCommandArguments().values["name"].operator jsonifier::string();
				jsonifier::string roleColor = argsNew.getCommandArguments().values["hexcolorvalue"].operator jsonifier::string();
				uint32_t roleCost			= argsNew.getCommandArguments().values["cost"].operator size_t();

				for (auto& value : discordGuild.data.guildShop.roles) {
					if (roleName == value.roleName) {
						jsonifier::string msgString = "------\n**sorry, but a role by that name already exists!**\n------";
						embed_data msgEmbed{};
						msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed.setColor("fefefe");
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Missing or invalid arguments:**__");
						respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
						dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						auto event = input_events::respondToInputEventAsync(dataPackage).get();
						return;
					}
				}

				permissions rolePermsString{};
				jsonifier::vector<permission> permissions;
				permissions.emplace_back(permission::Create_Instant_Invite);
				permissions.emplace_back(permission::Add_Reactions);
				permissions.emplace_back(permission::View_Channel);
				permissions.emplace_back(permission::Send_Messages);
				permissions.emplace_back(permission::Change_Nickname);
				permissions.emplace_back(permission::Use_External_Emojis);
				permissions.emplace_back(permission::connect);
				permissions.emplace_back(permission::Embed_Links);
				permissions.emplace_back(permission::Attach_Files);
				permissions.emplace_back(permission::speak);
				permissions.emplace_back(permission::Use_Application_Commands);
				permissions.emplace_back(permission::Read_Message_History);
				permissions.emplace_back(permission::stream);
				permissions.emplace_back(permission::Request_to_Speak);
				permissions.emplace_back(permission::Send_TTS_Messages);
				permissions.emplace_back(permission::Create_Private_Threads);
				permissions.emplace_back(permission::Use_VAD);

				rolePermsString.addPermissions(permissions);

				create_guild_role_data createRoleData{};
				createRoleData.hexColorValue = roleColor;
				createRoleData.guildId		 = guild.id;
				createRoleData.hoist		 = true;
				createRoleData.mentionable	 = true;
				createRoleData.name			 = roleName;
				createRoleData.permissions	 = rolePermsString.operator jsonifier::string();
				role_data role				 = roles::createGuildRoleAsync(createRoleData).get();
				if (role.id == 0) {
					throw std::runtime_error("role not initialized!");
				}
				inventory_role currentRole;
				currentRole.roleCost = roleCost;
				currentRole.roleId	 = role.id;
				currentRole.roleName = role.name;

				discordGuild.data.guildShop.roles.emplace_back(currentRole);
				discordGuild.writeDataToDB(managerAgent);
				jsonifier::string msgString = "";
				auto botUser		  = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				msgString = "nicely done! you've added a new role to the store's inventory, giving the server access to it!\nIt is as "
							"follows:\n------\n__**Role_data:**__ <@&" +
							currentRole.roleId + "> __**Cost**__ : " + jsonifier::toString(roleCost) + " " + discordUser.data.currencyName + "\n------";
				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**New role_data added:**__");
				respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
				dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				input_events::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const std::exception& error) {
				std::cout << "add_shop_role::execute()" << error.what() << std::endl;
			}
		}
		~add_shop_role(){};
	};
}// namespace discord_core_api
