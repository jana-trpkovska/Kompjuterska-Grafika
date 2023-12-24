namespace glm{
namespace detail
{
	template<length_t L, typename T, qualifier Q>
	struct compute_rgbToSrgb
	{
		GLM_FUNC_QUALIFIER static vec<L, T, Q> call(vec<L, T, Q> const& ColorRGB, T GammaCorrection)
		{
			vec<L, T, Q> const ClampedColor(clamp(ColorRGB, static_cast<T>(0), static_cast<T>(1)));

			return mix(
				pow(ClampedColor, vec<L, T, Q>(GammaCorrection)) * static_cast<T>(1.055) - static_cast<T>(0.055),
				ClampedColor * static_cast<T>(12.92),
				lessThan(ClampedColor, vec<L, T, Q>(static_cast<T>(0.0031308))));
		}
	};

	template<typename T, qualifier Q>
	struct compute_rgbToSrgb<4, T, Q>
	{
		GLM_FUNC_QUALIFIER static vec<4, T, Q> call(vec<4, T, Q> const& ColorRGB, T GammaCorrection)
		{
			return vec<4, T, Q>(compute_rgbToSrgb<3, T, Q>::call(vec<3, T, Q>(ColorRGB), GammaCorrection), ColorRGB.w);
		}
	};

	template<length_t L, typename T, qualifier Q>
	struct compute_srgbToRgb
	{
		GLM_FUNC_QUALIFIER static vec<L, T, Q> call(vec<L, T, Q> const& ColorSRGB, T Gamma)
		{
			return mix(
				pow((ColorSRGB + static_cast<T>(0.055)) * static_cast<T>(0.94786729857819905213270142180095), vec<L, T, Q>(Gamma)),
				ColorSRGB * static_cast<T>(0.07739938080495356037151702786378),
				lessThanEqual(ColorSRGB, vec<L, T, Q>(static_cast<T>(0.04045))));
		}
	};

	template<typename T, qualifier Q>
	struct compute_srgbToRgb<4, T, Q>
	{
		GLM_FUNC_QUALIFIER static vec<4, T, Q> call(vec<4, T, Q> const& ColorSRGB, T Gamma)
		{
			return vec<4, T, Q>(compute_srgbToRgb<3, T, Q>::call(vec<3, T, Q>(ColorSRGB), Gamma), ColorSRGB.w);
		}
	};

	template<typename T, qualifier Q, bool isInteger>
	class compute_YCoCgR {
	public:
		static GLM_FUNC_QUALIFIER vec<3, T, Q> rgb2YCoCgR
		(
			vec<3, T, Q> const& rgbColor
		)
		{
			vec<3, T, Q> result;
			result.x/*Y */ = rgbColor.g * static_cast<T>(0.5) + (rgbColor.r + rgbColor.b) * static_cast<T>(0.25);
			result.y/*Co*/ = rgbColor.r - rgbColor.b;
			result.z/*Cg*/ = rgbColor.g - (rgbColor.r + rgbColor.b) * static_cast<T>(0.5);
			return result;
		}

		static GLM_FUNC_QUALIFIER vec<3, T, Q> YCoCgR2rgb
		(
			vec<3, T, Q> const& YCoCgRColor
		)
		{
			vec<3, T, Q> result;
			T tmp = YCoCgRColor.x - (YCoCgRColor.z * static_cast<T>(0.5));
			result.g = YCoCgRColor.z + tmp;
			result.b = tmp - (YCoCgRColor.y * static_cast<T>(0.5));
			result.r = result.b + YCoCgRColor.y;
			return result;
		}
	};

	template<typename T, qualifier Q>
	class compute_YCoCgR<T, Q, true> {
	public:
		static GLM_FUNC_QUALIFIER vec<3, T, Q> rgb2YCoCgR
		(
			vec<3, T, Q> const& rgbColor
		)
		{
			vec<3, T, Q> result;
			result.y/*Co*/ = rgbColor.r - rgbColor.b;
			T tmp = rgbColor.b + (result.y >> 1);
			result.z/*Cg*/ = rgbColor.g - tmp;
			result.x/*Y */ = tmp + (result.z >> 1);
			return result;
		}

		static GLM_FUNC_QUALIFIER vec<3, T, Q> YCoCgR2rgb
		(
			vec<3, T, Q> const& YCoCgRColor
		)
		{
			vec<3, T, Q> result;
			T tmp = YCoCgRColor.x - (YCoCgRColor.z >> 1);
			result.g = YCoCgRColor.z + tmp;
			result.b = tmp - (YCoCgRColor.y >> 1);
			result.r = result.b + YCoCgRColor.y;
			return result;
		}
	};
}//namespace detail

	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, T, Q> convertLinearToSRGB(vec<L, T, Q> const& ColorLinear)
	{
		return detail::compute_rgbToSrgb<L, T, Q>::call(ColorLinear, static_cast<T>(0.41666));
	}

	// Based on Ian Taylor http://chilliant.blogspot.fr/2012/08/srgb-approximations-for-hlsl.html
	template<>
	GLM_FUNC_QUALIFIER vec<3, float, lowp> convertLinearToSRGB(vec<3, float, lowp> const& ColorLinear)
	{
		vec<3, float, lowp> S1 = sqrt(ColorLinear);
		vec<3, float, lowp> S2 = sqrt(S1);
		vec<3, float, lowp> S3 = sqrt(S2);
		return 0.662002687f * S1 + 0.684122060f * S2 - 0.323583601f * S3 - 0.0225411470f * ColorLinear;
	}

	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, T, Q> convertLinearToSRGB(vec<L, T, Q> const& ColorLinear, T Gamma)
	{
		return detail::compute_rgbToSrgb<L, T, Q>::call(ColorLinear, static_cast<T>(1) / Gamma);
	}

	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, T, Q> convertSRGBToLinear(vec<L, T, Q> const& ColorSRGB)
	{
		return detail::compute_srgbToRgb<L, T, Q>::call(ColorSRGB, static_cast<T>(2.4));
	}

	template<length_t L, typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<L, T, Q> convertSRGBToLinear(vec<L, T, Q> const& ColorSRGB, T Gamma)
	{
		return detail::compute_srgbToRgb<L, T, Q>::call(ColorSRGB, Gamma);
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> rgbColor(const vec<3, T, Q>& hsvColor)
	{
		vec<3, T, Q> hsv = hsvColor;
		vec<3, T, Q> rgbColor;

		if(hsv.y == static_cast<T>(0))
			// achromatic (grey)
			rgbColor = vec<3, T, Q>(hsv.z);
		else
		{
			T sector = floor(hsv.x * (T(1) / T(60)));
			T frac = (hsv.x * (T(1) / T(60))) - sector;
			// factorial part of h
			T o = hsv.z * (T(1) - hsv.y);
			T p = hsv.z * (T(1) - hsv.y * frac);
			T q = hsv.z * (T(1) - hsv.y * (T(1) - frac));

			switch(int(sector))
			{
			default:
			case 0:
				rgbColor.r = hsv.z;
				rgbColor.g = q;
				rgbColor.b = o;
				break;
			case 1:
				rgbColor.r = p;
				rgbColor.g = hsv.z;
				rgbColor.b = o;
				break;
			case 2:
				rgbColor.r = o;
				rgbColor.g = hsv.z;
				rgbColor.b = q;
				break;
			case 3:
				rgbColor.r = o;
				rgbColor.g = p;
				rgbColor.b = hsv.z;
				break;
			case 4:
				rgbColor.r = q;
				rgbColor.g = o;
				rgbColor.b = hsv.z;
				break;
			case 5:
				rgbColor.r = hsv.z;
				rgbColor.g = o;
				rgbColor.b = p;
				break;
			}
		}

		return rgbColor;
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> hsvColor(const vec<3, T, Q>& rgbColor)
	{
		vec<3, T, Q> hsv = rgbColor;
		float Min   = min(min(rgbColor.r, rgbColor.g), rgbColor.b);
		float Max   = max(max(rgbColor.r, rgbColor.g), rgbColor.b);
		float Delta = Max - Min;

		hsv.z = Max;

		if(Max != static_cast<T>(0))
		{
			hsv.y = Delta / hsv.z;
			T h = static_cast<T>(0);

			if(rgbColor.r == Max)
				// between yellow & magenta
				h = static_cast<T>(0) + T(60) * (rgbColor.g - rgbColor.b) / Delta;
			else if(rgbColor.g == Max)
				// between cyan & yellow
				h = static_cast<T>(120) + T(60) * (rgbColor.b - rgbColor.r) / Delta;
			else
				// between magenta & cyan
				h = static_cast<T>(240) + T(60) * (rgbColor.r - rgbColor.g) / Delta;

			if(h < T(0))
				hsv.x = h + T(360);
			else
				hsv.x = h;
		}
		else
		{
			// If r = g = b = 0 then s = 0, h is undefined
			hsv.y = static_cast<T>(0);
			hsv.x = static_cast<T>(0);
		}

		return hsv;
	}

	template<typename T>
	GLM_FUNC_QUALIFIER mat<4, 4, T, defaultp> saturation(T const s)
	{
		vec<3, T, defaultp> const rgbw = vec<3, T, defaultp>(T(0.2126), T(0.7152), T(0.0722));

		vec<3, T, defaultp> const col((T(1) - s) * rgbw);

		mat<4, 4, T, defaultp> result(T(1));
		result[0][0] = col.x + s;
		result[0][1] = col.x;
		result[0][2] = col.x;
		result[1][0] = col.y;
		result[1][1] = col.y + s;
		result[1][2] = col.y;
		result[2][0] = col.z;
		result[2][1] = col.z;
		result[2][2] = col.z + s;

		return result;
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> saturation(const T s, const vec<3, T, Q>& color)
	{
		return vec<3, T, Q>(saturation(s) * vec<4, T, Q>(color, T(0)));
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<4, T, Q> saturation(const T s, const vec<4, T, Q>& color)
	{
		return saturation(s) * color;
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER T luminosity(const vec<3, T, Q>& color)
	{
		vec<3, T, Q> const tmp(0.33, 0.59, 0.11);
		return dot(color, tmp);
	}


	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> rgb2YCoCg(vec<3, T, Q> const& rgbColor)
	{
		vec<3, T, Q> result;
		result.x/*Y */ =   rgbColor.r / T(4) + rgbColor.g / T(2) + rgbColor.b / T(4);
		result.y/*Co*/ =   rgbColor.r / T(2) + rgbColor.g * T(0) - rgbColor.b / T(2);
		result.z/*Cg*/ = - rgbColor.r / T(4) + rgbColor.g / T(2) - rgbColor.b / T(4);
		return result;
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> YCoCg2rgb(vec<3, T, Q> const& YCoCgColor)
	{
		vec<3, T, Q> result;
		result.r = YCoCgColor.x + YCoCgColor.y - YCoCgColor.z;
		result.g = YCoCgColor.x				   + YCoCgColor.z;
		result.b = YCoCgColor.x - YCoCgColor.y - YCoCgColor.z;
		return result;
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> rgb2YCoCgR(vec<3, T, Q> const& rgbColor)
	{
		return detail::compute_YCoCgR<T, Q, std::numeric_limits<T>::is_integer>::rgb2YCoCgR(rgbColor);
	}

	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> YCoCgR2rgb(vec<3, T, Q> const& YCoCgRColor)
	{
		return detail::compute_YCoCgR<T, Q, std::numeric_limits<T>::is_integer>::YCoCgR2rgb(YCoCgRColor);
	}

	// Converting pure hue to RGB
	template<typename T>
	vec<3, T, defaultp> HUEtoRGB(float H)
	{
		T const R = abs(H * 6 - 3) - 1;
		T const G = 2 - abs(H * 6 - 2);
		T const B = 2 - abs(H * 6 - 4);
		return saturate(float3(R,G,B));
	}

	// Converting RGB to hue/chroma/value
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> RGBtoHCV(vec<3, T, Q> const& RGB)
	{
		float const Epsilon = 1e-10;
		// Based on work by Sam Hocevar and Emil Persson
		float4 P = (RGB.g < RGB.b) ? float4(RGB.bg, -1.0, 2.0/3.0) : float4(RGB.gb, 0.0, -1.0/3.0);
		float4 Q = (RGB.r < P.x) ? float4(P.xyw, RGB.r) : float4(RGB.r, P.yzx);
		float C = Q.x - min(Q.w, Q.y);
		float H = abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
		return float3(H, C, Q.x);
	}

	// Converting HSV to RGB
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> HSVtoRGB(vec<3, T, Q> const& HSV)
	{
		vec<3, T, Q> const RGB = HUEtoRGB(HSV.x);
		return ((RGB - 1) * HSV.y + 1) * HSV.z;
	}

	// Converting HSL to RGB
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> HSLtoRGB(vec<3, T, Q> const& HSL)
	{
		vec<3, T, Q> const RGB = HUEtoRGB(HSL.x);
		float C = (1 - abs(2 * HSL.z - 1)) * HSL.y;
		return (RGB - 0.5) * C + HSL.z;
	}

	// Converting HCY to RGB
	// The weights of RGB contributions to luminance.
	// Should sum to unity.
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> HCYtoRGB(vec<3, T, Q> const& HCY)
	{
		vec<3, T, Q> const HCYwts(0.299, 0.587, 0.114);
		vec<3, T, Q> const RGB = HUEtoRGB(HCY.x);
		T const Z = dot(RGB, HCYwts);

		if(HCY.z < Z)
		{
			HCY.y *= HCY.z / Z;
		}
		else if(Z < 1)
		{
			HCY.y *= (1 - HCY.z) / (1 - Z);
		}

		return (RGB - Z) * HCY.y + HCY.z;
	}

	//Converting HCL to RGB
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> HCLtoRGB(vec<3, T, Q> const& HCL)
	{
		float HCLgamma = 3;
		float HCLy0 = 100;
		float HCLmaxL = 0.530454533953517; // == exp(HCLgamma / HCLy0) - 0.5
		float PI = 3.1415926536;

		vec<3, T, Q> RGB(0, 0, 0);
		if(HCL.z != 0)
		{
			float H = HCL.x;
			float C = HCL.y;
			float L = HCL.z * HCLmaxL;
			float Q = exp((1 - C / (2 * L)) * (HCLgamma / HCLy0));
			float U = (2 * L - C) / (2 * Q - 1);
			float V = C / Q;
			float T = tan((H + min(frac(2 * H) / 4, frac(-2 * H) / 8)) * PI * 2);
			H *= 6;
			if (H <= 1)
			{
				RGB.r = 1;
				RGB.g = T / (1 + T);
			}
			else if (H <= 2)
			{
				RGB.r = (1 + T) / T;
				RGB.g = 1;
			}
			else if (H <= 3)
			{
				RGB.g = 1;
				RGB.b = 1 + T;
			}
			else if (H <= 4)
			{
				RGB.g = 1 / (1 + T);
				RGB.b = 1;
			}
			else if (H <= 5)
			{
				RGB.r = -1 / T;
				RGB.b = 1;
			}
			else
			{
				RGB.r = 1;
				RGB.b = -T;
			}
			RGB = RGB * V + U;
		}
		return RGB;
	}

	// Converting RGB to HSV
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> RGBtoHSV(vec<3, T, Q> const& RGB)
	{
		vec<3, T, Q> HCV = RGBtoHCV(RGB);
		float S = HCV.y / (HCV.z + Epsilon);
		return vec<3, T, Q>(HCV.x, S, HCV.z);
	}
/*
	vec3 rgb2hsv(vec3 c)
	{
	    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
	    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

	    float d = q.x - min(q.w, q.y);
	    float e = 1.0e-10;
	    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
	}

	Update: ​Emil Persson suggests using the ternary operator explicitly to force compilers into using a fast conditional move instruction:

	    vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
	    vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);

	And because a lot of people get it wrong, too, here is the reverse operation in GLSL. It is the algorithm almost everyone uses (or should use):

	vec3 hsv2rgb(vec3 c)
	{
	    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
	}
*/

	// Converting RGB to HSL
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> RGBtoHSL(vec<3, T, Q> const& RGB)
	{
		vec<3, T, Q> HCV = RGBtoHCV(RGB);
		float L = HCV.z - HCV.y * 0.5;
		float S = HCV.y / (1 - abs(L * 2 - 1) + Epsilon);
		return float3(HCV.x, S, L);
	}

	// Converting RGB to HCY
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> RGBtoHCY(vec<3, T, Q> const& RGB)
	{
		// Corrected by David Schaeffer
		float3 HCV = RGBtoHCV(RGB);
		float Y = dot(RGB, HCYwts);
		float Z = dot(HUEtoRGB(HCV.x), HCYwts);

		if (Y < Z)
		{
			HCV.y *= Z / (Epsilon + Y);
		}
		else
		{
			HCV.y *= (1 - Z) / (Epsilon + 1 - Y);
		}

		return float3(HCV.x, HCV.y, Y);
	}

	// Converting RGB to HCL
	template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER vec<3, T, Q> RGBtoHCL(vec<3, T, Q> const& RGB)
	{
		vec<3, T, Q> HCL;
		T H = 0;
		T U = min(RGB.r, min(RGB.g, RGB.b));
		T V = max(RGB.r, max(RGB.g, RGB.b));
		T Q = HCLgamma / HCLy0;
		HCL.y = V - U;

		if (HCL.y != 0)
		{
			H = atan2(RGB.g - RGB.b, RGB.r - RGB.g) / PI;
			Q *= U / V;
		}
		Q = exp(Q);
		HCL.x = frac(H / 2 - min(frac(H), frac(-H)) / 6);
		HCL.y *= Q;
		HCL.z = lerp(-U, V, Q) / (HCLmaxL * 2);

		return HCL;
	}
}//namespace glm
