/// @file  triplets.cpp
/// @brief Definitions related to triplet comparisons.

#include <ordgeo/core/triplets.hpp>

using std::istream;
using std::ostream;
using std::string;

namespace ORDGEO_NAMESPACE {
namespace core {

/// Convert a comparison outcome to a string representation.
string to_string(Cmp cmp) {
	switch (cmp) {
		case AB_LT_AC:
			return "<";
		case AC_LT_AB:
			return ">";
		case AB_EQ_AC:
			return "=";
		case AB_NCMP:
			return "nab";
		case AC_NCMP:
			return "nac";
		case AB_AC_NCMP:
			return "nabc";
	}
	return "<invalid>";
}

/// Convert a comparison outcome from a string representation or throw
/// std::invalid_argument.
Cmp stocmp(string str) {
	if (str == "<") {
		return AB_LT_AC;
	} else if (str == ">") {
		return AC_LT_AB;
	} else if (str == "=") {
		return AB_EQ_AC;
	} else if (str == "nab") {
		return AB_NCMP;
	} else if (str == "nac") {
		return AC_NCMP;
	} else if (str == "nabc") {
		return AB_AC_NCMP;
	}
	throw std::invalid_argument("No Cmp value has string \"" + str + "\"");
}

/// Stream a comparison to output
ostream& operator<<(ostream& os, const CmpOutcome& cmp) {
	os << cmp.a << "," << cmp.b << "," << cmp.c << "," << to_string(cmp.cmp);
	return os;
}

/// Stream a comparison from input
istream& operator>>(istream& is, CmpOutcome& cmp) {
	string str;
	char comma; // ignored; may not be a comma
	is >> cmp.a >> comma >> cmp.b >> comma >> cmp.c >> comma >> str;
	if (is) {
		cmp.cmp = stocmp(str);
	}
	return is;
}

} // end namespace core
} // end namespace ORDGEO_NAMESPACE
