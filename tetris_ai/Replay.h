#pragma once
#ifndef REPLAY_H
#define REPLAY_H
#include "json.hpp"
#include "tetrisgame.h"
#include <string>
#include <vector>
#include <fstream>

namespace RP {
	using json = nlohmann::json;


	enum {
		FULL,
		KEYDOWN,
		KEYUP,
		START,
		TARGETS,
		IGE,
		IGE_C,
		END
	};
	enum {
		moveLeft,
		moveRight,
		hold,
		hardDrop,
		rotateCCW, 
		rotateCW,
		rotate180,
		softDrop
	};
	std::string key[8] = {
		"moveLeft",
		"moveRight",
		"hold",
		"hardDrop",
		"rotateCCW",
		"rotateCW",
		"rotate180",
		"softDrop"
	};
	/*
	* frame type data
	FULL > data options and some more
	START > data empty
	TARGET> data id diyusi frane 0 type targets
	key up/down > data key subframe
	ige > data type interaction/interaction_confirm data {type=garbage amt x? y? column0-9}sent_frame cid:number
	end > data reaseon topout/winner export{aggregatestats apm pps vsscore}

	*/
	typedef struct {
		int frame;
		double subframe;
	} frame_t;
	class playerRecord
	{
		int cid = 0;
		int id = 0;
		json info;
		json evt;
		json handling;
		json board;
		json options;
		json endGameStat;
		int totalFrames;
	public:
		playerRecord() {
			info = {
				{"user",{{"_id",0},{"username",""}}},
				{"success",false}
			};
			evt["events"] = json::array();
			totalFrames = 0;
			board = json::array();
			int row[10] = { 0 };
			for (int i = 0; i < 40; i++) {
				board.push_back(row);
			}
		}
		void reset() {
			evt.clear();
			options.clear();
			endGameStat.clear();
			totalFrames = 0;
		}
		// no need to reset
		void setUSer(std::string username, int id = 0) {
			info["user"]["username"] = username;
			info["user"]["_id"] = id;
		}
		// no need to reset
		void setHandling(int arr, int das, int sdf) {
			handling = {
				{"arr",arr},
				{"das",das},
				{"dcd",0},
				{"sdf",sdf * 2},
				{"safelock",true},
				{"cancel",true}
			};
		}
		void setWin(bool win) {
			info["success"] = win;
		}
		void setEndGameStat(int totalFrame, double apm, double pps, double vs) {
			endGameStat["apm"] = apm;
			endGameStat["pps"] = pps;
			endGameStat["vsscore"] = vs;
			evt["frames"] = totalFrame;
		}
		void setOption(unsigned seed, int cap) {
			options = {
				{"version",15},
				{"seed_random",false},
				{"seed",seed},
				{"g",0},
				{"stock",0},
				{"countdown",true},
				{"countdown_count",3},
				{"countdonw_interval",1000},
				{"precountdown",5000},
				{"prestart",1000},
				{"zoominto","slow"},
				{"slot_counter1","stopwatch"},
				{"slot_counter2","attack"},
				{"slot_counter3","pieces"},
				{"slot_counter4",0},
				{"slot_counter5","vs"},
				{"slot_bar1","impending"},
				{"display_fire",false},
				{"display_username",true},
				{"hasgarbage",true},
				{"neverstopbgm",true},
				{"display_next",true},
				{"display_hold",true},
				{"gmargin",3600},
				{"gincrease",0},
				{"garbagemultiplier",1},
				{"garbagemargin",0},
				{"garbageincrease",0},
				{"garbagecap", cap},
				{"garbagecapincrease",0},
				{"garbagecapmax",40},
				{"bagtype","7bag"},
				{"spinbonuses","T-spins"},
				{"kickset","SRS+"},
				{"nextcount",5},
				{"allow_harddrop",true},
				{"display_shadow",true},
				{"locktime",30},
				{"garbagespeed",1},
				{"forfeit_time",150},
				{"are",0},
				{"lineclear_are",0},
				{"infinitemovement",false},
				{"lockresets",15},
				{"allow180",true},
				{"manual_allowed",false},
				{"b2bchaining",true},
				{"clutch",true},
				{"passthrough",false},
				{"latencypreference","low"},
				{"noscope",true},
				{"username",info["user"]["username"]},
				{"physical",false},
				{"boardwidth",10},
				{"boardheight",20},
				{"boardbuffer",20},
				{"ghostskin","tetrio"},
				{"boardskin","generic"},
				{"minoskin",{
					{"z","tetrio"},
					{"l","tetrio"},
					{"o","tetrio"},
					{"s","tetrio"},
					{"i","tetrio"},
					{"j","tetrio"},
					{"t","tetrio"},
					{"other","tetrio"},
				}}
			};
		}
		void insertEvent(int evt, int frame, void* param, double subframe = 0.0) {
			json event;
			event["frame"] = frame;
			int k;
			atk_t atk;
			bool win;
			switch (evt) {
			case FULL:
				event["type"] = "full";
				event["data"] = {
					{"game", {
						{"board",board},
						{"handling",handling},
						{"playing", true}
						}
					},
					{"options",options }
				};
				break;
			case START:
				event["type"] = "start";
				event["data"] = json::object();
				break;
			case TARGETS:
				event["type"] = "targets";
				event["data"] = {
					{"id","diyusi"},
					{"frame",frame},
					{"type","targets"}
				};
				break;
			case KEYDOWN:
				k = *(int*)param;
				event["type"] = "keydown";
				event["data"] = {
					{"key",key[k]},
					{"subframe",0}
				};
				break;
			case KEYUP:
				k = *(int*)param;
				event["type"] = "keyup";
				event["data"] = {
					{"key",key[k]},
					{"subframe",0}
				};
				break;
			case IGE:
				atk = *(atk_t*)param;
				event["type"] = "ige";
				event["data"] = {
					{"type","ige"},
					{"data",{
						{"type","interaction"},
						{"sent_frame", frame-1},
						{"cid",++cid},
						{"data",{
							{"type","garbage"},
							{"amt",atk.atk},
							{"x",0},{"y",0},
							{"column",atk.pos}
							}
						}
						}
					},
					{"frame",frame},
					{"id",++id}
				};
				break;
			case IGE_C:
				atk = *(atk_t*)param;
				event["type"] = "ige";
				event["data"] = {
					{"type","ige"},
					{"data",{
						{"type","interaction_confirm"},
						{"sent_frame", frame - 1},
						{"cid",cid},
						{"data",{
							{"type","garbage"},
							{"amt",atk.atk},
							{"x",0},{"y",0},
							{"column",atk.pos}
							}
						}
						}
					},
					{"frame",frame},
					{"id",++id}
				};
				break;
			case END:
				win = *(bool*)param;
				event["type"] = "end";
				event["data"] = {
					{"reason",(win) ? "winner" : "topout"},
					{"export",{{"aggregatestats",endGameStat}}}
				};
				break;
			default:
				break;
			}
			this->evt["events"].push_back(event);
			if (evt == IGE) {
				insertEvent(IGE_C, frame, param);
			}
		}
		json getInfo() {
			return info;
		}
		json getEvents() {
			return evt;
		}
	};
	class GameRecord
	{
		json game;
	public:
		GameRecord() {
			game["board"] = json::array();
			game["replays"] = json::array();
		}
		void insertGame(playerRecord r) {
			game["board"].push_back(r.getInfo());
			game["replays"].push_back(r.getEvents());
		}
		void reset(){
			game.clear();
			game["board"] = json::array();
			game["replays"] = json::array();
		}
		json getGame() {
			return game;
		}
	};
	class Replay
	{
	std::string filename;
	json r, data;
	public:
	Replay(std::string filename, std::string timestamp) {
		this->filename = filename;
		r["ts"] = timestamp;
		r["ismulti"] = true;
		r["data"] = json::array();
		r["endcontext"] = json::array();
	}
	void reset(std::string filename, std::string timestamp) {
		this->filename = filename;
		r["ts"] = timestamp;
	}
	void toFile() {
		std::ofstream f;
		f.open(this->filename + ".ttrm");
		f << r;;
		f.close();
	}
	void setUser(std::string user1, std::string user2, int user1ID = 0, int user2ID = 0) {
		json juser1 = {
			{ "user",{{"_id",user1ID}, {"username",user1}} },
			{ "active",true }
		};
		json juser2 = {
			{ "user",{{"_id",user2ID}, {"username",user2}} },
			{ "active",true }
		};
		r["endcontext"].push_back(juser1);
		r["endcontext"].push_back(juser2);

	}
	void insertGame(GameRecord g) {
		r["data"].push_back(g.getGame());
	}
	};
}
#endif
