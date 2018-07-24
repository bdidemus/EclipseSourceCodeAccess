#pragma once

#include "EclipseSourceCodeAccessor.h"
#include "Modules/ModuleInterface.h"


class FEclipseSourceCodeAccessModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	FEclipseSourceCodeAccessor& GetAccessor();

private:
	FEclipseSourceCodeAccessor EclipseSourceCodeAccessor;
};
