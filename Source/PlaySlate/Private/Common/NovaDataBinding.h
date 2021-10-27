﻿#pragma once
#include "PlaySlate.h"

/**
 * Usage: TODO
 */

/**
 * @param AbstractData
 * @param InName
 */
#define GetDataBindingSP(AbstractData, InName) StaticCastSharedPtr<TDataBindingSP<AbstractData>>(NovaDB::Get(InName))
/**
 * @param AbstractData
 * @param InName
 */
#define GetDataBindingUObject(AbstractData, InName) StaticCastSharedPtr<TDataBindingUObject<AbstractData>>(NovaDB::Get(InName))
/**
 * @param AbstractData
 * @param InName
 */
#define GetDataBinding(AbstractData, InName) StaticCastSharedPtr<TDataBinding<AbstractData>>(NovaDB::Get(InName))

/**
 * @param AbstractData 数据原型的类型
 * @param InName
 * @param FieldName 自定义字段名
 * @param InLambda 自定义Lambda函数，其中自定义字段名会作为这个Lambda函数的首参数
 * @param OutHandle
 */
#define DataBindingSPBindRaw(AbstractData, InName, InUserObject, InFunc, OutHandle) \
{\
	auto DB = StaticCastSharedPtr<TDataBindingSP<AbstractData>>(NovaDB::Get(InName));\
	OutHandle = DB->Bind(TDataBindingSP<AbstractData>::DelegateType::CreateRaw(InUserObject, InFunc));\
}\

/**
 * @param AbstractData 数据原型的类型
 * @param InName
 * @param InUserObject 对象实例，可参考TDelegate的CreateRaw
 * @param InFunc 对实例方法的引用，可参考TDelegate的CreateRaw
 * @param OutHandle
 */
#define DataBindingBindRaw(AbstractData, InName, InUserObject, InFunc, OutHandle) \
{\
	auto DB = StaticCastSharedPtr<TDataBinding<AbstractData>>(NovaDB::Get(InName));\
	OutHandle = DB->Bind(TDataBinding<AbstractData>::DelegateType::CreateRaw(InUserObject, InFunc));\
}\

// /**
//  * @param AbstractData 数据原型的类型，必须派生自UObject
//  * @param InName
//  * @param InUserObject 对象实例，可参考TDelegate的CreateRaw
//  * @param InFunc 对实例方法的引用，可参考TDelegate的CreateRaw
//  * @param OutHandle
//  */
// #define CreateUObjectBind(InUserObject, InFunc, AbstractData) TDataBindingUObject<AbstractData>::DelegateType::CreateUObject(InUserObject, InFunc)


class IDataBinding
{
public:
	/** 触发绑定函数 */
	virtual void Trigger() = 0;
	/** 解除对数据原型、绑定函数等相关资源的引用 */
	virtual void Release() = 0;
	virtual ~IDataBinding() = default;
protected:
	IDataBinding(FName InName)
		: Name(InName) { }

	TSet<FDelegateHandle> DelegateHandleSet;// 保存所有存在于Slots中的Handle
	FName Name;                             // 数据绑定的名称
};

template <typename AbstractData>
class TDataBindingSP : public IDataBinding
{
	friend class NovaDataBinding;
public:
	virtual ~TDataBindingSP() override;

	typedef TDelegate<void (TSharedPtr<AbstractData>)> DelegateType;// 用于创建绑定函数的TDelegate签名
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
	TSharedPtr<AbstractData> GetData();
	/**
	 * 设置数据原型，触发绑定函数，如果与内部的数据原型相同则不触发
	 *
	 * @param InData 待设置的数据原型
	 */
	void SetData(TSharedPtr<AbstractData> InData);
	/** 触发绑定函数 */
	virtual void Trigger() override;
	/** 解除对数据原型、绑定函数等相关资源的引用 */
	virtual void Release() override;

protected:
	TDataBindingSP(FName InName, TSharedPtr<AbstractData> InData = nullptr);

	TSharedPtr<AbstractData> Data;// 数据原型 SP
	TArray<DelegateType> Slots;   // 保存绑定函数
};


