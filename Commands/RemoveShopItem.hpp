// RemoveShopItem.hpp - Header for the "remove shop item" command.
// jun 25, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class remove_shop_item : public base_function {
	  public:
		remove_shop_item() {
			this->commandName	  = "removeshopitem";
			this->helpDescription = "remove's an item from the server's shop.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /removeshopitem itemname.\n------");
			msgEmbed.setTitle("__**Remove shop item usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			discord_core_api::unique_ptr<base_function> newPtr = discord_core_api::makeUnique<remove_shop_item>();
			return newPtr;
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				discord_guild_member discordGuildMember(managerAgent, guildMember);
				auto inputEventData			 = argsNew.getInputEventData();
				bool doWeHaveAdminPermission = doWeHaveAdminPermissions(argsNew, inputEventData, discordGuild, channel, guildMember);

				if (doWeHaveAdminPermission == false) {
					return;
				}

				jsonifier::string itemName = argsNew.getCommandArguments().values.at("item").operator jsonifier::string();

				uint32_t itemIndex = 0;
				bool itemFound	   = false;
				for (uint32_t x = 0; x < discordGuild.data.guildShop.items.size(); x += 1) {
					if (itemName == discordGuild.data.guildShop.items[x].itemName) {
						itemIndex = x;
						itemFound = true;
						break;
					}
				}

				if (itemFound == false) {
					jsonifier::string msgString = "------\n**sorry, but that item was not found in the shop's inventory!**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Item issue:**__");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				jsonifier::string msgString;

				msgString = "alrighty then! you've removed an item from the shop!\n------\n__The removed item: __ " + discordGuild.data.guildShop.items[itemIndex].emoji +
							discordGuild.data.guildShop.items[itemIndex].itemName + "\n------";

				discordGuild.data.guildShop.items.erase(discordGuild.data.guildShop.items.begin() + itemIndex);
				discordGuild.writeDataToDB(managerAgent);

				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Shop item removed:**__");
				respond_to_input_event_data dataPackage(argsNew.getInputEventData());
				dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				auto newEvent = input_events::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const std::exception& error) {
				std::cout << "remove_shop_item::execute()" << error.what() << std::endl;
			}
		}
		~remove_shop_item(){};
	};
}// namespace discord_core_api
