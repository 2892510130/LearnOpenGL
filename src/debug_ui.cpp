#include "debug_ui.hpp"

void DebugUI::RenderUI()
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
}