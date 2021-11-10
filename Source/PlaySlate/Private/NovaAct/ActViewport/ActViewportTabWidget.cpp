#include "ActViewportTabWidget.h"

#include "ActViewport.h"

void SActViewportTabWidget::Construct(const FArguments& InArgs)
{
	TSharedPtr<SVerticalBox> ViewportContainer = nullptr;
	ChildSlot
	[
		SAssignNew(ViewportContainer, SVerticalBox)

		// Build our toolbar level toolbar
		+ SVerticalBox::Slot()
		.FillHeight(1)
		[
			SNew(SOverlay)

			// The viewport
			+ SOverlay::Slot()
			[
				SNew(SActViewport)
			]
		]
	];
}
