/**
 * OpenGL Graphics Pipeline
 * Manages the complete graphics rendering pipeline with support for
 * multiple render passes, depth testing, and advanced rendering techniques
 */

import * as THREE from 'three';
import { GLShaders } from './GLShaders';

export interface PipelineState {
  depthTest: boolean;
  depthWrite: boolean;
  cullFace: boolean;
  blend: boolean;
  blendFunc: [number, number];
}

export interface GLRenderPass {
  name: string;
  enabled: boolean;
  renderTargetIndex?: number;
  clearColor?: THREE.Color;
  clearDepth?: boolean;
}

export class GLGraphicsPipeline {
  private renderer: THREE.WebGLRenderer;
  private scene: THREE.Scene;
  private camera: THREE.Camera;
  private renderTargets: THREE.WebGLRenderTarget[] = [];
  private renderPasses: GLRenderPass[] = [];
  private pipelineState: PipelineState;
  private shaderMaterials: Map<string, THREE.ShaderMaterial> = new Map();
  private deferredGeometryBuffer: THREE.WebGLRenderTarget | null = null;
  private normalBuffer: THREE.WebGLRenderTarget | null = null;

  constructor(renderer: THREE.WebGLRenderer, scene: THREE.Scene, camera: THREE.Camera) {
    this.renderer = renderer;
    this.scene = scene;
    this.camera = camera;

    this.pipelineState = {
      depthTest: true,
      depthWrite: true,
      cullFace: true,
      blend: false,
      blendFunc: [THREE.SrcAlphaFactor, THREE.OneMinusSrcAlphaFactor]
    };

    this.initializeShaderMaterials();
    this.setupRenderTargets();
  }

  /**
   * Initialize all shader materials from GLSL shaders
   */
  private initializeShaderMaterials(): void {
    // Phong material
    this.shaderMaterials.set('phong', new THREE.ShaderMaterial({
      vertexShader: GLShaders.phongVertex,
      fragmentShader: GLShaders.phongFragment,
      uniforms: {
        uModelMatrix: { value: new THREE.Matrix4() },
        uViewMatrix: { value: new THREE.Matrix4() },
        uProjectionMatrix: { value: new THREE.Matrix4() },
        uNormalMatrix: { value: new THREE.Matrix3() },
        uLightPosition: { value: new THREE.Vector3(10, 20, 10) },
        uLightColor: { value: new THREE.Color(1, 1, 1) },
        uCameraPosition: { value: new THREE.Vector3() },
        uAmbient: { value: 0.3 },
        uSpecular: { value: 0.5 }
      },
      side: THREE.DoubleSide
    }));

    // Toon material
    this.shaderMaterials.set('toon', new THREE.ShaderMaterial({
      vertexShader: GLShaders.toonVertex,
      fragmentShader: GLShaders.toonFragment,
      uniforms: {
        uModelMatrix: { value: new THREE.Matrix4() },
        uViewMatrix: { value: new THREE.Matrix4() },
        uProjectionMatrix: { value: new THREE.Matrix4() },
        uNormalMatrix: { value: new THREE.Matrix3() },
        uLightPosition: { value: new THREE.Vector3(10, 20, 10) },
        uLightColor: { value: new THREE.Color(1, 1, 1) },
        uCameraPosition: { value: new THREE.Vector3() }
      },
      side: THREE.DoubleSide
    }));

    // Basic material
    this.shaderMaterials.set('basic', new THREE.ShaderMaterial({
      vertexShader: GLShaders.basicVertex,
      fragmentShader: GLShaders.basicFragment,
      uniforms: {
        uModelMatrix: { value: new THREE.Matrix4() },
        uViewMatrix: { value: new THREE.Matrix4() },
        uProjectionMatrix: { value: new THREE.Matrix4() }
      }
    }));

    // Emission material
    this.shaderMaterials.set('emission', new THREE.ShaderMaterial({
      vertexShader: GLShaders.emissionVertex,
      fragmentShader: GLShaders.emissionFragment,
      uniforms: {
        uModelMatrix: { value: new THREE.Matrix4() },
        uViewMatrix: { value: new THREE.Matrix4() },
        uProjectionMatrix: { value: new THREE.Matrix4() },
        uEmissionIntensity: { value: 1.0 }
      }
    }));
  }

  /**
   * Setup render targets for deferred rendering and post-processing
   */
  private setupRenderTargets(): void {
    const width = this.renderer.domElement.clientWidth;
    const height = this.renderer.domElement.clientHeight;

    // Geometry buffer for deferred rendering
    this.deferredGeometryBuffer = new THREE.WebGLRenderTarget(width, height, {
      format: THREE.RGBAFormat,
      type: THREE.FloatType,
    });

    // Normal buffer
    this.normalBuffer = new THREE.WebGLRenderTarget(width, height, {
      format: THREE.RGBAFormat,
      type: THREE.FloatType,
    });

    // Additional render targets for future effects
    this.renderTargets = [
      this.deferredGeometryBuffer,
      this.normalBuffer,
      new THREE.WebGLRenderTarget(width, height),
      new THREE.WebGLRenderTarget(width, height)
    ];
  }

