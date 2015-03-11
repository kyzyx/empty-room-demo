// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class RoomVisualizer : ModuleRules
{
    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }
	public RoomVisualizer(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore", "ShaderCore" });

		string EnginePath = "C:/Users/edzhang/UnrealEngine/";
        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "Win32";
        PublicIncludePaths.Add(Path.Combine(EnginePath, "Engine/Source/ThirdParty/openexr/Deploy/include"));
        string OpenExrLibrariesPath = Path.Combine(EnginePath,"Engine/Source/ThirdParty/openexr/Deploy/lib/VS2013");
        string[] libnames = { "Half", "Iex", "IlmImf", "IlmThread", "Imath" };
        foreach (string s in libnames) {
            PublicAdditionalLibraries.Add(Path.Combine(OpenExrLibrariesPath, PlatformString, "StaticRelease", s + ".lib"));
        }

        string ZlibLibrariesPath = Path.Combine(EnginePath,"Engine/Source/ThirdParty/zlib/zlib-1.2.5/Lib");
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ZlibLibrariesPath, "Win64", "zlib_64.lib"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Win32)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ZlibLibrariesPath, "Win32", "zlib.lib"));
        }
    }
}
