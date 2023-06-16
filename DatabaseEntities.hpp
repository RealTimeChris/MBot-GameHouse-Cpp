// DatabaseEntities.hpp - Database stuff.
// may 24, 2021
// chris m.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
	#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>

#include <discordcoreapi/index.hpp>

namespace discord_core_api {

	struct discord_user_data {
		jsonifier::vector<snowflake> botCommanders{ 0, 0, 0 };
		float hoursOfRobberyCooldown{ 0.100f };
		int64_t hoursOfDrugSaleCooldown{ 3 };
		int64_t hoursOfDepositCooldown{ 24 };
		jsonifier::string currencyName{ "mbux" };
		jsonifier::string userName{ "" };
		snowflake userId{ 0 };
	};

	struct card {
		uint64_t value{ 0 };
		jsonifier::string suit{ "" };
		jsonifier::string type{ "" };
	};

	class deck {
	  public:
		jsonifier::vector<card> cards{};

		deck() {
			this->cards.resize(52);

			for (uint64_t x = 0; x < 52; x += 1) {
				this->cards[x].suit = "";
				this->cards[x].type = "";
				this->cards[x].value = 0;

				if (trunc(x / 13) == 0) {
					this->cards[x].suit = ":hearts:";
				} else if (trunc(x / 13) == 1) {
					this->cards[x].suit = ":diamonds:";
				} else if (trunc(x / 13) == 2) {
					this->cards[x].suit = ":clubs:";
				} else if (trunc(x / 13) == 3) {
					this->cards[x].suit = ":spades:";
				}

				if (x % 13 == 0) {
					this->cards[x].type = "ace";
					this->cards[x].value = 0;
				} else if (x % 13 == 1) {
					this->cards[x].type = "2";
					this->cards[x].value = 2;
				} else if (x % 13 == 2) {
					this->cards[x].type = "3";
					this->cards[x].value = 3;
				} else if (x % 13 == 3) {
					this->cards[x].type = "4";
					this->cards[x].value = 4;
				} else if (x % 13 == 4) {
					this->cards[x].type = "5";
					this->cards[x].value = 5;
				} else if (x % 13 == 5) {
					this->cards[x].type = "6";
					this->cards[x].value = 6;
				} else if (x % 13 == 6) {
					this->cards[x].type = "7";
					this->cards[x].value = 7;
				} else if (x % 13 == 7) {
					this->cards[x].type = "8";
					this->cards[x].value = 8;
				} else if (x % 13 == 8) {
					this->cards[x].type = "9";
					this->cards[x].value = 9;
				} else if (x % 13 == 9) {
					this->cards[x].type = "10";
					this->cards[x].value = 10;
				} else if (x % 13 == 10) {
					this->cards[x].type = "jack";
					this->cards[x].value = 10;
				} else if (x % 13 == 11) {
					this->cards[x].type = "queen";
					this->cards[x].value = 10;
				} else if (x % 13 == 12) {
					this->cards[x].type = "king";
					this->cards[x].value = 10;
				}
			}
		}

		// draws a random card from the deck.
		card drawRandomcard() {
			if (this->cards.size() == 0) {
				card voidCard{
					.value = 0,
					.suit = "",
					.type = "",
				};
				voidCard.suit = ":black_large_square:";
				voidCard.type = "null";
				voidCard.value = 0;
				return voidCard;
			}
			std::mt19937_64 randomEngine{ static_cast<uint64_t>(
				std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) };
			uint64_t cardIndex = static_cast<uint64_t>(
				static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * static_cast<float>(this->cards.size()));
			card currentCard = this->cards.at(cardIndex);
			this->cards.erase(this->cards.begin() + cardIndex);
			return currentCard;
		}
	};

	struct roulette_bet {
		jsonifier::vector<jsonifier::string> winningNumbers{};
		jsonifier::string betOptions{ "" };
		uint64_t payoutAmount{ 0 };
		jsonifier::string betType{ "" };
		uint64_t betAmount{ 0 };
		snowflake userId{ 0 };
	};

	struct roulette {
		jsonifier::vector<roulette_bet> rouletteBets{};
		bool currentlySpinning{ false };
	};

	struct largest_payout {
		jsonifier::string timeStamp{ "" };
		jsonifier::string userName{ "" };
		snowflake userId{ 0 };
		int64_t amount{ 0 };
	};

	struct casino_stats {
		largest_payout largestBlackjackPayout{};
		largest_payout largestCoinFlipPayout{};
		largest_payout largestRoulettePayout{};
		largest_payout largestSlotsPayout{};
		int64_t totalBlackjackPayout{ 0 };
		int64_t totalCoinFlipPayout{ 0 };
		int64_t totalRoulettePayout{ 0 };
		int64_t totalSlotsPayout{ 0 };
		int64_t totalPayout{ 0 };
	};

	struct inventory_item {
		jsonifier::string itemName{ "" };
		jsonifier::string emoji{ "" };
		uint64_t itemCost{ 0 };
		uint64_t selfMod{ 0 };
		int64_t oppMod{ 0 };
	};

