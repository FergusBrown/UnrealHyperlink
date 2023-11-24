﻿using UnrealBuildTool;

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
                "ContentBrowser",
                "ContentBrowserData",
                "CoreUObject",
                "DeveloperSettings",
                "Engine",
                "InputCore",
                "Projects",
                "PropertyEditor",
                "Slate",
                "SlateCore",
                "UnrealEd",
            }
        );
    }
}