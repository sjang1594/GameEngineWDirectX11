## Project Creation:
To set up a DirectX11 project, follow the DirectXTK documentation for creating a basic game loop, as referenced: DirectXTK Basic Game Loop. Here¡¯s a concise guide to get started:
[DirectXTK](https://github.com/microsoft/DirectXTK/wiki/The-basic-game-loop)

## Environment Setup:
1. Manual Setup:
* Install Visual Studio (preferably 2022, Community Edition is fine).
* Ensure you have the Windows SDK installed (version 10.0.19041.0 or later for DirectX11 compatibility).
* Clone or download the DirectXTK library from GitHub.
* Set up a C++ project in Visual Studio, configuring it for DirectX11:
* Link against d3d11.lib, dxgi.lib, and DirectXTK.lib.
* Include necessary headers (e.g., d3d11.h, DirectXTK.h).
 
2. Simple Approach:
* Use Vcpkg

## Project CheckPoint:
It's important to figure out what to implement and the ideas are coming from the the image below 

<img src="Misc/jaKUP.png" alt="jaKUP" width="50%">

## Check Points
### CubeMapping [o]:

Status: Implemented.
Details: Cube mapping is likely used for a skybox or environment mapping (e.g., reflections on objects). You¡¯ve completed this, so your project likely has:
A cube map texture (six faces: +X, -X, +Y, -Y, +Z, -Z).
A skybox shader rendering the cube map.
Possibly environment mapping on objects for reflections.

### Height Mapping Settings [o]:
Status: In progress.
Details: Height mapping typically refers to techniques like:
Parallax Mapping: Uses a height map to offset texture coordinates for depth illusion.
Displacement Mapping: Modifies vertex positions based on a height map (requires tessellation).
Terrain Rendering: Uses a height map to define terrain geometry.

Tools:
Load the height map with DDSTextureLoader or WICTextureLoader (for PNG/JPG).
Use a normal map alongside the height map for better lighting.

### Schlick-Fresnel Effect [o]:
Status: Implemented.
Details: Likely refers to the Schlick-Fresnel effect, a simplified Fresnel approximation used in physically-based rendering (PBR) to model how light reflects more at grazing angles.
Implementation Notes:

Calculate the Fresnel term using Schlick¡¯s approximation:
$$F = F_0 + (1 - F_0) \cdot (1 - \cos\theta)^5$$
where $ F_0 $ is the base reflectivity, and $ \theta $ is the angle between the view vector and surface normal.
This is typically part of a PBR pipeline, affecting specular highlights.

Ensuring the effect integrates with your cube mapping (e.g., environment map reflections scale with Fresnel).
Test edge cases (e.g., grazing angles on smooth surfaces) to confirm visual accuracy. [TODO]

### HDRI [x]
Status: Not started.
Details: High Dynamic Range Imaging (HDRI) is used for realistic lighting, often with cube maps for environment lighting. The provided resource (DaySkyHDRI013B) is an HDRI texture for skyboxes or image-based lighting (IBL).
Implementation Guide:

Texture: Download the HDRI from ambientCG (e.g., in EXR or HDR format). Convert to a DDS cube map using tools like IBLBaker or NVIDIA Texture Tools.
IBL Setup:

Use the HDRI as a cube map for the skybox (similar to your existing cube mapping).
For IBL, precompute:

Irradiance Map: Convolve the HDRI for diffuse lighting.
Specular Map: Create a prefiltered environment map for specular reflections (split into mip levels for roughness).

Tools
IBLBaker: Use this tool to generate irradiance and specular maps from the HDRI. Integrate these into your shaders.
Shader: Modify your PBR shader to sample the irradiance map for diffuse lighting and the specular map for reflections, combining with the Fresnel effect.
DirectXTK: Use CreateDDSTextureFromFile to load the processed HDRI textures.
Priority: Since cube mapping is done, HDRI is a natural extension. Start by loading the ambientCG HDRI and rendering it as a skybox.

### GLTF Loader [x]:
Status: Not started.
Details: GLTF (GL Transmission Format) is a standard for 3D models, supporting meshes, materials, and animations. Loading GLTF models will allow you to import complex assets.
Implementation Guide:

Library: Use a GLTF loader like tinygltf or Microsoft¡¯s glTF SDK.
Integration:

Parse the GLTF file to extract meshes, textures, and materials.
Convert GLTF data to DirectX11-compatible formats (e.g., vertex buffers, index buffers).
Load textures using DirectXTK¡¯s texture loaders.

### Object Collision & Picking [x]:

Status: Not started.
Details: Collision detection involves checking if objects intersect, while picking involves selecting objects via mouse clicks (e.g., raycasting).
Implementation Guide:

Collision:

Use bounding volumes (e.g., spheres, AABBs) for simple collision detection.
For precise collision, implement triangle-ray intersection tests.
DirectXTK¡¯s SimpleMath::BoundingBox and BoundingSphere can simplify this.

Picking:

Convert mouse coordinates to a 3D ray using the camera¡¯s projection and view matrices.
Test the ray against object bounding volumes or triangles.
Example: Use SimpleMath::Ray to perform ray-AABB tests.

### Unreal PBR & Disney []:
Status: In progress.
Details: Likely refers to implementing a physically-based rendering (PBR) pipeline similar to Unreal Engine¡¯s, incorporating metallic-roughness workflows, IBL, and effects like Schlick-Fresnel.
Implementation Guide:

PBR Components:

Albedo: Base color texture.
Normal Map: For surface detail.
Metallic/Roughness: Textures defining material properties.
IBL: Use HDRI (irradiance + specular maps) for lighting.
Fresnel: Already implemented (Schlick-Fresnel).

Shader:
Write an HLSL shader using the Cook-Torrance BRDF model.
Sample albedo, normal, metallic, and roughness textures.
Combine with IBL (from HDRI feature) and direct lighting (e.g., a point light).

DirectXTK: Use PBREffect from DirectXTK for a pre-built PBR shader, or write a custom one for learning.
Progress: Since Schlick-Fresnel is done, focus on integrating metallic-roughness textures and IBL (ties into HDRI feature).
Resources: Study Unreal¡¯s PBR pipeline via Unreal Engine documentation for inspiration.

## Resource & Assets

### HDRI Texture (DaySkyHDRI013B):
Download the HDRI (EXR/HDR format) from ambientCG.
Use IBLBaker to convert it to a cube map and generate irradiance/specular maps.
Load into DirectX11 using DDSTextureLoader.
* [HDRI Texture Example](https://ambientcg.com/view?id=DaySkyHDRI013B)

### IBLBaker:
A tool for precomputing IBL data (irradiance and specular maps) from HDRIs.
Follow its documentation to process the ambientCG HDRI.
Output DDS files compatible with DirectXTK.
* [IBLBaker](https://github.com/derkreature/IBLBaker)

### Ground Texture
* [GroundTexture](https://cgaxis.com/)
