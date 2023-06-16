// HelperFunctions.hpp - Header for some helper functions.
// may 28, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "DatabaseEntities.hpp"

namespace discord_core_api {

	bool checkIfAllowedGamingInChannel(input_event_data eventData, discord_guild discordGuild) {
		bool isItFound = true;
		if (discordGuild.data.gameChannelIds.size() > 0) {
			isItFound = false;
			jsonifier::string msgString = "------\n**sorry, but please do that in one of the "
									"following channels:**\n------\n";
			embed_data msgEmbed{};
			for (auto& value: discordGuild.data.gameChannelIds) {
				if (eventData.getChannelData().id == value) {
					isItFound = true;
					break;
				} else {
					msgString += "<#" + value + ">\n";
				}
			}
			msgString += "------";
			if (isItFound == false) {
				msgEmbed.setAuthor(eventData.getUserData().userName, eventData.getUserData().getUserImageUrl(user_image_types::Avatar));
				msgEmbed.setColor("fefefe");
				msgEmbed.setDescription(msgString);
				msgEmbed.setTitle("__**Permissions issue:**__");
				respond_to_input_event_data replyMessageData{ eventData };
				replyMessageData.addMessageEmbed(msgEmbed);
				if (eventData.responseType != input_event_response_type::unset) {
					input_events::deleteInputEventResponseAsync(eventData).get();
					replyMessageData.setResponseType(input_event_response_type::Ephemeral_Follow_Up_Message);
				} else {
					replyMessageData.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
				}

				input_events::respondToInputEventAsync(replyMessageData).get();
			}
		}
		return isItFound;
	}

	bool checkForBotCommanderStatus(guild_member_data guildMember, discord_user& discordUser) {
		bool areWeACommander;
		for (auto& value: discordUser.data.botCommanders) {
			if (guildMember.user.id == value) {
				areWeACommander = true;
				return areWeACommander;
				break;
			}
		}
		return false;
	}

	bool doWeHaveAdminPermissions(const base_function_arguments& argsNew, input_event_data& eventData, discord_guild& discordGuild, channel_data& channel,
		guild_member_data& guildMember, bool displayResponse = true) {
		respond_to_input_event_data dataPackage{ eventData };
		dataPackage.setResponseType(input_event_response_type::Ephemeral_Deferred_Response);
		input_events::respondToInputEventAsync(dataPackage).get();
		bool doWeHaveAdmin = permissions{ guildMember.permissions }.checkForPermission(guildMember, channel, permission::administrator);
		if (doWeHaveAdmin) {
			return true;
		}
		discord_user discordUser(managerAgent, discord_core_client::getInstance()->getBotUser().userName, discord_core_client::getInstance()->getBotUser().id);
		bool areWeACommander = checkForBotCommanderStatus(guildMember, discordUser);
		if (areWeACommander) {
			return true;
		}

		if (displayResponse) {
			jsonifier::string msgString = "------\n**sorry, but you don't have the "
									"permissions required for that!**\n------";
			embed_data msgEmbed{};
			msgEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
			msgEmbed.setColor("fefefe");
			msgEmbed.setDescription(msgString);
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setTitle("__**Permissions issue:**__");
			respond_to_input_event_data dataPackage01{ eventData };
			dataPackage01.addMessageEmbed(msgEmbed);
			dataPackage01.setResponseType(input_event_response_type::Edit_Interaction_Response);
			eventData = input_events::respondToInputEventAsync(dataPackage01).get();
		}
		return false;
	}

	float applyAsymptoticTransform(float inputModValue, float horizontalStretch, float ceiling) {
		float finalModValue = 0;
		float newInputModValue = inputModValue;
		if (newInputModValue == 0) {
			newInputModValue += 1;
		}
		if (newInputModValue <= 0) {
			float newInputValue = newInputModValue * -1;

			finalModValue = -1 *
				( float )trunc(
					(ceiling * pow(newInputValue, 3)) / ((pow(newInputValue, 3) + ( int64_t )horizontalStretch * ( int64_t )newInputValue)));
			return finalModValue;
		}

		finalModValue = ( float )trunc(
			(ceiling * pow(newInputModValue, 3)) / ((pow(newInputModValue, 3) + ( int64_t )horizontalStretch * ( int64_t )newInputModValue)));

		return finalModValue;
	}

}// namespace discord_core_api
