// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#include "HelperFunctions.hpp"
#include "RegisterApplicationCommands.hpp"

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			this->commandName	  = "test";
			this->helpDescription = "testing purposes!";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /test!\n------");
			msgEmbed.setTitle("__**Test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		discord_core_api::unique_ptr<base_function> create() {
			return discord_core_api::makeUnique<test>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				jsonifier::vector<song> searchResults{};
				guild_data guild{ argsNew.getInteractionData().guildId };
				guild_member_data guildMember{ argsNew.getGuildMemberData() };
				respond_to_input_event_data dataPackage0(argsNew.getInputEventData());
				dataPackage0.setResponseType(input_event_response_type::Deferred_Response);
				jsonifier::vector<embed_data> embedsFromSearch;
				embed_data newEmbed{};
				newEmbed.setDescription("TEST EMBED");
				embedsFromSearch.emplace_back(newEmbed);
				create_message_data createMessageData{};
				auto newEvent = input_events::respondToInputEventAsync(dataPackage0).get();

				respond_to_input_event_data responseData{ newEvent };
				responseData.setResponseType(input_event_response_type::Edit_Interaction_Response);
				responseData.addMessageEmbed(embedsFromSearch[0]);
				responseData.addButton(false, "test_button", "Test", button_style::Primary, unicode_emojis::x);
				newEvent = input_events::respondToInputEventAsync(responseData).get();
				button_collector buttonCollector{ newEvent };
				auto results = buttonCollector.collectButtonData(false, 120000, 1, {}, argsNew.getUserData().id).get();
				respond_to_input_event_data newInputData{ *results[0].interactionData };
				std::cout << "CURRENT EMOJI: " << results[0].emojiName << std::endl;
				for (auto& value: results[0].emojiName) {
					std::cout << "CURRENT EMOJI: " << +value << std::endl;
				}
				newInputData.setResponseType(input_event_response_type::Edit_Interaction_Response);
				newInputData.addMessageEmbed(embedsFromSearch[0]);
				newInputData.addButton(false, "test_button", "Test", button_style::Primary, results[0].emojiName);
				input_events::respondToInputEventAsync(newInputData).get();


			} catch (const std::exception& error) {
				std::cout << "test::execute()" << error.what() << std::endl;
			}
		}
		~test(){};
	};
}// namespace discord_core_api
