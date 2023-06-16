// help.hpp - header for the "help" command.
// may 20, 2021
// chris m.
// https://github.com/real_time_chris

#pragma once

#include "HelperFunctions.hpp"

namespace discord_core_api {

	class help : public base_function {
	  public:
		help() {
			this->commandName	  = "help";
			this->helpDescription = "A help command for this bot!";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /help, and follow the instructions!\n------");
			msgEmbed.setTitle("__**Help Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fefefe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<help>();
		}

		void execute(const base_function_arguments& argsNew) {
			try {
				bool isItFirst{ true };
				input_event_data newEvent01(argsNew.getInputEventData());

				while (1) {
					respond_to_input_event_data responseData{ newEvent01 };
					jsonifier::vector<jsonifier::vector<select_option_data>> selectOptions;
					uint64_t counter{ 0 };
					uint64_t currentHelpPage{ 0 };
					for (auto& [key, value]: discord_core_client::getInstance()->getCommandController().getFunctions()) {
						if (counter % 24 == 0) {
							selectOptions.emplace_back(jsonifier::vector<select_option_data>());
							currentHelpPage += 1;
						}
						jsonifier::string newString;
						newString.emplace_back(( char )toupper(value->commandName[0]));
						newString += value->commandName.substr(1, value->commandName.size() - 1);
						select_option_data newData;
						newData.label		= newString;
						newData.description = value->helpDescription;
						newData.value		= convertToLowerCase(newString);
						value->helpEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						newData.emoji.name = "✅";
						bool doWeContinue{ false };
						for (auto& value02: selectOptions) {
							for (auto& value03: value02) {
								if (value03.value == newData.value) {
									doWeContinue = true;
									break;
								}
							}
						}
						if (doWeContinue) {
							continue;
						}
						selectOptions.at(currentHelpPage - 1).emplace_back(newData);
						counter += 1;
					}
					select_option_data newData;
					newData.label		= "Go Back";
					newData.description = "go back to the previous menu.";
					newData.value		= "go back";
					newData.emoji.name	= "❌";
					jsonifier::vector<jsonifier::vector<select_option_data>> selectOptionsNew;
					for (auto& value: selectOptions) {
						value.emplace_back(newData);
						selectOptionsNew.emplace_back(value);
					}

					uint64_t counter02{ 0 };
					jsonifier::string messageNew = "------\nSelect which page of help items you would like to view, by clicking a button below!\n------";
					embed_data newEmbed{};
					newEmbed.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
					newEmbed.setColor("fefefe");
					newEmbed.setTimeStamp(getTimeAndDate());
					newEmbed.setDescription(messageNew);
					newEmbed.setTitle("__**" + static_cast<jsonifier::string>(discord_core_client::getInstance()->getBotUser().userName) + " help: front page**__");

					jsonifier::string msgString = "------\nHello! how are you doing today?! i'm " +
						static_cast<jsonifier::string>(discord_core_client::getInstance()->getBotUser().userName) + " and i'm here to help you out!\n" +
						"Please, select one of my commands from the drop-down menu below, to gain more information about them! (or select 'go back' "
						"to go back "
						"to the previous menu)\n------";
					input_event_data newEvent{};
					jsonifier::vector<jsonifier::string> numberEmojiNames{
						jsonifier::string{ "✅" },
						jsonifier::string{ "🍬" },
						jsonifier::string{ "🅱" },
						jsonifier::string{ "❌" },
					};
					jsonifier::vector<jsonifier::string> numberEmojiId;

					responseData.addMessageEmbed(newEmbed);
					for (uint64_t x = 0; x < selectOptionsNew.size(); x += 1) {
						jsonifier::string customId{ "select_page" + jsonifier::toString(x) };
						responseData.addButton(false, customId, jsonifier::toString(x), button_style::Success, numberEmojiNames.at(x));
						numberEmojiId.emplace_back(customId);
					}
					responseData.addButton(false, "exit", "Exit", button_style::Danger, "❌");
					if (isItFirst) {
						responseData.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						isItFirst  = false;
						newEvent01 = input_events::respondToInputEventAsync(responseData).get();
					} else {
						responseData.setResponseType(input_event_response_type::Edit_Interaction_Response);
						newEvent01 = input_events::respondToInputEventAsync(responseData).get();
					}
					unique_ptr<button_collector> button{ makeUnique<button_collector>(newEvent01) };
					auto createResponseData = makeUnique<create_interaction_response_data>();
					auto embedData			= makeUnique<embed_data>();
					embedData->setColor("fefefe");
					embedData->setTitle("__**Permissions Issue:**__");
					embedData->setTimeStamp(getTimeAndDate());
					embedData->setDescription("Sorry, but that button can only be pressed by <@" + argsNew.getUserData().id + ">!");
					createResponseData->addMessageEmbed(*embedData);
					createResponseData->setResponseType(interaction_callback_type::Channel_Message_With_Source);
					createResponseData->setFlags(64);
					auto buttonData = button->collectButtonData(false, 120000, 1, *createResponseData, argsNew.getUserData().id).get();
					uint64_t counter03{ 0 };
					jsonifier::vector<respond_to_input_event_data> editInteractionResponseData00;
					for (auto& value: selectOptionsNew) {
						embed_data msgEmbed00;
						msgEmbed00.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
						msgEmbed00.setColor("fefefe");
						msgEmbed00.setTimeStamp(getTimeAndDate());
						msgEmbed00.setDescription(msgString);
						msgEmbed00.setTitle("__**" + static_cast<jsonifier::string>(discord_core_client::getInstance()->getBotUser().userName) + " Help: Page " +
							jsonifier::toString(counter03 + 1) + " of " + jsonifier::toString(selectOptions.size()) + "**__");
						respond_to_input_event_data responseData03(*buttonData.at(0).interactionData);
						responseData03.setResponseType(input_event_response_type::Edit_Interaction_Response);
						responseData03.addMessageEmbed(msgEmbed00);
						responseData03.addSelectMenu(false, "help_menu", value, "Commands", 1, 1, select_menu_type::String_Select);
						editInteractionResponseData00.emplace_back(responseData03);
						counter03 += 1;
					}
					if (buttonData.size() > 0) {
						if (buttonData.at(0).buttonId == "exit" || buttonData.at(0).buttonId == "empty") {
							embed_data msgEmbed00;
							msgEmbed00.setAuthor(argsNew.getUserData().userName, argsNew.getUserData().getUserImageUrl(user_image_types::Avatar));
							msgEmbed00.setColor("fefefe");
							msgEmbed00.setTimeStamp(getTimeAndDate());
							msgEmbed00.setDescription(messageNew);
							msgEmbed00.setTitle("__**" + static_cast<jsonifier::string>(discord_core_client::getInstance()->getBotUser().userName) + " Help: Page " +
								jsonifier::toString(counter03 + 1) + " of " + jsonifier::toString(selectOptions.size()) + "**__");
							respond_to_input_event_data responseData03(*buttonData.at(0).interactionData);
							responseData03.setResponseType(input_event_response_type::Edit_Interaction_Response);
							responseData03.addMessageEmbed(msgEmbed00);
							newEvent = input_events::respondToInputEventAsync(responseData03).get();
							break;
						}
						counter02 = 0;
						for (uint64_t y = 0; y < numberEmojiId.size(); y += 1) {
							if (buttonData.at(0).buttonId == numberEmojiId.at(y)) {
								counter02 = y;
								break;
							}
						}
						newEvent = input_events::respondToInputEventAsync(editInteractionResponseData00.at(counter02)).get();
					} else {
						break;
					}
					unique_ptr<select_menu_collector> selectMenu{ makeUnique<select_menu_collector>(newEvent01) };
					auto selectMenuReturnData = selectMenu->collectSelectMenuData(false, 120000, 1, *createResponseData, argsNew.getUserData().id).get();
					embed_data msgEmbed{};
					for (auto& [key, value]: discord_core_client::getInstance()->getCommandController().getFunctions()) {
						for (size_t x = 0; x < key.size(); ++x) {
							msgEmbed = discord_core_client::getInstance()->getCommandController().getFunctions().at(key)->helpEmbed;
						}
					}
					if (selectMenuReturnData.at(0).values.at(0) == "go back") {
						respond_to_input_event_data responseData02(*selectMenuReturnData.at(0).interactionData);
						responseData02.setResponseType(input_event_response_type::Edit_Interaction_Response);
						responseData02.addMessageEmbed(msgEmbed);
						for (uint64_t x = 0; x < selectOptionsNew.size(); x += 1) {
							jsonifier::string customId{ "select_page" + jsonifier::toString(x) };
							responseData02.addButton(false, customId, jsonifier::toString(x), button_style::Success, numberEmojiNames.at(x));
							numberEmojiId.emplace_back(customId);
						}
						responseData02.addButton(false, "exit", "Exit", button_style::Danger, "❌");
						newEvent = input_events::respondToInputEventAsync(responseData02).get();
						continue;
					}

					respond_to_input_event_data responseData02(*selectMenuReturnData.at(0).interactionData);
					responseData02.setResponseType(input_event_response_type::Edit_Interaction_Response);
					responseData02.addMessageEmbed(msgEmbed);
					responseData02.addButton(false, "back", "Back", button_style::Success, "🔙");
					responseData02.addButton(false, "exit", "Exit", button_style::Success, "❌");
					newEvent				= input_events::respondToInputEventAsync(responseData02).get();
					auto buttonReturnData02 = button_collector{ newEvent01 }.collectButtonData(false, 120000, 1, *createResponseData, argsNew.getUserData().id).get();
					if (buttonReturnData02.at(0).buttonId == "back") {
						responseData = respond_to_input_event_data{ *buttonReturnData02.at(0).interactionData };
						responseData.setResponseType(input_event_response_type::Deferred_Response);
						auto interactionData = input_events::respondToInputEventAsync(responseData).get().getInteractionData();
						responseData		 = respond_to_input_event_data{ interactionData };
						continue;
					} else if (buttonReturnData02.at(0).buttonId == "exit" || buttonReturnData02.at(0).buttonId == "empty") {
						respond_to_input_event_data responseData03(*buttonReturnData02.at(0).interactionData);
						responseData03.setResponseType(input_event_response_type::Edit_Interaction_Response);
						responseData03.addMessageEmbed(msgEmbed);
						newEvent = input_events::respondToInputEventAsync(responseData03).get();
						break;
					}
				}

				return;
			} catch (const std::runtime_error& error) {
				std::cout << "help::execute()" << error.what() << std::endl;
			}
		}
		~help(){};
	};
}// namespace discord_core_api
