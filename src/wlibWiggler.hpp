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
	* プラグインの初期化処理を行う。maya側からコールされる関数。
	*/
	extern MStatus initializePlugin(MObject _obj);

	/** uninitializePlugin
	* プラグインのアンロード処理を行う。maya側からコールされる関数。
	*/
	extern MStatus uninitializePlugin(MObject _obj);

};

#endif //end of include guard
