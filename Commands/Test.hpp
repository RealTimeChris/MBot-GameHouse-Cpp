// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

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

		virtual void execute(BaseFunctionArguments& args) {
			try {
				RespondToInputEventData dataPackage{ *args.eventData };
				dataPackage.addContent("TESTING");
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				auto newEvent = InputEvents::respondToEvent(dataPackage);
				RespondToInputEventData dataPackage02{ *newEvent };
				dataPackage02.addContent("TESTING!");
				dataPackage02.setResponseType(InputEventResponseType::Follow_Up_Message);
				auto newEvent02 = InputEvents::respondToEvent(dataPackage02);
				InputEvents::deleteInputEventResponseAsync(std::move(newEvent02), 5000).get();


				return;
			} catch (...) {
				reportException("Test::execute()");
			}
		}
		virtual ~Test(){};
	};
}
