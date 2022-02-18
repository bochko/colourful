// colourful.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <array>

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
		static constexpr auto esc = '\033';
		static constexpr auto brak = '[';
		static constexpr auto term_seq = 'm';
		static constexpr auto term_char = '\0';
		static constexpr auto value = parameter_pack_to_array<esc, brak, arr[Index]..., term_seq, term_char>::value;
	};
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

}

static constexpr auto reset = colourful::control_sequence::control_sequence<0>{}; 
static constexpr auto red = colourful::control_sequence::control_sequence<31>{};