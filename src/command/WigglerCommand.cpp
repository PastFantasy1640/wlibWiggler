#include "WigglerCommand.hpp"
#include "exception/MStatusException.hpp"
#include <maya/MGlobal.h>
#include <random>

wlib::WigglerCommand::WigglerCommand()
	: seed_(0), period_(1), strength_(1.0), start_(0), end_(1), smoothing_("linear")
{
}

wlib::WigglerCommand::~WigglerCommand()
{
}

MStatus wlib::WigglerCommand::doIt(const MArgList & args)
{
	std::cout << "doIt!" << std::endl;
	MString flag("");
	const MString place("wlib::WigglerCommand::doIt");
	auto isEqual = [&flag](const MString & long_f, const MString & short_f = "") {
		return (flag == long_f || (short_f == "" && short_f == flag));
	};
	MStatus stat, ret_stat;
	bool is_set_start = false, is_set_end = false, is_set_strength = false;

	try {
		//�����p�[�X
		for (unsigned int i = 0; i < args.length(); i++) {
			if (flag.length() > 0) {
				//�t���O�ɂ���ďꍇ����
				if (isEqual("-seed", "-sd")) {
					//seed�͏ȗ��\
					this->seed_ = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "seed��int�l�ł���K�v������܂�", place);
				}
				else if (isEqual("-period", "-pr")) {
					//period�͏ȗ��\(default:1.0)
					this->period_ = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "period��int�l�ł���K�v������܂�", place);
					if (this->period_ < 1) MStatusException::throwIf(MStatus::kInvalidParameter, "period��1�ȏ�̒l�ł���K�v������܂�", place);
				}
				else if (isEqual("-strength", "-st")) {
					//strength�͏ȗ��s�\
					this->strength_ = args.asDouble(i, &stat);
					MStatusException::throwIf(stat, "strength��double�l�ł���K�v������܂�", place);
					is_set_strength = true;
				}/*
				else if (isEqual("-offset", "-o")) {
					//offset�͏ȗ��\
					this->offset_ = args.asDouble(i, &stat);
					MStatusException::throwIf(stat, "offset��double�l�ł���K�v������܂�", place);
				}*/
				else if (isEqual("-start", "-s")) {
					//start�͏ȗ��\�B�ȗ������ꍇ�^�C���X���C�_�̍ŏ��ɐݒ�
					this->start_ = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "start��int�l�ł���K�v������܂�", place);
					is_set_start = true;
				}
				else if (isEqual("-end", "-e")) {
					//end�͏ȗ��\�B�ȗ������ꍇ�^�C���X���C�_�̍Ō�ɐݒ�
					this->end_ = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "end��int�l�ł���K�v������܂�", place);
					is_set_end = true;
				}
				else if (isEqual("-smoothing", "-sm")) {
					//smoothing�͏ȗ��\�B(default:1)
					this->smoothing_ = args.asString(i, &stat);
					MStatusException::throwIf(stat, "smoothing��String�ł���K�v������܂�", place);
				}
				else {
					MStatusException(MStatus::kInvalidParameter, "�t���O" + flag + "�͌�����܂���ł���", place);
				}
				flag.clear();
			}
			else {
				//flag����
				//�n�C�t���t�������Ȃ�t���O�ɑ���B
				//�n�C�t���i�V�Ȃ�A�g���r���[�g�Ƃ��ēo�^
				if (args.asString(i).asChar()[0] == '-') {
					if (this->attributes_.size() > 0) MStatusException::throwIf(MStatus::kInvalidParameter, "�A�g���r���[�g�̎w��͈�ԍŌ�ɂ܂Ƃ߂čs���K�v������܂�", place);
					flag = args.asString(i); 
				}
				else {
					this->attributes_.push_back(AttributePair::create(args.asString(i)));
				}
			}
		}

		//��͏I���
		//�G���[�`�F�b�N
		if (!is_set_strength) MStatusException(MStatus::kInvalidParameter, "-strength�͏ȗ��ł��܂���", place);

		if (!is_set_start) {
			//�^�C���X���C�_�̍ŏ����擾
			double start_tmp = 0;
			MStatusException::throwIf(MGlobal::executeCommand("playbackOptions -q -minTime", start_tmp), "�^�C���X���C�_�̊J�n�t���[���擾�̃R�}���h���s�Ɏ��s", place);
			this->start_ = static_cast<int>(start_tmp);
		}

		if (!is_set_end) {
			//�^�C���X���C�_�̍Ō���擾
			double end_tmp = 0;
			MStatusException::throwIf(MGlobal::executeCommand("playbackOptions -q -maxTime", end_tmp), "�^�C���X���C�_�̏I���t���[���擾�̃R�}���h���s�Ɏ��s", place);
			this->end_ = static_cast<int>(end_tmp);
		}

		if (this->start_ > this->end_) MStatusException::throwIf(MStatus::kInvalidParameter, "�J�n�t���[�����I���t���[���������Ă��܂�", place);

		//Smoothing�̋K��l�`�F�b�N
		if (this->smoothing_ != "spline" &&
			this->smoothing_ != "linear" &&
			this->smoothing_ != "fast" &&
			this->smoothing_ != "slow" &&
			this->smoothing_ != "flat" &&
			this->smoothing_ != "step" &&
			this->smoothing_ != "clamped") MStatusException::throwIf(MStatus::kInvalidParameter, "�t���[���⊮�̒l���ُ�ł�", place);

		//�A�g���r���[�g�����邩
		if (this->attributes_.size() == 0) MStatusException::throwIf(MStatus::kInvalidParameter, "�A�g���r���[�g���w�肳��Ă��܂���", place);
		for (auto attr = this->attributes_.begin(); attr != this->attributes_.end(); ++attr) {
			if (!attr->isExist()) MStatusException::throwIf(MStatus::kInvalidParameter, "�w�肳�ꂽ�A�g���r���[�g:" + attr->fullpath_ + "�����݂��Ȃ����A�L�[�t���[�����łĂȂ��ݒ�ɂȂ��Ă��܂�", place);
		}

		//�ϐ��ɃZ�b�g�ł���
		//�v�Z
		ret_stat = this->redoIt();

	}
	catch (MStatusException e) {
		MGlobal::displayError(e.toString());
		std::cout << e.toString() << std::endl;
		ret_stat = e.stat;
	}
	return ret_stat;
}

