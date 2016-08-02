#include "EclipseSourceCodeAccessPrivatePCH.h"
#include "EclipseSourceCodeAccessor.h"
#include "ModuleManager.h"
#include "DesktopPlatformModule.h"

#if WITH_EDITOR
#include "Developer/HotReload/Public/IHotReload.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogEclipseAccessor, Log, All);

#define LOCTEXT_NAMESPACE "EclipseSourceCodeAccessor"

// http://help.eclipse.org/juno/index.jsp?topic=%2Forg.eclipse.platform.doc.isv%2Freference%2Fmisc%2Fruntime-options.html

bool FEclipseSourceCodeAccessor::CanAccessSourceCode() const
{
	return true;
}

FName FEclipseSourceCodeAccessor::GetFName() const
{
	return FName("EclipseSourceCodeAccessor");
}

FText FEclipseSourceCodeAccessor::GetNameText() const
{
	return LOCTEXT("EclipseDisplayName", "Eclipse");
}

FText FEclipseSourceCodeAccessor::GetDescriptionText() const
{
	return LOCTEXT("EclipseDisplayDesc", "Open source code files with Eclipse");
}

bool FEclipseSourceCodeAccessor::OpenSolution()
{
	FString Filename = FPaths::GetBaseFilename(GetSolutionPath()) + ".cproject";
	FString Directory = FPaths::GetPath(GetSolutionPath());
	FString Solution = "\"" + Directory + "/" + Filename + "\"";
	FString EclipsePath;
	if (!CanRunEclipse(EclipsePath))
	{
		return false;
	}

	UE_LOG(LogEclipseAccessor, Warning, TEXT("FEclipseSourceCodeAccessor::OpenSolution: %s %s"), *EclipsePath, *Solution);

	FProcHandle Proc = FPlatformProcess::CreateProc(*EclipsePath, *Solution, true, false, false, nullptr, 0, nullptr, nullptr);
	if (Proc.IsValid())
	{
		FPlatformProcess::CloseProc(Proc);
		return true;
	}
	return false;
}

bool FEclipseSourceCodeAccessor::OpenFileAtLine(const FString& FullPath, int32 LineNumber, int32 ColumnNumber)
{
	TArray<FString> Files;
	Files.Emplace(FString::Printf(TEXT("%s:%i "), *FullPath, LineNumber));
	return OpenSourceFiles(Files);
}

bool FEclipseSourceCodeAccessor::OpenSourceFiles(const TArray<FString>& AbsoluteSourcePaths)
{
	FString EclipsePath;
	if (!CanRunEclipse(EclipsePath))
	{
		return false;
	}
	FString Args = FString(TEXT("--launcher.timeout 60 --launcher.openFile "));
	for (const FString& SourcePath : AbsoluteSourcePaths)
	{
		const FString NewSourcePath = FString::Printf(TEXT("\"%s\" "), *SourcePath);
		Args.Append(NewSourcePath);
	}
	FProcHandle Proc = FPlatformProcess::CreateProc(*EclipsePath, *Args, true, false, false, nullptr, 0, nullptr, nullptr);
	if (Proc.IsValid())
	{
		FPlatformProcess::CloseProc(Proc);
		return true;
	}
	return false;
}

bool FEclipseSourceCodeAccessor::AddSourceFiles(const TArray<FString>& AbsoluteSourcePaths, const TArray<FString>& AvailableModules)
{
	return false;
}

bool FEclipseSourceCodeAccessor::SaveAllOpenDocuments() const
{
	return false;
}

void FEclipseSourceCodeAccessor::Tick(const float DeltaTime)
{
}

bool FEclipseSourceCodeAccessor::CanRunEclipse(FString& OutPath) const
{
	// TODO This might be not a good idea to find an executable.
	OutPath = TEXT("/usr/bin/eclipse");
	if (!FPaths::FileExists(OutPath))
	{
		TCHAR EclipseBinaryEnv[32768] = { 0 };
		FPlatformMisc::GetEnvironmentVariable(TEXT("UE4_ECLIPSE_BINARY"), EclipseBinaryEnv, ARRAY_COUNT(EclipseBinaryEnv));
		OutPath = EclipseBinaryEnv;

		if (!FPaths::FileExists(OutPath))
		{
			UE_LOG(LogEclipseAccessor, Warning, TEXT("FEclipseSourceCodeAccessor: Can not find eclipse binary - export UE4_ECLIPSE_BINARY environment variable"));
			return false;
		}
	}
	return true;
}

FString FEclipseSourceCodeAccessor::GetSolutionPath() const
{
	if (IsInGameThread())
	{
		FString SolutionPath;
		if (FDesktopPlatformModule::Get()->GetSolutionPath(SolutionPath))
		{
			CachedSolutionPath = FPaths::ConvertRelativePathToFull(SolutionPath);
		}
	}
	return CachedSolutionPath;
}

void RefreshAvailability()
{
	// Just a stub
	UE_LOG(LogEclipseAccessor, Warning, TEXT("FIXME: Function stub FEclipseSourceCodeAccessor::RefreshAvailability") );
}

void FEclipseSourceCodeAccessor::Startup()
{
	// Cache this so we don't have to do it on a background thread
	GetSolutionPath();
}

void FEclipseSourceCodeAccessor::Shutdown()
{
}
