# Shaders

**Used on:** Day 158 (three shader effects). Optional, but cheap and high impact.

Why this matters: a handful of small shaders give you effects that would be impossible or expensive
otherwise - a whole-sprite white flash, a palette shift for a flashback, a dissolve transition. You do
not need to become a graphics programmer. You need about 50 lines of GLSL total.

This note is deliberately narrow: the minimum to get three useful effects working, plus the pitfalls.

---

## 1. The mental model

The GPU runs your shader once per **vertex** and once per **pixel** (fragment). For 2D work you almost
always leave the vertex shader alone and write only fragment shaders.

A fragment shader's job: given a texture coordinate, output a colour.

```glsl
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;   // the sprite/texture raylib binds
uniform vec4 colDiffuse;      // raylib's tint

out vec4 finalColor;

void main() {
    vec4 texel = texture(texture0, fragTexCoord);
    finalColor = texel * colDiffuse * fragColor;
}
```

That is the pass-through shader - it does exactly what raylib does by default. Every effect below is a
modification of that one function.

**Key terms:** fragment shader, vertex shader, uniform, varying/in-out, texel, sampler, GLSL version,
normalised coordinates (0 to 1).

**Questions to answer**

- What is the difference between a uniform and an input?
- Why are texture coordinates 0 to 1 rather than in pixels?
- How many times does your fragment shader run for a full-screen effect at 480x270? (129,600.)

---

## 2. Using shaders in raylib

Three steps:

```cpp
Shader s = LoadShader(nullptr, "assets/shaders/flash.fs");   // default vertex shader
int loc = GetShaderLocation(s, "flashAmount");
SetShaderValue(s, loc, &amount, SHADER_UNIFORM_FLOAT);
BeginShaderMode(s);
    DrawTexture(...);   // anything drawn here goes through the shader
EndShaderMode();
```

Practical notes:

- Cache uniform locations at load; do not call `GetShaderLocation` every frame.
- Sprite shaders wrap individual `DrawTexture` calls; full-screen shaders wrap drawing your render
  texture (Day 42's virtual resolution makes this natural).
- Use `#version 330` for desktop GL. If you ever target web, `#version 100` and different syntax are
  needed - a good reason to keep shaders trivial.
- `LoadShader` failing returns a default shader and logs a warning; check and fall back rather than
  assuming success.

---

## 3. The three shaders worth writing

**White flash** (hit feedback). Mix the texel toward white by an amount:

```glsl
finalColor = mix(texel, vec4(1.0, 1.0, 1.0, texel.a), flashAmount) * colDiffuse;
```

Cleaner and cheaper than maintaining a second set of white sprites, and it works on every sprite
automatically.

**Palette / colour grade** (mood, flashbacks, poison vignette). Convert to greyscale and remap, or
multiply by a tint, or look up a palette texture. Applied full-screen to your render texture:

```glsl
float lum = dot(texel.rgb, vec3(0.299, 0.587, 0.114));
finalColor = vec4(mix(texel.rgb, tint.rgb * lum, strength), texel.a);
```

**Dissolve / pixelate transition** (battle entry). Quantise the texture coordinate to grow blocky, or
compare a noise value against a threshold and discard below it. Both are a few lines and look far more
deliberate than a plain fade.

Optional extras, in order of value: water distortion (offset the coordinate with a sine of time), a
vignette (darken by distance from the centre), and a simple 2D light overlay for dungeons.

---

## 4. Pitfalls

- **Fallback path.** If shader compilation fails on a player's driver, the game must still run. Check the
  shader loaded, and skip `BeginShaderMode` if not. A crash on startup on someone else's machine is the
  worst possible bug, and shaders are the most driver-dependent thing you will ship.
- **Render texture orientation.** raylib's render textures are stored upside down; full-screen shaders
  applied to them need the same negative-source-height handling as normal drawing.
- **Precision and version differences.** Desktop GLSL and GLSL ES differ. Keep shaders simple and you
  avoid most of it.
- **Uniforms not set.** An unset uniform is zero, which usually means the effect silently does nothing.
  If a shader appears to do nothing, print the uniform location - `-1` means the name did not match.
- **Cost.** Full-screen shaders run per pixel. At your internal resolution this is negligible, but a
  shader applied per sprite with an expensive loop inside is not. Measure (Day 159).
- **Point filtering.** Sampling with linear filtering in a pixel-art game blurs it. Keep
  `TEXTURE_FILTER_POINT`.

---

## 5. Scope discipline

Shaders are a rabbit hole with a very appealing entrance. On Day 158 you have one day.

- Write three effects, each under 20 lines.
- Do not write a lighting system, a normal-mapped 2D pipeline, or a post-processing stack.
- Measure each effect's frame time cost and drop any that is not clearly worth it.
- If a shader is taking more than an hour, cut it and move on. It is polish, not a requirement.

Everything else you want to learn about shaders will still be there after you ship, and you will learn it
faster with a finished game behind you.

---

## Exercises

1. Get the pass-through shader working and confirm the game looks unchanged. That proves your plumbing.
2. Modify it to tint everything red. This is your "shaders work" milestone.
3. Write the white flash and hook it to hit feedback.
4. Write the full-screen colour grade and use it for one area's mood.
5. Write the dissolve transition for battle entry.
6. Deliberately break a shader (a syntax error) and confirm your fallback path keeps the game running.
7. Measure each shader's cost in your frame time graph.

## Done criteria

- [ ] I can write a fragment shader that samples a texture and modifies the colour
- [ ] Uniform locations are cached, and I can debug an unset uniform
- [ ] Three effects work: flash, colour grade, and a transition
- [ ] A shader compilation failure does not crash or blank the game
- [ ] I have measured each effect's frame time cost

## Sources

- [The Book of Shaders](https://thebookofshaders.com/) - chapters 1-5 are all you need, and they are
  excellent
- raylib `shaders_*` examples - especially `shaders_texture_tiling`, `shaders_palette_switch`, and
  `shaders_postprocessing`
- [Shadertoy](https://www.shadertoy.com/) - for inspiration; most of it is far beyond what you need
- Search "2d sprite shader white flash hit effect" for the exact effect from section 3
