// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		void execute(BaseFunctionArguments& argsNew) {
			try {
				GuildMember guildMember =
					GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = argsNew.eventData.getAuthorId(), .guildId = argsNew.eventData.getGuildId() }).get();
				Guild guild = Guilds::getCachedGuildAsync({ .guildId = argsNew.eventData.getGuildId() }).get();
				DiscordGuild discordGuild{ guild };
				RespondToInputEventData theData{ argsNew.eventData };
				theData.setResponseType(InputEventResponseType::Ephemeral_Deferred_Response);
				auto theResult = InputEvents::respondToInputEventAsync(theData).get();
				RespondToInputEventData theData01{ theResult };
				theData01.setResponseType(InputEventResponseType::Edit_Interaction_Response);
				theData01.addContent("TESTING CONTENT");
				theResult = InputEvents::respondToInputEventAsync(theData01).get();
				for (int32_t x = 0; x < 20; x++) {
					RespondToInputEventData theData02{ theResult };
					theData02.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					File theFile{};
					theFile.data = loadFileContents("C:/Users/Chris/Downloads/nft profile pic.png");
					theFile.fileName = "nft profile pic.png";
					theData02.addFile(theFile);
					EmbedData theEmbed{};
					std::string theTime{ "<t:" + std::to_string(std::time(nullptr)) + std::string{ ":F>" } };
					theEmbed.setDescription("TESTING " + theTime);
					theEmbed.setImage("attachment://nft profile pic.png");
					theData02.addMessageEmbed(theEmbed);
					InputEvents::respondToInputEventAsync(theData02);
				}

				/*
				VoiceStateData voiceStateData{};
				Channel channel = Channels::getCachedChannelAsync({ .channelId = argsNew.eventData.getChannelId() }).get();
				bool areTheyACommander = doWeHaveAdminPermissions(argsNew, argsNew.eventData, discordGuild, channel, guildMember);

				if (!areTheyACommander) {
					return;
				}
				
				if (guild.voiceStates.contains(guildMember.id)) {
					voiceStateData = guild.voiceStates.at(guildMember.id);
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
					newEmbed->setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**Enjoy!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Joining Now:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				} else {
					std::unique_ptr<DiscordCoreAPI::EmbedData> newEmbed{ std::make_unique<DiscordCoreAPI::EmbedData>() };
					newEmbed->setAuthor(argsNew.eventData.getUserName(), argsNew.eventData.getAvatarUrl());
					newEmbed->setDescription("------\n__**Sorry, but you need to be in a correct voice channel to issue those commands!**__\n------");
					newEmbed->setTimeStamp(getTimeAndDate());
					newEmbed->setTitle("__**Playing Issue:**__");
					newEmbed->setColor(discordGuild.data.borderColor);
					RespondToInputEventData dataPackage(argsNew.eventData);
					dataPackage.setResponseType(InputEventResponseType::Follow_Up_Message);
					dataPackage.addMessageEmbed(*newEmbed);
					auto newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					dataPackage.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					newerEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
					InputEvents::deleteInputEventResponseAsync(argsNew.eventData).get();
					InputEvents::deleteInputEventResponseAsync(newerEvent, 20000);
					return;
				}
				UpdateVoiceStateData theData{};
				theData.channelId = guild.voiceStates[guildMember.id].channelId;
				theData.guildId = guild.id;
				for (uint32_t x = 0; x < 100; x += 1) {
					theData.channelId = guild.voiceStates[guildMember.id].channelId;
					argsNew.discordCoreClient->getBotUser().updateVoiceStatus(theData);
					std::this_thread::sleep_for(250ms);
					theData.channelId = 0;
					argsNew.discordCoreClient->getBotUser().updateVoiceStatus(theData);
					std::this_thread::sleep_for(250ms);
				}
				*/

				return;
			} catch (...) {
				reportException("Test::execute()");
			}
		}
		~Test(){};
	};
}
