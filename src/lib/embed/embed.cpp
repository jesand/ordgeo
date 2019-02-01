/// @file  embed.cpp
/// @brief Utility methods common to embedding routines.

#include <ordgeo/embed/embed.hpp>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

namespace ORDGEO_NAMESPACE {
namespace embed {

/// Raise an error, optionally also printing it if running in verbose mode
void EmbedErr::report(string message, bool verbose) {
	if (verbose) {
		cout << message << endl;
	}
	throw EmbedErr(message);
}

} // end namespace embed
} // end namespace ORDGEO_NAMESPACE
