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
 * mayaAPIのMStatusに例外処理を加えたもの。ただc++のstd::exceptionとは切り離して独自の例外型として処理する。
 * @see MStatus https://help.autodesk.com/view/MAYAUL/2016/ENU/?guid=__cpp_ref_class_m_status_html
 */
class MStatusException{
public:
	//////////////////////////////
	// PUBLIC MEMBER CONSTANT
	//////////////////////////////

	/** ステータス本体
	 */
	const MStatus stat;

	/** エラーメッセージ
	 */
	const MString message;

	/** エラーの発生箇所。コンストラクタで設定しない場合は<unknown>となる。
	 * 可能な限り分かりやすく設定するべき。
	 */
	const MString place;
	
	//////////////////////////////
	// PUBLIC MEMBER FUNCTION
	//////////////////////////////
	
	/**
	 * コンストラクタ。エラーのステータスとメッセージを登録できる。
	 * @param stat エラーのステータス
	 * @param message メッセージ
	 * @param place 発生個所を特定できる文字列
	 */
	MStatusException(const MStatus & stat, const MString & message, const MString & place = "<unknown>");
	
	/** デクストラクタ
	 */
	virtual ~MStatusException();

	/**
	 * 指定フォーマットに成形されたエラーメッセージを取得する。
	 * @return エラーメッセージ
	 */
	MString toString(void) const;

	/**
	 * もしもステータスがkSuccess以外の時に、MStatusExceptionをスローするユーティリティー関数
	 * @param stat エラーのステータス
	 * @param message メッセージ
	 * @param place 発生個所を特定できる文字列
	 * @throws MStatusException ステータスがkSuccess以外だった場合
	 */
	static void throwIf(const MStatus & stat, const MString & message, const MString & place = "<unlogged>");

protected:

private:
	MStatusException() = delete;
};

};

#endif //end of include guard
