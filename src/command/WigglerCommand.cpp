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
		//引数パース
		for (int i = 0; i < args.length(); i++) {
			if (flag.length() > 0) {
				//フラグによって場合分け
				if (isEqual("-seed", "-sd")) {
					//seedは省略可能
					this->seed_ = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "seedはint値である必要があります", place);
				}
				else if (isEqual("-period", "-pr")) {
					//periodは省略可能(default:1.0)
					this->period_ = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "periodはint値である必要があります", place);
					if (this->period_ < 1) MStatusException::throwIf(MStatus::kInvalidParameter, "periodは1以上の値である必要があります", place);
				}
				else if (isEqual("-strength", "-st")) {
					//strengthは省略不可能
					this->strength_ = args.asDouble(i, &stat);
					MStatusException::throwIf(stat, "strengthはdouble値である必要があります", place);
					is_set_strength = true;
				}/*
				else if (isEqual("-offset", "-o")) {
					//offsetは省略可能
					this->offset_ = args.asDouble(i, &stat);
					MStatusException::throwIf(stat, "offsetはdouble値である必要があります", place);
				}*/
				else if (isEqual("-start", "-s")) {
					//startは省略可能。省略した場合タイムスライダの最初に設定
					this->start_ = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "startはint値である必要があります", place);
					is_set_start = true;
				}
				else if (isEqual("-end", "-e")) {
					//endは省略可能。省略した場合タイムスライダの最後に設定
					this->end_ = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "endはint値である必要があります", place);
					is_set_end = true;
				}
				else if (isEqual("-smoothing", "-sm")) {
					//smoothingは省略可能。(default:1)
					int tmp = args.asInt(i, &stat);
					MStatusException::throwIf(stat, "smoothingはint値である必要があります", place);
					if (tmp == 0) this->smoothing_ = 0;
					else this->smoothing_ = 1;
				}
				else {
					MStatusException(MStatus::kInvalidParameter, "フラグ" + flag + "は見つかりませんでした", place);
				}
				flag.clear();
			}
			else {
				//flagが空
				//ハイフン付き文字ならフラグに代入。
				//ハイフンナシならアトリビュートとして登録
				if (args.asString(i).substring(0, 1) == "-") {
					if (this->attribute_.length() > 0) MStatusException::throwIf(MStatus::kInvalidParameter, "アトリビュートの指定は一番最後にまとめて行う必要があります", place);
					flag = args.asString(i);
				}
				else {
					this->attribute_.append(args.asString(i));
				}
			}
		}

		//解析終わり
		//エラーチェック
		if (!is_set_strength) MStatusException(MStatus::kInvalidParameter, "-strengthは省略できません", place);

		if (!is_set_start) {
			//タイムスライダの最初を取得
			double start_tmp = 0;
			MStatusException::throwIf(MGlobal::executeCommand("playbackOptions -q -minTime", start_tmp), "タイムスライダの開始フレーム取得のコマンド実行に失敗", place);
			this->start_ = static_cast<int>(this->start_);
		}

		if (!is_set_end) {
			//タイムスライダの最後を取得
			double end_tmp = 0;
			MStatusException::throwIf(MGlobal::executeCommand("playbackOptions -q -maxTime", end_tmp), "タイムスライダの終了フレーム取得のコマンド実行に失敗", place);
			this->end_ = static_cast<int>(this->end_);
		}

		if (this->start_ > this->end_) MStatusException::throwIf(MStatus::kInvalidParameter, "開始フレームが終了フレームを上回っています", place);

		//アトリビュートがあるか
		for (int i = 0; i < this->attribute_.length(); i++) {
			int ret_cmd = 0;
			MStringArray ats;
			MStatusException::throwIf(this->attribute_[i].split('.', ats), "アトリビュートのspiltに失敗", place);
			if (ats.length() != 2) MStatusException::throwIf(MStatus::kFailure, "アトリビュートのspiltの結果が異常", place);
			//TODO:atsのそれぞれにスペースが入っていないかチェック
			MStatusException::throwIf(MGlobal::executeCommand("attributeQuery -node " + ats[0] + " -exists " + ats[1], ret_cmd), "attributeQueryに失敗", place);
			if (!ret_cmd) MStatusException::throwIf(MStatus::kInvalidParameter, "アトリビュート名:" + this->attribute_[i] + "は見つかりませんでした", place);
			MStatusException::throwIf(MGlobal::executeCommand("attributeQuery -node " + ats[0] + " -keyable " + ats[1], ret_cmd), "attributeQueryに失敗", place);
			if (!ret_cmd) MStatusException::throwIf(MStatus::kInvalidParameter, "アトリビュート名:" + this->attribute_[i] + "はキー設定可能ではありません", place);
		}

		//変数にセットできた
		//計算
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
			//オフセットの設定
			//ウィグラー開始フレーム時点の値
			double offset;
			MStatusException::throwIf(MGlobal::executeCommand("getAttr " + attr, offset), "アトリビュート:" + attr + "の取得に失敗", place);
			
			//開始点に打つ

			for (int frame = this->start_)
		}

	}
	catch (MStatusException e) {
		MGlobal::displayError(e.place + ":" + e.message);
		ret_stat = e.stat;
	}

	return ret_stat;
}

