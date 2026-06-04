# Asset Manifest Specification (assets_manifest_spec.md)

This document defines the schema, validation rules, naming conventions, and formats for the central asset manifest catalog file `assets_manifest.json` utilized in Cyberpunk Cannon Shooter.

---

## 1. Role of the Manifest

In compliance with [ADR-0003](adr/ADR-0003-resource-manager.md), C++ gameplay views and states must never contain hardcoded disk paths (e.g. `"assets/textures/cannon.png"`). All resources are resolved using unique manifest string IDs. The manifest file maps these logical IDs to physical file paths.

---

## 2. Manifest Schema Definition

The manifest is stored as a single JSON file (`assets/assets_manifest.json`) using the following structure:

```json
{
  "textures": {
    "cannon_base": "textures/cannon.png",
    "brick_crystal": "textures/brick.png",
    "canister_multishot": "textures/canister_multi.png"
  },
  "audio": {
    "brick_shatter": "audio/sfx/shatter.wav",
    "theme_stage1": "audio/music/neon_slums.ogg"
  },
  "fonts": {
    "neon_large": "fonts/orbitron_bold.ttf",
    "hud_numeric": "fonts/roboto_mono.ttf"
  },
  "shaders": {
    "bloom_pass": "shaders/bloom.frag",
    "crt_scanline": "shaders/crt.frag"
  }
}
```

### Path Resolution Rules
1. **Base Directory**: Path strings listed in the manifest are relative to the root asset directory (`assets/`).
2. **Pack Resolution**: During production release packaging, the packing utility (`packer.py`) reads this manifest to compress paths relative to the generated binary archive (`assets.pack`).

---

## 3. Manifest Invariant Rules

Reviewers and asset pipeline scripts enforce the following constraints:

### 1. Key Taxonomy Rules
* **String Identifier Format**: All manifest keys (logical resource IDs) must use `snake_case` (lowercase with underscores).
* **Uniqueness**: Registry IDs must be unique across the entire manifest namespace (e.g., you cannot have a texture and a font both sharing the identifier `"hud_numeric"`).
* **Descriptive prefixes**: IDs should begin with a category descriptor:
  - Textures: `texture_` or specific noun context (e.g., `brick_crystal`, `cannon_base`).
  - Audio: `sfx_` or `music_` (e.g., `sfx_click`, `music_stage1`).
  - Shaders: `shader_` (e.g., `shader_bloom`).

### 2. Supported Formats
* **Textures**: `.png` (restricted to 32-bit RGBA maps).
* **SFX**: `.wav` (uncompressed PCM formats for immediate, low-latency playback).
* **Music Loops**: `.ogg` (compressed OGG Vorbis streams for memory-efficient background music playback).
* **Fonts**: `.ttf` (TrueType Font formats) or `.otf` (OpenType formats).
* **Shaders**: `.frag` (fragment GLSL shader files) or `.vert` (vertex GLSL shader files).

---

## 4. Pipeline Validation Checks

Our CI pipeline executes `tools/verify_assets.py` to validate manifest compliance on every PR merge:
- **Parse Verify**: Asserts that `assets_manifest.json` is valid, parsing-error-free JSON.
- **Reference Check**: Validates that every file path listed in the manifest actually exists on the filesystem under the `assets/` directory.
- **Licensing Compliance**: Confirms that every listed asset contains a matching license reference inside the legal registry (`assets/licenses.txt`).
