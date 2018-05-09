// #pragma once

// #include <tuple>

// #include "embxx/error/ErrorCode.h"
// #include "embxx/util/StaticFunction.h"

// #include "device/SSIMaster.hpp"
// #include "device/OutputPin.hpp"

// #include "component/SSIEncoder.hpp"
// #include "component/LED.hpp"

// #include "app/common/EventLoop.hpp"

// #include "app/encoders/Encoder0.hpp"
// #include "app/encoders/Encoder1.hpp"

// namespace app {
// namespace encoders {

// class EncoderMgr
// {
// public:
// 	using ErrorCode = embxx::error::ErrorCode;
// 	using Position = component::Position;
// 	using EventLoop = common::EventLoop;

// 	using Encoders = std::tuple<
// 		Encoder0
// 		// ,Encoder1
// 	>;

// 	//! Constructor
// 	explicit EncoderMgr(EventLoop& eventLoop);

// 	//! Captures current encoder position or returns an error on fail
// 	void captureInputs(Position& position, ErrorCode& ec);

// 	template<typename THandler>
// 	void asyncCaptureInputs(Position* destPosition, THandler&& handler)
// 	{
// 		// Submodule should not be busy and should be active
// 		auto& encoder = std::get<0>(_encoders);
// 		assert(!encoder.isBusy());
// 		assert(encoder.isActive());

// 		encoder.asyncCaptureInputs(destPosition,
// 			std::forward<THandler>(handler));
// 	}

// 	//! Returns, whether module is in fail state or not
// 	bool isFailed() const;

// 	//! Returns, whether module is in active state or not
// 	bool isActive() const;

// private:
// 	Encoders _encoders;
// };

// } // namespace encoders
// } // namespace app
