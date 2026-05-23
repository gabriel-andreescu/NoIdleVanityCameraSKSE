# No Idle Vanity Camera SKSE

Lightweight SKSE plugin that completely disables the idle rotating camera around the player.

---

## Clone and Build

Open terminal (e.g., PowerShell) and run the following commands:

```powershell
git clone --recurse-submodules -j8 https://github.com/gabriel-andreescu/NoIdleVanityCameraSKSE.git
cd NoIdleVanityCameraSKSE
cmake --preset=default
cmake --build build --config RelWithDebInfo
```

Optionally:

```powershell
cp CMakeUserPresets.json.example CMakeUserPresets.json
# Edit CMakeUserPresets.json and set DEPLOY_DIR to your Skyrim Data directory
cmake --preset=user-default
cmake --build --preset=release
```

### **Debugging**

- [Steamless](https://github.com/atom0s/Steamless/releases)

- build [SKSE](https://github.com/ianpatt/skse64) from sources with the Debug config
- copy the built files and their PDB to the Skyrim folder
- run `skse64_loader.exe`
- attach debugger to `SkyrimSE.exe`

### **Deployment**

When `DEPLOY_DIR` is set (for example via `CMakeUserPresets.json` created from `CMakeUserPresets.json.example`), any
successful build will automatically copy the plugin (and its PDB) to:

`<DEPLOY_DIR>/SKSE/Plugins`

For deployment to multiple targets, split the paths with a `;`.

If you use the example user presets, running:

`cmake --build --preset=release`

will build the `RelWithDebInfo` configuration and deploy the plugin to your Skyrim Data directory.

## Requirements

- [Git](https://git-scm.com/downloads)
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
    - Desktop development with C++
- [CMake](https://cmake.org/)
    - Add the cmake.exe install path to the `PATH` environment variable
- [Vcpkg](https://learn.microsoft.com/en-us/vcpkg/get-started/get-started?pivots=shell-powershell#1---set-up-vcpkg)
    - Add a new `VCPKG_ROOT` environment variable pointing to the root folder of vcpkg (e.g., `C:\vcpkg`)

This project is developed using the **non-commercial** version of [CLion](https://www.jetbrains.com/clion/)

### **Register Visual Studio as a Generator**

Open PowerShell and run the following command:

```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" amd64
```

---

## User Requirements

- [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

---

## Compatibility

This is an SE/AE SKSE plugin. Skyrim VR runtime support is disabled.

Mods that replace the idle camera with another behavior, such
as [Sandbox When Idle](https://www.nexusmods.com/skyrimspecialedition/mods/131350), are alternatives rather than
something to combine with this.

---

## Credits

Thanks to the entire open source community, it provided a ton of invaluable information during my learning.
A special thanks to [Doodlum](https://github.com/doodlum), who pointed me in the right direction when I started.

The following projects and repositories were directly used or served as important references for my mods:

- [CommonLibSSE NG](https://github.com/alandtse/CommonLibVR/tree/ng)
- [CLibUtil](https://github.com/powerof3/CLibUtil)
- [skyrim-community-shaders](https://github.com/doodlum/skyrim-community-shaders)
- [CLibNGPluginTemplate](https://github.com/ThirdEyeSqueegee/CLibNGPluginTemplate)
- [powerof3's repos](https://github.com/powerof3)
- [Monitor221hz's repos](https://github.com/Monitor221hz)
- ThirdEyeSqueegee's repos
- the xRE SE Discord channel

---
