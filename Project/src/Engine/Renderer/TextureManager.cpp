#include "./TextureManager.h"

std::unordered_map<std::string, std::shared_ptr<Texture>> TextureManager::textureCache;

std::shared_ptr<Texture> TextureManager::Load(const std::string& path)
{
    auto it = textureCache.find(path);
    if (it != textureCache.end())
        return it->second; // reuse existing

    auto texture = std::make_shared<Texture>(path);
    textureCache[path] = texture;
    return texture;
}

void TextureManager::Clear()
{
    textureCache.clear();
}