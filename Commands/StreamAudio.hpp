// StreamAudio.hpp - Header for the "disconnect" std::function.
// aug 25, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class stream_audio : public base_function {
	  public:
		stream_audio() {
			this->commandName	  = "streamaudio";
			this->helpDescription = "stream audio from one bot to another.";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /streamaudio!\n------");
			msgEmbed.setTitle("__**Stream Audio Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<stream_audio>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				channel_cache_data channel{ argsNew.getChannelData() };


				guild_cache_data guild{ argsNew.getInteractionData().guildId };
				discord_guild discordGuild{ managerAgent, guild };

				guild_member_cache_data guildMember{ argsNew.getGuildMemberData() };

				snowflake currentVoiceChannelId{};
				if (guildMember.getVoiceStateData().channelId != 0) {
					currentVoiceChannelId = guildMember.getVoiceStateData().channelId;
				} else {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(guildMember.getUserData().userName, guildMember.getGuildMemberImageUrl(guild_member_image_types::Avatar));
					newEmbed->setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Playing Issue:**__");
					newEmbed->setColor("fefefe");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}
				stream_info theInfo{};
				theInfo.address = argsNew.getCommandArguments().values["connectionip"].operator jsonifier::string();
				theInfo.port	= 51072;
				theInfo.type	= stream_type::client;
				if (argsNew.getCommandArguments().values["botaudio"].operator jsonifier::string() == "true") {
					theInfo.streamBotAudio = true;
				} else {
					theInfo.streamBotAudio = false;
				}
				voice_connection& voiceConnection = guild.connectToVoice(guildMember.user.id, 0, false, false, theInfo);
				if (!voiceConnection.areWeConnected()) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(guildMember.getUserData().userName, guildMember.getGuildMemberImageUrl(guild_member_image_types::Avatar));
					newEmbed->setDescription("------\n__**Sorry, but there is no voice connection that is currently held by me!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Connection Issue:**__");
					newEmbed->setColor("fefefe");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				if (guildMember.getVoiceStateData().channelId == 0 || guildMember.getVoiceStateData().channelId != voiceConnection.getChannelId()) {
					unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
					newEmbed->setAuthor(guildMember.getUserData().userName, guildMember.getGuildMemberImageUrl(guild_member_image_types::Avatar));
					newEmbed->setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Playing Issue:**__");
					newEmbed->setColor("fefefe");
					respond_to_input_event_data dataPackage(argsNew.getInputEventData());
					dataPackage.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					input_events::respondToInputEventAsync(dataPackage).get();
					return;
				}

				std::this_thread::sleep_for(1000ms);
				discord_core_client::getSongAPI(guild.id).play();

				unique_ptr<embed_data> newEmbed{ makeUnique<embed_data>() };
				newEmbed->setAuthor(guildMember.getUserData().userName, guildMember.getGuildMemberImageUrl(guild_member_image_types::Avatar));
				newEmbed->setDescription("------\n__**Congratulations - you've been connected to an audio channel to stream audio!**__\n------");
				newEmbed->setTimeStamp(getTimeAndDate());
				newEmbed->setTitle("__**Streaming Audio:**__");
				newEmbed->setColor("fefefe");
				respond_to_input_event_data dataPackage02{ argsNew.getInputEventData() };
				dataPackage02.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
				dataPackage02.addMessageEmbed(*newEmbed);
				input_events::respondToInputEventAsync(dataPackage02).get();
				return;
			} catch (const std::runtime_error& error) {
				std::cout << "stream_audio::execute()" << error.what() << std::endl;
			}
		}
		~stream_audio(){};
	};

}// namespace discord_core_api
