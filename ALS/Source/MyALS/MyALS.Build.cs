using UnrealBuildTool;

public class MyALS : ModuleRules
{
    public MyALS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine" });
        PrivateDependencyModuleNames.AddRange(new[] { "EnhancedInput", "AnimGraphRuntime" });
    }
}
