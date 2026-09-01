#include "debug_ui.hpp"
#include "imgui.h"
#include <malloc.h>

void DebugUI::RenderUI(SceneManager &scene_manager, float delta_time)
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

    float currentFPS = 1 / delta_time;
    if (m_smooth_fps == 0.0f)
    {
        m_smooth_fps = currentFPS;
    }
    else
    {
        m_smooth_fps = m_smooth_fps * (1 - m_alpha) + currentFPS * m_alpha;
    }
    ImGui::Text("FPS: %.2f", m_smooth_fps);

    if (scene_manager.getCurrentIndex() != -1) {
        ImGui::Separator();
        ImGui::Text("Current scene: %s", scenes[scene_manager.getCurrentIndex()]->getName());
        ImGui::Separator();

        scene_manager.render_ui();
    }
    

    ImGui::End();
}
