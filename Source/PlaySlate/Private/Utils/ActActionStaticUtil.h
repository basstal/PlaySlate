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
	};
}
