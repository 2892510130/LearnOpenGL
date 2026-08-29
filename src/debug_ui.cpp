#include "debug_ui.hpp"
#include "imgui.h"

void DebugUI::RenderUI(SceneManager &scene_manager)
{
    ImGui::Begin("Scene Selector");
    
    auto& scenes = scene_manager.getScenes();
    
    for (int i = 0; i < scenes.size(); i++) {
        bool isSelected = (scene_manager.getCurrentIndex() == i);
        
        if (ImGui::Selectable(scenes[i]->getName(), isSelected)) {
            scene_manager.switchTo(i);
        }
    }
    
    if (scene_manager.getCurrentIndex() != -1) {
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::Text("Current scene: %s", scenes[scene_manager.getCurrentIndex()]->getName());
        ImGui::Separator();

        scene_manager.render_ui();
    }
    

    ImGui::End();
}
