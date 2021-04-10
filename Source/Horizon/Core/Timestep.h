#pragma once

namespace Horizon
{
	class Timestep
	{
	public:
		Timestep(float deltaTimeInSeconds = 0.0f) : mDeltaTimeInSeconds(deltaTimeInSeconds) {}
		float Seconds() const { return mDeltaTimeInSeconds; }
		float Milliseconds() const { return mDeltaTimeInSeconds * 1000.0f; }
	private:
		float mDeltaTimeInSeconds;
	};
}
