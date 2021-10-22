#pragma once

/**
 * Usage:
	TSharedRef<TDataBinding<ActActionSequence::FActActionTimeSliderArgs>> TimeSliderArgsModel = NovaDataBinding::Create<ActActionSequence::FActActionTimeSliderArgs>("TimeSliderArgs");
	TSharedPtr<ActActionSequence::FActActionTimeSliderArgs> TimeSliderArgs = MakeShareable(new ActActionSequence::FActActionTimeSliderArgs());
	TimeSliderArgsModel->SetData(TimeSliderArgs);
	auto BindFunc = DBCreateLambda(ActActionSequence::FActActionTimeSliderArgs,
								   InData,
								   {
								   UE_LOG(LogActAction, Log, TEXT("DataChanged Data->PlaybackRange 11 : %d"), InData->DBTestValue)
								   });

	auto Handler = TimeSliderArgsModel->Bind(BindFunc);
	TimeSliderArgsModel->Trigger();
	TimeSliderArgs->DBTestValue = 999;
	auto DelegateFunc1 = DBCreateRaw(ActActionSequence::FActActionTimeSliderArgs, this, &FActActionSequenceEditor::MyDelegateImpl);
	TimeSliderArgsModel->UnBind(Handler);
	TimeSliderArgsModel->Bind(DelegateFunc1);
	TimeSliderArgsModel->Trigger();
	TSharedRef<TDataBinding<ActActionSequence::FActActionTimeSliderArgs>> ModelAgain = NovaDataBinding::GetOrCreate<ActActionSequence::FActActionTimeSliderArgs>("TimeSliderArgs");
	UE_LOG(LogActAction, Log, TEXT("TimeSliderArgsModel equal ModelAgain : %d"), ModelAgain == TimeSliderArgsModel);
	ModelAgain = NovaDataBinding::GetOrCreate<ActActionSequence::FActActionTimeSliderArgs>("TimeSliderArgs1");
	UE_LOG(LogActAction, Log, TEXT("TimeSliderArgsModel equal ModelAgain next : %d"), ModelAgain == TimeSliderArgsModel);
 */
/**
 * @param AbstractData 数据原型的类型
 * @param FieldName 自定义字段名
 * @param InLambda 自定义Lambda函数，其中自定义字段名会作为这个Lambda函数的首参数
 */
#define DBCreateLambda(AbstractData, FieldName, InLambda) TDataBinding<AbstractData>::DelegateType::CreateLambda([](TSharedPtr<AbstractData> FieldName)InLambda)
/**
 * @param AbstractData 数据原型的类型
 * @param InUserObject 对象实例，可参考TDelegate的CreateRaw
 * @param InFunc 对实例方法的引用，可参考TDelegate的CreateRaw
 */
#define DBCreateRaw(AbstractData, InUserObject, InFunc) TDataBinding<AbstractData>::DelegateType::CreateRaw(InUserObject, InFunc)
/**
 * @param AbstractData 数据原型的类型，必须派生自UObject
 * @param InUserObject 对象实例，可参考TDelegate的CreateRaw
 * @param InFunc 对实例方法的引用，可参考TDelegate的CreateRaw
 */
#define DBCreateUObject(AbstractData, InUserObject, InFunc) TDataBinding<AbstractData>::DelegateTypeUObject::CreateUObject(InUserObject, InFunc)


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
	typedef TDelegate<void (TSharedPtr<AbstractData>)> DelegateType;// 用于创建绑定函数的TDelegate签名
	typedef TDelegate<void (AbstractData*)> DelegateTypeUObject;    // 用于创建绑定函数的TDelegate签名，供UObject使用
	/**
	 * 添加数据的绑定函数
	 *
	 * @param InDelegate 外部构造的绑定函数，推荐使用相关宏构造：DBCreateLambda、DBCreateRaw
	 * @return 绑定函数FDelegate的Handle，用于解绑时作为参数传入
	 */
	FDelegateHandle Bind(TDelegateBase<FDefaultDelegateUserPolicy> InDelegate);
	/**
	 * 解绑函数
	 *
	 * @param InDelegateHandle 传入绑定函数FDelegate的Handle，用于查找内部存储的对应Delegate实例并解除绑定
	 */
	bool UnBind(FDelegateHandle InDelegateHandle);
	/**
	 * 获得数据原型
	 *
	 * @return 内部的数据原型SP
	 */
	TSharedPtr<AbstractData> GetData();
	/**
	 * 设置数据原型，触发绑定函数，如果与内部的数据原型相同则不触发
	 *
	 * @param InData 待设置的数据原型SP
	 */
	void SetData(TSharedPtr<AbstractData> InData);
	/**
	 * 获得数据原型
	 *
	 * @return 内部的数据原型，原型类型必须派生自UObject
	 */
	AbstractData* GetUObject();
	/**
	 * 设置数据原型，触发绑定函数，如果与内部的数据原型相同则不触发
	 *
	 * @param InUObject 待设置的数据原型，原型类型必须派生自UObject
	 */
	void SetUObject(AbstractData* InUObject);
	/** 触发绑定函数 */
	void Trigger();
	/** 解除对数据原型、绑定函数等相关资源的引用 */
	void Release();
	/** 数据绑定是否为UObject */
	bool IsUObject();
