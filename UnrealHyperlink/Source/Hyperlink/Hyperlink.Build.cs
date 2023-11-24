using UnrealBuildTool;

public class Hyperlink : ModuleRules
{
    public Hyperlink(ReadOnlyTargetRules Target) : base(Target)
    {
        //OptimizeCode = CodeOptimization.Never;
        
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "Engine",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "DeveloperSettings",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );
        
        // Editor only dependencies
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[] 
                {
                    "AssetRegistry",
                    "AssetTools",
                    "ContentBrowser",
                }
            );
        }
    }
}