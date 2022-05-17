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
				GetGuildAuditLogsData dataPackage{};
				dataPackage.actionType = AuditLogEvent::Member_Disconnect;
				dataPackage.guildId = argsNew.eventData.getGuildId();
				dataPackage.limit = 10;
				dataPackage.userId = "";
				auto resultLots = Guilds::getGuildAuditLogsAsync(dataPackage).get();
				RespondToInputEventData dataPackage02{ argsNew.eventData };
				dataPackage02.addContent(
					"The Date: " + resultLots.auditLogEntries[0].getCreatedAtTimestamp(TimeFormat::LongDate) + "\n<@" + resultLots.auditLogEntries[0].userId + ">");
				dataPackage02.setResponseType(InputEventResponseType::Interaction_Response);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage02).get();


				return;
			} catch (...) {
				reportException("Test::execute()");
			}
		}
		~Test(){};
	};
}
