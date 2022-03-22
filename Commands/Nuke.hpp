// Nuke.hpp - Header for the "nuke" command.
// May 29, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Nuke :public BaseFunction {
	public:
		Nuke() {
			this->commandName = "nuke";
			this->helpDescription = "Nukes a server.";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nEnter !nuke.\n------");
			msgEmbed.setTitle("__**Nuke Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		 std::unique_ptr<BaseFunction> create() {
			return  std::make_unique<Nuke>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({ .channelId = args.eventData->getChannelId() }).get();

				GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId(),.guildId = args.eventData->getGuildId() }).get();

				bool areWeInADm = areWeInADM(*args.eventData, channel);

				if (areWeInADm == true) {
					return;
				}

				InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();

				Guild guild = Guilds::getCachedGuildAsync({ .guildId = args.eventData->getGuildId() }).get();

				for (auto& [key, value] : guild.members) {
					CreateGuildBanData dataPackage{};
					dataPackage.guildId = guild.id;
					dataPackage.guildMemberId = value.user.id;
					dataPackage.reason = "BYE!";
					Guilds::createGuildBanAsync(dataPackage).get();
				}
				
				return;
			}
			catch (...) {
				reportException("AddShopItem::executeAsync()");
			}

		}

		virtual ~Nuke() {};
	};

}