	struct inventory_role {
		uint64_t roleCost{ 0 };
		jsonifier::string roleName{ "" };
		snowflake roleId{ 0 };
	};

	struct guild_shop {
		jsonifier::vector<inventory_item> items{};
		jsonifier::vector<inventory_role> roles{};
	};

	struct currency {
		uint64_t timeOfLastDeposit{ 0 };
		uint64_t wallet{ 10000 };
		uint64_t bank{ 10000 };
	};

	struct discord_guild_data {
		jsonifier::vector<snowflake> gameChannelIds{};
		jsonifier::string borderColor{ "fefefe" };
		jsonifier::vector<card> blackjackStack{};
		jsonifier::string guildName{ "" };
		uint64_t memberCount{ 0 };
		casino_stats casinoStats{};
		roulette rouletteGame{};
		snowflake guildId{ 0 };
		guild_shop guildShop{};
	};

	struct discord_guild_member_data {
		uint64_t lastTimeRobbed{ 0 };
		uint64_t lastTimeWorked{ 0 };
		jsonifier::string guildMemberMention{ "" };
		jsonifier::vector<inventory_item> items{};
		jsonifier::vector<inventory_role> roles{};
		snowflake guildMemberId{ 0 };
		jsonifier::string displayName{ "" };
		jsonifier::string globalId{ "" };
		jsonifier::string userName{ "" };
		snowflake guildId{ 0 };
		currency currency{};
	};

	enum class database_workload_type {
		Discord_User_Write = 0,
		Discord_User_Read = 1,
		Discord_Guild_Write = 2,
		Discord_Guild_Read = 3,
		Discord_Guild_Member_Write = 4,
		Discord_Guild_Member_Read = 5
	};

	struct database_workload {
		discord_guild_member_data guildMemberData{};
		database_workload_type workloadType{};
		discord_guild_data guildData{};
		discord_user_data userData{};
	};

	struct database_return_value {
		discord_guild_member_data discordGuildMember{};
		discord_guild_data discordGuild{};
		discord_user_data discordUser{};
	};

	class database_manager_agent {
	  public:
		inline void initialize(snowflake botUserIdNew) {
			database_manager_agent::botUserId = botUserIdNew;
			auto newClient = database_manager_agent::getClient();
			mongocxx::database newDataBase	   = (*newClient)[database_manager_agent::botUserId.operator jsonifier::string().data()];
			mongocxx::collection newCollection = newDataBase[database_manager_agent::botUserId.operator jsonifier::string().data()];
		}

		inline mongocxx::pool::entry getClient() {
			return database_manager_agent::thePool.acquire();
		}

