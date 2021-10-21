#include "Common/NovaDataBinding.h"

TMap<FName, TSharedRef<IDataBinding>> NovaDataBinding::DataBindingMap = TMap<FName, TSharedRef<IDataBinding>>();
