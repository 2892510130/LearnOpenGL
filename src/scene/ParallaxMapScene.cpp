#include "scene/ParallaxMapScene.hpp"
#include "camera.hpp"
#include "imgui.h"
#include "texture.hpp"
#include "config.hpp"

#include <GLFW/glfw3.h>

void ParallaxMapScene::init()
{
    glEnable(GL_DEPTH_TEST);

    std::filesystem::path current_path = std::filesystem::current_path();
    std::filesystem::path vertexPath = current_path / "res/shaders/parallax_map/pm.vs";
    std::filesystem::path fragmentPath = current_path / "res/shaders/parallax_map/pm.fs";
    std::filesystem::path baseVertexPath = current_path / "res/shaders/normal_map/base.vs";
    std::filesystem::path baseFragmentPath = current_path / "res/shaders/normal_map/base.fs";
    m_shader = Shader(vertexPath.string().c_str(), fragmentPath.string().c_str());
    m_base_shader = Shader(baseVertexPath.string().c_str(), baseFragmentPath.string().c_str());

    std::filesystem::path diffuseMapPath = current_path / "res/textures/bricks2.jpg";
    std::filesystem::path normalMapPath = current_path / "res/textures/bricks2_normal.jpg";
    std::filesystem::path depthMapPath = current_path / "res/textures/bricks2_disp.jpg";
    std::filesystem::path diffuseMapPath2 = current_path / "res/textures/wood.png";
    std::filesystem::path normalMapPath2 = current_path / "res/textures/toy_box_normal.png";
    std::filesystem::path depthMapPath2 = current_path / "res/textures/toy_box_disp.png";

    TextureProperties prop;
    m_diffuse_texture = Texture(diffuseMapPath.string().c_str(), prop);
    m_normal_texture = Texture(normalMapPath.string().c_str(), prop);
    m_depth_texture = Texture(depthMapPath.string().c_str(), prop);
    m_diffuse_texture2 = Texture(diffuseMapPath2.string().c_str(), prop);
    m_normal_texture2 = Texture(normalMapPath2.string().c_str(), prop);
    m_depth_texture2 = Texture(depthMapPath2.string().c_str(), prop);

    m_shader.use();
    m_shader.setInt("diffuseMap", 0);
    m_shader.setInt("normalMap", 1);
    m_shader.setInt("depthMap", 2);

    m_base_shader.use();
    m_base_shader.setInt("diffuseMap", 0);

}

void ParallaxMapScene::update(float deltaTime, Camera &camera)
{
    m_projection_matrix = glm::perspective(glm::radians(camera.Zoom), (float)Config::WindowWidth / (float)Config::WindowHeight, 0.1f, 100.0f);
    m_view_matrix = camera.GetViewMatrix();
    m_cameraPos = camera.Position;
}

void ParallaxMapScene::render()
{
    m_model_matrix = glm::mat4(1.0f);
    if (m_enable_rotate)
    {
        m_model_matrix = glm::rotate(m_model_matrix, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
    }

    if (!m_use_basic)
    {
        // render the plane
        m_shader.use();
        m_shader.setMat4("projection", m_projection_matrix);
        m_shader.setMat4("view", m_view_matrix);
        m_shader.setMat4("model", m_model_matrix);
        m_shader.setVec3("viewPos", m_cameraPos);
        m_shader.setVec3("lightPos", m_lightPos);
        m_shader.setBool("useBasicPM", !m_enable_steep);
        m_shader.setBool("enableDiscard", m_enable_discard);
        m_shader.setBool("divideZ", m_divide_z);
        m_shader.setFloat("heightScale", m_height_scale);
        m_shader.setFloat("maxLayers", m_max_layer);
        if (!m_use_toy_box)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_diffuse_texture.m_ID);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_normal_texture.m_ID);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_depth_texture.m_ID);
        }
        else
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_diffuse_texture2.m_ID);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_normal_texture2.m_ID);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_depth_texture2.m_ID);
        }

        renderQuad();
    }
    else
    {
        m_base_shader.use();
        m_base_shader.setMat4("projection", m_projection_matrix);
        m_base_shader.setMat4("view", m_view_matrix);
        m_base_shader.setMat4("model", m_model_matrix);
        m_base_shader.setVec3("viewPos", m_cameraPos);
        m_base_shader.setVec3("lightPos", m_lightPos);
        glActiveTexture(GL_TEXTURE0);
        if (!m_use_toy_box)
            glBindTexture(GL_TEXTURE_2D, m_diffuse_texture.m_ID);
        else
            glBindTexture(GL_TEXTURE_2D, m_diffuse_texture2.m_ID);
        renderBaseQuad();
    }
}

void ParallaxMapScene::render_ui()
{
    ImGui::Checkbox("Use Basic Shader", &m_use_basic);
    ImGui::Checkbox("Use Steep PM", &m_enable_steep);
    ImGui::Checkbox("Enable Discard", &m_enable_discard);
    ImGui::Checkbox("Enable Rotate", &m_enable_rotate);
    ImGui::Checkbox("Divede Z", &m_divide_z);
    ImGui::Checkbox("Use Toy Box", &m_use_toy_box);
    ImGui::SliderFloat("Height Scale", &m_height_scale, 0.0f, 0.5f);
    ImGui::SliderFloat("Max Layers of Steep", &m_max_layer, 24.0f, 64.0f);
}

void ParallaxMapScene::cleanup()
{

}

void ParallaxMapScene::renderBaseQuad()
{
    if (m_baseQuadVAO == 0)
    {
        // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);  
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);
        
        float quadVertices[] = {
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y
        };
        
        glGenVertexArrays(1, &m_baseQuadVAO);
        glGenBuffers(1, &m_baseQuadVBO);
        glBindVertexArray(m_baseQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_baseQuadVBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(m_baseQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


void ParallaxMapScene::renderQuad()
{
    if (m_quadVAO == 0)
    {
        // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);


        float quadVertices[] = {
            // positions            // normal         // texcoords  // tangent                          // bitangent
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &m_quadVBO);
        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
