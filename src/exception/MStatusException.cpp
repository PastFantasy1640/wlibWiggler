#include "MStatusException.hpp"
#include <string.h>

wlib::MStatusException::MStatusException(const MStatus & stat, const MString & message, const MString & place)
	: stat(stat), message(message), place(place)
{}

wlib::MStatusException::~MStatusException() {}

MString wlib::MStatusException::toString(void) const {
	//FORMAT | [STAT] PLACE : MESSAGE
	std::string stat_str;
	switch (stat.statusCode()) {
	case MStatus::MStatusCode::kEndOfFile: stat_str = "EndOfFile"; break;
	case MStatus::MStatusCode::kFailure: stat_str = "Failure"; break;
	case MStatus::MStatusCode::kInsufficientMemory: stat_str = "InsufficientMemory"; break;
	case MStatus::MStatusCode::kInvalidParameter: stat_str = "InvalidParameter"; break;
	case MStatus::MStatusCode::kLicenseFailure: stat_str = "LicenseFailure"; break;
	case MStatus::MStatusCode::kNotFound: stat_str = "NotFound"; break;
	case MStatus::MStatusCode::kNotImplemented: stat_str = "NotImplemented"; break;
	case MStatus::MStatusCode::kSuccess: stat_str = "Success"; break;
	case MStatus::MStatusCode::kUnknownParameter: stat_str = "UnknownParameter"; break;
	//default: stat_str = "UNKNOWN"; break;
	}

	return MString(std::string("[" + stat_str + "] " + place.asChar() + " : " + message.asChar()).c_str());
}

void wlib::MStatusException::throwIf(const MStatus & stat, const MString & message, const MString & place) {
	if (stat.error()) throw MStatusException(stat, message, place);
}

