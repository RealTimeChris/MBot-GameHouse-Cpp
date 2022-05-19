// DatabaseEntities.hpp - Database stuff.
// May 24, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
	#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>

#include <discordcoreapi/Index.hpp>

namespace DiscordCoreAPI {

	struct DiscordUserData {
		std::vector<uint64_t> botCommanders{ 0, 0, 0 };
		float hoursOfRobberyCooldown{ 0.100f };
		int32_t hoursOfDrugSaleCooldown{ 3 };
		int32_t hoursOfDepositCooldown{ 24 };
		std::string currencyName{ "MBux" };
		std::string userName{ "" };
		uint64_t userId{ 0 };
	};

	struct Card {
		uint32_t value{ 0 };
		std::string suit{ "" };
		std::string type{ "" };
	};

	class Deck {
	  public:
		std::vector<Card> cards{};

		Deck() {
			this->cards.resize(52);

			for (auto x = 0; x < 52; x += 1) {
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
					this->cards[x].type = "Ace";
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
					this->cards[x].type = "Jack";
					this->cards[x].value = 10;
				} else if (x % 13 == 11) {
					this->cards[x].type = "Queen";
					this->cards[x].value = 10;
				} else if (x % 13 == 12) {
					this->cards[x].type = "King";
					this->cards[x].value = 10;
				}
			}
		}

