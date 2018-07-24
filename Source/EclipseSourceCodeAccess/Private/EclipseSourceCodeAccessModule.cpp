#include "EclipseSourceCodeAccessModule.h"
#include "EclipseSourceCodeAccessPrivatePCH.h"
#include "Runtime/Core/Public/Features/IModularFeatures.h"

IMPLEMENT_MODULE(FEclipseSourceCodeAccessModule, EclipseSourceCodeAccess);

void FEclipseSourceCodeAccessModule::StartupModule()
{
	IModularFeatures::Get().RegisterModularFeature(TEXT("SourceCodeAccessor"), &EclipseSourceCodeAccessor);
}

void FEclipseSourceCodeAccessModule::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(TEXT("SourceCodeAccessor"), &EclipseSourceCodeAccessor);
}

FEclipseSourceCodeAccessor& FEclipseSourceCodeAccessModule::GetAccessor()
{
	return EclipseSourceCodeAccessor;
}
