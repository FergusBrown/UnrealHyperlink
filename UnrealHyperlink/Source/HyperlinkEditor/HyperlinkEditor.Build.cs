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
                "CoreUObject",
                "EditorSubsystem",
                "Engine",
                "Projects",
                "Slate",
                "SlateCore",
            }
        );
    }
}