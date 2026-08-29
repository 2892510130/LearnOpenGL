#pragma once

#include <memory>

#include "Scene.hpp"
#include "BasicScene.hpp"
#include "ShadowScene.hpp"
#include "PointShadowScene.hpp"

class SceneManager {
public:
    SceneManager() { registerScenes(); };
    
    void registerScenes() {
        m_scenes.push_back(std::make_unique<BasicScene>());
        m_scenes.push_back(std::make_unique<ShadowScene>());
        m_scenes.push_back(std::make_unique<PointShadowScene>());
    }
    
    void switchTo(int index) {
        if (index < 0 || index >= m_scenes.size()) return;
        if (index == m_currentIndex) return;
        
        if (m_currentIndex != -1) {
            m_scenes[m_currentIndex]->cleanup();
        }
        
        m_currentIndex = index;
        m_scenes[m_currentIndex]->init();
    }
    
    void update(float deltaTime, Camera &camera) {
        if (m_currentIndex != -1) {
            m_scenes[m_currentIndex]->update(deltaTime, camera);
        }
    }
    
    void render() {
        if (m_currentIndex != -1) {
            m_scenes[m_currentIndex]->render();
        }
    }

    void render_ui() {
        if (m_currentIndex != -1) {
            m_scenes[m_currentIndex]->render_ui();
        }
    }
    
    const std::vector<std::unique_ptr<Scene>>& getScenes() const {
        return m_scenes;
    }
    
    int getCurrentIndex() const { return m_currentIndex; }
    
private:
    std::vector<std::unique_ptr<Scene>> m_scenes;
    int m_currentIndex = -1;
};
