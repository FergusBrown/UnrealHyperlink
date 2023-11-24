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
                "Engine",
                "Hyperlink",
                "Projects",
                "Slate",
                "SlateCore",
                "UnrealEd",
            }
        );
    }
}