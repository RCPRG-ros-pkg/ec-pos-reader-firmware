#pragma once

#include <cstdint>

namespace hohner {

class Position
{
public:
	using ValueType = int;

	explicit constexpr Position(ValueType value)
		:	_value(value)
	{}

	constexpr ValueType value() const
	{
		return _value;
	}

	constexpr bool operator<(Position other) noexcept
	{
		return _value < other._value;
	}

	constexpr bool operator==(Position other) const noexcept
	{
		return _value == other._value;
	}

	constexpr bool operator!=(Position other) const noexcept
	{
		return !(*this == other);
	}

private:
	const ValueType _value;
};

} // namespace hohner
