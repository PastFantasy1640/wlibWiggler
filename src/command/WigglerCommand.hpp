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
		virtual MStatus redoIt() override;

		static void* creator();

	protected:

	private:

		int seed_;
		int period_;
		double strength_;
		int start_;
		int end_;
		int smoothing_;
		MStringArray attribute_;


	};

};

inline void * wlib::WigglerCommand::creator()
{
	return new WigglerCommand();
}

#endif //end of include guard
