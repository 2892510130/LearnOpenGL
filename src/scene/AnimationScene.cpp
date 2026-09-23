#include "scene/AnimationScene.hpp"
#include "config.hpp"

void AnimationScene::init()
{
    stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);
    
    std::filesystem::path current_path = std::filesystem::current_path();
    std::filesystem::path aniVexPath   = current_path / "res/shaders/animation/ani.vs";
    std::filesystem::path aniFragPath   = current_path / "res/shaders/animation/ani.fs";
	m_shader = Shader(aniVexPath.string().c_str(), aniFragPath.string().c_str());

    auto modelPath = current_path / "res/models/vampire/dancing_vampire.dae";
	m_model = AnimationModel(modelPath.string().c_str());
	m_dancing_animation = Animation(modelPath.string().c_str(), &m_model);
	m_animator = Animator(&m_dancing_animation);
}

void AnimationScene::update(float deltaTime, Camera &camera)
{
    m_animator.UpdateAnimation(deltaTime);
    m_projection_matrix = glm::perspective(glm::radians(camera.Zoom), (float)Config::WindowWidth / (float)Config::WindowHeight, 0.1f, 200.0f);
    m_view_matrix = camera.GetViewMatrix();
    m_cameraPos = camera.Position;
}

void AnimationScene::render()
{
    m_shader.use();
    m_shader.setMat4("projection", m_projection_matrix);
	m_shader.setMat4("view", m_view_matrix);

    auto transforms = m_animator.GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
		m_shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

    glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	m_shader.setMat4("model", model);
	m_model.Draw(m_shader);
}

void AnimationScene::cleanup()
{

}

void AnimationScene::render_ui()
{

}