template <typename AbstractData>
class TDataBindingUObject : public IDataBinding
{
	friend class NovaDataBinding;
public:
	virtual ~TDataBindingUObject() override;

	typedef TDelegate<void (AbstractData*)> DelegateType;// 用于创建绑定函数的TDelegate签名
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
	AbstractData* GetData();
	/**
	 * 设置数据原型，触发绑定函数，如果与内部的数据原型相同则不触发
	 *
	 * @param InData 待设置的数据原型
	 */
	void SetData(AbstractData* InData);
	/** 触发绑定函数 */
	virtual void Trigger() override;
	/** 解除对数据原型、绑定函数等相关资源的引用 */
	virtual void Release() override;
protected:
	TDataBindingUObject(FName InName, AbstractData* InData = nullptr);

	AbstractData* Data;        // 数据原型
	TArray<DelegateType> Slots;// 保存绑定函数
};


template <typename AbstractData>
class TDataBinding : public IDataBinding
{
	friend class NovaDataBinding;
public:
	virtual ~TDataBinding() override;

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
	virtual void Trigger() override;
	/** 解除对数据原型、绑定函数等相关资源的引用 */
	virtual void Release() override;
protected:
	TDataBinding(FName InName, AbstractData InData = nullptr);

	AbstractData Data;         // 数据原型
	TArray<DelegateType> Slots;// 保存绑定函数
};

class NovaDataBinding
{
public:
	/**
	 * 获得指定名称的数据绑定，
	 * 推荐UObject及其子类模板加裸指针，其他类型结构模板加TSharedPtr，基本数据类型可直接使用
	 *
	 * @param InName 数据绑定对象自定义名称，同时查找用
	 * @return 数据绑定
	 */
	static TSharedPtr<IDataBinding> Get(FName InName);

	/**
	 * 构造指定名称的数据绑定，并传入数据原型，
	 * 推荐UObject及其子类模板加裸指针，其他类型结构模板加TSharedPtr，基本数据类型可直接使用
	 *
	 * @param InName 数据绑定对象自定义名称，同时查找用
	 * @param InData 数据原型
	 * @return 数据绑定
	 */
	template <typename AbstractData>
	static TSharedPtr<TDataBindingSP<AbstractData>> CreateSP(FName InName, TSharedPtr<AbstractData> InData);

	template <typename AbstractData>
	static TSharedPtr<TDataBindingUObject<AbstractData>> CreateUObject(FName InName, AbstractData* InData);

	template <typename AbstractData>
	static TSharedPtr<TDataBinding<AbstractData>> Create(FName InName, AbstractData InData);

	/**
	 * 清除指定名称的数据绑定
	 *
	 * @param InName 数据绑定对象自定义名称
	 * @return 是否成功删除
	 */
	static bool Delete(FName InName);

	/**
	 * 触发指定名称的数据绑定的所有绑定函数
	 *
	 * @param InName 指定名称的数据绑定
	 */
	static void Trigger(FName InName);
protected:
	static TMap<FName, TSharedRef<IDataBinding>> DataBindingMap;// 名称到数据绑定的映射
};

typedef NovaDataBinding NovaDB;

template <typename AbstractData>
TDataBindingSP<AbstractData>::TDataBindingSP(FName InName, TSharedPtr<AbstractData> InData)
	: IDataBinding(InName),
	  Data(InData) {}

template <typename AbstractData>
TDataBindingUObject<AbstractData>::TDataBindingUObject(FName InName, AbstractData* InData)
	: IDataBinding(InName),
	  Data(InData) {}


template <typename AbstractData>
TDataBinding<AbstractData>::TDataBinding(FName InName, AbstractData InData)
	: IDataBinding(InName),
	  Data(InData) {}


template <typename AbstractData>
TDataBindingSP<AbstractData>::~TDataBindingSP()
{
	TDataBindingSP<AbstractData>::Release();
}

template <typename AbstractData>
TDataBindingUObject<AbstractData>::~TDataBindingUObject()
{
	TDataBindingUObject<AbstractData>::Release();
}

template <typename AbstractData>
TDataBinding<AbstractData>::~TDataBinding()
{
	TDataBinding<AbstractData>::Release();
}

