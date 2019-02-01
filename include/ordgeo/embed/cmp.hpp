/// @file  cmp.hpp
/// @brief Declarations for embedding methods using classic ordinal constraints

#pragma once
#ifndef ORDGEO_EMBED_CMP_HPP
#define ORDGEO_EMBED_CMP_HPP

#include <ordgeo/config.hpp>
#include <ordgeo/core/triplets.hpp>
#include <ordgeo/embed/embed.hpp>
#include <Eigen/Dense>
#include <vector>

namespace ORDGEO_NAMESPACE {
namespace embed {

/// Embeds a dataset using Soft Ordinal Embedding.
/// This method uses the margin parameter to set the scale. A default value of
/// 0.1 will be used if no margin is provided.
///
/// Citation: Y. Terada and U. von Luxburg, "Local ordinal embedding," presented
/// at the Proceedings of the 31st International Conference on Machine Learning,
/// 2014.
EmbedResult embedCmpWithSOE(std::vector<ORDGEO_NAMESPACE::core::CmpConstraint> cons,
	const Eigen::MatrixXd& X0, EmbedConfig config);

} // end namespace embed
} // end namespace ORDGEO_NAMESPACE
#endif /* ORDGEO_EMBED_CMP_HPP */
