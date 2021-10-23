#pragma once

/**
 * Usage: TODO
 */
/**
 * @param AbstractData 数据原型的类型
 * @param FieldName 自定义字段名
 * @param InLambda 自定义Lambda函数，其中自定义字段名会作为这个Lambda函数的首参数
 */
#define CreateLambdaBind(AbstractData, FieldName, InLambda) TDataBinding<AbstractData>::DelegateType::CreateLambda([](AbstractData FieldName)InLambda)
/**
 * @param AbstractData 数据原型的类型
 * @param InUserObject 对象实例，可参考TDelegate的CreateRaw
 * @param InFunc 对实例方法的引用，可参考TDelegate的CreateRaw
 */
#define CreateRawBind(InUserObject, InFunc, AbstractData) TDataBinding<AbstractData>::DelegateType::CreateRaw(InUserObject, InFunc)
/**
 * @param AbstractData 数据原型的类型，必须派生自UObject
 * @param InUserObject 对象实例，可参考TDelegate的CreateRaw
 * @param InFunc 对实例方法的引用，可参考TDelegate的CreateRaw
 */
#define CreateUObjectBind(InUserObject, InFunc, AbstractData) TDataBinding<AbstractData>::DelegateType::CreateUObject(InUserObject, InFunc)
#include "PlaySlate.h"


class IDataBinding
{
protected:
	IDataBinding(FName InName)
		: Name(InName) { }

	FName Name;// 数据绑定的名称
};

template <typename AbstractData>
class TDataBinding : public IDataBinding
{
	friend class NovaDataBinding;
public:
	typedef TDelegate<void (AbstractData)> DelegateType;// 用于创建绑定函数的TDelegate签名
	/**
	 * 添加数据的绑定函数
	 *
	 * @param InDelegate 外部构造的绑定函数，推荐使用相关宏构造：CreateLambdaBind、CreateRawBind
	 * @return 绑定函数FDelegate的Handle，用于解绑时作为参数传入
	 */
	FDelegateHandle Bind(DelegateType InDelegate);
	/**
	 * 解绑函数
	 *
	 * @param InDelegateHandle 传入绑定函数FDelegate的Handle，用于查找内部存储的对应Delegate实例并解除绑定
	 */
	bool UnBind(FDelegateHandle InDelegateHandle);
	/**
	 * 获得数据原型
	 *
	 * @return 内部的数据原型
	 */
	AbstractData GetData();
	/**
	 * 设置数据原型，触发绑定函数，如果与内部的数据原型相同则不触发
	 *
	 * @param InData 待设置的数据原型
	 */
	void SetData(AbstractData InData);
	/** 触发绑定函数 */
	void Trigger();
	/** 解除对数据原型、绑定函数等相关资源的引用 */
	void Release();
protected:
	TDataBinding(FName InName, AbstractData InData = nullptr);

	AbstractData Data;                      // 数据原型
	TSet<FDelegateHandle> DelegateHandleSet;// 保存所有存在于Slots中的Handle
	TArray<DelegateType> Slots;             // 保存绑定函数
};

class NovaDataBinding
{
public:
	/**
	 * 获得指定名称的数据绑定，如果不存在则构造一个
	 * 推荐UObject及其子类模板加裸指针，其他类型结构模板加TSharedPtr，基本数据类型可直接使用
	 *
	 * @param InName 数据绑定对象自定义名称，同时查找用
	 * @param InData 数据绑定待设置的数据原型
	 * @return 数据绑定
	 */
	template <typename AbstractData>
	static TSharedRef<TDataBinding<AbstractData>> GetOrCreate(FName InName, AbstractData InData = nullptr);

	/**
	 * 清除指定名称的数据绑定
	 *
	 * @param InName 数据绑定对象自定义名称
	 * @return 是否成功删除
	 */
	template <typename AbstractData>
	static bool Delete(FName InName);
protected:
	static TMap<FName, TSharedRef<IDataBinding>> DataBindingMap;// 名称到数据绑定的映射
};

typedef NovaDataBinding NovaDB;

template <typename AbstractData>
TDataBinding<AbstractData>::TDataBinding(FName InName, AbstractData InData)
	: IDataBinding(InName),
	  Data(InData) {}

template <typename AbstractData>
FDelegateHandle TDataBinding<AbstractData>::Bind(DelegateType InDelegate)
{
	FDelegateHandle DelegateHandle = InDelegate.GetHandle();
	if (!DelegateHandleSet.Contains(DelegateHandle))
	{
		Slots.Add(InDelegate);
		DelegateHandleSet.Add(DelegateHandle);
	}
	return DelegateHandle;
}

template <typename AbstractData>
bool TDataBinding<AbstractData>::UnBind(FDelegateHandle InDelegateHandle)
{
	if (DelegateHandleSet.Contains(InDelegateHandle))
	{
		DelegateHandleSet.Remove(InDelegateHandle);
		int32 Index = 0;
		for (auto Item = Slots.begin(); Item != Slots.end(); ++Index, ++Item)
		{
			if (StaticCast<DelegateType>(*Item).GetHandle() == InDelegateHandle)
			{
				Slots.RemoveAt(Index);
				return true;
			}
		}
		UE_LOG(LogActAction, Error, TEXT("UnBind in DelegateHandleSet, but not in Slots??"))
	}
	return false;
}

template <typename AbstractData>
AbstractData TDataBinding<AbstractData>::GetData()
{
	return Data;
}

template <typename AbstractData>
void TDataBinding<AbstractData>::SetData(AbstractData InData)
{
	// ** TODO:可置空
	if (InData && InData != Data)
	{
		Data = InData;
		Trigger();
	}
}

template <typename AbstractData>
void TDataBinding<AbstractData>::Trigger()
{
	for (DelegateType& Slot : Slots)
	{
		Slot.ExecuteIfBound(Data);
	}
}

template <typename AbstractData>
void TDataBinding<AbstractData>::Release()
{
	Data = nullptr;
	DelegateHandleSet.Empty();
	Slots.Empty();
}

template <typename AbstractData>
TSharedRef<TDataBinding<AbstractData>> NovaDataBinding::GetOrCreate(FName InName, AbstractData InData)
{
	auto ValueTypePtr = NovaDataBinding::DataBindingMap.Find(InName);
	if (ValueTypePtr)
	{
		return StaticCastSharedRef<TDataBinding<AbstractData>>(*ValueTypePtr);
	}
	TSharedRef<TDataBinding<AbstractData>> CreatedDataBinding = MakeShareable(new TDataBinding<AbstractData>(InName, InData));
	NovaDataBinding::DataBindingMap.Add(InName, CreatedDataBinding);
	return CreatedDataBinding;
}

template <typename AbstractData>
bool NovaDataBinding::Delete(FName InName)
{
	auto ValueTypePtr = NovaDataBinding::DataBindingMap.Find(InName);
	if (ValueTypePtr)
	{
		TSharedRef<TDataBinding<AbstractData>> DataBindingRef = StaticCastSharedRef<TDataBinding<AbstractData>>(*ValueTypePtr);
		DataBindingRef->Release();
		NovaDataBinding::DataBindingMap.FindAndRemoveChecked(InName);
		return true;
	}
	return false;
}
