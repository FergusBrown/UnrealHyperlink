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
                "DeveloperSettings",
                "Json",
                "Engine",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "JsonUtilities",
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
                    "ApplicationCore",
                    "Blutility",
                    "InputCore",
                    "PythonScriptPlugin",
                    "ToolMenus",
                    "UnrealEd",
                }
            );
        }
    }
}