// SetBorderColor.hpp - Header for the "set border color" command.
// Jun 26, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "Index.hpp"
#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

    class SetBorderColor :public BaseFunction {
    public:
        SetBorderColor() {
            this->commandName = "setbordercolor";
            this->helpDescription = "Set the bot's default border color for message embeds.";
            EmbedData msgEmbed;
            msgEmbed.setDescription("------\nSimply enter /setbordercolor <HEXCOLORVALUE>!\n------");
            msgEmbed.setTitle("__**Set Border Color Usage:**__");
            msgEmbed.setTimeStamp(getTimeAndDate());
            msgEmbed.setColor("FeFeFe");
            this->helpEmbed = msgEmbed;
        }

        std::unique_ptr<BaseFunction> create() {
            return std::make_unique<SetBorderColor>();
        }

        virtual void execute(BaseFunctionArguments& args) {
            try {
                Channel channel = Channels::getCachedChannelAsync({ args.eventData->getChannelId() }).get();

                bool areWeInADm = areWeInADM(*args.eventData, channel);

                if (areWeInADm) {
                    return;
                }

                InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();
                Guild guild = Guilds::getCachedGuildAsync({ args.eventData->getGuildId() }).get();
                DiscordGuild discordGuild(guild);
                GuildMember guildMember = GuildMembers::getCachedGuildMemberAsync({ .guildMemberId = args.eventData->getAuthorId() ,.guildId = args.eventData->getGuildId() }).get();
                bool doWeHaveAdminPermission = doWeHaveAdminPermissions(args, *args.eventData, discordGuild, channel, guildMember);
                if (!doWeHaveAdminPermission) {
                    return;
                }
                std::string borderColor;

                if (std::stoll(args.commandData.optionsArgs[0], 0, 16) < 0 || std::stoll(args.commandData.optionsArgs[0], 0, 16) > std::stoll("fefefe", 0, 16)) {
                    std::string msgString = "------\n**Please, enter a hex-color value between 0 and FeFeFe! (!setbordercolor = BOTNAME, HEXCOLORVALUE)**\n------";
                    EmbedData msgEmbed;
                    msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
                    msgEmbed.setColor(discordGuild.data.borderColor);
                    msgEmbed.setDescription(msgString);
                    msgEmbed.setTimeStamp(getTimeAndDate());
                    msgEmbed.setTitle("__**Missing Or Invalid Arguments:**__");
                    RespondToInputEventData dataPackage(*args.eventData);
                    dataPackage.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
                    dataPackage.addMessageEmbed(msgEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
                    return;
                }
                else {
                    borderColor = args.commandData.optionsArgs[0];

                    discordGuild.data.borderColor = borderColor;
                    discordGuild.writeDataToDB();

                    EmbedData msgEmbed;
                    msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
                    msgEmbed.setColor(discordGuild.data.borderColor);
                    msgEmbed.setDescription("Nicely done, you've updated the default border color for this bot!\n------\n__**Border Color Values:**__ " + discordGuild.data.borderColor + "\n------");
                    msgEmbed.setTimeStamp(getTimeAndDate());
                    msgEmbed.setTitle("__**Updated Border Color:**__");
                    RespondToInputEventData dataPackage(*args.eventData);
                    dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
                    dataPackage.addMessageEmbed(msgEmbed);
                    auto newEvent = InputEvents::respondToEvent(dataPackage);
                    return;
                }
            }
            catch (...) {
                reportException("SetBorderColor::execute()");
            }

        };
        virtual ~SetBorderColor() {};
    };
};