		inline database_return_value submitWorkloadAndGetResults(database_workload workload) {
			std::lock_guard<std::mutex> workloadLock{ database_manager_agent::workloadMutex01 };
			while (database_manager_agent::botUserId == 0) {
				std::this_thread::sleep_for(1ms);
			}
			database_return_value newData{};
			mongocxx::pool::entry thePtr = database_manager_agent::getClient();
			try {
				auto newDataBase   = (*thePtr)[database_manager_agent::botUserId.operator jsonifier::string().data()];
				auto newCollection = newDataBase[database_manager_agent::botUserId.operator jsonifier::string().data()];
				switch (workload.workloadType) {
					case (database_workload_type::Discord_User_Write): {
						auto doc = database_manager_agent::convertUserDataToDBDoc(workload.userData);
						bsoncxx::builder::basic::document document{};
						document.append(
							bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int64(static_cast<uint64_t>(workload.userData.userId))));
						auto resultNew = newCollection.find_one(document.view());
						auto resultNewer = newCollection.find_one_and_replace(document.view(), std::move(doc.extract()),
							mongocxx::v_noabi::options::find_one_and_replace{}.return_document(mongocxx::v_noabi::options::return_document::k_after));
						if (!resultNewer) {
							auto doc02 = database_manager_agent::convertUserDataToDBDoc(workload.userData);
							newCollection.insert_one(std::move(doc02.extract()));
							return newData;
						}
						break;
					}
					case (database_workload_type::Discord_User_Read): {
						bsoncxx::builder::basic::document document{};
						document.append(
							bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int64(static_cast<uint64_t>(workload.userData.userId))));
						auto resultNew = newCollection.find_one(document.view());
						if (resultNew) {
							discord_user_data userData = database_manager_agent::parseUserData(*resultNew);
							newData.discordUser = userData;
							return newData;
						} else {
							return newData;
						}
						break;
					}
					case (database_workload_type::Discord_Guild_Write): {
						auto doc = database_manager_agent::convertGuildDataToDBDoc(workload.guildData);
						bsoncxx::builder::basic::document document{};
						document.append(
							bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int64(static_cast<uint64_t>(workload.guildData.guildId))));
						auto resultNewer = newCollection.find_one_and_replace(document.view(), std::move(doc.extract()),
							mongocxx::v_noabi::options::find_one_and_replace{}.return_document(mongocxx::v_noabi::options::return_document::k_after));
						if (!resultNewer) {
							auto doc02 = database_manager_agent::convertGuildDataToDBDoc(workload.guildData);
							newCollection.insert_one(std::move(doc02.extract()));
							return newData;
						}
						break;
					}
					case (database_workload_type::Discord_Guild_Read): {
						bsoncxx::builder::basic::document document{};
						document.append(
							bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int64(static_cast<uint64_t>(workload.guildData.guildId))));
						auto resultNew = newCollection.find_one(document.view());
						if (resultNew) {
							discord_guild_data guildData = database_manager_agent::parseGuildData(*resultNew);
							newData.discordGuild = guildData;
							return newData;
						} else {
							return newData;
						}
						break;
					}
					case (database_workload_type::Discord_Guild_Member_Write): {
						auto doc = database_manager_agent::convertGuildMemberDataToDBDoc(workload.guildMemberData);
						bsoncxx::builder::basic::document document{};
						document.append(bsoncxx::builder::basic::kvp("_id", workload.guildMemberData.globalId));
						auto resultNewer = newCollection.find_one_and_replace(document.view(), std::move(doc.extract()),
							mongocxx::v_noabi::options::find_one_and_replace{}.return_document(mongocxx::v_noabi::options::return_document::k_after));
						if (!resultNewer) {
							auto doc02 = database_manager_agent::convertGuildMemberDataToDBDoc(workload.guildMemberData);
							newCollection.insert_one(std::move(doc02.extract()));
							return newData;
						}
						break;
					}
					case (database_workload_type::Discord_Guild_Member_Read): {
						bsoncxx::builder::basic::document document{};
						document.append(bsoncxx::builder::basic::kvp("_id", workload.guildMemberData.globalId));
						auto resultNew = newCollection.find_one(document.view());
						if (resultNew) {
							discord_guild_member_data guildMemberData = database_manager_agent::parseGuildMemberData(*resultNew);
							newData.discordGuildMember = guildMemberData;
							return newData;
						} else {
							return newData;
						}
						break;
					}
				}
				return newData;
			} catch (const std::runtime_error& error) {
				std::cout << "database_manager_agent::run()" << error.what() << std::endl;
				return newData;
			}
		}

	  protected:
		mongocxx::instance instance{};
		std::mutex workloadMutex01{};
		mongocxx::pool thePool{};
		snowflake botUserId{};

		template<typename value_type_to_search>
		static void getValueIfNotNull(jsonifier::string& value, value_type_to_search&& valueToSearch, const jsonifier::string& valueToFind) {
			if (valueToSearch[valueToFind].type() != bsoncxx::v_noabi::type::k_null) {
				value = jsonifier::string{ valueToSearch[valueToFind].get_string().value };
			}
		}

		template<typename value_type_to_search>
		static void getValueIfNotNull(snowflake& value, value_type_to_search&& valueToSearch, const jsonifier::string& valueToFind) {
			if (valueToSearch[valueToFind].type() != bsoncxx::v_noabi::type::k_null) {
				value = valueToSearch[valueToFind].get_int64().value;
			}
		}

		template<jsonifier::concepts::enum_t enum_type, typename value_type_to_search>
		static void getValueIfNotNull(enum_type& value, value_type_to_search&& valueToSearch, const jsonifier::string& valueToFind) {
			if (valueToSearch[valueToFind].type() != bsoncxx::v_noabi::type::k_null) {
				value = static_cast<enum_type>(valueToSearch[valueToFind].get_int64().value);
			}
		}

		template<jsonifier::concepts::signed_t value_type_new, typename value_type_to_search>
		static void getValueIfNotNull(value_type_new& value, value_type_to_search&& valueToSearch, const jsonifier::string& valueToFind) {
			if (valueToSearch[valueToFind].type() != bsoncxx::v_noabi::type::k_null) {
				value = valueToSearch[valueToFind].get_int64().value;
			}
		}

		template<jsonifier::concepts::unsigned_t value_type_new, typename value_type_to_search>
		static void getValueIfNotNull(value_type_new& value, value_type_to_search&& valueToSearch, const jsonifier::string& valueToFind) {
			if (valueToSearch[valueToFind].type() != bsoncxx::v_noabi::type::k_null) {
				value = valueToSearch[valueToFind].get_int64().value;
			}
		}

		template<jsonifier::concepts::float_t value_type_new, typename value_type_to_search>
		static void getValueIfNotNull(value_type_new& value, value_type_to_search&& valueToSearch, const jsonifier::string& valueToFind) {
			if (valueToSearch[valueToFind].type() != bsoncxx::v_noabi::type::k_null) {
				value = valueToSearch[valueToFind].get_double().value;
			}
		}

		template<typename value_type_to_search> static void getValueIfNotNull(bool& value, value_type_to_search&& valueToSearch, const jsonifier::string& valueToFind) {
			if (valueToSearch[valueToFind].type() != bsoncxx::v_noabi::type::k_null) {
				value = valueToSearch[valueToFind].get_bool().value;
			}
		}

		static bsoncxx::builder::basic::document convertUserDataToDBDoc(discord_user_data& discordUserData) {
			bsoncxx::builder::basic::document buildDoc;
			try {
				using bsoncxx::builder::basic::kvp;
				buildDoc.append(kvp("_id", bsoncxx::types::b_int64(static_cast<uint64_t>(discordUserData.userId))));
				buildDoc.append(kvp("userId", bsoncxx::types::b_int64(static_cast<uint64_t>(discordUserData.userId))));
				buildDoc.append(kvp("userName", discordUserData.userName));
				buildDoc.append(kvp("currencyName", discordUserData.currencyName));
				buildDoc.append(kvp("hoursOfDepositCooldown", bsoncxx::types::b_int64(static_cast<int64_t>(discordUserData.hoursOfDepositCooldown))));
				buildDoc.append(
					kvp("hoursOfDrugSaleCooldown", bsoncxx::types::b_int64(static_cast<int64_t>(discordUserData.hoursOfDrugSaleCooldown))));
				buildDoc.append(kvp("hoursOfRobberyCooldown", bsoncxx::types::b_double(static_cast<double>(discordUserData.hoursOfRobberyCooldown))));
				buildDoc.append(kvp("botCommanders", [discordUserData](bsoncxx::builder::basic::sub_array subArray) {
					for (auto& value: discordUserData.botCommanders) {
						subArray.append(bsoncxx::types::b_int64(static_cast<uint64_t>(value)));
					}
				}));
				return buildDoc;
			} catch (const std::exception& error) {
				std::cout << "database_manager_agent::convertUserDataToDBDoc()" << error.what() << std::endl;
				return buildDoc;
			}
		}

		static discord_user_data parseUserData(bsoncxx::document::value& docValue) {
			discord_user_data userData{};
			try {
				getValueIfNotNull(userData.userName, docValue.view(), "userName");
				getValueIfNotNull(userData.currencyName, docValue.view(), "currencyName");
				getValueIfNotNull(userData.hoursOfDepositCooldown, docValue.view(), "hoursOfDepositCooldown");
				getValueIfNotNull(userData.hoursOfDrugSaleCooldown, docValue.view(), "hoursOfDrugSaleCooldown");
				getValueIfNotNull(userData.hoursOfRobberyCooldown, docValue.view(), "hoursOfRobberyCooldown");
				getValueIfNotNull(userData.userId, docValue.view(), "userId");
				auto botCommandersArray = docValue.view()["botCommanders"].get_array();
				jsonifier::vector<snowflake> newVector;
				for (const auto& value: botCommandersArray.value) {
					newVector.emplace_back(value.get_int64().value);
				}
				userData.botCommanders = newVector;
				return userData;
			} catch (const std::exception& error) {
				std::cout << "database_manager_agent::parseUserData()" << error.what() << std::endl;
				return userData;
			}
		}

		static bsoncxx::builder::basic::document convertGuildDataToDBDoc(discord_guild_data& discordGuildData) {
			bsoncxx::builder::basic::document buildDoc;
			try {
				using bsoncxx::builder::basic::kvp;
				buildDoc.append(kvp("_id", bsoncxx::types::b_int64(static_cast<uint64_t>(discordGuildData.guildId))));
				buildDoc.append(kvp("guildId", bsoncxx::types::b_int64(static_cast<uint64_t>(discordGuildData.guildId))));
				buildDoc.append(kvp("guildName", discordGuildData.guildName));
				buildDoc.append(kvp("memberCount", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.memberCount))));
				buildDoc.append(kvp("blackjackStack", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
					for (auto& value: discordGuildData.blackjackStack) {
						subArray.append([value](bsoncxx::builder::basic::sub_document subDocument) {
							subDocument.append(kvp("suit", value.suit), kvp("type", value.type),
								kvp("value", bsoncxx::types::b_int64(static_cast<int64_t>(value.value))));
						});
					}
				}));
				buildDoc.append(kvp("borderColor", discordGuildData.borderColor));
				buildDoc.append(kvp("gameChannelIds", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
					for (auto& value: discordGuildData.gameChannelIds) {
						subArray.append(bsoncxx::types::b_int64(static_cast<uint64_t>(value)));
					}
				}));
				buildDoc.append(kvp("guildShop", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
					subDocument.append(kvp("items",
						[discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
							for (const auto& value: discordGuildData.guildShop.items) {
								subArray.append([value](bsoncxx::builder::basic::sub_document subDocument2) {
									subDocument2.append(kvp("itemName", value.itemName),
										kvp("itemCost", bsoncxx::types::b_int64(static_cast<int64_t>(value.itemCost))),
										kvp("selfMod", bsoncxx::types::b_int64(static_cast<int64_t>(value.selfMod))),
										kvp("oppMod", bsoncxx::types::b_int64(static_cast<int64_t>(value.oppMod))),
										kvp("emoji", value.emoji));
								});
							}
						})),
						subDocument.append(kvp("roles", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
							for (const auto& value: discordGuildData.guildShop.roles) {
								subArray.append([value](bsoncxx::builder::basic::sub_document subDocument2) {
									subDocument2.append(kvp("roleName", value.roleName));
									subDocument2.append(kvp("roleId", bsoncxx::types::b_int64(static_cast<uint64_t>(value.roleId))));
									subDocument2.append(kvp("roleCost", bsoncxx::types::b_int64(static_cast<int64_t>(value.roleCost))));
								});
							}
						}));
				}));
				buildDoc.append(kvp("casinoStats", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
					subDocument.append(kvp("largestBlackjackPayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
						subDoc2.append(
							kvp("amount", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.casinoStats.largestBlackjackPayout.amount))));
						subDoc2.append(kvp("userId",
							bsoncxx::types::b_int64(static_cast<uint64_t>(discordGuildData.casinoStats.largestBlackjackPayout.userId))));
						subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestBlackjackPayout.timeStamp));
						subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestBlackjackPayout.userName));
					}));
					subDocument.append(kvp("totalBlackjackPayout",
						bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.casinoStats.totalBlackjackPayout))));
					subDocument.append(kvp("largestCoinFlipPayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
						subDoc2.append(
							kvp("amount", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.casinoStats.largestCoinFlipPayout.amount))));
						subDoc2.append(
							kvp("userId", bsoncxx::types::b_int64(static_cast<uint64_t>(discordGuildData.casinoStats.largestCoinFlipPayout.userId))));
						subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestCoinFlipPayout.timeStamp));
						subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestCoinFlipPayout.userName));
					}));
					subDocument.append(
						kvp("totalCoinFlipPayout", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.casinoStats.totalCoinFlipPayout))));
					subDocument.append(kvp("largestRoulettePayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
						subDoc2.append(
							kvp("amount", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.casinoStats.largestRoulettePayout.amount))));
						subDoc2.append(
							kvp("userId", bsoncxx::types::b_int64(static_cast<uint64_t>(discordGuildData.casinoStats.largestRoulettePayout.userId))));
						subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestRoulettePayout.timeStamp));
						subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestRoulettePayout.userName));
					}));
					subDocument.append(
						kvp("totalRoulettePayout", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.casinoStats.totalRoulettePayout))));
					subDocument.append(kvp("largestSlotsPayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
						subDoc2.append(
							kvp("amount", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.casinoStats.largestSlotsPayout.amount))));
						subDoc2.append(
							kvp("userId", bsoncxx::types::b_int64(static_cast<uint64_t>(discordGuildData.casinoStats.largestSlotsPayout.userId))));
						subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestSlotsPayout.timeStamp));
						subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestSlotsPayout.userName));
					}));
					subDocument.append(
						kvp("totalSlotsPayout", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.casinoStats.totalSlotsPayout))));
					subDocument.append(kvp("totalPayout", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildData.casinoStats.totalPayout))));
				}));
				buildDoc.append(kvp("rouletteGame", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
					subDocument.append(
						kvp("rouletteBets",
							[discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
								for (auto& value: discordGuildData.rouletteGame.rouletteBets) {
									subArray.append([value](bsoncxx::builder::basic::sub_document subDoc2) {
										subDoc2.append(kvp("betAmount", bsoncxx::types::b_int64(static_cast<int64_t>(value.betAmount))));
										subDoc2.append(kvp("betOptions", value.betOptions));
										subDoc2.append(kvp("betType", value.betType));
										subDoc2.append(kvp("userId", bsoncxx::types::b_int64(static_cast<uint64_t>(value.userId))));
										subDoc2.append(kvp("payoutAmount", bsoncxx::types::b_int64(static_cast<int64_t>(value.payoutAmount))));
										subDoc2.append(kvp("winningNumbers", [value](bsoncxx::builder::basic::sub_array subArray2) {
											for (auto& value2: value.winningNumbers) {
												subArray2.append(value2);
											}
										}));
									});
								}
							}),
						kvp("currentlySpinning", bsoncxx::types::b_bool(discordGuildData.rouletteGame.currentlySpinning)));
				}));
				return buildDoc;
			} catch (const std::exception& error) {
				std::cout << "database_manager_agent::convertGuildDataToDBDoc()" << error.what() << std::endl;
				return buildDoc;
			}
		};

		static discord_guild_data parseGuildData(bsoncxx::document::value& docValue) {
			discord_guild_data guildData{};
			try {
				getValueIfNotNull(guildData.guildId, docValue.view(), "guildId");
				getValueIfNotNull(guildData.guildName, docValue.view(), "guildName");
				getValueIfNotNull(guildData.memberCount, docValue.view(), "memberCount");
				if (docValue.view()["blackjackStack"].type() != bsoncxx::v_noabi::type::k_null) {
					for (auto& value: docValue.view()["blackjackStack"].get_array().value) {
						card blackjackCard{};
						getValueIfNotNull(blackjackCard.suit, value.get_document().view(), "suit");
						getValueIfNotNull(blackjackCard.type, value.get_document().view(), "type");
						getValueIfNotNull(blackjackCard.value, value.get_document().view(), "value");
						guildData.blackjackStack.emplace_back(blackjackCard);
					}
				}
				guildData.borderColor = docValue.view()["borderColor"].get_utf8().value;
				for (auto& value: docValue.view()["gameChannelIds"].get_array().value) {
					guildData.gameChannelIds.emplace_back(value.get_int64().value);
				}
				for (auto& value: docValue.view()["guildShop"].get_document().value["items"].get_array().value) {
					inventory_item item;
					getValueIfNotNull(item.emoji, value, "emoji");
					getValueIfNotNull(item.itemCost, value, "itemCost");
					getValueIfNotNull(item.itemName, value, "itemName");
					getValueIfNotNull(item.oppMod, value, "oppMod");
					getValueIfNotNull(item.selfMod, value, "selfMod");
					guildData.guildShop.items.emplace_back(item);
				}
				for (auto& value: docValue.view()["guildShop"].get_document().value["roles"].get_array().value) {
					inventory_role role;
					getValueIfNotNull(role.roleCost, value, "roleCost");
					getValueIfNotNull(role.roleId, value, "roleId");
					getValueIfNotNull(role.roleName, value, "roleName");
					guildData.guildShop.roles.emplace_back(role);
				}
				auto largestBlackjackPayout = docValue.view()["casinoStats"].get_document().value["largestBlackjackPayout"].get_document();
				getValueIfNotNull(guildData.casinoStats.largestBlackjackPayout.amount, largestBlackjackPayout.value, "amount");
				getValueIfNotNull(guildData.casinoStats.largestBlackjackPayout.timeStamp, largestBlackjackPayout.value, "timeStamp");
				getValueIfNotNull(guildData.casinoStats.largestBlackjackPayout.userId, largestBlackjackPayout.value, "userId");
				getValueIfNotNull(guildData.casinoStats.largestBlackjackPayout.userName, largestBlackjackPayout.value, "userName");
				getValueIfNotNull(guildData.casinoStats.totalBlackjackPayout, docValue.view()["casinoStats"].get_document().value, "totalBlackjackPayout");
				auto largestCoinflipPayout = docValue.view()["casinoStats"].get_document().value["largestCoinFlipPayout"].get_document();
				getValueIfNotNull(guildData.casinoStats.largestCoinFlipPayout.amount, largestCoinflipPayout.value, "amount");
				getValueIfNotNull(guildData.casinoStats.largestCoinFlipPayout.timeStamp, largestCoinflipPayout.value, "timeStamp");
				getValueIfNotNull(guildData.casinoStats.largestCoinFlipPayout.userId, largestCoinflipPayout.value, "userId");
				getValueIfNotNull(guildData.casinoStats.largestCoinFlipPayout.userName, largestCoinflipPayout.value, "userName");
				getValueIfNotNull(guildData.casinoStats.totalCoinFlipPayout, docValue.view()["casinoStats"].get_document().value, "totalCoinFlipPayout");
				auto largestRoulettePayout = docValue.view()["casinoStats"].get_document().value["largestRoulettePayout"].get_document();
				getValueIfNotNull(guildData.casinoStats.largestRoulettePayout.amount, largestRoulettePayout.value, "amount");
				getValueIfNotNull(guildData.casinoStats.largestRoulettePayout.timeStamp, largestRoulettePayout.value, "timeStamp");
				getValueIfNotNull(guildData.casinoStats.largestRoulettePayout.userId, largestRoulettePayout.value, "userId");
				getValueIfNotNull(guildData.casinoStats.largestRoulettePayout.userName, largestRoulettePayout.value, "userName");
				getValueIfNotNull(guildData.casinoStats.totalRoulettePayout, docValue.view()["casinoStats"].get_document().value, "totalRoulettePayout");
				auto largestSlotsPayout = docValue.view()["casinoStats"].get_document().value["largestSlotsPayout"].get_document();
				getValueIfNotNull(guildData.casinoStats.largestSlotsPayout.amount, largestSlotsPayout.value, "amount");
				getValueIfNotNull(guildData.casinoStats.largestSlotsPayout.timeStamp, largestSlotsPayout.value, "timeStamp");
				getValueIfNotNull(guildData.casinoStats.largestSlotsPayout.userId, largestSlotsPayout.value, "userId");
				getValueIfNotNull(guildData.casinoStats.largestSlotsPayout.userName, largestSlotsPayout.value, "userName");
				getValueIfNotNull(guildData.casinoStats.totalSlotsPayout, docValue.view()["casinoStats"].get_document().value, "totalSlotsPayout");
				getValueIfNotNull(guildData.casinoStats.totalPayout, docValue.view()["casinoStats"].get_document().value, "totalPayout");
				getValueIfNotNull(guildData.rouletteGame.currentlySpinning, docValue.view()["rouletteGame"].get_document().value, "currentlySpinning");
				for (auto& value: docValue.view()["rouletteGame"].get_document().value["rouletteBets"].get_array().value) {
					roulette_bet rouletteBet{};
					getValueIfNotNull(rouletteBet.betAmount, value.get_document().value, "betAmount");
					getValueIfNotNull(rouletteBet.betOptions, value.get_document().value, "betOptions");
					getValueIfNotNull(rouletteBet.betType, value.get_document().value, "betType");
					getValueIfNotNull(rouletteBet.payoutAmount, value.get_document().value, "payoutAmount");
					getValueIfNotNull(rouletteBet.userId, value.get_document().value, "userId");
					for (auto& value2: value.get_document().value["winningNumbers"].get_array().value) {
						rouletteBet.winningNumbers.emplace_back(static_cast<jsonifier::string>(value2.get_utf8().value));
					}
					guildData.rouletteGame.rouletteBets.emplace_back(rouletteBet);
				}
				return guildData;
			} catch (const std::exception& error) {
				std::cout << "database_manager_agent::parseGuildData(), from guild_data: " << error.what() << std::endl;
				return guildData;
			}
		};

		static bsoncxx::builder::basic::document convertGuildMemberDataToDBDoc(discord_guild_member_data& discordGuildMemberData) {
			bsoncxx::builder::basic::document buildDoc;
			try {
				using bsoncxx::builder::basic::kvp;
				buildDoc.append(kvp("guildMemberMention", discordGuildMemberData.guildMemberMention));
				buildDoc.append(kvp("_id", discordGuildMemberData.globalId));
				buildDoc.append(kvp("guildId", bsoncxx::types::b_int64(static_cast<uint64_t>(discordGuildMemberData.guildId))));
				buildDoc.append(kvp("guildMemberId", bsoncxx::types::b_int64(static_cast<uint64_t>(discordGuildMemberData.guildMemberId))));
				buildDoc.append(kvp("globalId", discordGuildMemberData.globalId));
				buildDoc.append(kvp("userName", discordGuildMemberData.userName));
				buildDoc.append(kvp("displayName", discordGuildMemberData.displayName));
				buildDoc.append(kvp("lastTimeRobbed", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildMemberData.lastTimeRobbed))));
				buildDoc.append(kvp("lastTimeWorked", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildMemberData.lastTimeWorked))));
				buildDoc.append(kvp("currency", [discordGuildMemberData](bsoncxx::builder::basic::sub_document subDocument) {
					subDocument.append(kvp("bank", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildMemberData.currency.bank))),
						kvp("wallet", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildMemberData.currency.wallet))),
						kvp("timeOfLastDeposit", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildMemberData.currency.timeOfLastDeposit))));
				}));
				buildDoc.append(kvp("items",
					[discordGuildMemberData](bsoncxx::builder::basic::sub_array subArray) {
						for (uint64_t x = 0; x < discordGuildMemberData.items.size(); x += 1) {
							subArray.append([discordGuildMemberData, x](bsoncxx::builder::basic::sub_document subDocument) {
								subDocument.append(kvp("itemName", discordGuildMemberData.items.at(x).itemName),
									kvp("itemCost", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildMemberData.items.at(x).itemCost))),
									kvp("selfMod", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildMemberData.items.at(x).selfMod))),
									kvp("oppMod", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildMemberData.items.at(x).oppMod))),
									kvp("emoji", discordGuildMemberData.items.at(x).emoji));
							});
						}
					})),
					buildDoc.append(kvp("roles", [discordGuildMemberData](bsoncxx::builder::basic::sub_array subArray) {
						for (uint64_t x = 0; x < discordGuildMemberData.roles.size(); x += 1) {
							subArray.append([discordGuildMemberData, x](bsoncxx::builder::basic::sub_document subDocument) {
								subDocument.append(kvp("roleName", discordGuildMemberData.roles.at(x).roleName),
									kvp("roleId", bsoncxx::types::b_int64(static_cast<uint64_t>(discordGuildMemberData.roles.at(x).roleId))),
									kvp("roleCost", bsoncxx::types::b_int64(static_cast<int64_t>(discordGuildMemberData.roles.at(x).roleCost))));
							});
						}
					}));

				return buildDoc;
			} catch (const std::exception& error) {
				std::cout << "database_manager_agent::convertGuildMemberDataToDBDoc()" << error.what() << std::endl;
				return buildDoc;
			}
		};

		static discord_guild_member_data parseGuildMemberData(bsoncxx::document::value& docValue) {
			discord_guild_member_data guildMemberData{};
			try {
				getValueIfNotNull(guildMemberData.guildMemberMention, docValue.view(), "guildMemberMention");
				getValueIfNotNull(guildMemberData.guildId, docValue.view(), "guildId");
				getValueIfNotNull(guildMemberData.displayName, docValue.view(), "displayName");
				getValueIfNotNull(guildMemberData.globalId, docValue.view(), "globalId");
				getValueIfNotNull(guildMemberData.guildMemberId, docValue.view(), "guildMemberId");
				getValueIfNotNull(guildMemberData.userName, docValue.view(), "userName");
				getValueIfNotNull(guildMemberData.lastTimeWorked, docValue.view(), "lastTimeWorked");
				getValueIfNotNull(guildMemberData.lastTimeRobbed, docValue.view(), "lastTimeRobbed");
				getValueIfNotNull(guildMemberData.currency.bank, docValue.view()["currency"].get_document().value, "bank");
				getValueIfNotNull(guildMemberData.currency.wallet, docValue.view()["currency"].get_document().value, "wallet");
				getValueIfNotNull(guildMemberData.currency.timeOfLastDeposit, docValue.view()["currency"].get_document().value, "timeOfLastDeposit");

				for (auto& value: docValue.view()["items"].get_array().value) {
					inventory_item item;
					item.emoji = value["emoji"].get_utf8().value;
					item.itemName = value["itemName"].get_utf8().value;
					item.itemCost = value["itemCost"].get_int64().value;
					item.oppMod = value["oppMod"].get_int64().value;
					item.selfMod = value["selfMod"].get_int64().value;
					guildMemberData.items.emplace_back(item);
				}
				for (auto& value: docValue.view()["roles"].get_array().value) {
					inventory_role role;
					role.roleCost = value["roleCost"].get_int64().value;
					role.roleId = value["roleId"].get_int64().value;
					role.roleName = value["roleName"].get_utf8().value;
					guildMemberData.roles.emplace_back(role);
				}
				return guildMemberData;
			} catch (const std::exception& error) {
				std::cout << "database_manager_agent::parseGuildMemberData()" << error.what() << std::endl;
				return guildMemberData;
			}
		}
	};

	class discord_user {
	  public:
		static int64_t guildCount;

		discord_user_data data{};

		inline discord_user(database_manager_agent& other, jsonifier::string userNameNew, snowflake userIdNew) {
			this->data.userId = userIdNew;
			this->data.userName = userNameNew;
			this->getDataFromDB(other);
			this->data.userId = userIdNew;
			this->data.userName = userNameNew;
		}

		inline void writeDataToDB(database_manager_agent& other) {
			database_workload workload{};
			workload.workloadType = database_workload_type::Discord_User_Write;
			workload.userData = this->data;
			other.submitWorkloadAndGetResults(workload);
		}

		inline void getDataFromDB(database_manager_agent& other) {
			database_workload workload{};
			workload.workloadType = database_workload_type::Discord_User_Read;
			workload.userData = this->data;
			auto result = other.submitWorkloadAndGetResults(workload);
			if (result.discordUser.userId != 0) {
				this->data = result.discordUser;
			}
		}
	};

	class discord_guild {
	  public:
		inline discord_guild() noexcept = default;
		discord_guild_data data{};

		inline discord_guild(database_manager_agent& other, guild_cache_data guildData) {
			this->data.guildId = guildData.id;
			this->data.guildName = guildData.name;
			this->data.memberCount = guildData.memberCount;
			this->getDataFromDB(other);
			this->data.guildId = guildData.id;
			this->data.guildName = guildData.name;
			this->data.memberCount = guildData.memberCount;
		}

		inline void writeDataToDB(database_manager_agent& other) {
			database_workload workload{};
			workload.workloadType = database_workload_type::Discord_Guild_Write;
			workload.guildData = this->data;
			other.submitWorkloadAndGetResults(workload);
		}

		inline void getDataFromDB(database_manager_agent& other) {
			database_workload workload{};
			workload.workloadType = database_workload_type::Discord_Guild_Read;
			workload.guildData = this->data;
			auto result = other.submitWorkloadAndGetResults(workload);
			if (result.discordGuild.guildId != 0) {
				this->data = result.discordGuild;
			}
		}
	};

	class discord_guild_member {
	  public:
		discord_guild_member_data data{};

		inline discord_guild_member() noexcept = default;

		inline discord_guild_member(database_manager_agent& other, guild_member_cache_data guildMemberData) {
			this->data.guildMemberId = guildMemberData.user.id;
			this->data.guildId = guildMemberData.guildId.operator const uint64_t&();
			this->data.globalId = this->data.guildId.operator jsonifier::string() + " + " + this->data.guildMemberId;
			this->getDataFromDB(other);
			if (guildMemberData.nick == "") {
				this->data.displayName = guildMemberData.getUserData().userName;
				this->data.guildMemberMention = "<@" + this->data.guildMemberId + ">";
			} else {
				this->data.displayName = guildMemberData.nick;
				this->data.guildMemberMention = "<@!" + this->data.guildMemberId + ">";
			}
			this->data.userName = guildMemberData.getUserData().userName;
		}

		inline void writeDataToDB(database_manager_agent& other) {
			database_workload workload{};
			workload.workloadType = database_workload_type::Discord_Guild_Member_Write;
			workload.guildMemberData = this->data;
			other.submitWorkloadAndGetResults(workload);
		}

		inline void getDataFromDB(database_manager_agent& other) {
			database_workload workload{};
			workload.workloadType = database_workload_type::Discord_Guild_Member_Read;
			workload.guildMemberData = this->data;
			auto result = other.submitWorkloadAndGetResults(workload);
			if (result.discordGuildMember.globalId != "") {
				this->data = result.discordGuildMember;
			}
		}
	};


	static discord_core_api::database_manager_agent managerAgent{};
};