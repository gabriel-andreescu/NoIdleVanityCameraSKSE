set_xmakever("3.1.1")
set_project("NoIdleVanityCameraSKSE")
set_license("MIT")
set_policy("package.requires_lock", true)

local version = "2.0.1"

add_repositories("bmk https://github.com/gabriel-andreescu/BethesdaModKit.git")
add_addons("bmk 0.3.0")
includes("@addon/bmk/project")
includes("@addon/bmk/native")

-- Dependencies
add_requires("commonlibsse-ng 8.0.1", { system = false })

-- Build targets

target("Native", function()
    set_default(false)
    set_basename("NoIdleVanityCameraSKSE")
    set_version(version)
    add_rules("@commonlibsse-ng/plugin", {
        author = "gabriel-andreescu",
        description = "Disables the idle vanity camera.",
    })
    add_rules("@addon/bmk/skyrim.plugin")
    add_files("$(projectdir)/src/**.cpp")
    add_includedirs("$(projectdir)/src")
    set_pcxxheader("src/PCH.h")
    add_packages("commonlibsse-ng")
end)

-- Packages
target("NoIdleVanityCameraSKSE", function()
    set_version(version)
    add_rules("@addon/bmk/skyrim.package", {
        targets = {
            "Native",
        },
        nexus = {
            mod_id = "7318624438998",
            file_id = "6207775",
            category = "main",
            primary = true,
            display_name = "No Idle Vanity Camera SKSE",
        },
    })
    add_installfiles("$(projectdir)/assets/(**)")
end)
