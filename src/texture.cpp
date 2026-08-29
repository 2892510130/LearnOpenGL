#include "texture.hpp"

#include <iostream>

Texture::Texture(const std::string& texturePath, const TextureProperties &textureProperties)
: m_texturePath(texturePath), m_properties(textureProperties)
{
    loadTexture();
}

Texture::Texture(Texture&& other) noexcept
{
    m_ID = other.m_ID;
    other.m_ID = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other) {
        if (m_ID) glDeleteTextures(1, &m_ID);
        m_ID = other.m_ID;
        other.m_ID = 0;
    }
    return *this;
}

Texture::~Texture() {
    if (m_ID) {
        glDeleteTextures(1, &m_ID);
    }
}

bool Texture::loadTexture()
{
    if (m_ID) {
        glDeleteTextures(1, &m_ID);
        m_ID = 0;
    }

    // 设置翻转
    stbi_set_flip_vertically_on_load(m_properties.flipImage);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(m_texturePath.c_str(), &width, &height, &nrComponents, 0);
    
    if (!data) {
        std::cout << "Texture failed to load at path: " << m_texturePath << std::endl;
        return false;
    }

    GLenum internalFormat;
    GLenum dataFormat;
    if (nrComponents == 1) {
        internalFormat = dataFormat = GL_RED;
    } else if (nrComponents == 3) {
        internalFormat = m_properties.gammaCorrection ? GL_SRGB : GL_RGB;
        dataFormat = GL_RGB;
    } else if (nrComponents == 4) {
        internalFormat = m_properties.gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
        dataFormat = GL_RGBA;
    }

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    
    if (m_properties.generateMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_properties.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_properties.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_properties.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_properties.magFilter);

    stbi_image_free(data);
    return true;
}