template <typename AbstractData>
FDelegateHandle TDataBindingSP<AbstractData>::Bind(DelegateType InDelegate)
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
FDelegateHandle TDataBindingUObject<AbstractData>::Bind(DelegateType InDelegate)
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
bool TDataBindingSP<AbstractData>::UnBind(FDelegateHandle InDelegateHandle)
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
		UE_LOG(LogNovaAct, Error, TEXT("UnBind in DelegateHandleSet, but not in Slots??"))
	}
	return false;
}


template <typename AbstractData>
bool TDataBindingUObject<AbstractData>::UnBind(FDelegateHandle InDelegateHandle)
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
		UE_LOG(LogNovaAct, Error, TEXT("UnBind in DelegateHandleSet, but not in Slots??"))
	}
	return false;
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
		UE_LOG(LogNovaAct, Error, TEXT("UnBind in DelegateHandleSet, but not in Slots??"))
	}
	return false;
}

template <typename AbstractData>
TSharedPtr<AbstractData> TDataBindingSP<AbstractData>::GetData()
{
	return Data;
}


template <typename AbstractData>
AbstractData* TDataBindingUObject<AbstractData>::GetData()
{
	return Data;
}


template <typename AbstractData>
AbstractData TDataBinding<AbstractData>::GetData()
{
	return Data;
}

template <typename AbstractData>
void TDataBindingSP<AbstractData>::SetData(TSharedPtr<AbstractData> InData)
{
	Data = InData;
	Trigger();
}

template <typename AbstractData>
void TDataBindingUObject<AbstractData>::SetData(AbstractData* InData)
{
	Data = InData;
	Trigger();
}

template <typename AbstractData>
void TDataBinding<AbstractData>::SetData(AbstractData InData)
{
	Data = InData;
	Trigger();
}

template <typename AbstractData>
void TDataBindingSP<AbstractData>::Trigger()
{
	for (DelegateType& Slot : Slots)
	{
		Slot.ExecuteIfBound(Data);
	}
}

template <typename AbstractData>
void TDataBindingUObject<AbstractData>::Trigger()
{
	for (DelegateType& Slot : Slots)
	{
		Slot.ExecuteIfBound(Data);
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
void TDataBindingSP<AbstractData>::Release()
{
	Data = nullptr;
	DelegateHandleSet.Empty();
	Slots.Empty();
}

template <typename AbstractData>
void TDataBindingUObject<AbstractData>::Release()
{
	Data = nullptr;
	DelegateHandleSet.Empty();
	Slots.Empty();
}


template <typename AbstractData>
void TDataBinding<AbstractData>::Release()
{
	DelegateHandleSet.Empty();
	Slots.Empty();
}

template <typename AbstractData>
TSharedPtr<TDataBinding<AbstractData>> NovaDataBinding::Create(FName InName, AbstractData InData)
{
	TSharedPtr<TDataBinding<AbstractData>> CreatedDataBinding = MakeShareable(new TDataBinding<AbstractData>(InName, InData));
	NovaDataBinding::DataBindingMap.Add(InName, CreatedDataBinding.ToSharedRef());
	return CreatedDataBinding;
}

template <typename AbstractData>
TSharedPtr<TDataBindingUObject<AbstractData>> NovaDataBinding::CreateUObject(FName InName, AbstractData* InData)
{
	TSharedPtr<TDataBindingUObject<AbstractData>> CreatedDataBinding = MakeShareable(new TDataBindingUObject<AbstractData>(InName, InData));
	NovaDataBinding::DataBindingMap.Add(InName, CreatedDataBinding.ToSharedRef());
	return CreatedDataBinding;
}

template <typename AbstractData>
TSharedPtr<TDataBindingSP<AbstractData>> NovaDataBinding::CreateSP(FName InName, TSharedPtr<AbstractData> InData)
{
	TSharedPtr<TDataBindingSP<AbstractData>> CreatedDataBinding = MakeShareable(new TDataBindingSP<AbstractData>(InName, InData));
	NovaDataBinding::DataBindingMap.Add(InName, CreatedDataBinding.ToSharedRef());
	return CreatedDataBinding;
}
