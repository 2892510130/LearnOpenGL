#include "debug_ui.hpp"

void DebugUI::RenderUI(SceneManager &scene_manager)
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save")) {

            }
            if (ImGui::MenuItem("Load")) {

            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Scene Selector");
    
    auto& scenes = scene_manager.getScenes();
    
    for (int i = 0; i < scenes.size(); i++) {
        bool isSelected = (scene_manager.getCurrentIndex() == i);
        
        if (ImGui::Selectable(scenes[i]->getName(), isSelected)) {
            scene_manager.switchTo(i);
        }
    }
    
    ImGui::Separator();
    
    if (scene_manager.getCurrentIndex() != -1) {
        ImGui::Text("Current: %s", 
            scenes[scene_manager.getCurrentIndex()]->getName());
    }
    
    ImGui::End();
}