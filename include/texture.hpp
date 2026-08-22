#pragma once

#include <string>
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

enum class TextureType {
    Diffuse,
    Specular,
    Normal,
    Roughness,
    Metallic,
    AmbientOcclusion,
    Emissive
};

struct TextureProperties {
    bool gammaCorrection = false;
    bool flipImage = true;
    bool generateMipmaps = true;
    GLenum wrapS = GL_REPEAT;
    GLenum wrapT = GL_REPEAT;
    GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLenum magFilter = GL_LINEAR;
    TextureType type = TextureType::Diffuse;
};

class Texture {
public:
    Texture() = default;
    Texture(const std::string& texturePath, const TextureProperties &textureProperties);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    unsigned int getID() const { return m_ID; }

    void bind(unsigned int unit = 0) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_ID);
    }

private:
    unsigned int m_ID;
    std::string m_texturePath;
    TextureProperties m_properties;

    bool loadTexture();
};