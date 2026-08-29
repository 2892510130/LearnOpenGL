#pragma once

#include <memory>
#include <vector>
#include <filesystem>

#include "camera.hpp"

class Scene {
public:
    virtual ~Scene() = default;
    virtual void init() = 0;
    virtual void update(float deltaTime, Camera &camera) = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
    virtual void render_ui() = 0;
    virtual const char* getName() const = 0;
};
