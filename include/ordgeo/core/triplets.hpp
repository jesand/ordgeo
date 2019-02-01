/// @file  triplets.hpp
/// @brief Basic declarations for triplets.

#pragma once
#ifndef ORDGEO_CORE_ORACLE_HPP
#define ORDGEO_CORE_ORACLE_HPP

#include <ordgeo/config.hpp>
#include <iostream>

namespace ORDGEO_NAMESPACE {
namespace core {

/// The possible outcomes of a triple comparison.
enum Cmp {

	// dist(a,b) < dist(a,c)
	AB_LT_AC,

	// dist(a,c) < dist(a,b)
	AC_LT_AB,

	// dist(a,b) = dist(a,c)
	AB_EQ_AC,

	// dist(a,b) cannot be measured, but dist(a,c) can
	AB_NCMP,

	// dist(a,c) cannot be measured, but dist(a,b) can
	AC_NCMP,

	// comparison result cannot be determined for this triple
	AB_AC_NCMP
};

/// Convert a comparison outcome to a string representation
std::string to_string(Cmp cmp);

/// Convert a comparison outcome from a string representation or throw
/// std::invalid_argument.
Cmp stocmp(std::string str);

/// A comparison outcome.
struct CmpOutcome {

	/// Default constructor
	CmpOutcome() {}

	/// Constructor
	CmpOutcome(size_t a, size_t b, size_t c, Cmp cmp)
		: a(a), b(b), c(c), cmp(cmp) {}

	/// Equality test
	bool operator==(const CmpOutcome& copy) const {
		return a == copy.a && b == copy.b && c == copy.c && cmp == copy.cmp;
	}

	/// Give the outcome with the specified order for b and c
	Cmp outcome(size_t b, size_t c) {
		if (b == c) {
			return AB_EQ_AC;
		} else if (b == this->b && c == this->c) {
			return cmp;
		} else if (c == this->b && b == this->c) {
			switch(cmp) {
			case AB_LT_AC:
				return AC_LT_AB;
			case AB_NCMP:
				return AC_NCMP;
			case AC_LT_AB:
				return AB_LT_AC;
			case AC_NCMP:
				return AB_NCMP;
			case AB_EQ_AC:
			case AB_AC_NCMP:
				return cmp;
			}
		}
		return AB_AC_NCMP; // answer not inferable from this comparison
	}

	size_t a, b, c;
	Cmp cmp;
};

/// Stream a comparison to output
std::ostream& operator<<(std::ostream& os, const CmpOutcome& cmp);

/// Stream a comparison from input
std::istream& operator>>(std::istream& is, CmpOutcome& cmp);

/// An ordinal constraint specifying that dist(a,b) < dist(c,d).
struct CmpConstraint {

	/// Default constructor.
	CmpConstraint() : a(0), b(0), c(0), d(0) {}

	/// Constructor with four points
	CmpConstraint(size_t a, size_t b, size_t c, size_t d)
		: a(a), b(b), c(c), d(d) {}

	/// Constructor with three points: dist(a,b) < dist(a,c).
	CmpConstraint(size_t a, size_t b, size_t c)
		: CmpConstraint(a, b, c, a) {}

	/// Constructor from an oracle comparison
	CmpConstraint(const ORDGEO_NAMESPACE::core::CmpOutcome& cmp)
		: CmpConstraint(
			cmp.a,
			cmp.cmp == ORDGEO_NAMESPACE::core::AB_LT_AC ? cmp.b : cmp.c,
			cmp.cmp == ORDGEO_NAMESPACE::core::AB_LT_AC ? cmp.c : cmp.b) {}

	/// The objects being constrained
	size_t a, b, c, d;
};

} // end namespace core
} // end namespace ORDGEO_NAMESPACE
#endif /* ORDGEO_CORE_ORACLE_HPP */
