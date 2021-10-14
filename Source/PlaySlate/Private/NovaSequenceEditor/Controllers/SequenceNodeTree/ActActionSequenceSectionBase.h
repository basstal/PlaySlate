#pragma once

class FActActionSequenceSectionBase
{
public:
	virtual ~FActActionSequenceSectionBase() = default;

	virtual UObject* GetSectionObject() = 0;
	/**
	 * Allows each section to have it's own unique widget for advanced editing functionality
	 * OnPaintSection will still be called if a widget is provided.  OnPaintSection is still used for the background section display
	 * 
	 * @return The generated widget 
	 */
	virtual TSharedRef<SWidget> GenerateSectionWidget() = 0;
	/**
	 * @return Whether this section is read only.
	 */
	virtual bool IsReadOnly() const = 0;
	/**
	 * @return The height of the section
	 */
	virtual float GetSectionHeight() const = 0;
};
