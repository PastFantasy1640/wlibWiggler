#include "wlibWiggler.hpp"

#include <vector>
#include <string>
#include <maya/MStringArray.h>
#include <maya/MGlobal.h>
#include "exception/MStatusException.hpp"

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
	* @throws MStatusException コマンドが登録できない時
	*/
	void addCommands(MFnPlugin & plugin) {
		std::vector<CommandPair> cmd_pairs = getCommands();
		for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
			wlib::MStatusException::throwIf(plugin.registerCommand(p->command, p->creator), "コマンドの登録中にエラーが発生しました : コマンド名" + p->command, "addCommands");
		}
	}


	/** [hidden]addNodes
	* ノードを追加する。ノードの一覧はgetNodes関数内に記述する
	* @param plugin プラグインインスタンス
	* @throws MStatusException ノードが登録できない時
	*/
	void addNodes(MFnPlugin & plugin) {
		std::vector<NodePair> node_pairs = getNodes();
		for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
			wlib::MStatusException::throwIf(plugin.registerNode(p->node, p->id, p->creator, p->initialize), "ノードの登録中にエラーが発生しました : ノード名" + p->node, "addNode");
		}
	}


	/** [hidden]removeCommands
	* コマンドを登録解除する。コマンドの一覧はgetCommands関数内に記述する
	* @param plugin プラグインインスタンス
	* @throws MStatusException コマンドが登録解除できない時
	*/
	void removeCommands(MFnPlugin & plugin) {
		std::vector<CommandPair> cmd_pairs = getCommands();
		for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
			wlib::MStatusException::throwIf(plugin.deregisterCommand(p->command), "コマンドの登録解除中にエラーが発生しました : コマンド名" + p->command, "addCommands");
		}
	}


	/** [hidden]addNodes
	* ノードを登録解除する。ノードの一覧はgetNodes関数内に記述する
	* @param plugin プラグインインスタンス
	* @throws MStatusException ノードが登録解除できない時
	*/
	void removeNodes(MFnPlugin & plugin) {
		std::vector<NodePair> node_pairs = getNodes();
		for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
			wlib::MStatusException::throwIf(plugin.deregisterNode(p->id) ,"ノードの登録中にエラーが発生しました : ノード名" + p->node, "addNode");
		}
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
	try{
		addCommands(plugin);
		addNodes(plugin); 
	}
	catch (MStatusException e) {
		std::cerr << e.toString() << std::endl;
		stat = e.stat;
	}

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
	try {
		removeCommands(plugin);
		removeNodes(plugin);
	}
	catch (MStatusException e) {
		std::cerr << e.toString() << std::endl;
		stat = e.stat;
	}

	//メニューの解除
	//MGlobal::executeCommand("ccr_DeleteUI");

	return stat;
}