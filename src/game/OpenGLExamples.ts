/**
 * OpenGL Implementation Examples
 * Demonstrates how to use the OpenGL rendering system
 */

import * as THREE from 'three';
import { OpenGLRenderer } from '../game/OpenGLRenderer';
import { SceneManager } from '../game/SceneManager';
import { SoundManager } from '../game/SoundManager';

/**
 * Example 1: Basic OpenGL Renderer Setup
 */
export function exampleBasicSetup() {
  const canvas = document.getElementById('canvas') as HTMLCanvasElement;
  const scene = new THREE.Scene();
  const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
  
  // Create OpenGL renderer
  const glRenderer = new OpenGLRenderer(canvas, scene, camera, {
    antialias: true,
    enableDepthTest: true,
    enableFaceCulling: true,
    clearColor: new THREE.Color(0x87CEEB)
  });

  // Add a directional light
  glRenderer.addLight({
    type: 'directional',
    position: new THREE.Vector3(10, 10, 10),
    color: new THREE.Color(1, 1, 1),
    intensity: 1.0
  });

  // Start rendering
  glRenderer.start();
}

/**
 * Example 2: Applying Shaders to Meshes
 */
export function exampleShaderApplication(glRenderer: OpenGLRenderer) {
  // Create a cube mesh
  const geometry = new THREE.BoxGeometry(1, 1, 1);
  const material = new THREE.MeshStandardMaterial({ color: 0xff0000 });
  const cube = new THREE.Mesh(geometry, material);

  // Apply Phong shader for realistic lighting
  glRenderer.applyShader(cube, 'phong');

  // Update shader uniforms
  glRenderer.updateShaderUniforms('phong', {
    uSpecular: 0.8,
    uAmbient: 0.3
  });

  return cube;
}

/**
 * Example 3: Toon/Cel Shading
 */
export function exampleToonShading(glRenderer: OpenGLRenderer) {
  const geometry = new THREE.SphereGeometry(1, 32, 32);
  const material = new THREE.MeshStandardMaterial({ color: 0x00ff00 });
  const sphere = new THREE.Mesh(geometry, material);

  // Apply toon shader for stylized rendering
  glRenderer.applyShader(sphere, 'toon');

  return sphere;
}

/**
 * Example 4: Multiple Lights
 */
export function exampleMultipleLights(glRenderer: OpenGLRenderer) {
  // Directional light (sun)
  glRenderer.addLight({
    type: 'directional',
    position: new THREE.Vector3(10, 20, 10),
    color: new THREE.Color(1, 1, 0.9),
    intensity: 1.2
  });

  // Point light (warm)
  glRenderer.addLight({
    type: 'point',
    position: new THREE.Vector3(-5, 5, 5),
    color: new THREE.Color(1, 0.5, 0),
    intensity: 0.8
  });

  // Point light (cool)
  glRenderer.addLight({
    type: 'point',
    position: new THREE.Vector3(5, 5, -5),
    color: new THREE.Color(0, 0.5, 1),
    intensity: 0.6
  });
}

/**
 * Example 5: Render Target and Post-Processing
 */
export function exampleRenderTargets(glRenderer: OpenGLRenderer) {
  // Get render target texture for custom post-processing
  const renderTargetTexture = glRenderer.getRenderTargetTexture(0);

  if (renderTargetTexture) {
    // Use this texture in a custom shader or as input to post-processing
    const postProcessMaterial = new THREE.MeshBasicMaterial({
      map: renderTargetTexture
    });

    return postProcessMaterial;
  }
}

/**
 * Example 6: Pipeline State Control
 */
export function examplePipelineState(glRenderer: OpenGLRenderer) {
  // Enable depth testing
  glRenderer.setDepthTest(true);

  // Enable face culling
  glRenderer.setFaceCulling(true);

  // Get current pipeline state
  const state = glRenderer.getPipelineState();
  console.log('Pipeline State:', state);

  // Set custom pipeline state
  glRenderer.setPipelineState({
    blend: true,
    blendFunc: [THREE.SrcAlphaFactor, THREE.OneMinusSrcAlphaFactor],
    depthWrite: true,
    cullFace: true
  });
}

/**
 * Example 7: Performance Monitoring
 */
export function examplePerformanceMonitoring(glRenderer: OpenGLRenderer) {
  setInterval(() => {
    const info = glRenderer.getInfo();
    console.log('Memory:', {
      geometries: info.memory.geometries,
      textures: info.memory.textures,
      programs: info.memory.programs
    });
    console.log('Render:', {
      calls: info.render.calls,
      triangles: info.render.triangles,
      points: info.render.points,
      lines: info.render.lines,
      frame: info.render.frame
    });
  }, 1000);
}

/**
 * Example 8: Screenshot Capture
 */
