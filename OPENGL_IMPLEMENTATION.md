# OpenGL Implementation Guide

## Overview

This project now includes a comprehensive **OpenGL graphics rendering pipeline** built on top of Three.js's WebGL renderer. The implementation provides professional-grade graphics capabilities including custom shaders, advanced lighting models, and a complete graphics pipeline.

## Components

### 1. **OpenGLRenderer.ts**
High-level OpenGL rendering engine that provides:
- WebGL context management
- Shader material application
- Lighting system (Directional, Point, Spot lights)
- Graphics pipeline orchestration
- Screenshot and render target capture capabilities
- Performance monitoring

**Key Features:**
```typescript
const glRenderer = new OpenGLRenderer(canvas, scene, camera, {
  antialias: true,
  enableDepthTest: true,
  enableFaceCulling: true,
  clearColor: new THREE.Color(0x87CEEB)
});

// Add lights
glRenderer.addLight({
  type: 'directional',
  position: new THREE.Vector3(15, 30, 10),
  color: new THREE.Color(1, 1, 1),
  intensity: 1.2
});

// Apply shaders to meshes
glRenderer.applyShader(mesh, 'phong');

// Control rendering
glRenderer.start();
glRenderer.render();
glRenderer.stop();
```

### 2. **GLGraphicsPipeline.ts**
Advanced graphics pipeline with:
- Render pass management
- Shader uniform updates
- Deferred rendering support
- Render target management
- Pipeline state control (depth testing, culling, blending)

**Key Features:**
```typescript
const pipeline = glRenderer.getPipeline();

// Configure pipeline state
pipeline.setPipelineState({
  depthTest: true,
  depthWrite: true,
  cullFace: true,
  blend: true,
  blendFunc: [THREE.SrcAlphaFactor, THREE.OneMinusSrcAlphaFactor]
});

// Update shader uniforms
pipeline.updateShaderUniforms('phong', {
  uLightPosition: new THREE.Vector3(10, 20, 10),
  uLightColor: new THREE.Color(1, 1, 1),
  uAmbient: 0.3
});
```

### 3. **GLShaders.ts**
Professional GLSL shader collection:

#### Available Shaders:

**Phong Lighting Model**
- Advanced per-pixel lighting
- Ambient, diffuse, and specular components
- Realistic material properties
- Normal mapping support
- Vertex and fragment shaders included

**Toon/Cel Shading**
- Stylized rendering with outlines
- Silhouette edge detection
- Quantized lighting levels
- Perfect for comic-style graphics

**Normal Mapping**
- Tangent space transformations
- High-detail surface variations
- TBN matrix calculations
- Enhanced surface realism

**Parallax Mapping**
- Height-based surface displacement
- Self-shadowing effects
- Advanced depth perception
- Dynamic height scale control

**Basic Rendering**
- Simple position and color
- Lightweight wireframe support
- Efficient outline rendering

**Depth Visualization**
- Debug depth buffer
- Depth-based rendering
- Scene analysis tools

**Emission/Glow**
- Self-illuminating materials
- HDR effects support
- Bloom compatibility

## Integration with SceneManager

The SceneManager now integrates OpenGL rendering:

```typescript
// In constructor:
this.glRenderer = new OpenGLRenderer(
  this.renderer.domElement,
  this.scene,
  this.camera,
  { antialias: true, enableDepthTest: true }
);

// Add OpenGL lighting
this.glRenderer.addLight({
  type: 'directional',
  position: new THREE.Vector3(15, 30, 10),
  color: new THREE.Color(1, 1, 1),
  intensity: 1.2
});

// In loadLevel():
if (block.type === 'pillar') {
  this.glRenderer.applyShader(mesh, 'toon');
}
```

## Advanced Features

### 1. **Custom Shader Application**
Apply different rendering modes to different objects:
```typescript
glRenderer.applyShader(mesh, 'phong');      // Realistic
glRenderer.applyShader(mesh, 'toon');       // Stylized
glRenderer.applyShader(mesh, 'emission');   // Glowing
```

### 2. **Pipeline State Control**
Fine-grained control over rendering state:
```typescript
glRenderer.setDepthTest(true);
glRenderer.setFaceCulling(true);
glRenderer.setPipelineState({
  blend: true,
  blendFunc: [THREE.SrcAlphaFactor, THREE.OneMinusSrcAlphaFactor]
});
```

