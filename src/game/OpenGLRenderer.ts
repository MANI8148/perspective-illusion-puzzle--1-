/**
 * OpenGL Renderer
 * High-level wrapper around Three.js WebGL implementation
 * Provides OpenGL-style rendering capabilities and graphics pipeline management
 */

import * as THREE from 'three';
import { GLGraphicsPipeline, GLRenderPass, PipelineState } from './GLGraphicsPipeline';

export interface OpenGLConfig {
  antialias?: boolean;
  pixelRatio?: number;
  enableDepthTest?: boolean;
  enableFaceCulling?: boolean;
  clearColor?: THREE.Color;
}

export interface GLLight {
  position: THREE.Vector3;
  color: THREE.Color;
  intensity: number;
  type: 'directional' | 'point' | 'spot';
}

export class OpenGLRenderer {
  private renderer: THREE.WebGLRenderer;
  private scene: THREE.Scene;
  private camera: THREE.Camera;
  private pipeline: GLGraphicsPipeline;
  private lights: GLLight[] = [];
  private animationFrameId: number | null = null;
  private isRunning: boolean = false;
  private renderCallback?: () => void;
  private config: Required<OpenGLConfig>;

  constructor(
    canvas: HTMLCanvasElement | string,
    scene: THREE.Scene,
    camera: THREE.Camera,
    config?: OpenGLConfig
  ) {
    this.config = {
      antialias: config?.antialias ?? true,
      pixelRatio: config?.pixelRatio ?? window.devicePixelRatio ?? 1,
      enableDepthTest: config?.enableDepthTest ?? true,
      enableFaceCulling: config?.enableFaceCulling ?? true,
      clearColor: config?.clearColor ?? new THREE.Color(0x87CEEB)
    };

    const canvasElement = typeof canvas === 'string'
      ? (document.getElementById(canvas) as HTMLCanvasElement)
      : canvas;

    this.renderer = new THREE.WebGLRenderer({
      canvas: canvasElement,
      antialias: this.config.antialias,
      alpha: true
    });

    this.renderer.setPixelRatio(this.config.pixelRatio);
    this.renderer.setClearColor(this.config.clearColor);
    this.renderer.shadowMap.enabled = true;
    this.renderer.shadowMap.type = THREE.PCFShadowMap;

    this.scene = scene;
    this.camera = camera;

    // Initialize graphics pipeline
    this.pipeline = new GLGraphicsPipeline(this.renderer, this.scene, this.camera);

    // Setup default pipeline state
    this.pipeline.setPipelineState({
      depthTest: this.config.enableDepthTest,
      depthWrite: true,
      cullFace: this.config.enableFaceCulling,
      blend: true,
      blendFunc: [THREE.SrcAlphaFactor, THREE.OneMinusSrcAlphaFactor]
    });

    // Setup default render pass
    this.setupDefaultRenderPass();

    // Add event listeners
    this.setupEventListeners();
  }

  /**
   * Setup default rendering pass
   */
  private setupDefaultRenderPass(): void {
    const pass: GLRenderPass = {
      name: 'default',
      enabled: true,
      clearColor: this.config.clearColor,
      clearDepth: true
    };
    this.pipeline.addRenderPass(pass);
  }

  /**
   * Setup event listeners for window resize
   */
  private setupEventListeners(): void {
    const handleResize = () => this.handleResize();
    window.addEventListener('resize', handleResize);
  }

  /**
   * Handle window resize
   */
  private handleResize(): void {
    const width = window.innerWidth;
    const height = window.innerHeight;
    this.setSize(width, height);
  }

  /**
   * Set renderer size
   */
  public setSize(width: number, height: number): void {
    this.renderer.setSize(width, height);
    this.pipeline.resize(width, height);

    if (this.camera instanceof THREE.PerspectiveCamera) {
      this.camera.aspect = width / height;
      this.camera.updateProjectionMatrix();
    } else if (this.camera instanceof THREE.OrthographicCamera) {
      this.camera.left = -width / 2;
      this.camera.right = width / 2;
      this.camera.top = height / 2;
      this.camera.bottom = -height / 2;
      this.camera.updateProjectionMatrix();
    }
  }

