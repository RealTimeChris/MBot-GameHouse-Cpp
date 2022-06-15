// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {


	std::vector<std::string> redNumbers{ ":red_square:32", ":red_square:19", ":red_square:21", ":red_square:25", ":red_square:34", ":red_square:27", ":red_square:36",
		":red_square:30", ":red_square:23", ":red_square:5", ":red_square:16", ":red_square:1", ":red_square:14", ":red_square:9", ":red_square:18", ":red_square:7",
		":red_square:12", ":red_square:3" };
	std::vector<std::string> blackNumbers{ ":black_large_square:15", ":black_large_square:4", ":black_large_square:2", ":black_large_square:17", ":black_large_square:6",
		":black_large_square:13", ":black_large_square:11", ":black_large_square:8", ":black_large_square:10", ":black_large_square:24", ":black_large_square:33",
		":black_large_square:20", ":black_large_square:31", ":black_large_square:22", ":black_large_square:29", ":black_large_square:28", ":black_large_square:35",
		":black_large_square:26" };

	std::string function() {
		std::mt19937_64 randomEngine{ static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
		std::string finalRoll = std::to_string(static_cast<uint32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * 38.0f));
		return getNumberString(finalRoll, redNumbers, blackNumbers);
	}

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
				RespondToInputEventData dataPackage{ argsNew.eventData };
				dataPackage.addContent("The FINAL ROLL: " + function());
				dataPackage.setResponseType(InputEventResponseType::Interaction_Response);
				auto newEvent = InputEvents::respondToInputEventAsync(dataPackage).get();
				RespondToInputEventData dataPackage02{ newEvent };
				for (int32_t x = 0; x < 50; x += 1) {
					dataPackage02.addContent("The FINAL ROLL: " + function());
					dataPackage02.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
					auto newEvent = InputEvents::respondToInputEventAsync(dataPackage02).get();
				}



				return;
			} catch (...) {
				reportException("Test::execute()");
			}
		}
		~Test(){};
	};
}
