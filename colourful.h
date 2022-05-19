// colourful.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <array>
#include <string_view>
#include <tuple>
#include <concepts>

namespace colourful::constant {

	static constexpr auto Esc = '\x1B';
	static constexpr auto Csi = '[';
	static constexpr auto Mmm = 'm';
	
}

namespace colourful::consteval_chars {
namespace type {

template <unsigned... digits>
struct ToCharArray {
	static constexpr auto value = std::array<char, sizeof...(digits)>{('0' + digits)...};
};

template <unsigned remainder, unsigned... digits>
struct ToChars : ToChars<(remainder / 10), (remainder % 10), digits...>{};

template <unsigned... digits>
struct ToChars<0, digits...> : ToCharArray<digits...>{};

template <>
struct ToChars<0> : ToCharArray<0>{};
}

template <unsigned... digits>
static constexpr auto ToChars = type::ToChars<digits...>::value;

/* test */ static constexpr auto test = ToChars<19>;

}

namespace colourful::tparam {
namespace type {
	template <auto... args>
	struct ToArray {
		static constexpr auto value = std::array{ args... };
	};
}

template <auto... args>
static constexpr auto ToArray = type::ToArray<args...>::value;

/* test */ static constexpr auto test = ToArray<'1', '2', '3'>;

}

namespace colourful::combine {
namespace type {
template <
	auto element,
	auto array,
	typename IndexSequence = std::make_index_sequence<array.size()>>
struct Precede {};

template <
	auto element,
	auto array,
	std::size_t... Index>
struct Precede<element, array, std::index_sequence<Index...>> {
	static constexpr auto value = colourful::tparam::ToArray<
		element, array[Index]...
	>;
};

template <
	auto element,
	auto array,
	typename IndexSequence = std::make_index_sequence<array.size()>>
struct Trail {};

template <
	auto element,
	auto array,
	std::size_t... Index>
struct Trail<element, array, std::index_sequence<Index...>> {
	static constexpr auto value = colourful::tparam::ToArray<
		array[Index]..., element
	>;
};

}

template <auto element, auto array>
static constexpr auto Precede = type::Precede<element, array>::value;

template <auto element, auto array>
static constexpr auto Trail = type::Trail<element, array>::value;


/* test */ static constexpr auto testPrecede = Precede<'a', consteval_chars::ToChars<69>>;
/* test */ static constexpr auto testTrail = Trail<'a', consteval_chars::ToChars<69>>;

}

namespace colourful::ansi {
namespace type {
template <unsigned code>
class Format3Bit {
public:
	constexpr auto string_view() const noexcept {
		return std::string_view{value.begin(), value.end()};
	}
private:
	static constexpr auto value = combine::Trail<
	constant::Mmm,
	combine::Precede<
		constant::Esc, 
		combine::Precede<
			constant::Csi,
			consteval_chars::ToChars<code>>>>;
};

}

template <unsigned code>
static constexpr auto Format3Bit = type::Format3Bit<code>{};

}

static constexpr auto kReset = colourful::ansi::Format3Bit<0>;
static constexpr auto kRed = colourful::ansi::Format3Bit<31>;

namespace colourful::control_sequence {
namespace utility {
	/// <summary>
	/// Utility: Explode unsigned integer to a string
	/// </summary>
	template<unsigned... digits>
	struct map_to_chars {
		static constexpr auto value = std::array<char, sizeof...(digits)>{ ('0' + digits)... };
	};

	template<unsigned rem, unsigned... digits>
	struct explode_unsigned : explode_unsigned<(rem / 10), (rem % 10), digits...> {};

	template<unsigned... digits>
	struct explode_unsigned<0, digits...> : map_to_chars<digits...> {};

	struct ansi_char_def {
		static constexpr auto esc = '\x1B';
		static constexpr auto csi = '[';
		static constexpr auto m = 'm';
		static constexpr auto nul = '\0';
	};

	/// <summary>
	/// Utility: Embed an array of characters into a larger array containing it
	/// and surrounding control characters needed for terminal formatting sequences
	/// </summary>
	/// <typeparam name="ISeq"></typeparam>
	template <char... chars>
	struct parameter_pack_to_array {
		static constexpr auto value = std::array{ chars... };
	};

	template <
		auto arr,
		typename ISeq = decltype(std::make_index_sequence<arr.size()>())>
		struct implode_array_with_esc_sequence {};

	template <
		auto arr,
		std::size_t... Index>
		struct implode_array_with_esc_sequence<arr, std::index_sequence<Index...>> {
		static constexpr auto value 
			= parameter_pack_to_array<
				ansi_char_def::esc, 
				ansi_char_def::csi, 
				arr[Index]..., 
				ansi_char_def::m, 
				ansi_char_def::nul
			>::value;
	};

	template <
		auto lhs_array,
		auto rhs_array,
		typename LhsISeq = decltype(std::make_index_sequence<lhs_array.size()>()),
		typename RhsISeq = decltype(std::make_index_sequence<rhs_array.size()>())
	> struct lhsrhsarray {};

	template <
		auto lhs_array,
		auto rhs_array,
		std::size_t... LhsIndex,
		std::size_t... RhsIndex
	> struct lhsrhsarray<lhs_array, rhs_array, std::index_sequence<LhsIndex...>, std::index_sequence<RhsIndex...>> {
		static constexpr auto value = parameter_pack_to_array<
			lhs_array[LhsIndex]...,
			rhs_array[RhsIndex]...
		>::value;
	};

	// template <auto ...arrays>
	// consteval auto concat_arrays() {
	// 	constexpr std::size_t n_arrays = sizeof...(arrays);
	// 	constexpr auto aggregate = std::make_tuple(arrays...);
	// 	using return_type = std::array<char, arrays.size()...>;
	// 	auto ret = return_type{};
	// 	std::size_t ret_tracker = 0;
	// 	for (std::size_t over_aggregate = 0; over_aggregate < n_arrays; ++over_aggregate) {
	// 		ret[ret_tracker++] = std::get<over_aggregate>(aggregate);
	// 	}
	// 	return true;
	// }

	// constexpr auto a = concat_arrays <std::array<char, 2>{'g', 'h'}> ();


}

template <unsigned control>
class control_sequence {
public:
	consteval auto string_view() const noexcept -> std::string_view { 
		return std::string_view{ value.begin(), value.end() }; 
	}
private:
	static constexpr auto value = 
		utility::implode_array_with_esc_sequence<
			utility::explode_unsigned<control>::value
		>::value;
};

template <unsigned colour_code>
class ansi_sgr_colour_3bit {
public:
	consteval ansi_sgr_colour_3bit() = default;
	consteval auto string_view() const noexcept -> std::string_view {
		return std::string_view{ value.begin(), value.end() };
	}

	static constexpr auto value = utility::lhsrhsarray < std::array{ '\x1B', '[' }, utility::explode_unsigned<colour_code>::value > ::value;
};

}

static constexpr auto reset = colourful::control_sequence::control_sequence<0>{}; 
static constexpr auto red = colourful::control_sequence::control_sequence<31>{};

static constexpr auto reset_ = colourful::control_sequence::ansi_sgr_colour_3bit<0>::value;