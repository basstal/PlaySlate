#pragma once


namespace ActActionSequence
{
	class ActActionStaticUtil
	{
	public:
		static float GetBoundsZOffset(const FBoxSphereBounds& Bounds)
		{
			// Return half the height of the bounds plus one to avoid ZFighting with the floor plane
			return Bounds.BoxExtent.Z + 1;
		}

		static FFrameNumber DiscreteExclusiveUpper(const TRangeBound<FFrameNumber>& InUpperBound)
		{
			check(!InUpperBound.IsOpen());

			// Add one for inclusive upper bounds since they finish on the next subsequent frame
			static const int32 Offsets[] = {0, 1};
			const int32 OffsetIndex = (int32)InUpperBound.IsInclusive();

			return InUpperBound.GetValue() + Offsets[OffsetIndex];
		}

		static FFrameNumber DiscreteInclusiveLower(const TRangeBound<FFrameNumber>& InLowerBound)
		{
			check(!InLowerBound.IsOpen());

			// Add one for exclusive lower bounds since they start on the next subsequent frame
			static const int32 Offsets[] = {0, 1};
			const int32 OffsetIndex = (int32)InLowerBound.IsExclusive();

			return InLowerBound.GetValue() + Offsets[OffsetIndex];
		}
	};
}