protected:
	TDataBinding(FName InName, TSharedPtr<AbstractData> InData = nullptr);
	TDataBinding(FName InName, UObject* InUObject = nullptr);

	bool bIsUObject;                                     // 数据原型是否为UObject
	UObject* UData;                                      // 数据原型UObject
	TSharedPtr<AbstractData> Data;                       // 数据原型SP
	TMap<FDelegateHandle, int32> DelegateHandleToSlotMap;// 保存FDelegateHandle到Slots下标的对应关系，用于解绑时查找绑定函数在数组中的下标，并删除元素
	TArray<DelegateType> Slots;                          // 保存绑定函数
};

class NovaDataBinding
{
public:
	/**
	 * 获得指定名称的数据绑定，如果不存在则构造一个
	 *
	 * @param InName 数据绑定对象自定义名称，同时查找用
	 * @param InData 数据绑定待设置的数据原型
	 * @return 数据绑定SP
	 */
	template <typename AbstractData>
	static TSharedRef<TDataBinding<AbstractData>> GetOrCreate(FName InName, TSharedPtr<AbstractData> InData = nullptr);

	template <typename AbstractData>
	static TSharedRef<TDataBinding<AbstractData>> GetOrCreate(FName InName, UObject* InUObject = nullptr);

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
TDataBinding<AbstractData>::TDataBinding(FName InName, TSharedPtr<AbstractData> InData)
	: IDataBinding(InName),
	  bIsUObject(false),
	  UData(nullptr),
	  Data(InData) {}

template <typename AbstractData>
TDataBinding<AbstractData>::TDataBinding(FName InName, UObject* InUObject)
	: IDataBinding(InName),
	  bIsUObject(true),
	  UData(InUObject),
	  Data(nullptr) {}

template <typename AbstractData>
FDelegateHandle TDataBinding<AbstractData>::Bind(TDelegateBase<FDefaultDelegateUserPolicy> InDelegate)
{
	FDelegateHandle DelegateHandle = InDelegate.GetHandle();
	if (!DelegateHandleToSlotMap.Contains(DelegateHandle))
	{
		int32 Index = Slots.Add(InDelegate);
		DelegateHandleToSlotMap.Add(DelegateHandle, Index);
	}
	return DelegateHandle;
}

template <typename AbstractData>
bool TDataBinding<AbstractData>::UnBind(FDelegateHandle InDelegateHandle)
{
	if (DelegateHandleToSlotMap.Contains(InDelegateHandle))
	{
		int32 Index;
		DelegateHandleToSlotMap.RemoveAndCopyValue(InDelegateHandle, Index);
		Slots.RemoveAt(Index);
		return true;
	}
	return false;
}

template <typename AbstractData>
TSharedPtr<AbstractData> TDataBinding<AbstractData>::GetData()
{
	return Data;
}

template <typename AbstractData>
void TDataBinding<AbstractData>::SetUObject(AbstractData* InUObject)
{
	// ** TODO:可置空
	if (InUObject && InUObject != UData)
	{
		UData = InUObject;
		Trigger();
	}
}

template <typename AbstractData>
void TDataBinding<AbstractData>::SetData(TSharedPtr<AbstractData> InData)
{
	// ** TODO:可置空
	if (InData && InData != Data)
	{
		Data = InData;
		Trigger();
	}
}

template <typename AbstractData>
AbstractData* TDataBinding<AbstractData>::GetUObject()
{
	return StaticCast<AbstractData>(UData);
}

template <typename AbstractData>
void TDataBinding<AbstractData>::Trigger()
{
	if (!Data.IsValid())
	{
		return;
	}
	for (DelegateType& Slot : Slots)
	{
		Slot.ExecuteIfBound(bIsUObject ? UData : Data);
	}
}

template <typename AbstractData>
void TDataBinding<AbstractData>::Release()
{
	Data = nullptr;
	UData = nullptr;
	DelegateHandleToSlotMap.Empty();
	Slots.Empty();
}

template <typename AbstractData>
bool TDataBinding<AbstractData>::IsUObject()
{
	return bIsUObject;
}

template <typename AbstractData>
TSharedRef<TDataBinding<AbstractData>> NovaDataBinding::GetOrCreate(FName InName, TSharedPtr<AbstractData> InData)
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
TSharedRef<TDataBinding<AbstractData>> NovaDataBinding::GetOrCreate(FName InName, UObject* InUObject)
{
	auto ValueTypePtr = NovaDataBinding::DataBindingMap.Find(InName);
	if (ValueTypePtr)
	{
		return StaticCastSharedRef<TDataBinding<AbstractData>>(*ValueTypePtr);
	}
	TSharedRef<TDataBinding<AbstractData>> CreatedDataBinding = MakeShareable(new TDataBinding<AbstractData>(InName, InUObject));
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
