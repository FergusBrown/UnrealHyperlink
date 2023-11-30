# Unreal Hyperlink

Streamline collaboration on your project with this Unreal plugin. Generate hyperlinks which when clicked navigate the user to a specific location in the editor. For example, navigating to a node in a blueprint graph or an actor in a level. 

This plugin provides several link types out of the box but it's easy to implement your own links if you want to via C++, Blueprint or Python!


## Summary of Link Types

The 6 link types implemented in this plugin are summaried below. Most links can only be generated inside the editor with the exception of the Viewport link which can also be generated at runtime in a build which has developer tools enabled.

|Name|What it does|Where to generate |
|:--|:--|:--|
|Edit|Opens the asset editor for an asset. |Asset context menu in the content browser or asset menu in the asset editor.|
|Browse|Focuses a folder or asset in the content browser.|Asset or folder context menu in the content browser.|
|LevelActor|Opens a level and focuses on an actor.| Level editor actor context menu. |
|Node | Focuses a node in a graph editor. Only supports blueprints and materials at the moment. | Node context menu. |
|Script| Executes a Blutility's Run function or a python script. | Blutility links are copied from the content browser context menu. Python links are made on the command line with "uhl.CopyLink Script \<script path\>". |
|Viewport| Moves the viewport camera to the desired position and rotation. | In the level editor use the actor context menu and viewport burger menu (☰). At runtime use "uhl.CopyLink Viewport". |

## Install Instructions
Currently the plugin only works with UE 5.2/5.3

### Building from Source

1. Clone this repo or download its files.
2. Copy the UnrealHyperlink folder to the engine plugin folder ([UE Root]/Engine/Plugins) or your project's plugin folder (/[Project Root]/Plugins).
3. (Optional) If you want to have a look at some basic examples of extending the plugin with blueprint and python you can install the example plugin. To do this copy the HyperlinkExamples folder to your project's plugin folder. Once built you'll also need to enable this plugin in your project's plugin settings.
4. Build your project.