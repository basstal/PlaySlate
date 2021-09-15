#pragma once
// #include "ActActionSequenceNodeTree.h"

/**
 * 整个Sequence的主要控制器
 * 对应的View模块为SActActionSequenceMain
 */
class FActActionSequenceController : public TSharedFromThis<FActActionSequenceController>
{
public:
	/**
	 * ** FIX:Builds up the object bindings in sequencer's "Add Track" menu.
	 * 
	 * @param MenuBuilder 被添加内容的菜单
	 */
	void BuildAddObjectBindingsMenu(FMenuBuilder& MenuBuilder);
	/**
	 * ** FIX:Builds up the sequencer's "Add Track" menu.
	 *
	 * @param MenuBuilder 被添加内容的菜单
	 */
	void BuildAddTrackMenu(const FMenuBuilder& MenuBuilder);


// protected:
// 	/**
// 	 * 左侧Track的所有可见节点都储存在NodeTree中
// 	 */
// 	TSharedRef<FActActionSequenceNodeTree> NodeTree;
// public:
// 	TSharedRef<FActActionSequenceNodeTree> GetNodeTree() const
// 	{
// 		return NodeTree;
// 	}
};
