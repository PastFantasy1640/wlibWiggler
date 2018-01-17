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
		//引数パース
		for (unsigned int i = 0; i < args.length(); i++) {
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
					this->smoothing_ = args.asString(i, &stat);
					MStatusException::throwIf(stat, "smoothingはStringである必要があります", place);
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
				if (args.asString(i).asChar()[0] == '-') {
					if (this->attributes_.size() > 0) MStatusException::throwIf(MStatus::kInvalidParameter, "アトリビュートの指定は一番最後にまとめて行う必要があります", place);
					flag = args.asString(i); 
				}
				else {
					this->attributes_.push_back(AttributePair::create(args.asString(i)));
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
			this->start_ = static_cast<int>(start_tmp);
		}

		if (!is_set_end) {
			//タイムスライダの最後を取得
			double end_tmp = 0;
			MStatusException::throwIf(MGlobal::executeCommand("playbackOptions -q -maxTime", end_tmp), "タイムスライダの終了フレーム取得のコマンド実行に失敗", place);
			this->end_ = static_cast<int>(end_tmp);
		}

		if (this->start_ > this->end_) MStatusException::throwIf(MStatus::kInvalidParameter, "開始フレームが終了フレームを上回っています", place);

		//Smoothingの規定値チェック
		if (this->smoothing_ != "spline" &&
			this->smoothing_ != "linear" &&
			this->smoothing_ != "fast" &&
			this->smoothing_ != "slow" &&
			this->smoothing_ != "flat" &&
			this->smoothing_ != "step" &&
			this->smoothing_ != "clamped") MStatusException::throwIf(MStatus::kInvalidParameter, "フレーム補完の値が異常です", place);

		//アトリビュートがあるか
		if (this->attributes_.size() == 0) MStatusException::throwIf(MStatus::kInvalidParameter, "アトリビュートが指定されていません", place);
		for (auto attr = this->attributes_.begin(); attr != this->attributes_.end(); ++attr) {
			if (!attr->isExist()) MStatusException::throwIf(MStatus::kInvalidParameter, "指定されたアトリビュート:" + attr->fullpath_ + "が存在しないか、キーフレームが打てない設定になっています", place);
		}

		//変数にセットできた
		//計算
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
		MStatusException::throwIf(MGlobal::executeCommand("setKeyframe -attribute " + attr->attribute_ + " -time " + frame + " -value " + value + " -inTangentType " + this->smoothing_ + " -outTangentType " + this->smoothing_ + " " + attr->object_, ret_keys), "キーフレームの設定に失敗", place);
		return ret_keys;
	};


	try {
		for (auto attr = this->attributes_.begin(); attr != this->attributes_.end(); ++attr) {

			std::cout << "[setKeyframe]オブジェクト:" << attr->object_ << " アトリビュート:" << attr->attribute_ << std::endl;

			std::mt19937 mt(this->seed_);
			std::uniform_real_distribution<> rand_range(0.0, this->strength_);
			//オフセットの設定
			//ウィグラー開始フレーム時点の値
			double offset = 0.0;
			MStatusException::throwIf(MGlobal::executeCommand("getAttr " + attr->fullpath_, offset), "アトリビュート:" + attr->fullpath_ + "の取得に失敗", place);
			
			//開始点に打つ
			setKeyframe(attr, this->start_, offset);
			setKeyframe(attr, this->end_, offset);
			
			for (int frame = this->start_ + this->period_; frame < this->end_; frame += this->period_) {
				//乱数を発生させる
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

	std::cout << "ふるぱす" << fullpath << std::endl;
	MStatusException::throwIf(fullpath.split('.', ret), "アトリビュートのspiltに失敗", place);
	if (ret.length() != 2) MStatusException::throwIf(MStatus::kFailure, "アトリビュートのspiltの結果が異常", place);

	//XSS対策まがい
	for (unsigned int i = 0; i < ret.length(); i++) {
		MStringArray tmp;
		MStatusException::throwIf(fullpath.split(' ', tmp), "チェック段階のアトリビュート名のspiltに失敗", place);
		if (tmp.length() != 1) MStatusException::throwIf(MStatus::kInvalidParameter, "アトリビュート名が不適切です", place);
	}
	return AttributePair(fullpath, ret[0], ret[1]);
}

bool wlib::WigglerCommand::AttributePair::isExist(void) const
{
	MString place("wlib::WigglerCommand::AttributePair::isExist");
	int ret_cmd = 0;
	std::cout << "オブジェクト:" << this->object_ << " アトリビュート:" << this->attribute_ << std::endl;

	MStatusException::throwIf(MGlobal::executeCommand("objExists " + this->object_, ret_cmd), "existsに失敗", place);
	if (!ret_cmd) { std::cout << "no exists object" << std::endl; return false; }
	MStatusException::throwIf(MGlobal::executeCommand("attributeQuery -node " + this->object_ + " -exists " + this->attribute_, ret_cmd), "attributeQueryに失敗", place);
	if (!ret_cmd) return false;
	MStatusException::throwIf(MGlobal::executeCommand("attributeQuery -node " + this->object_ + " -keyable " + this->attribute_, ret_cmd), "attributeQueryに失敗", place);
	if (!ret_cmd) return false;

	return true;
}
