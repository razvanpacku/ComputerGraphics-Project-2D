#pragma once
#include "Texture.h"

#include <unordered_map>
#include <memory>
#include <string>

//static class for managing textures
class TextureManager
{
public:
	static std::shared_ptr<Texture> Load(const std::string& path);
	static void Clear();
private:
	static std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
};

