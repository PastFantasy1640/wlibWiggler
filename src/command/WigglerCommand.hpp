/////////////////////////////////////////////////////
//WigglerCommand.hpp
//
//Copyright (c) 2017 Shotaro Shirao
//White Library
/////////////////////////////////////////////////////

#pragma once
#ifndef WLIB_WIGGLERCOMMAND_HPP_
#define WLIB_WIGGLERCOMMAND_HPP_

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <vector>

namespace wlib {

	/** WigglerCommand CLASS
	* ê‡ñæ
	*/
	class WigglerCommand : public MPxCommand{
	public:
		//////////////////////////////
		// PUBLIC MEMBER CONSTANT
		//////////////////////////////


		//////////////////////////////
		// PUBLIC MEMBER VALIABLES
		//////////////////////////////


		//////////////////////////////
		// PUBLIC MEMBER FUNCTION
		//////////////////////////////

		/** Constructor
		*/
		WigglerCommand();

		/** Destructor
		*/
		~WigglerCommand();

		virtual MStatus doIt(const MArgList& args) override;
		virtual MStatus redoIt(void) override;
		virtual MStatus undoIt(void) override;
		virtual bool isUndoable(void) const override;

		static void* creator();

	protected:

	private:

		struct AttributePair {
			const MString fullpath_;
			const MString object_;
			const MString attribute_;
			AttributePair(const MString & fullpath, const MString & object, const MString & attribute);
			static AttributePair create(const MString & fullpath);
			bool isExist(void) const;
		};

		int seed_;
		int period_;
		double strength_;
		int start_;
		int end_;
		MString smoothing_;
		
		std::vector<AttributePair> attributes_;

		typedef std::pair<int, double> Key;
		std::vector<Key> keys_;
		

	};

};

inline void * wlib::WigglerCommand::creator()
{
	return new WigglerCommand();
}

#endif //end of include guard