  /**
   * Set pipeline state (depth testing, culling, blending, etc.)
   */
  public setPipelineState(state: Partial<PipelineState>): void {
    this.pipelineState = { ...this.pipelineState, ...state };

    const gl = this.renderer.getContext() as WebGLRenderingContext;

    if (state.depthTest !== undefined) {
      state.depthTest ? gl.enable(gl.DEPTH_TEST) : gl.disable(gl.DEPTH_TEST);
    }

    if (state.depthWrite !== undefined) {
      gl.depthMask(state.depthWrite);
    }

    if (state.cullFace !== undefined) {
      state.cullFace ? gl.enable(gl.CULL_FACE) : gl.disable(gl.CULL_FACE);
    }

    if (state.blend !== undefined) {
      state.blend ? gl.enable(gl.BLEND) : gl.disable(gl.BLEND);
    }

    if (state.blendFunc !== undefined) {
      gl.blendFunc(state.blendFunc[0], state.blendFunc[1]);
    }
  }

  /**
   * Add a render pass to the pipeline
   */
  public addRenderPass(pass: GLRenderPass): void {
    this.renderPasses.push(pass);
  }

  /**
   * Clear render passes
   */
  public clearRenderPasses(): void {
    this.renderPasses = [];
  }

  /**
   * Get a shader material by name
   */
  public getShaderMaterial(name: string): THREE.ShaderMaterial | undefined {
    return this.shaderMaterials.get(name);
  }

  /**
   * Apply shader material to a mesh
   */
  public applyShaderToMesh(mesh: THREE.Mesh, shaderName: string): void {
    const material = this.shaderMaterials.get(shaderName);
    if (material) {
      mesh.material = material;
    }
  }

  /**
   * Update shader uniforms
   */
  public updateShaderUniforms(shaderName: string, uniforms: Record<string, any>): void {
    const material = this.shaderMaterials.get(shaderName);
    if (material && material.uniforms) {
      Object.keys(uniforms).forEach(key => {
        if (key in material.uniforms) {
          material.uniforms[key].value = uniforms[key];
        }
      });
    }
  }

  /**
   * Execute the complete graphics pipeline
   */
  public renderPipeline(): void {
    // Update all shader uniforms
    this.updateAllShaderUniforms();

    // Render each pass
    for (const pass of this.renderPasses) {
      if (!pass.enabled) continue;

      if (pass.clearColor && this.renderer) {
        const originalClearColor = new THREE.Color();
        this.renderer.getClearColor(originalClearColor);
        this.renderer.setClearColor(pass.clearColor);
      }

      if (pass.renderTargetIndex !== undefined && this.renderTargets[pass.renderTargetIndex]) {
        this.renderer.setRenderTarget(this.renderTargets[pass.renderTargetIndex]);
      }

      if (pass.clearDepth) {
        this.renderer.clear(true, true);
      }

      this.renderer.render(this.scene, this.camera);

      if (pass.renderTargetIndex !== undefined) {
        this.renderer.setRenderTarget(null);
      }
    }

    this.renderer.render(this.scene, this.camera);
  }

  /**
   * Update all shader uniforms with current matrices and camera position
   */
  private updateAllShaderUniforms(): void {
    const cameraPosition = (this.camera as THREE.PerspectiveCamera).position || new THREE.Vector3();

    this.shaderMaterials.forEach((material) => {
      if (material.uniforms) {
        // Update common uniforms
        if ('uViewMatrix' in material.uniforms) {
          material.uniforms['uViewMatrix'].value = this.getViewMatrix();
        }
        if ('uProjectionMatrix' in material.uniforms) {
          material.uniforms['uProjectionMatrix'].value = this.getProjectionMatrix();
        }
        if ('uCameraPosition' in material.uniforms) {
          material.uniforms['uCameraPosition'].value = cameraPosition;
        }
      }
    });
  }

  /**
   * Get view matrix from camera
   */
  private getViewMatrix(): THREE.Matrix4 {
    const matrix = new THREE.Matrix4();
    matrix.lookAt(
      (this.camera as THREE.PerspectiveCamera).position || new THREE.Vector3(),
      new THREE.Vector3(0, 0, 0),
      new THREE.Vector3(0, 1, 0)
    );
    return matrix;
  }

  /**
   * Get projection matrix from camera
   */
  private getProjectionMatrix(): THREE.Matrix4 {
    if (this.camera instanceof THREE.PerspectiveCamera) {
      return this.camera.projectionMatrix;
    }
    return new THREE.Matrix4();
  }

  /**
   * Enable/disable depth testing
   */
  public setDepthTest(enabled: boolean): void {
    this.pipelineState.depthTest = enabled;
    const gl = this.renderer.getContext() as WebGLRenderingContext;
    enabled ? gl.enable(gl.DEPTH_TEST) : gl.disable(gl.DEPTH_TEST);
  }

  /**
   * Enable/disable face culling
   */
  public setFaceCulling(enabled: boolean): void {
    this.pipelineState.cullFace = enabled;
    const gl = this.renderer.getContext() as WebGLRenderingContext;
    enabled ? gl.enable(gl.CULL_FACE) : gl.disable(gl.CULL_FACE);
  }

  /**
   * Get current pipeline state
   */
  public getPipelineState(): PipelineState {
    return { ...this.pipelineState };
  }

  /**
   * Get render target by index
   */
  public getRenderTarget(index: number): THREE.WebGLRenderTarget | undefined {
    return this.renderTargets[index];
  }

  /**
   * Resize all render targets (call on window resize)
   */
  public resize(width: number, height: number): void {
    this.renderTargets.forEach(target => {
      target.setSize(width, height);
    });
  }
}
