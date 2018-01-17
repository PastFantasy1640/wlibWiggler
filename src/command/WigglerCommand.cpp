#include "WigglerCommand.hpp"
#include "exception/MStatusException.hpp"
#include <maya/MGlobal.h>
#include <random>

wlib::WigglerCommand::WigglerCommand()
	: seed_(0), period_(1), strength_(1.0), offset_(0.0), start_(0), end_(1), smoothing_(1)
{
}

wlib::WigglerCommand::~WigglerCommand()
{
}

MStatus wlib::WigglerCommand::doIt(const MArgList & args)
{
	MString flag("");
	const MString place("wlib::WigglerCommand::doIt");
	auto isEqual = [&flag](const MString & long_f, const MString & short_f = "") {
		return (flag == long_f || (short_f == "" && short_f == flag));
	};
	MStatus stat, ret_stat;
	bool is_set_start = false, is_set_end = false, is_set_strength = false;

	try {
		//�����p�[�X
		for (int i = 0; i < args.length(); i++) {
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
					int tmp = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "smoothing��int�l�ł���K�v������܂�", place);
					if (tmp == 0) this->smoothing_ = 0;
					else this->smoothing_ = 1;
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
				if (args.asString(i).substring(0, 1) == "-") {
					if (this->attribute_.length() > 0) MStatusException::throwIf(MStatus::kInvalidParameter, "�A�g���r���[�g�̎w��͈�ԍŌ�ɂ܂Ƃ߂čs���K�v������܂�", place);
					flag = args.asString(i);
				}
				else {
					this->attribute_.append(args.asString(i));
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
			this->start_ = static_cast<int>(this->start_);
		}

		if (!is_set_end) {
			//�^�C���X���C�_�̍Ō���擾
			double end_tmp = 0;
			MStatusException::throwIf(MGlobal::executeCommand("playbackOptions -q -maxTime", end_tmp), "�^�C���X���C�_�̏I���t���[���擾�̃R�}���h���s�Ɏ��s", place);
			this->end_ = static_cast<int>(this->end_);
		}

		if (this->start_ > this->end_) MStatusException::throwIf(MStatus::kInvalidParameter, "�J�n�t���[�����I���t���[���������Ă��܂�", place);

		//�A�g���r���[�g�����邩
		for (int i = 0; i < this->attribute_.length(); i++) {
			int ret_cmd = 0;
			MStringArray ats;
			MStatusException::throwIf(this->attribute_[i].split('.', ats), "�A�g���r���[�g��spilt�Ɏ��s", place);
			if (ats.length() != 2) MStatusException::throwIf(MStatus::kFailure, "�A�g���r���[�g��spilt�̌��ʂ��ُ�", place);
			//TODO:ats�̂��ꂼ��ɃX�y�[�X�������Ă��Ȃ����`�F�b�N
			MStatusException::throwIf(MGlobal::executeCommand("attributeQuery -node " + ats[0] + " -exists " + ats[1], ret_cmd), "attributeQuery�Ɏ��s", place);
			if (!ret_cmd) MStatusException::throwIf(MStatus::kInvalidParameter, "�A�g���r���[�g��:" + this->attribute_[i] + "�͌�����܂���ł���", place);
			MStatusException::throwIf(MGlobal::executeCommand("attributeQuery -node " + ats[0] + " -keyable " + ats[1], ret_cmd), "attributeQuery�Ɏ��s", place);
			if (!ret_cmd) MStatusException::throwIf(MStatus::kInvalidParameter, "�A�g���r���[�g��:" + this->attribute_[i] + "�̓L�[�ݒ�\�ł͂���܂���", place);
		}

		//�ϐ��ɃZ�b�g�ł���
		//�v�Z
		ret_stat = this->redoIt();

	}
	catch (MStatusException e) {
		MGlobal::displayError(e.place + ":" + e.message);
		ret_stat = e.stat;
	}
	return ret_stat;
}

MStatus wlib::WigglerCommand::redoIt()
{
	MStatus ret_stat, stat;
	MString place("wlib::WigglerCommand::redoIt");

	try {
		for (int attr_idx = 0; attr_idx < this->attribute_.length(); attr_idx++) {
			const MString & attr = this->attribute_[attr_idx];
			//�I�t�Z�b�g�̐ݒ�
			//�E�B�O���[�J�n�t���[�����_�̒l
			double offset;
			MStatusException::throwIf(MGlobal::executeCommand("getAttr " + attr, offset), "�A�g���r���[�g:" + attr + "�̎擾�Ɏ��s", place);
			
			//�J�n�_�ɑł�

			for (int frame = this->start_)
		}

	}
	catch (MStatusException e) {
		MGlobal::displayError(e.place + ":" + e.message);
		ret_stat = e.stat;
	}

	return ret_stat;
}

