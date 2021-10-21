#pragma once

class IDataBinding
{
protected:
	IDataBinding(FName InName)
		: Name(InName) { }

	FName Name;
};

template <typename AbstractData>
class TFunctionWrap
{
public:
	FName Id;
	TFunction<void (TSharedPtr<AbstractData>)> Function;

	friend bool operator==(const TFunctionWrap<AbstractData>& Lhs, const TFunctionWrap<AbstractData>& Rhs)
	{
		return Lhs.Id == Rhs.Id;
	}

	friend bool operator!=(const TFunctionWrap<AbstractData>& Lhs, const TFunctionWrap<AbstractData>& Rhs)
	{
		return !(Lhs == Rhs);
	}
};

template <typename AbstractData>
class TDataBinding : public IDataBinding
{
	friend class NovaDataBinding;
public:
	/** 添加数据的绑定函数 */
	void Bind(FName InTabId, TFunction<void (TSharedPtr<AbstractData>)> InFunction);
	/** 获取模型数据 */
	TSharedPtr<AbstractData> GetData();
	/** 设置数据，触发回调 */
	void SetData(TSharedPtr<AbstractData> InData);
	/** 触发回调 */
	void Trigger();
	/** 释放这个数据绑定引用的资源 */
	void Release();
protected:
	TDataBinding(FName InName, TSharedPtr<AbstractData> InData = nullptr);

	TSharedPtr<AbstractData> Data;
	TArray<TFunctionWrap<AbstractData>> Slots;
};

class NovaDataBinding
{
public:
	/** 构造指定名称的数据绑定 */
	template <typename AbstractData>
	static TSharedRef<TDataBinding<AbstractData>> Create(FName InName, TSharedPtr<AbstractData> InData = nullptr);

	/** 获得指定名称的数据绑定，如果没有就构造一个 */
	template <typename AbstractData>
	static TSharedRef<TDataBinding<AbstractData>> GetOrCreate(FName InName, TSharedPtr<AbstractData> InData = nullptr);

	/** 删除指定名称的数据绑定 */
	template <typename AbstractData>
	static bool Delete(FName InName);
protected:
	static TMap<FName, TSharedRef<IDataBinding>> DataBindingMap;
};

typedef NovaDataBinding NovaDB;

template <typename AbstractData>
TDataBinding<AbstractData>::TDataBinding(FName InName, TSharedPtr<AbstractData> InData)
	: IDataBinding(InName),
	  Data(InData) {}

template <typename AbstractData>
void TDataBinding<AbstractData>::Bind(FName InTabId, TFunction<void(TSharedPtr<AbstractData>)> InFunction)
{
	TFunctionWrap<AbstractData> Wrap{InTabId, InFunction};
	if (!Slots.Contains(Wrap))
	{
		Slots.Add(Wrap);
	}
}

template <typename AbstractData>
TSharedPtr<AbstractData> TDataBinding<AbstractData>::GetData()
{
	return Data;
}

template <typename AbstractData>
void TDataBinding<AbstractData>::SetData(TSharedPtr<AbstractData> InData)
{
	if (InData && InData != Data)
	{
		Data = InData;
		Trigger();
	}
}

template <typename AbstractData>
void TDataBinding<AbstractData>::Trigger()
{
	if (!Data.IsValid())
	{
		return;
	}
	for (auto& Slot : Slots)
	{
		Slot.Function(Data);
	}
}

template <typename AbstractData>
void TDataBinding<AbstractData>::Release()
{
	Data = nullptr;
	Slots.Empty();
}

template <typename AbstractData>
TSharedRef<TDataBinding<AbstractData>> NovaDataBinding::Create(FName InName, TSharedPtr<AbstractData> InData)
{
	TSharedRef<TDataBinding<AbstractData>> CreatedDataBinding = MakeShareable(new TDataBinding<AbstractData>(InName, InData));
	NovaDataBinding::DataBindingMap.Add(InName, CreatedDataBinding);
	return CreatedDataBinding;
}

template <typename AbstractData>
TSharedRef<TDataBinding<AbstractData>> NovaDataBinding::GetOrCreate(FName InName, TSharedPtr<AbstractData> InData)
{
	auto ValueTypePtr = NovaDataBinding::DataBindingMap.Find(InName);
	if (ValueTypePtr)
	{
		return StaticCastSharedRef<TDataBinding<AbstractData>>(*ValueTypePtr);
	}
	return NovaDataBinding::Create<AbstractData>(InName, InData);
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
