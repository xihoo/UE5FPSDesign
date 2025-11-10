using UnrealBuildTool;

public class SKGShooterFrameworkActors : ModuleRules
{
    public SKGShooterFrameworkActors(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "GameplayTags",
                "StructUtils",
                "SKGShooterFramework",
                "SKGAttachment",
                "SKGLightLaser",
                "SKGMuzzle",
                "SKGOptic",
                "SKGProceduralAnim",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "NetCore"
            }
        );
    }
}