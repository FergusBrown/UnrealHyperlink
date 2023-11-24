using UnrealBuildTool;

public class HyperlinkEditor : ModuleRules
{
    public HyperlinkEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        //OptimizeCode = CodeOptimization.Never;
        
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Hyperlink",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "ApplicationCore",
                "AssetRegistry",
                "AssetTools",
                "ContentBrowser",
                "ContentBrowserData",
                "CoreUObject",
                "DeveloperSettings",
                "Engine",
                "Projects",
                "Slate",
                "SlateCore",
                "UnrealEd",
            }
        );
    }
}