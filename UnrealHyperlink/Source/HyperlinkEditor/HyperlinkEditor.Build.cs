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
                "HTTPServer", 
                "Hyperlink", 
                "Json"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "ApplicationCore",
                "AssetRegistry",
                "Blutility",
                "CoreUObject",
                "DeveloperSettings",
                "EditorFramework",
                "Engine",
                "InputCore",
                "JsonUtilities",
                "MaterialEditor",
                "Projects",
                "PropertyEditor",
                "PythonScriptPlugin",
                "Slate",
                "SlateCore",
                "UnrealEd",
            }
        );
    }
}