export function exampleScreenshot(glRenderer: OpenGLRenderer) {
  const screenshotButton = document.getElementById('screenshot-btn');
  if (screenshotButton) {
    screenshotButton.addEventListener('click', () => {
      const dataUrl = glRenderer.captureScreenshot();
      const link = document.createElement('a');
      link.href = dataUrl;
      link.download = 'screenshot.png';
      link.click();
    });
  }
}

/**
 * Example 9: Integration with SceneManager
 */
export function exampleSceneManagerIntegration() {
  const container = document.getElementById('game-container') as HTMLElement;
  const soundManager = new SoundManager();

  // Create scene manager (which now includes OpenGL renderer)
  const sceneManager = new SceneManager(container, soundManager, () => {
    console.log('Level complete!');
  });

  // The OpenGL renderer is now available as:
  const glRenderer = sceneManager.glRenderer;

  // Apply custom shading
  sceneManager.scene.traverse(object => {
    if (object instanceof THREE.Mesh && object.name === 'pillar') {
      glRenderer.applyShader(object, 'toon');
    }
  });

  return sceneManager;
}

/**
 * Example 10: Custom Shader Uniforms
 */
export function exampleCustomUniforms(glRenderer: OpenGLRenderer) {
  // Update shader uniforms for Phong lighting
  glRenderer.updateShaderUniforms('phong', {
    uLightPosition: new THREE.Vector3(15, 30, 10),
    uLightColor: new THREE.Color(1, 1, 1),
    uCameraPosition: new THREE.Vector3(0, 0, 10),
    uAmbient: 0.4,
    uSpecular: 0.9
  });

  // Update emission shader
  glRenderer.updateShaderUniforms('emission', {
    uEmissionIntensity: 2.0
  });
}

/**
 * Example 11: Canvas and Context Access
 */
export function exampleCanvasAccess(glRenderer: OpenGLRenderer) {
  // Get the canvas element
  const canvas = glRenderer.getCanvas();
  console.log('Canvas size:', canvas.width, canvas.height);

  // Get the WebGL context
  const context = glRenderer.getContext();
  console.log('WebGL context:', context);

  // Get the underlying Three.js renderer
  const threeRenderer = glRenderer.getThreeRenderer();
  console.log('Three.js renderer:', threeRenderer);
}

/**
 * Example 12: Animation Loop with OpenGL Renderer
 */
export function exampleAnimationLoop(glRenderer: OpenGLRenderer, scene: THREE.Scene) {
  let rotation = 0;

  glRenderer.start(() => {
    // This callback is called each frame before rendering
    rotation += 0.01;

    // Update scene objects
    scene.traverse(object => {
      if (object instanceof THREE.Mesh) {
        object.rotation.x += 0.001;
        object.rotation.y += 0.002;
      }
    });
  });

  // Stop rendering when needed
  // glRenderer.stop();
}

/**
 * Example 13: Shader Material Retrieval
 */
export function exampleShaderMaterials(glRenderer: OpenGLRenderer) {
  // Get a shader material
  const phongShader = glRenderer.getShader('phong');
  console.log('Phong shader:', phongShader);

  // Get the graphics pipeline
  const pipeline = glRenderer.getPipeline();

  // Get all lights
  const lights = glRenderer.getLights();
  console.log('Lights in renderer:', lights.length);
}

/**
 * Example 14: Cleanup and Disposal
 */
export function exampleCleanup(glRenderer: OpenGLRenderer) {
  // Stop rendering
  glRenderer.stop();

  // Dispose all resources
  glRenderer.dispose();

  console.log('OpenGL renderer disposed');
}

/**
 * Example 15: Complex Scene with Multiple Shaders
 */
export function exampleComplexScene(glRenderer: OpenGLRenderer, scene: THREE.Scene) {
  // Create objects with different shaders
  
  // Realistic object (Phong)
  const cubeGeo = new THREE.BoxGeometry(2, 2, 2);
  const cubeMat = new THREE.MeshStandardMaterial({ color: 0xff0000 });
  const cube = new THREE.Mesh(cubeGeo, cubeMat);
  cube.position.x = -4;
  glRenderer.applyShader(cube, 'phong');
  scene.add(cube);

  // Stylized object (Toon)
  const sphereGeo = new THREE.SphereGeometry(1.5, 32, 32);
  const sphereMat = new THREE.MeshStandardMaterial({ color: 0x00ff00 });
  const sphere = new THREE.Mesh(sphereGeo, sphereMat);
  sphere.position.x = 0;
  glRenderer.applyShader(sphere, 'toon');
  scene.add(sphere);

  // Glowing object (Emission)
  const torusGeo = new THREE.TorusGeometry(1.5, 0.4, 16, 100);
  const torusMat = new THREE.MeshStandardMaterial({ color: 0x0000ff });
  const torus = new THREE.Mesh(torusGeo, torusMat);
  torus.position.x = 4;
  glRenderer.applyShader(torus, 'emission');
  glRenderer.updateShaderUniforms('emission', {
    uEmissionIntensity: 3.0
  });
  scene.add(torus);

  return { cube, sphere, torus };
}
