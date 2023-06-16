// AddShopItem.hpp - Header for the "add shop item" command.
// may 29, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"
#include <regex>

namespace discord_core_api {

	class add_shop_item : public base_function {
	  public:
		add_shop_item() {
			this->commandName	  = "addshopitem";
			this->helpDescription = "add an item to the server's shop.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nEnter /addshopitem itemname, selfmod, oppmod, itemcost, emoji.\n------");
			msgEmbed.setTitle("__**Add shop item usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<add_shop_item>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_data channel{ argsNew.getChannelData() };

				guild_member_data guildMember{ argsNew.getGuildMemberData() };

				guild_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };
				auto inputEventData = argsNew.getInputEventData();
				bool doWeHaveAdmin	= doWeHaveAdminPermissions(argsNew, inputEventData, discordGuild, channel, guildMember);

				if (doWeHaveAdmin == false) {
					return;
				}

				bool areWeAllowed = checkIfAllowedGamingInChannel(argsNew.getInputEventData(), discordGuild);

				if (areWeAllowed == false) {
					return;
				}
				int32_t theInt = static_cast<int32_t>(std::bit_cast<int64_t>(argsNew.getCommandArguments().values["oppmod"].operator size_t()));
				std::regex emojiRegExp{ ".{1,32}" };
				if (argsNew.getCommandArguments().values.size() < 2 || !argsNew.getCommandArguments().values.contains("selfmod") ||
					argsNew.getCommandArguments().values.at("selfmod").operator std::streamoff() > 100 ||
					argsNew.getCommandArguments().values.at("selfmod").operator std::streamoff() < 0) {
					jsonifier::string msgString = "------\n**Please enter a valid self-mod value, between 0 and 100! (!addshopitem = itemname, selfmod, oppmod, "
												  "itemcost, emoji)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_event_data event = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.getCommandArguments().values.size() < 3 || theInt < -100 || theInt > 0) {
					jsonifier::string msgString = "------\n**Please enter a valid opp-mod value between -100 and 0! (!addshopitem = itemname, selfmod, oppmod, "
											"itemcost, emoji)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_event_data eventNew = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.getCommandArguments().values.size() < 4 || !argsNew.getCommandArguments().values.contains("itemcost") ||
					argsNew.getCommandArguments().values.at("itemcost").operator std::streamoff() < 1) {
					jsonifier::string msgString = "------\n**Please enter a valid item cost! (!addshopitem = itemname, selfmod, oppmod, itemcost, emoji)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_event_data eventNew = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				if (argsNew.getCommandArguments().values.size() < 5 || !argsNew.getCommandArguments().values.contains("emoji")) {
					jsonifier::string msgString = "------\n**Please enter a valid emoji! (!addshopitem = itemname, selfmod, oppmod, itemcost, emoji)**\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					msgEmbed.setColor("fefefe");
					msgEmbed.setDescription(msgString);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setTitle("__**Missing or invalid arguments:**__");
					respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
					dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
					dataPackage.addMessageEmbed(msgEmbed);
					input_event_data eventNew = input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				jsonifier::string itemName = argsNew.getCommandArguments().values.at("itemname").operator jsonifier::string();
				std::cmatch matchResults;
				uint32_t selfMod		= argsNew.getCommandArguments().values["selfmod"].operator size_t();
				int32_t oppMod			= argsNew.getCommandArguments().values["oppmod"].operator size_t();
				uint32_t itemCost		= argsNew.getCommandArguments().values["itemcost"].operator size_t();
				jsonifier::string emoji = argsNew.getCommandArguments().values.at("emoji").operator jsonifier::string();

				for (auto& value : discordGuild.data.guildShop.items) {
					if (itemName == value.itemName) {
						jsonifier::string msgString = "------\n**sorry, but an item by that name already exists!**\n------";
						embed_data msgEmbed{};
						msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed.setColor("fefefe");
						msgEmbed.setDescription(msgString);
						msgEmbed.setTimeStamp(getTimeAndDate());
						msgEmbed.setTitle("__**Item issue:**__");
						respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
						dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
						dataPackage.addMessageEmbed(msgEmbed);
						input_event_data eventNew = input_events::respondToInputEventAsync(dataPackage).get();
						return;
					}
				}

				inventory_item newItem;
				newItem.emoji	 = emoji;
				newItem.itemCost = itemCost;
				newItem.itemName = itemName;
				newItem.oppMod	 = oppMod;
				newItem.selfMod	 = selfMod;

				discordGuild.data.guildShop.items.emplace_back(newItem);
				discordGuild.writeDataToDB(managerAgent);
				auto botUser = discord_core_client::getInstance()->getBotUser();
				jsonifier::string theString{ botUser.userName };
				discord_user discordUser(managerAgent, theString, botUser.id);
				jsonifier::string msgString = "";
				msgString			  = "good job! you've added a new item to the shop, making it available for purchase by the members of this server!\n\
				the item's stats are as follows:\n__Item Name__: " +
							itemName + "\n__Self-mod Value__: " + jsonifier::toString(selfMod) + "\n__Opp-mod Value__: " + jsonifier::toString(oppMod) + "\n\
				__Item Cost__: " +
							jsonifier::toString(itemCost) + " " + discordUser.data.currencyName + "\n__Emoji__: " + emoji;
				embed_data msgEmbed{};
				msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription(msgString);
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**New shop item added:**__");
				respond_to_input_event_data dataPackage{ argsNew.getInputEventData() };
				dataPackage.setResponseType(input_event_response_type::Edit_Interaction_Response);
				dataPackage.addMessageEmbed(msgEmbed);
				input_event_data eventNew = input_events::respondToInputEventAsync(dataPackage).get();
				return;
			} catch (const std::exception& error) {
				std::cout << "add_shop_item::execute()" << error.what() << std::endl;
			}
		}

		~add_shop_item(){};
	};

}// namespace discord_core_api
