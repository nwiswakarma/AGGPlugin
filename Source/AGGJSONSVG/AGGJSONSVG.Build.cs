/*
 AGGJSONSVG 0.0.1
 -----
 
*/
using UnrealBuildTool;

public class AGGJSONSVG: ModuleRules
{
	public AGGJSONSVG(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine"
            });

		PublicDependencyModuleNames.AddRange(
            new string[] {
                "AGGPlugin"
            });

		PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Json",
                "JsonUtilities",
                "JSONSVGPlugin"
            });
	}
}
