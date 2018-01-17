#include "wlibWiggler.hpp"

#include <vector>
#include <string>
#include <maya/MStringArray.h>
#include <maya/MGlobal.h>

//*** INCLUDE HEADERS ***

//#include "Hogehoge.hpp"
#include "command/WigglerCommand.hpp"

//***********************

namespace {
	class CommandPair {
	public:
		const MString command;
		void * (*creator)();
		CommandPair(const MString & command, void * (*creator)()) : command(command), creator(creator) {}
		virtual ~CommandPair() {}
	private:
		CommandPair() = delete;
	};

	class NodePair {
	public:
		const MString node;
		const MTypeId id;
		void * (*creator)();
		MStatus(*initialize)();
		NodePair(const MString & node, const MTypeId id, void * (*creator)(), MStatus(*initialize)()) : node(node), id(id), creator(creator), initialize(initialize) {}
		virtual ~NodePair() {}
	private:
		NodePair() = delete;
	};

	//*** 新しいコマンドを追加する場所 ***
	std::vector<CommandPair> getCommands(void) {
		//コマンドをここに追加
		//CommandPair("コマンド名", [クリエイター関数のポインタ]),
		return std::vector<CommandPair> {
			//CommandPair("Hogehoge", ccr::Hogehoge::creator),
			CommandPair("wlibWiggler", wlib::WigglerCommand::creator)
		};
	}

	//*** 新しいノードを追加する場所 ***
	std::vector<NodePair> getNodes(void) {
		//ノードをここに追加
		//NodePair("ノード名", [Node ID], [クリエイター関数のポインタ], [initialize関数のポインタ]),
		return std::vector<NodePair> {
			//NodePair("Hogehoge", 0x70000, ccr::Hogehoge::creator, ccr::Hogehoge::initialize),
		};
	}


	/** [hidden]addCommands
	* コマンドを追加する。コマンドの一覧はgetCommands関数内に記述する
	* @param plugin プラグインインスタンス
	* @returns 追加に関する結果ステータス
	*/
	MStatus addCommands(MFnPlugin & plugin) {
		std::vector<CommandPair> cmd_pairs = getCommands();
		MStatus stat;
		for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
			stat = plugin.registerCommand(p->command, p->creator);
			if (!stat) {
				//コマンド登録時にエラーが発生
				stat.perror(std::string("An error occured during registering command : " + std::string(p->command.asChar())).c_str());
				break;
			}
		}
		return stat;
	}


	/** [hidden]addNodes
	* ノードを追加する。ノードの一覧はgetNodes関数内に記述する
	* @param plugin プラグインインスタンス
	* @returns 追加に関する結果ステータス
	*/
	MStatus addNodes(MFnPlugin & plugin) {
		std::vector<NodePair> node_pairs = getNodes();
		MStatus stat;
		for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
			stat = plugin.registerNode(p->node, p->id, p->creator, p->initialize);
			if (!stat) {
				//コマンド登録時にエラーが発生
				stat.perror(std::string("An error occured during registering dipendency node : " + std::string(p->node.asChar())).c_str());
				break;
			}
		}
		return stat;
	}


	/** [hidden]removeCommands
	* コマンドを登録解除する。コマンドの一覧はgetCommands関数内に記述する
	* @param plugin プラグインインスタンス
	* @returns 追加に関する結果ステータス
	*/
	MStatus removeCommands(MFnPlugin & plugin) {
		std::vector<CommandPair> cmd_pairs = getCommands();
		MStatus stat;
		for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
			stat = plugin.deregisterCommand(p->command);
			if (!stat) {
				//コマンド登録解除時にエラーが発生
				stat.perror(std::string("An error occured during deregistering command : " + std::string(p->command.asChar())).c_str());
				break;
			}
		}
		return stat;
	}


	/** [hidden]addNodes
	* ノードを登録解除する。ノードの一覧はgetNodes関数内に記述する
	* @param plugin プラグインインスタンス
	* @returns 追加に関する結果ステータス
	*/
	MStatus removeNodes(MFnPlugin & plugin) {
		std::vector<NodePair> node_pairs = getNodes();
		MStatus stat;
		for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
			stat = plugin.deregisterNode(p->id);
			if (!stat) {
				//コマンド登録解除時にエラーが発生
				stat.perror(std::string("An error occured during deregistering dipendency node : " + std::string(p->node.asChar())).c_str());
				break;
			}
		}
		return stat;
	}

};


/** initializePlugin
* プラグインの初期化処理を行う。maya側からコールされる関数。
*/
MStatus wlib::initializePlugin(MObject _obj) {
	MFnPlugin plugin(_obj, "Autodesk", "2015 update2");
	std::cerr << "White Library - wlibWiggler" << std::endl;
	std::cerr << "version 0.1" << std::endl;
	MStatus stat = MStatus::kSuccess;
	do {
		if ((stat = addCommands(plugin)) != MStatus::kSuccess) break;
		if ((stat = addNodes(plugin)) != MStatus::kSuccess) break;

	} while (false);

	//読み込み時に他に何かするならここに追加


	if (stat == MStatus::kSuccess) {
		//読み込み完了
		std::cerr << "Finished to load plug-in." << std::endl;
		std::cerr << "(c) 2017 S.Shirao" << std::endl;
#ifdef _DEBUG
		//デバッグ時のみ標準出力をエラー出力へ切り替え
		std::cout.rdbuf(std::cerr.rdbuf());
		std::cout << "wlibWiggler is debug mode." << std::endl;
#endif
	}
	else {
		//読み込み失敗
		std::cerr << "Failed to initialize plugin." << std::endl;
	}

	return stat;
}


/** uninitializePlugin
* プラグインのアンロード処理を行う。maya側からコールされる関数。
*/
MStatus wlib::uninitializePlugin(MObject _obj) {
	MFnPlugin plugin(_obj);
	MStatus stat = MStatus::kSuccess;
	do {
		if ((stat = removeCommands(plugin)) != MStatus::kSuccess) break;
		if ((stat = removeNodes(plugin)) != MStatus::kSuccess) break;
	} while (false);

	//メニューの解除
	//MGlobal::executeCommand("ccr_DeleteUI");

	return stat;
}