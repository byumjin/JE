#ifndef ASSET_H
#define ASSET_H


#include <string>
#include <tuple>

// Base class for assets like meshes, textures, shaders etc.
// Some of these assets will persist in memory as long as the user decides
class Asset
{
public:
	Asset();

	// Not all assets must implement this, but if you want it to be cached you must
	virtual void LoadFromFilename(std::string filename);
	virtual ~Asset();
};

#endif // ASSET_H
