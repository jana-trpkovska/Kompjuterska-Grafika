/// @ref ext_vector_color
/// @file glm/ext/vector_color.hpp
///
/// @defgroup ext_vector_color GLM_EXT_vector_color
/// @ingroup ext
///
/// Exposes comparison functions for vector types that take a user defined epsilon values.
///
/// Include <glm/ext/vector_color.hpp> to use the features of this extension.
///
/// @see ext_vector_float4
/// @see ext_vector_relational

#pragma once

// Dependencies
#include "../detail/qualifier.hpp"

#if GLM_MESSAGES == GLM_ENABLE && !defined(GLM_EXT_INCLUDED)
#	pragma message("GLM: GLM_EXT_vector_color extension included")
#endif

namespace glm
{
	/// @addtogroup ext_vector_color
	/// @{

	/// Convert a linear color to sRGB color using a standard gamma correction.
	/// IEC 61966-2-1:1999 / Rec. 709 specification https://www.w3.org/Graphics/Color/srgb
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> convertLinearToSRGB(vec<L, T, Q> const& ColorLinear);

	/// Convert a linear color to sRGB color using a custom gamma correction.
	/// IEC 61966-2-1:1999 / Rec. 709 specification https://www.w3.org/Graphics/Color/srgb
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> convertLinearToSRGB(vec<L, T, Q> const& ColorLinear, T Gamma);

	/// Convert a sRGB color to linear color using a standard gamma correction.
	/// IEC 61966-2-1:1999 / Rec. 709 specification https://www.w3.org/Graphics/Color/srgb
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> convertSRGBToLinear(vec<L, T, Q> const& ColorSRGB);

	/// Convert a sRGB color to linear color using a custom gamma correction.
	// IEC 61966-2-1:1999 / Rec. 709 specification https://www.w3.org/Graphics/Color/srgb
	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_DECL vec<L, T, Q> convertSRGBToLinear(vec<L, T, Q> const& ColorSRGB, T Gamma);

	/// Converts a color from HSV color space to its color in RGB color space.
	/// @see gtx_color_space
	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<3, T, Q> rgbColor(
		vec<3, T, Q> const& hsvValue);

	/// Converts a color from RGB color space to its color in HSV color space.
	/// @see gtx_color_space
	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<3, T, Q> hsvColor(
		vec<3, T, Q> const& rgbValue);

	/// Build a saturation matrix.
	/// @see gtx_color_space
	template<typename T>
	GLM_FUNC_DECL mat<4, 4, T, defaultp> saturation(
		T const s);

	/// Modify the saturation of a color.
	/// @see gtx_color_space
	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<3, T, Q> saturation(
		T const s,
		vec<3, T, Q> const& color);

	/// Modify the saturation of a color.
	/// @see gtx_color_space
	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<4, T, Q> saturation(
		T const s,
		vec<4, T, Q> const& color);

	/// Compute color luminosity associating ratios (0.33, 0.59, 0.11) to RGB canals.
	/// @see gtx_color_space
	template<typename T, qualifier Q>
	GLM_FUNC_DECL T luminosity(
		vec<3, T, Q> const& color);


	/// Convert a color from RGB color space to YCoCg color space.
	/// @see gtx_color_space_YCoCg
	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<3, T, Q> rgb2YCoCg(
		vec<3, T, Q> const& rgbColor);

	/// Convert a color from YCoCg color space to RGB color space.
	/// @see gtx_color_space_YCoCg
	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<3, T, Q> YCoCg2rgb(
		vec<3, T, Q> const& YCoCgColor);

	/// Convert a color from RGB color space to YCoCgR color space.
	/// @see "YCoCg-R: A Color Space with RGB Reversibility and Low Dynamic Range"
	/// @see gtx_color_space_YCoCg
	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<3, T, Q> rgb2YCoCgR(
		vec<3, T, Q> const& rgbColor);

	/// Convert a color from YCoCgR color space to RGB color space.
	/// @see "YCoCg-R: A Color Space with RGB Reversibility and Low Dynamic Range"
	/// @see gtx_color_space_YCoCg
	template<typename T, qualifier Q>
	GLM_FUNC_DECL vec<3, T, Q> YCoCgR2rgb(
		vec<3, T, Q> const& YCoCgColor);

	/// @}
}//namespace glm

#include "vector_color.inl"
