#pragma comment(user, "wlibWiggler Plug-in Header")
/////////////////////////////////////////////////////
//wlibWiggler.hpp
//
//Copyright (c) 2017 Shotaro Shirao
//White Library
/////////////////////////////////////////////////////

#pragma once
#ifndef WLIB_WIGGLER_HPP_
#define WLIB_WIGGLER_HPP_

#include <maya/MFnPlugin.h>
#include <maya/MObject.h>

namespace wlib {

	/** initializePlugin
	* �v���O�C���̏������������s���Bmaya������R�[�������֐��B
	*/
	extern MStatus initializePlugin(MObject _obj);

	/** uninitializePlugin
	* �v���O�C���̃A�����[�h�������s���Bmaya������R�[�������֐��B
	*/
	extern MStatus uninitializePlugin(MObject _obj);

};

#endif //end of include guard