### 3. **Render Targets**
Capture to render targets for post-processing:
```typescript
const target = glRenderer.getRenderTargetTexture(0);
```

### 4. **Performance Monitoring**
Check rendering performance:
```typescript
const info = glRenderer.getInfo();
console.log('Memory:', info.memory);
console.log('Render:', info.render);
```

### 5. **Screenshot Capture**
Save the current frame:
```typescript
const dataUrl = glRenderer.captureScreenshot();
```

## GLSL Shader Details

### Phong Vertex Shader
- Transforms positions to world space
- Calculates transformed normals
- Passes color to fragment shader
- Outputs clip-space position

### Phong Fragment Shader
- Calculates view direction
- Implements ambient lighting
- Calculates diffuse component
- Adds specular highlights
- Combines all components for final color

### Toon Fragment Shader
- Quantizes lighting levels (0, 0.3, 0.6, 1.0)
- Detects silhouette edges
- Renders black outlines
- Creates cartoon effect

## Usage Examples

### Example 1: Apply Phong Lighting to a Mesh
```typescript
const mesh = new THREE.Mesh(geometry, material);
glRenderer.applyShader(mesh, 'phong');
glRenderer.updateShaderUniforms('phong', {
  uSpecular: 0.8,
  uAmbient: 0.4
});
```

### Example 2: Create Cel-Shaded Objects
```typescript
const torusMesh = new THREE.Mesh(torusGeo, torusMat);
glRenderer.applyShader(torusMesh, 'toon');
```

### Example 3: Custom Render Pipeline
```typescript
const pipeline = glRenderer.getPipeline();
pipeline.addRenderPass({
  name: 'shadow_pass',
  enabled: true,
  clearColor: new THREE.Color(0x000000),
  clearDepth: true
});
```

## Performance Optimization

1. **Shader Selection**: Choose appropriate shaders based on visual requirements
   - `basic`: Fastest, minimal features
   - `toon`: Fast, stylized
   - `phong`: Medium cost, realistic
   - `parallax`: Expensive, high detail

2. **Render Target Management**: Reuse render targets to reduce memory

3. **Light Optimization**: Limit number of dynamic lights

4. **Pipeline State**: Minimize state changes between drawcalls

## Browser Compatibility

- **WebGL 2.0**: Full support for all shaders
- **Chrome**: ✓ Fully supported
- **Firefox**: ✓ Fully supported
- **Safari**: ✓ Supported (iOS 15+)
- **Edge**: ✓ Fully supported

## WebGL/OpenGL Correspondence

This implementation creates an OpenGL-like API using WebGL:

| OpenGL | WebGL | Implementation |
|--------|-------|-----------------|
| Shaders | GLSL 300 es | GLShaders.ts |
| Vertex Attribs | Buffer Attributes | BufferGeometry |
| Uniforms | Uniform Buffer | ShaderMaterial.uniforms |
| Render Passes | Render Targets | WebGLRenderTarget |
| Lighting | Lights + Shaders | GLLight type |
| Blending | GL Blend Modes | setPipelineState |

## Architecture

```
OpenGLRenderer
├── GLGraphicsPipeline
│   ├── Render Passes
│   ├── Render Targets
│   └── Shader Materials (GLShaders)
├── Lights
├── Camera & Scene
└── WebGL Context
```

## Types

See `types.ts` for OpenGL-related interfaces:
- `GLShaderConfig`: Shader configuration
- `GLLightConfig`: Light configuration
- `GLRenderingMode`: Rendering mode selection

## Future Enhancements

- [ ] Shadow mapping implementation
- [ ] Deferred rendering pipeline
- [ ] Post-processing effects
- [ ] Compute shaders support
- [ ] WebGL 3.0 support
- [ ] Multi-pass rendering
- [ ] Advanced material system

## Resources

- [Three.js Documentation](https://threejs.org/docs/)
- [WebGL Specification](https://www.khronos.org/webgl/wiki/Main_Page)
- [GLSL Reference](https://www.khronos.org/files/webgl/webgl-reference-card-1_0.pdf)
- [Shader Tutorials](https://www.shadertoy.com/)

## License

This OpenGL implementation is part of the Perspective Illusion Puzzle game and follows the same license as the main project.
