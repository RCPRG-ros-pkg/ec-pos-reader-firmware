#pragma once

namespace component {

enum class LogicDesign
{
	ActiveHigh,
	ActiveLow
};

template<typename TPin, LogicDesign TLogicDesign>
class LED
{
public:
	LED(TPin& pin)
		:	_pin(pin)
	{

	}

	void turnOn()
	{
		if constexpr(TLogicDesign == LogicDesign::ActiveHigh)
		{
			_pin.write(true);
		}
		else
		{
			_pin.write(false);
		}
	}

	void turnOff()
	{
		if constexpr(TLogicDesign == LogicDesign::ActiveHigh)
		{
			_pin.write(false);
		}
		else
		{
			_pin.write(true);
		}
	}

	bool isTurnedOn() const
	{
		if constexpr(TLogicDesign == LogicDesign::ActiveHigh)
		{
			return _pin.read() ? true : false;
		}
		else
		{
			return _pin.read() ? false : true;
		}
	}

	bool isTurnedOff() const
	{
		if constexpr(TLogicDesign == LogicDesign::ActiveHigh)
		{
			return _pin.read() ? false : true;
		}
		else
		{
			return _pin.read() ? true : false;
		}
	}

private:
	TPin& _pin;
};

} // namespace component
