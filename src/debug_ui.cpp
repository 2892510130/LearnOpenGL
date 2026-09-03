#include "debug_ui.hpp"
#include "imgui.h"
#include <malloc.h>

void DebugUI::RenderUI(SceneManager &scene_manager)
{
    ImGui::Begin("Scene Selector");
    
    auto& scenes = scene_manager.getScenes();
    
    if (ImGui::TreeNode("Scenes")) {
        for (int i = 0; i < scenes.size(); i++) {
            bool isSelected = (scene_manager.getCurrentIndex() == i);
        
            if (ImGui::Selectable(scenes[i]->getName(), isSelected)) {
                scene_manager.switchTo(i);
            }
        }
        ImGui::TreePop();
    }

    auto &io = ImGui::GetIO();
    ImGui::Text("FPS: %.2f", io.Framerate); // rolling average over 60 frames

    if (scene_manager.getCurrentIndex() != -1) {
        ImGui::Separator();
        ImGui::Text("Current scene: %s", scenes[scene_manager.getCurrentIndex()]->getName());
        ImGui::Separator();

        scene_manager.render_ui();
    }
    

    ImGui::End();
}
