#include "scene/BasicScene.hpp"
#include "config.hpp"

#include <iostream>

void BasicScene::init()
{
    glEnable(GL_DEPTH_TEST);
    stbi_set_flip_vertically_on_load(true);
    std::filesystem::path current_path = std::filesystem::current_path();
    std::filesystem::path vertexPath = current_path / "res/shaders/vertexModel.shader";
    std::filesystem::path fragmentPath = current_path / "res/shaders/fragmentModel.shader";
    m_shader = Shader(vertexPath.string().c_str(), fragmentPath.string().c_str());

    std::filesystem::path modelPath = current_path / "res/models/backpack/backpack.obj";
    m_model = Model(modelPath.string().c_str());

    m_shader.use();
    m_shader.setFloat("material.shininess", 32.0f);
    m_shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    m_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    m_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    m_shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    m_shader.setVec3("pointLights.position", 0.7f, 0.2f, 2.0f);
    m_shader.setVec3("pointLights.ambient", 0.05f, 0.05f, 0.05f);
    m_shader.setVec3("pointLights.diffuse", 0.8f, 0.8f, 0.8f);
    m_shader.setVec3("pointLights.specular", 1.0f, 1.0f, 1.0f);
    m_shader.setFloat("pointLights.constant", 1.0f);
    m_shader.setFloat("pointLights.linear", 0.09f);
    m_shader.setFloat("pointLights.quadratic", 0.032f);

    m_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    m_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    m_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    m_shader.setFloat("spotLight.constant", 1.0f);
    m_shader.setFloat("spotLight.linear", 0.09f);
    m_shader.setFloat("spotLight.quadratic", 0.032f);
    m_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    m_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f))); 
}

void BasicScene::update(float deltaTime, Camera &camera)
{
    m_shader.use();

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    m_model_matrix = glm::scale(m_model_matrix, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    m_shader.setMat4("model", m_model_matrix);

    m_projection_matrix = glm::perspective(glm::radians(camera.Zoom), (float)Config::WindowWidth / (float)Config::WindowHeight, 0.1f, 100.0f);
    m_view_matrix = camera.GetViewMatrix();
    m_shader.setMat4("projection", m_projection_matrix);
    m_shader.setMat4("view", m_view_matrix);

    m_shader.setVec3("viewPos", camera.Position);
    m_shader.setVec3("spotLight.position", m_lightPos);
    m_shader.setVec3("spotLight.direction", -m_lightPos);
}

void BasicScene::render()
{
    m_model.Draw(m_shader);
}

void BasicScene::cleanup()
{

}