MStatus wlib::WigglerCommand::redoIt()
{
	MStatus ret_stat, stat;
	MString place("wlib::WigglerCommand::redoIt");

	auto setKeyframe = [this, place](const std::vector<AttributePair>::const_iterator & attr, const int frame, const double value) {
		int ret_keys = 0;
		std::cout << ("setKeyframe -attribute " + attr->attribute_ + " -time " + frame + " -value " + value + " -inTangentType " + this->smoothing_ + " -outTangentType " + this->smoothing_ + " " + attr->object_) << std::endl;
		MStatusException::throwIf(MGlobal::executeCommand("setKeyframe -attribute " + attr->attribute_ + " -time " + frame + " -value " + value + " -inTangentType " + this->smoothing_ + " -outTangentType " + this->smoothing_ + " " + attr->object_, ret_keys), "�L�[�t���[���̐ݒ�Ɏ��s", place);
		return ret_keys;
	};


	try {
		for (auto attr = this->attributes_.begin(); attr != this->attributes_.end(); ++attr) {

			std::cout << "[setKeyframe]�I�u�W�F�N�g:" << attr->object_ << " �A�g���r���[�g:" << attr->attribute_ << std::endl;

			std::mt19937 mt(this->seed_);
			std::uniform_real_distribution<> rand_range(0.0, this->strength_);
			//�I�t�Z�b�g�̐ݒ�
			//�E�B�O���[�J�n�t���[�����_�̒l
			double offset = 0.0;
			MStatusException::throwIf(MGlobal::executeCommand("getAttr " + attr->fullpath_, offset), "�A�g���r���[�g:" + attr->fullpath_ + "�̎擾�Ɏ��s", place);
			
			//�J�n�_�ɑł�
			setKeyframe(attr, this->start_, offset);
			setKeyframe(attr, this->end_, offset);
			
			for (int frame = this->start_ + this->period_; frame < this->end_; frame += this->period_) {
				//�����𔭐�������
				setKeyframe(attr, frame, offset + rand_range(mt));
			}
		}
	}
	catch (MStatusException e) {
		MGlobal::displayError(e.toString());
		std::cout << e.toString() << std::endl;
		ret_stat = e.stat;
	}

	return ret_stat;
}


//////////////////////////////////////
// AttributePair
//////////////////////////////////////
wlib::WigglerCommand::AttributePair::AttributePair(const MString & fullpath, const MString & object, const MString & attribute)
	: fullpath_(fullpath), object_(object), attribute_(attribute){}

wlib::WigglerCommand::AttributePair wlib::WigglerCommand::AttributePair::create(const MString & fullpath)
{
	MStringArray ret;
	const MString place("wlib::WigglerCommand::AttributePair::create");

	std::cout << "�ӂ�ς�" << fullpath << std::endl;
	MStatusException::throwIf(fullpath.split('.', ret), "�A�g���r���[�g��spilt�Ɏ��s", place);
	if (ret.length() != 2) MStatusException::throwIf(MStatus::kFailure, "�A�g���r���[�g��spilt�̌��ʂ��ُ�", place);

	//XSS�΍�܂���
	for (unsigned int i = 0; i < ret.length(); i++) {
		MStringArray tmp;
		MStatusException::throwIf(fullpath.split(' ', tmp), "�`�F�b�N�i�K�̃A�g���r���[�g����spilt�Ɏ��s", place);
		if (tmp.length() != 1) MStatusException::throwIf(MStatus::kInvalidParameter, "�A�g���r���[�g�����s�K�؂ł�", place);
	}
	return AttributePair(fullpath, ret[0], ret[1]);
}

bool wlib::WigglerCommand::AttributePair::isExist(void) const
{
	MString place("wlib::WigglerCommand::AttributePair::isExist");
	int ret_cmd = 0;
	std::cout << "�I�u�W�F�N�g:" << this->object_ << " �A�g���r���[�g:" << this->attribute_ << std::endl;

	MStatusException::throwIf(MGlobal::executeCommand("objExists " + this->object_, ret_cmd), "exists�Ɏ��s", place);
	if (!ret_cmd) { std::cout << "no exists object" << std::endl; return false; }
	MStatusException::throwIf(MGlobal::executeCommand("attributeQuery -node " + this->object_ + " -exists " + this->attribute_, ret_cmd), "attributeQuery�Ɏ��s", place);
	if (!ret_cmd) return false;
	MStatusException::throwIf(MGlobal::executeCommand("attributeQuery -node " + this->object_ + " -keyable " + this->attribute_, ret_cmd), "attributeQuery�Ɏ��s", place);
	if (!ret_cmd) return false;

	return true;
}
