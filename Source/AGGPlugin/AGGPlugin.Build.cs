/*
 AGGPlugin 0.0.1
 -----
 
*/
using System.IO;
using System.Collections;
using UnrealBuildTool;

public class AGGPlugin: ModuleRules
{
    public AGGPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine"
            });

        PrivateIncludePaths.Add("AGGPlugin/Private");

        string ThirdPartyPath = Path.Combine(ModuleDirectory, "../../ThirdParty");

        // -- AGG include and lib path

        string AGGPath = Path.Combine(ThirdPartyPath, "AGG");
        string AGGInclude = Path.Combine(AGGPath, "include");
        string AGGLib = Path.Combine(AGGPath, "lib");

        PublicIncludePaths.Add(Path.GetFullPath(AGGInclude));
        PublicLibraryPaths.Add(Path.GetFullPath(AGGLib));

        PublicAdditionalLibraries.Add("AGG.lib");
    }
}
