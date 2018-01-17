#pragma comment(user, "wlibControllerNode Plug-in Header")
/////////////////////////////////////////////////////
//MStatusException.hpp
//
//Copyright (c) 2017 Shotaro Shirao
//
//This software is released under the MIT License.
//http://opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////

#pragma once
#ifndef CMA_MSTATUSEXCEPTION_HPP_
#define CMA_MSTATUSEXCEPTION_HPP_

#include <maya/MString.h>
#include <maya/MStatus.h>

namespace wlib {

/** 
 * mayaAPI��MStatus�ɗ�O���������������́B����c++��std::exception�Ƃ͐؂藣���ēƎ��̗�O�^�Ƃ��ď�������B
 * @see MStatus https://help.autodesk.com/view/MAYAUL/2016/ENU/?guid=__cpp_ref_class_m_status_html
 */
class MStatusException{
public:
	//////////////////////////////
	// PUBLIC MEMBER CONSTANT
	//////////////////////////////

	/** �X�e�[�^�X�{��
	 */
	const MStatus stat;

	/** �G���[���b�Z�[�W
	 */
	const MString message;

	/** �G���[�̔����ӏ��B�R���X�g���N�^�Őݒ肵�Ȃ��ꍇ��<unknown>�ƂȂ�B
	 * �\�Ȍ��蕪����₷���ݒ肷��ׂ��B
	 */
	const MString place;
	
	//////////////////////////////
	// PUBLIC MEMBER FUNCTION
	//////////////////////////////
	
	/**
	 * �R���X�g���N�^�B�G���[�̃X�e�[�^�X�ƃ��b�Z�[�W��o�^�ł���B
	 * @param stat �G���[�̃X�e�[�^�X
	 * @param message ���b�Z�[�W
	 * @param place �����������ł��镶����
	 */
	MStatusException(const MStatus & stat, const MString & message, const MString & place = "<unknown>");
	
	/** �f�N�X�g���N�^
	 */
	virtual ~MStatusException();

	/**
	 * �w��t�H�[�}�b�g�ɐ��`���ꂽ�G���[���b�Z�[�W���擾����B
	 * @return �G���[���b�Z�[�W
	 */
	MString toString(void) const;

	/**
	 * �������X�e�[�^�X��kSuccess�ȊO�̎��ɁAMStatusException���X���[���郆�[�e�B���e�B�[�֐�
	 * @param stat �G���[�̃X�e�[�^�X
	 * @param message ���b�Z�[�W
	 * @param place �����������ł��镶����
	 * @throws MStatusException �X�e�[�^�X��kSuccess�ȊO�������ꍇ
	 */
	static void throwIf(const MStatus & stat, const MString & message, const MString & place = "<unlogged>");

protected:

private:
	MStatusException() = delete;
};

};

#endif //end of include guard
