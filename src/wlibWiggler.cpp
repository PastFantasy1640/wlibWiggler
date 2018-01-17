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

	//*** �V�����R�}���h��ǉ�����ꏊ ***
	std::vector<CommandPair> getCommands(void) {
		//�R�}���h�������ɒǉ�
		//CommandPair("�R�}���h��", [�N���G�C�^�[�֐��̃|�C���^]),
		return std::vector<CommandPair> {
			//CommandPair("Hogehoge", ccr::Hogehoge::creator),
			CommandPair("wlibWiggler", wlib::WigglerCommand::creator)
		};
	}

	//*** �V�����m�[�h��ǉ�����ꏊ ***
	std::vector<NodePair> getNodes(void) {
		//�m�[�h�������ɒǉ�
		//NodePair("�m�[�h��", [Node ID], [�N���G�C�^�[�֐��̃|�C���^], [initialize�֐��̃|�C���^]),
		return std::vector<NodePair> {
			//NodePair("Hogehoge", 0x70000, ccr::Hogehoge::creator, ccr::Hogehoge::initialize),
		};
	}


	/** [hidden]addCommands
	* �R�}���h��ǉ�����B�R�}���h�̈ꗗ��getCommands�֐����ɋL�q����
	* @param plugin �v���O�C���C���X�^���X
	* @throws MStatusException �R�}���h���o�^�ł��Ȃ���
	*/
	void addCommands(MFnPlugin & plugin) {
		std::vector<CommandPair> cmd_pairs = getCommands();
		for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
			wlib::MStatusException::throwIf(plugin.registerCommand(p->command, p->creator), "�R�}���h�̓o�^���ɃG���[���������܂��� : �R�}���h��" + p->command, "addCommands");
		}
	}


	/** [hidden]addNodes
	* �m�[�h��ǉ�����B�m�[�h�̈ꗗ��getNodes�֐����ɋL�q����
	* @param plugin �v���O�C���C���X�^���X
	* @throws MStatusException �m�[�h���o�^�ł��Ȃ���
	*/
	void addNodes(MFnPlugin & plugin) {
		std::vector<NodePair> node_pairs = getNodes();
		for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
			wlib::MStatusException::throwIf(plugin.registerNode(p->node, p->id, p->creator, p->initialize), "�m�[�h�̓o�^���ɃG���[���������܂��� : �m�[�h��" + p->node, "addNode");
		}
	}


	/** [hidden]removeCommands
	* �R�}���h��o�^��������B�R�}���h�̈ꗗ��getCommands�֐����ɋL�q����
	* @param plugin �v���O�C���C���X�^���X
	* @throws MStatusException �R�}���h���o�^�����ł��Ȃ���
	*/
	void removeCommands(MFnPlugin & plugin) {
		std::vector<CommandPair> cmd_pairs = getCommands();
		for (auto p = cmd_pairs.begin(); p != cmd_pairs.end(); ++p) {
			wlib::MStatusException::throwIf(plugin.deregisterCommand(p->command), "�R�}���h�̓o�^�������ɃG���[���������܂��� : �R�}���h��" + p->command, "addCommands");
		}
	}


	/** [hidden]addNodes
	* �m�[�h��o�^��������B�m�[�h�̈ꗗ��getNodes�֐����ɋL�q����
	* @param plugin �v���O�C���C���X�^���X
	* @throws MStatusException �m�[�h���o�^�����ł��Ȃ���
	*/
	void removeNodes(MFnPlugin & plugin) {
		std::vector<NodePair> node_pairs = getNodes();
		for (auto p = node_pairs.begin(); p != node_pairs.end(); ++p) {
			wlib::MStatusException::throwIf(plugin.deregisterNode(p->id) ,"�m�[�h�̓o�^���ɃG���[���������܂��� : �m�[�h��" + p->node, "addNode");
		}
	}

};


/** initializePlugin
* �v���O�C���̏������������s���Bmaya������R�[�������֐��B
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

	//�ǂݍ��ݎ��ɑ��ɉ�������Ȃ炱���ɒǉ�


	if (stat == MStatus::kSuccess) {
		//�ǂݍ��݊���
		std::cerr << "Finished to load plug-in." << std::endl;
		std::cerr << "(c) 2017 S.Shirao" << std::endl;
#ifdef _DEBUG
		//�f�o�b�O���̂ݕW���o�͂��G���[�o�͂֐؂�ւ�
		std::cout.rdbuf(std::cerr.rdbuf());
		std::cout << "wlibWiggler is debug mode." << std::endl;
#endif
	}
	else {
		//�ǂݍ��ݎ��s
		std::cerr << "Failed to initialize plugin." << std::endl;
	}

	return stat;
}


/** uninitializePlugin
* �v���O�C���̃A�����[�h�������s���Bmaya������R�[�������֐��B
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

	//���j���[�̉���
	//MGlobal::executeCommand("ccr_DeleteUI");

	return stat;
}