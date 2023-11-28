import sys
import unreal
from urllib.parse import quote, unquote

@unreal.uclass()
class HyperlinkPythonBridgeImplementation(unreal.HyperlinkPythonBridge):

    @unreal.ufunction(override=True)
    def parse_url_string(self, in_string: str):
        return unquote(in_string)
    
    @unreal.ufunction(override=True)
    def escape_url_string(self, in_string: str):
        return quote(in_string)
    
    @unreal.ufunction(override=True)
    def get_system_paths(self):
        return sys.path
