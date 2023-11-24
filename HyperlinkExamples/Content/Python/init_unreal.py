import unreal
import json
from collections import namedtuple

PythonExamplePayload = namedtuple("PythonExamplePayload", "map_name actor_name")

@unreal.uclass()
class HyperlinkPythonExample(unreal.HyperlinkDefinitionBlueprintBase):
    
    @unreal.ufunction(override=True)
    def initialize_impl(self) -> None:
        unreal.HyperlinkBlueprintFunctionLibrary.add_copy_link_menu_entry("LevelEditor.ActorContextMenu",
                                                                          "ActorOptions",
                                                                          "Copy Python Example Link",
                                                                          "Copy a link to select the selected actor in the current level",
                                                                          self)
    
    @unreal.ufunction(override=True)
    def generate_payload_string_impl(self) -> str:
        map_name = ''
        actor_name = ''

        selected_actors = unreal.EditorLevelLibrary.get_selected_level_actors()
        if selected_actors:
            a = selected_actors[0]
            map_name = str(unreal.GameplayStatics.get_current_level_name(a))
            asset_data = unreal.AssetRegistryHelpers.create_asset_data(a)
            actor_name = str(asset_data.asset_name)
        
        payload = PythonExamplePayload(map_name, actor_name)
        return json.dumps(payload._asdict(), separators=(',', ':'))
    
    @unreal.ufunction(override=True)
    def execute_payload_string_impl(self, payload_string: str) -> None:

        payload_dict = json.loads(payload_string)
        payload = PythonExamplePayload(**payload_dict)
        
        unreal.HyperlinkBlueprintFunctionLibrary.open_editor_for_asset(payload.map_name)

        level_actors = unreal.EditorLevelLibrary.get_all_level_actors()

        for a in level_actors:
            asset_data = unreal.AssetRegistryHelpers.create_asset_data(a)
            if asset_data.asset_name == payload.actor_name:
                actor_list = [a]
                unreal.EditorLevelLibrary.set_selected_level_actors(actor_list)
                break