		// Draws a random card from the Deck.
		Card drawRandomcard() {
			if (this->cards.size() == 0) {
				Card voidCard{
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
			uint32_t cardIndex =
				static_cast<uint32_t>(static_cast<float>(randomEngine()) / static_cast<float>(randomEngine.max()) * static_cast<float>(this->cards.size()));
			Card currentCard = this->cards.at(cardIndex);
			this->cards.erase(this->cards.begin() + cardIndex);
			return currentCard;
		}
	};

	struct RouletteBet {
		std::vector<std::string> winningNumbers{};
		std::string betOptions{ "" };
		uint32_t payoutAmount{ 0 };
		std::string betType{ "" };
		uint32_t betAmount{ 0 };
		uint64_t userId{ 0 };
	};

	struct Roulette {
		std::vector<RouletteBet> rouletteBets{};
		bool currentlySpinning{ false };
	};

	struct LargestPayout {
		std::string timeStamp{ "" };
		std::string userName{ "" };
		uint64_t userId{ 0 };
		int32_t amount{ 0 };
	};

	struct CasinoStats {
		LargestPayout largestBlackjackPayout{};
		LargestPayout largestCoinFlipPayout{};
		LargestPayout largestRoulettePayout{};
		LargestPayout largestSlotsPayout{};
		int32_t totalBlackjackPayout{ 0 };
		int32_t totalCoinFlipPayout{ 0 };
		int32_t totalRoulettePayout{ 0 };
		int32_t totalSlotsPayout{ 0 };
		int32_t totalPayout{ 0 };
	};

	struct InventoryItem {
		uint32_t itemCost{ 0 };
		uint32_t selfMod{ 0 };
		std::string itemName{ "" };
		std::string emoji{ "" };
		int32_t oppMod{ 0 };
	};

	struct InventoryRole {
		uint32_t roleCost{ 0 };
		std::string roleName{ "" };
		uint64_t roleId{ 0 };
	};

	struct GuildShop {
		std::vector<InventoryItem> items{};
		std::vector<InventoryRole> roles{};
	};

	struct Currency {
		uint32_t timeOfLastDeposit{ 0 };
		uint32_t wallet{ 10000 };
		uint32_t bank{ 10000 };
	};

	struct DiscordGuildData {
		std::vector<uint64_t> gameChannelIds{};
		std::string borderColor{ "FEFEFE" };
		std::vector<Card> blackjackStack{};
		std::string guildName{ "" };
		uint32_t memberCount{ 0 };
		CasinoStats casinoStats{};
		Roulette rouletteGame{};
		uint64_t guildId{ 0 };
		GuildShop guildShop{};
	};

	struct DiscordGuildMemberData {
		uint32_t lastTimeRobbed{ 0 };
		uint32_t lastTimeWorked{ 0 };
		std::string guildMemberMention{ "" };
		std::vector<InventoryItem> items{};
		std::vector<InventoryRole> roles{};
		uint64_t guildMemberId{ 0 };
		std::string displayName{ "" };
		std::string globalId{ "" };
		std::string userName{ "" };
		uint64_t guildId{ 0 };
		Currency currency{};
	};

	enum class DatabaseWorkloadType {
		DISCORD_USER_WRITE = 0,
		DISCORD_USER_READ = 1,
		DISCORD_GUILD_WRITE = 2,
		DISCORD_GUILD_READ = 3,
		DISCORD_GUILD_MEMBER_WRITE = 4,
		DISCORD_GUILD_MEMBER_READ = 5
	};

	struct DatabaseWorkload {
		DiscordGuildMemberData guildMemberData{};
		DatabaseWorkloadType workloadType{};
		DiscordGuildData guildData{};
		DiscordUserData userData{};
		std::string guildMemberId{ "" };
		std::string globalId{ "" };
		std::string guildId{ "" };
	};

	struct DatabaseReturnValue {
		DiscordGuildMemberData discordGuildMember{};
		DiscordGuildData discordGuild{};
		DiscordUserData discordUser{};
	};

	class DatabaseManagerAgent {
	  public:
		static void initialize(std::string botUserIdNew) {
			DatabaseManagerAgent::botUserId = botUserIdNew;
			auto newClient = DatabaseManagerAgent::getClient();
			mongocxx::database newDataBase = (*newClient)[DatabaseManagerAgent::botUserId];
			mongocxx::collection newCollection = newDataBase[DatabaseManagerAgent::botUserId];
		}

		static mongocxx::pool::entry getClient() {
			return DatabaseManagerAgent::thePool.acquire();
		}

		static DatabaseReturnValue submitWorkloadAndGetResults(DatabaseWorkload workload) {
			std::lock_guard<std::mutex> workloadLock{ DatabaseManagerAgent::workloadMutex };
			DatabaseReturnValue newData{};
			try {
				mongocxx::pool::entry thePtr = DatabaseManagerAgent::getClient();
				auto newDataBase = (*thePtr)[DatabaseManagerAgent::botUserId];
				auto newCollection = newDataBase[DatabaseManagerAgent::botUserId];
				switch (workload.workloadType) {
					case (DatabaseWorkloadType::DISCORD_USER_WRITE): {
						auto doc = DatabaseManagerAgent::convertUserDataToDBDoc(workload.userData);
						bsoncxx::builder::basic::document document{};
						document.append(bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int64(workload.userData.userId)));
						auto resultNew = newCollection.find_one(document.view());
						auto resultNewer = newCollection.find_one_and_replace(document.view(), std::move(doc.extract()),
							mongocxx::v_noabi::options::find_one_and_replace{}.return_document(mongocxx::v_noabi::options::return_document::k_after));
						if (resultNewer.get_ptr() == NULL) {
							auto doc02 = DatabaseManagerAgent::convertUserDataToDBDoc(workload.userData);
							newCollection.insert_one(std::move(doc02.extract()));
							return newData;
						}
						break;
					}
					case (DatabaseWorkloadType::DISCORD_USER_READ): {
						bsoncxx::builder::basic::document document{};
						document.append(bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int64(workload.userData.userId)));
						auto resultNew = newCollection.find_one(document.view());
						if (resultNew.get_ptr() != NULL) {
							DiscordUserData userData = DatabaseManagerAgent::parseUserData(*resultNew.get_ptr());
							newData.discordUser = userData;
							return newData;
						} else {
							return newData;
						}
						break;
					}
					case (DatabaseWorkloadType::DISCORD_GUILD_WRITE): {
						auto doc = DatabaseManagerAgent::convertGuildDataToDBDoc(workload.guildData);
						bsoncxx::builder::basic::document document{};
						document.append(bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int64(workload.guildData.guildId)));
						auto resultNewer = newCollection.find_one_and_replace(document.view(), std::move(doc.extract()),
							mongocxx::v_noabi::options::find_one_and_replace{}.return_document(mongocxx::v_noabi::options::return_document::k_after));
						if (resultNewer.get_ptr() == NULL) {
							auto doc02 = DatabaseManagerAgent::convertGuildDataToDBDoc(workload.guildData);
							newCollection.insert_one(std::move(doc02.extract()));
							return newData;
						}
						break;
					}
					case (DatabaseWorkloadType::DISCORD_GUILD_READ): {
						bsoncxx::builder::basic::document document{};
						document.append(bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_int64(workload.guildData.guildId)));
						auto resultNew = newCollection.find_one(document.view());
						if (resultNew.get_ptr() != NULL) {
							DiscordGuildData guildData = DatabaseManagerAgent::parseGuildData(*resultNew.get_ptr());
							newData.discordGuild = guildData;
							return newData;
						} else {
							return newData;
						}
						break;
					}
					case (DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE): {
						auto doc = DatabaseManagerAgent::convertGuildMemberDataToDBDoc(workload.guildMemberData);
						bsoncxx::builder::basic::document document{};
						document.append(bsoncxx::builder::basic::kvp("_id", workload.guildMemberData.globalId.c_str()));
						auto resultNewer = newCollection.find_one_and_replace(document.view(), std::move(doc.extract()),
							mongocxx::v_noabi::options::find_one_and_replace{}.return_document(mongocxx::v_noabi::options::return_document::k_after));
						if (resultNewer.get_ptr() == NULL) {
							auto doc02 = DatabaseManagerAgent::convertGuildMemberDataToDBDoc(workload.guildMemberData);
							newCollection.insert_one(std::move(doc02.extract()));
							return newData;
						}
						break;
					}
					case (DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ): {
						bsoncxx::builder::basic::document document{};
						document.append(bsoncxx::builder::basic::kvp("_id", workload.guildMemberData.globalId.c_str()));
						auto resultNew = newCollection.find_one(document.view());
						if (resultNew.get_ptr() != NULL) {
							DiscordGuildMemberData guildMemberData = DatabaseManagerAgent::parseGuildMemberData(*resultNew.get_ptr());
							newData.discordGuildMember = guildMemberData;
							return newData;
						} else {
							return newData;
						}
						break;
					}
				}
				return newData;
			} catch (...) {
				reportException("DatabaseManagerAgent::run()");
				return newData;
			}
		}
		
	  protected:
		static mongocxx::instance instance;
		static std::mutex workloadMutex;
		static mongocxx::pool thePool;
		static std::string botUserId;

		static bsoncxx::builder::basic::document convertUserDataToDBDoc(DiscordUserData discordUserData) {
			bsoncxx::builder::basic::document buildDoc;
			try {
				using bsoncxx::builder::basic::kvp;
				buildDoc.append(kvp("_id", bsoncxx::types::b_int64(discordUserData.userId)));
				buildDoc.append(kvp("userId", bsoncxx::types::b_int64(discordUserData.userId)));
				buildDoc.append(kvp("userName", discordUserData.userName.c_str()));
				buildDoc.append(kvp("currencyName", discordUserData.currencyName.c_str()));
				buildDoc.append(kvp("hoursOfDepositCooldown", bsoncxx::types::b_int32(discordUserData.hoursOfDepositCooldown)));
				buildDoc.append(kvp("hoursOfDrugSaleCooldown", bsoncxx::types::b_int32(discordUserData.hoursOfDrugSaleCooldown)));
				buildDoc.append(kvp("hoursOfRobberyCooldown", bsoncxx::types::b_double(discordUserData.hoursOfRobberyCooldown)));
				buildDoc.append(kvp("botCommanders", [discordUserData](bsoncxx::builder::basic::sub_array subArray) {
					for (auto& value: discordUserData.botCommanders) {
						subArray.append(bsoncxx::types::b_int64(value));
					}
				}));
				return buildDoc;
			} catch (...) {
				reportException("DatabaseManagerAgent::convertUserDataToDBDoc()");
				return buildDoc;
			}
		}

		static DiscordUserData parseUserData(bsoncxx::document::value docValue) {
			DiscordUserData userData{};
			try {
				userData.userName = docValue.view()["userName"].get_utf8().value.to_string();
				userData.currencyName = docValue.view()["currencyName"].get_utf8().value.to_string();
				userData.hoursOfDepositCooldown = docValue.view()["hoursOfDepositCooldown"].get_int32();
				userData.hoursOfDrugSaleCooldown = docValue.view()["hoursOfDrugSaleCooldown"].get_int32();
				userData.hoursOfRobberyCooldown = ( float )docValue.view()["hoursOfRobberyCooldown"].get_double();
				userData.userId = docValue.view()["userId"].get_int64().value;
				auto botCommandersArray = docValue.view()["botCommanders"].get_array();
				std::vector<uint64_t> newVector;
				for (const auto& value: botCommandersArray.value) {
					newVector.push_back(value.get_int64().value);
				}
				userData.botCommanders = newVector;
				return userData;
			} catch (...) {
				reportException("DatabaseManagerAgent::parseUserData()");
				return userData;
			}
		}

		static bsoncxx::builder::basic::document convertGuildDataToDBDoc(DiscordGuildData discordGuildData) {
			bsoncxx::builder::basic::document buildDoc;
			try {
				using bsoncxx::builder::basic::kvp;
				buildDoc.append(kvp("_id", bsoncxx::types::b_int64(discordGuildData.guildId)));
				buildDoc.append(kvp("guildId", bsoncxx::types::b_int64(discordGuildData.guildId)));
				buildDoc.append(kvp("guildName", discordGuildData.guildName.c_str()));
				buildDoc.append(kvp("memberCount", bsoncxx::types::b_int32(discordGuildData.memberCount)));
				buildDoc.append(kvp("blackjackStack", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
					for (auto& value: discordGuildData.blackjackStack) {
						subArray.append([value](bsoncxx::builder::basic::sub_document subDocument) {
							subDocument.append(kvp("suit", value.suit.c_str()), kvp("type", value.type.c_str()), kvp("value", bsoncxx::types::b_int32(value.value)));
						});
					}
				}));
				buildDoc.append(kvp("borderColor", discordGuildData.borderColor));
				buildDoc.append(kvp("gameChannelIds", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
					for (auto& value: discordGuildData.gameChannelIds) {
						subArray.append(bsoncxx::types::b_int64(value));
					}
				}));
				buildDoc.append(kvp("guildShop", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
					subDocument.append(kvp("items",
						[discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
							for (const auto& value: discordGuildData.guildShop.items) {
								subArray.append([value](bsoncxx::builder::basic::sub_document subDocument2) {
									subDocument2.append(kvp("itemName", value.itemName.c_str()), kvp("itemCost", bsoncxx::types::b_int32(value.itemCost)),
										kvp("selfMod", bsoncxx::types::b_int32(value.selfMod)), kvp("oppMod", bsoncxx::types::b_int32(value.oppMod)),
										kvp("emoji", value.emoji.c_str()));
								});
							}
						})),
						subDocument.append(kvp("roles", [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
							for (const auto& value: discordGuildData.guildShop.roles) {
								subArray.append([value](bsoncxx::builder::basic::sub_document subDocument2) {
									subDocument2.append(kvp("roleName", value.roleName.c_str()));
									subDocument2.append(kvp("roleId", bsoncxx::types::b_int64(value.roleId)));
									subDocument2.append(kvp("roleCost", bsoncxx::types::b_int32(value.roleCost)));
								});
							}
						}));
				}));
				buildDoc.append(kvp("casinoStats", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
					subDocument.append(kvp("largestBlackjackPayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
						subDoc2.append(kvp("amount", bsoncxx::types::b_int32(discordGuildData.casinoStats.largestBlackjackPayout.amount)));
						subDoc2.append(kvp("userId", bsoncxx::types::b_int64(discordGuildData.casinoStats.largestBlackjackPayout.userId)));
						subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestBlackjackPayout.timeStamp.c_str()));
						subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestBlackjackPayout.userName.c_str()));
					}));
					subDocument.append(kvp("totalBlackjackPayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalBlackjackPayout)));
					subDocument.append(kvp("largestCoinFlipPayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
						subDoc2.append(kvp("amount", bsoncxx::types::b_int32(discordGuildData.casinoStats.largestCoinFlipPayout.amount)));
						subDoc2.append(kvp("userId", bsoncxx::types::b_int64(discordGuildData.casinoStats.largestCoinFlipPayout.userId)));
						subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestCoinFlipPayout.timeStamp.c_str()));
						subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestCoinFlipPayout.userName.c_str()));
					}));
					subDocument.append(kvp("totalCoinFlipPayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalCoinFlipPayout)));
					subDocument.append(kvp("largestRoulettePayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
						subDoc2.append(kvp("amount", bsoncxx::types::b_int32(discordGuildData.casinoStats.largestRoulettePayout.amount)));
						subDoc2.append(kvp("userId", bsoncxx::types::b_int64(discordGuildData.casinoStats.largestRoulettePayout.userId)));
						subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestRoulettePayout.timeStamp.c_str()));
						subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestRoulettePayout.userName.c_str()));
					}));
					subDocument.append(kvp("totalRoulettePayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalRoulettePayout)));
					subDocument.append(kvp("largestSlotsPayout", [discordGuildData](bsoncxx::builder::basic::sub_document subDoc2) {
						subDoc2.append(kvp("amount", bsoncxx::types::b_int32(discordGuildData.casinoStats.largestSlotsPayout.amount)));
						subDoc2.append(kvp("userId", bsoncxx::types::b_int64(discordGuildData.casinoStats.largestSlotsPayout.userId)));
						subDoc2.append(kvp("timeStamp", discordGuildData.casinoStats.largestSlotsPayout.timeStamp.c_str()));
						subDoc2.append(kvp("userName", discordGuildData.casinoStats.largestSlotsPayout.userName.c_str()));
					}));
					subDocument.append(kvp("totalSlotsPayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalSlotsPayout)));
					subDocument.append(kvp("totalPayout", bsoncxx::types::b_int32(discordGuildData.casinoStats.totalPayout)));
				}));
				buildDoc.append(kvp("rouletteGame", [discordGuildData](bsoncxx::builder::basic::sub_document subDocument) {
					subDocument.append(kvp("rouletteBets",
										   [discordGuildData](bsoncxx::builder::basic::sub_array subArray) {
											   for (auto& value: discordGuildData.rouletteGame.rouletteBets) {
												   subArray.append([value](bsoncxx::builder::basic::sub_document subDoc2) {
													   subDoc2.append(kvp("betAmount", bsoncxx::types::b_int32(value.betAmount)));
													   subDoc2.append(kvp("betOptions", value.betOptions.c_str()));
													   subDoc2.append(kvp("betType", value.betType.c_str()));
													   subDoc2.append(kvp("userId", bsoncxx::types::b_int64(value.userId)));
													   subDoc2.append(kvp("payoutAmount", bsoncxx::types::b_int32(value.payoutAmount)));
													   subDoc2.append(kvp("winningNumbers", [value](bsoncxx::builder::basic::sub_array subArray2) {
														   for (auto& value2: value.winningNumbers) {
															   subArray2.append(value2.c_str());
														   }
													   }));
												   });
											   }
										   }),
						kvp("currentlySpinning", bsoncxx::types::b_bool(discordGuildData.rouletteGame.currentlySpinning)));
				}));
				return buildDoc;
			} catch (...) {
				reportException("DatabaseManagerAgent::convertGuildDataToDBDoc()");
				return buildDoc;
			}
		};

		static DiscordGuildData parseGuildData(bsoncxx::document::value docValue) {
			DiscordGuildData guildData{};
			try {
				guildData.guildId = docValue.view()["guildId"].get_int64().value;
				guildData.guildName = docValue.view()["guildName"].get_utf8().value.to_string();
				guildData.memberCount = docValue.view()["memberCount"].get_int32().value;
				for (auto& value: docValue.view()["blackjackStack"].get_array().value) {
					Card blackjackCard;
					blackjackCard.suit = value.get_document().view()["suit"].get_utf8().value.to_string();
					blackjackCard.type = value.get_document().view()["type"].get_utf8().value.to_string();
					blackjackCard.value = value.get_document().view()["value"].get_int32().value;
					guildData.blackjackStack.push_back(blackjackCard);
				}
				guildData.borderColor = docValue.view()["borderColor"].get_utf8().value.to_string();
				for (auto& value: docValue.view()["gameChannelIds"].get_array().value) {
					guildData.gameChannelIds.push_back(value.get_int64().value);
				}
				for (auto& value: docValue.view()["guildShop"].get_document().value["items"].get_array().value) {
					InventoryItem item;
					item.emoji = value["emoji"].get_utf8().value.to_string();
					item.itemCost = value["itemCost"].get_int32().value;
					item.itemName = value["itemName"].get_utf8().value.to_string();
					item.oppMod = value["oppMod"].get_int32().value;
					item.selfMod = value["selfMod"].get_int32().value;
					guildData.guildShop.items.push_back(item);
				}
				for (auto& value: docValue.view()["guildShop"].get_document().value["roles"].get_array().value) {
					InventoryRole role;
					role.roleCost = value["roleCost"].get_int32().value;
					role.roleId = value["roleId"].get_int64().value;
					role.roleName = value["roleName"].get_utf8().value.to_string();
					guildData.guildShop.roles.push_back(role);
				}
				auto largestBlackjackPayout = docValue.view()["casinoStats"].get_document().value["largestBlackjackPayout"].get_document();
				guildData.casinoStats.largestBlackjackPayout.amount = largestBlackjackPayout.value["amount"].get_int32().value;
				guildData.casinoStats.largestBlackjackPayout.timeStamp = largestBlackjackPayout.value["timeStamp"].get_utf8().value.to_string();
				guildData.casinoStats.largestBlackjackPayout.userId = largestBlackjackPayout.value["userId"].get_int64().value;
				guildData.casinoStats.largestBlackjackPayout.userName = largestBlackjackPayout.value["userName"].get_utf8().value.to_string();
				guildData.casinoStats.totalBlackjackPayout = docValue.view()["casinoStats"].get_document().value["totalBlackjackPayout"].get_int32().value;
				auto largestCoinflipPayout = docValue.view()["casinoStats"].get_document().value["largestCoinFlipPayout"].get_document();
				guildData.casinoStats.largestCoinFlipPayout.amount = largestCoinflipPayout.value["amount"].get_int32().value;
				guildData.casinoStats.largestCoinFlipPayout.timeStamp = largestCoinflipPayout.value["timeStamp"].get_utf8().value.to_string();
				guildData.casinoStats.largestCoinFlipPayout.userId = largestCoinflipPayout.value["userId"].get_int64().value;
				guildData.casinoStats.largestCoinFlipPayout.userName = largestCoinflipPayout.value["userName"].get_utf8().value.to_string();
				guildData.casinoStats.totalCoinFlipPayout = docValue.view()["casinoStats"].get_document().value["totalCoinFlipPayout"].get_int32().value;
				auto largestRoulettePayout = docValue.view()["casinoStats"].get_document().value["largestRoulettePayout"].get_document();
				guildData.casinoStats.largestRoulettePayout.amount = largestRoulettePayout.value["amount"].get_int32().value;
				guildData.casinoStats.largestRoulettePayout.timeStamp = largestRoulettePayout.value["timeStamp"].get_utf8().value.to_string();
				guildData.casinoStats.largestRoulettePayout.userId = largestRoulettePayout.value["userId"].get_int64().value;
				guildData.casinoStats.largestRoulettePayout.userName = largestRoulettePayout.value["userName"].get_utf8().value.to_string();
				guildData.casinoStats.totalRoulettePayout = docValue.view()["casinoStats"].get_document().value["totalRoulettePayout"].get_int32().value;
				auto largestSlotsPayout = docValue.view()["casinoStats"].get_document().value["largestSlotsPayout"].get_document();
				guildData.casinoStats.largestSlotsPayout.amount = largestSlotsPayout.value["amount"].get_int32().value;
				guildData.casinoStats.largestSlotsPayout.timeStamp = largestSlotsPayout.value["timeStamp"].get_utf8().value.to_string();
				guildData.casinoStats.largestSlotsPayout.userId = largestSlotsPayout.value["userId"].get_int64().value;				
				guildData.casinoStats.largestSlotsPayout.userName = largestSlotsPayout.value["userName"].get_utf8().value.to_string();
				guildData.casinoStats.totalSlotsPayout = docValue.view()["casinoStats"].get_document().value["totalSlotsPayout"].get_int32().value;
				guildData.casinoStats.totalPayout = docValue.view()["casinoStats"].get_document().value["totalPayout"].get_int32().value;
				guildData.rouletteGame.currentlySpinning = docValue.view()["rouletteGame"].get_document().value["currentlySpinning"].get_bool().value;
				for (auto& value: docValue.view()["rouletteGame"].get_document().value["rouletteBets"].get_array().value) {
					RouletteBet rouletteBet{};
					rouletteBet.betAmount = value.get_document().value["betAmount"].get_int32().value;
					rouletteBet.betOptions = value.get_document().value["betOptions"].get_utf8().value.to_string();
					rouletteBet.betType = value.get_document().value["betType"].get_utf8().value.to_string();
					rouletteBet.payoutAmount = value.get_document().value["payoutAmount"].get_int32().value;
					rouletteBet.userId = value.get_document().value["userId"].get_int64().value;
					for (auto& value2: value.get_document().value["winningNumbers"].get_array().value) {
						rouletteBet.winningNumbers.push_back(value2.get_utf8().value.to_string());
					}
					guildData.rouletteGame.rouletteBets.push_back(rouletteBet);
				}
				return guildData;
			} catch (...) {
				reportException("DatabaseManagerAgent::parseGuildData()");
				return guildData;
			}
		};

		static bsoncxx::builder::basic::document convertGuildMemberDataToDBDoc(DiscordGuildMemberData discordGuildMemberData) {
			bsoncxx::builder::basic::document buildDoc;
			try {
				using bsoncxx::builder::basic::kvp;
				buildDoc.append(kvp("guildMemberMention", discordGuildMemberData.guildMemberMention.c_str()));
				buildDoc.append(kvp("_id", discordGuildMemberData.globalId.c_str()));
				buildDoc.append(kvp("guildId", bsoncxx::types::b_int64(discordGuildMemberData.guildId)));
				buildDoc.append(kvp("guildMemberId", bsoncxx::types::b_int64(discordGuildMemberData.guildMemberId)));
				buildDoc.append(kvp("globalId", discordGuildMemberData.globalId.c_str()));
				buildDoc.append(kvp("userName", discordGuildMemberData.userName.c_str()));
				buildDoc.append(kvp("displayName", discordGuildMemberData.displayName.c_str()));
				buildDoc.append(kvp("lastTimeRobbed", bsoncxx::types::b_int32(discordGuildMemberData.lastTimeRobbed)));
				buildDoc.append(kvp("lastTimeWorked", bsoncxx::types::b_int32(discordGuildMemberData.lastTimeWorked)));
				buildDoc.append(kvp("currency", [discordGuildMemberData](bsoncxx::builder::basic::sub_document subDocument) {
					subDocument.append(kvp("bank", bsoncxx::types::b_int32(discordGuildMemberData.currency.bank)),
						kvp("wallet", bsoncxx::types::b_int32(discordGuildMemberData.currency.wallet)),
						kvp("timeOfLastDeposit", bsoncxx::types::b_int32(discordGuildMemberData.currency.timeOfLastDeposit)));
				}));
				buildDoc.append(kvp("items",
					[discordGuildMemberData](bsoncxx::builder::basic::sub_array subArray) {
						for (uint32_t x = 0; x < discordGuildMemberData.items.size(); x += 1) {
							subArray.append([discordGuildMemberData, x](bsoncxx::builder::basic::sub_document subDocument) {
								subDocument.append(kvp("itemName", discordGuildMemberData.items.at(x).itemName.c_str()),
									kvp("itemCost", bsoncxx::types::b_int32(discordGuildMemberData.items.at(x).itemCost)),
									kvp("selfMod", bsoncxx::types::b_int32(discordGuildMemberData.items.at(x).selfMod)),
									kvp("oppMod", bsoncxx::types::b_int32(discordGuildMemberData.items.at(x).oppMod)),
									kvp("emoji", discordGuildMemberData.items.at(x).emoji.c_str()));
							});
						}
					})),
					buildDoc.append(kvp("roles", [discordGuildMemberData](bsoncxx::builder::basic::sub_array subArray) {
						for (uint32_t x = 0; x < discordGuildMemberData.roles.size(); x += 1) {
							subArray.append([discordGuildMemberData, x](bsoncxx::builder::basic::sub_document subDocument) {
								subDocument.append(kvp("roleName", discordGuildMemberData.roles.at(x).roleName.c_str()),
									kvp("roleId", bsoncxx::types::b_int64(discordGuildMemberData.roles.at(x).roleId)),
									kvp("roleCost", bsoncxx::types::b_int32(discordGuildMemberData.roles.at(x).roleCost)));
							});
						}
					}));

				return buildDoc;
			} catch (...) {
				reportException("DatabaseManagerAgent::convertGuildMemberDataToDBDoc()");
				return buildDoc;
			}
		};

		static DiscordGuildMemberData parseGuildMemberData(bsoncxx::document::value docValue) {
			DiscordGuildMemberData guildMemberData{};
			try {
				guildMemberData.guildMemberMention = docValue.view()["guildMemberMention"].get_utf8().value.to_string();
				guildMemberData.guildId = docValue.view()["guildId"].get_int64().value;				
				guildMemberData.displayName = docValue.view()["displayName"].get_utf8().value.to_string();
				guildMemberData.globalId = docValue.view()["globalId"].get_utf8().value.to_string();
				guildMemberData.guildMemberId = docValue.view()["guildMemberId"].get_int64().value;				
				guildMemberData.userName = docValue.view()["userName"].get_utf8().value.to_string();
				guildMemberData.lastTimeWorked = docValue.view()["lastTimeWorked"].get_int32().value;
				guildMemberData.lastTimeRobbed = docValue.view()["lastTimeRobbed"].get_int32().value;
				guildMemberData.currency.bank = docValue.view()["currency"].get_document().value["bank"].get_int32().value;
				guildMemberData.currency.wallet = docValue.view()["currency"].get_document().value["wallet"].get_int32().value;
				guildMemberData.currency.timeOfLastDeposit = docValue.view()["currency"].get_document().value["timeOfLastDeposit"].get_int32().value;
				for (auto& value: docValue.view()["items"].get_array().value) {
					InventoryItem item;
					item.emoji = value["emoji"].get_utf8().value.to_string();
					item.itemName = value["itemName"].get_utf8().value.to_string();
					item.itemCost = value["itemCost"].get_int32().value;
					item.oppMod = value["oppMod"].get_int32().value;
					item.selfMod = value["selfMod"].get_int32().value;
					guildMemberData.items.push_back(item);
				}
				for (auto& value: docValue.view()["roles"].get_array().value) {
					InventoryRole role;
					role.roleCost = value["roleCost"].get_int32().value;
					role.roleId = value["roleId"].get_int64().value;
					role.roleName = value["roleName"].get_utf8().value.to_string();
					guildMemberData.roles.push_back(role);
				}
				return guildMemberData;
			} catch (...) {
				reportException("DatabaseManagerAgent::parseGuildMemberData()");
				return guildMemberData;
			}
		}
	};

	class DiscordUser {
	  public:
		static int32_t guildCount;

		DiscordUserData data{};

		DiscordUser(std::string userNameNew, uint64_t userIdNew) {
			this->data.userId = userIdNew;
			this->data.userName = userNameNew;
			this->getDataFromDB();
			this->data.userId = userIdNew;
			this->data.userName = userNameNew;
			this->writeDataToDB();
		}

		void writeDataToDB() {
			DatabaseWorkload workload{};
			workload.workloadType = DatabaseWorkloadType::DISCORD_USER_WRITE;
			workload.userData = this->data;
			DatabaseManagerAgent::submitWorkloadAndGetResults(workload);
		}

		void getDataFromDB() {
			DatabaseWorkload workload{};
			workload.workloadType = DatabaseWorkloadType::DISCORD_USER_READ;
			workload.userData = this->data;
			auto result = DatabaseManagerAgent::submitWorkloadAndGetResults(workload);
			if (result.discordUser.userId != 0) {
				this->data = result.discordUser;
			}
		}
	};

	class DiscordGuild {
	  public:
		DiscordGuildData data{};

		DiscordGuild(Guild guildData) {
			this->data.guildId = guildData.id;
			this->data.guildName = guildData.name;
			this->data.memberCount = guildData.memberCount;
			this->getDataFromDB();
			this->data.guildId = guildData.id;
			this->data.guildName = guildData.name;
			this->data.memberCount = guildData.memberCount;
			this->writeDataToDB();
		}

		void writeDataToDB() {
			DatabaseWorkload workload{};
			workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_WRITE;
			workload.guildData = this->data;
			DatabaseManagerAgent::submitWorkloadAndGetResults(workload);
		}

		void getDataFromDB() {
			DatabaseWorkload workload{};
			workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_READ;
			workload.guildData = this->data;
			auto result = DatabaseManagerAgent::submitWorkloadAndGetResults(workload);
			if (result.discordGuild.guildId != 0) {
				this->data = result.discordGuild;
			}
		}
	};

	class DiscordGuildMember {
	  public:
		DiscordGuildMemberData data{};

		DiscordGuildMember(GuildMember guildMemberData) {
			this->data.guildMemberId = guildMemberData.user.id;
			this->data.guildId = guildMemberData.guildId;
			this->data.globalId = std::to_string(this->data.guildId) + " + " + std::to_string(this->data.guildMemberId);
			this->getDataFromDB();
			if (guildMemberData.nick == "") {
				this->data.displayName = guildMemberData.user.userName;
				this->data.guildMemberMention = "<@" + std::to_string(this->data.guildMemberId) + ">";
			} else {
				this->data.displayName = guildMemberData.nick;
				this->data.guildMemberMention = "<@!" + std::to_string(this->data.guildMemberId) + ">";
			}
			this->data.userName = guildMemberData.user.userName;
			this->writeDataToDB();
		}

		void writeDataToDB() {
			DatabaseWorkload workload{};
			workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_WRITE;
			workload.guildMemberData = this->data;
			DatabaseManagerAgent::submitWorkloadAndGetResults(workload);
		}

		void getDataFromDB() {
			DatabaseWorkload workload{};
			workload.workloadType = DatabaseWorkloadType::DISCORD_GUILD_MEMBER_READ;
			workload.guildMemberData = this->data;
			auto result = DatabaseManagerAgent::submitWorkloadAndGetResults(workload);
			if (result.discordGuildMember.globalId != "") {
				this->data = result.discordGuildMember;
			}
		}
	};

	mongocxx::instance DatabaseManagerAgent::instance{};
	mongocxx::pool DatabaseManagerAgent::thePool{ mongocxx::uri{} };
	std::string DatabaseManagerAgent::botUserId{ "" };
	std::mutex DatabaseManagerAgent::workloadMutex{};
	int32_t DiscordUser::guildCount{ 0 };

}