  /**
   * Add a light to the scene
   */
  public addLight(light: GLLight): THREE.Light {
    let threeLight: THREE.Light;

    switch (light.type) {
      case 'directional': {
        threeLight = new THREE.DirectionalLight(light.color, light.intensity);
        (threeLight as THREE.DirectionalLight).position.copy(light.position);
        (threeLight as THREE.DirectionalLight).castShadow = true;
        break;
      }
      case 'point': {
        threeLight = new THREE.PointLight(light.color, light.intensity);
        (threeLight as THREE.PointLight).position.copy(light.position);
        (threeLight as THREE.PointLight).castShadow = true;
        break;
      }
      case 'spot': {
        threeLight = new THREE.SpotLight(light.color, light.intensity);
        (threeLight as THREE.SpotLight).position.copy(light.position);
        (threeLight as THREE.SpotLight).castShadow = true;
        break;
      }
      default:
        throw new Error(`Unknown light type: ${light.type}`);
    }

    this.scene.add(threeLight);
    this.lights.push(light);

    // Update shader uniforms with light info
    this.pipeline.updateShaderUniforms('phong', {
      uLightPosition: light.position,
      uLightColor: light.color
    });

    return threeLight;
  }

  /**
   * Apply shader material to mesh
   */
  public applyShader(mesh: THREE.Mesh, shaderName: string): void {
    this.pipeline.applyShaderToMesh(mesh, shaderName);
  }

  /**
   * Get shader material
   */
  public getShader(name: string): THREE.ShaderMaterial | undefined {
    return this.pipeline.getShaderMaterial(name);
  }

  /**
   * Update shader uniforms
   */
  public updateShaderUniforms(shaderName: string, uniforms: Record<string, any>): void {
    this.pipeline.updateShaderUniforms(shaderName, uniforms);
  }

  /**
   * Set pipeline state
   */
  public setPipelineState(state: Partial<PipelineState>): void {
    this.pipeline.setPipelineState(state);
  }

  /**
   * Get pipeline state
   */
  public getPipelineState(): PipelineState {
    return this.pipeline.getPipelineState();
  }

  /**
   * Enable/disable depth testing
   */
  public setDepthTest(enabled: boolean): void {
    this.pipeline.setDepthTest(enabled);
  }

  /**
   * Enable/disable face culling
   */
  public setFaceCulling(enabled: boolean): void {
    this.pipeline.setFaceCulling(enabled);
  }

  /**
   * Set clear color
   */
  public setClearColor(color: THREE.Color | number): void {
    this.renderer.setClearColor(color);
  }

  /**
   * Render a single frame
   */
  public render(): void {
    this.pipeline.renderPipeline();
  }

  /**
   * Start continuous rendering
   */
  public start(callback?: () => void): void {
    if (this.isRunning) return;

    this.isRunning = true;
    this.renderCallback = callback;

    const animate = () => {
      this.animationFrameId = requestAnimationFrame(animate);

      if (this.renderCallback) {
        this.renderCallback();
      }

      this.render();
    };

    animate();
  }

  /**
   * Stop continuous rendering
   */
  public stop(): void {
    if (this.animationFrameId !== null) {
      cancelAnimationFrame(this.animationFrameId);
      this.animationFrameId = null;
    }
    this.isRunning = false;
  }

  /**
   * Check if renderer is running
   */
  public isActive(): boolean {
    return this.isRunning;
  }

  /**
   * Get the underlying Three.js renderer
   */
  public getThreeRenderer(): THREE.WebGLRenderer {
    return this.renderer;
  }

  /**
   * Get graphics pipeline
   */
  public getPipeline(): GLGraphicsPipeline {
    return this.pipeline;
  }

  /**
   * Get all lights
   */
  public getLights(): GLLight[] {
    return this.lights;
  }

  /**
   * Dispose resources
   */
  public dispose(): void {
    this.stop();
    this.renderer.dispose();
    this.scene.clear();
  }

  /**
   * Get renderer context (WebGL context)
   */
  public getContext(): WebGLRenderingContext | WebGL2RenderingContext | null {
    return this.renderer.getContext() as WebGLRenderingContext | WebGL2RenderingContext;
  }

  /**
   * Get canvas element
   */
  public getCanvas(): HTMLCanvasElement {
    return this.renderer.domElement;
  }

  /**
   * Capture screenshot
   */
  public captureScreenshot(): string {
    return this.renderer.domElement.toDataURL('image/png');
  }

  /**
   * Get render target texture (for compositing or post-processing)
   */
  public getRenderTargetTexture(index: number): THREE.Texture | null {
    const target = this.pipeline.getRenderTarget(index);
    return target ? target.texture : null;
  }

  /**
   * Get renderer info (for debugging)
   */
  public getInfo(): {
    memory: any;
    render: any;
  } {
    return {
      memory: (this.renderer as any).info.memory,
      render: (this.renderer as any).info.render
    };
  }
}
