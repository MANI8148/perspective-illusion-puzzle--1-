import * as THREE from 'three';
import { EffectComposer } from 'three/examples/jsm/postprocessing/EffectComposer.js';
import { RenderPass } from 'three/examples/jsm/postprocessing/RenderPass.js';
import { UnrealBloomPass } from 'three/examples/jsm/postprocessing/UnrealBloomPass.js';
import { LevelData, LevelNode, GameState, BlockData } from './types';
import { IllusionSystem } from './IllusionSystem';
import { SoundManager } from './SoundManager';

export class SceneManager {
  public scene: THREE.Scene;
  public camera: THREE.OrthographicCamera;
  public renderer: THREE.WebGLRenderer;
  private composer: EffectComposer;
  private illusionSystem: IllusionSystem;
  private blockTexture: THREE.Texture;
  private soundManager: SoundManager;
  
  private nodes: LevelNode[] = [];
  private player: THREE.Mesh;
  private playerTrail: THREE.Points;
  private trailPositions: Float32Array;
  private trailIndex: number = 0;
  private currentNodeId: string = '';
  private targetNodeId: string = '';
  private movePath: LevelNode[] = [];
  private moveSpeed: number = 0.1;
  private rotatingBlocks: { mesh: THREE.Mesh, blockData: BlockData, nodeIds: string[] }[] = [];
  
  private onLevelComplete?: () => void;
  private container: HTMLElement;

  private handleResize = () => {
    if (!this.renderer.domElement.parentElement) return;
    const container = this.renderer.domElement.parentElement;
    let width = container.clientWidth;
    let height = container.clientHeight;
    
    if (width === 0 || height === 0) {
      width = window.innerWidth;
      height = window.innerHeight;
    }
    
    console.log('Resizing to:', width, height);
    
    const aspect = width / height;
    const d = 12;
    this.camera.left = -d * aspect;
    this.camera.right = d * aspect;
    this.camera.top = d;
    this.camera.bottom = -d;
    this.camera.updateProjectionMatrix();
    this.renderer.setSize(width, height);
    this.composer.setSize(width, height);
  };

  private handleKeyDownBound = (e: KeyboardEvent) => this.handleKeyDown(e);
  private handleWheelBound = (e: WheelEvent) => this.handleWheel(e);

  constructor(container: HTMLElement, soundManager: SoundManager, onLevelComplete?: () => void) {
    this.container = container;
    this.onLevelComplete = onLevelComplete;
    this.soundManager = soundManager;
    this.scene = new THREE.Scene();
    this.scene.background = new THREE.Color('#e0f7fa');
    this.scene.fog = new THREE.FogExp2('#e0f7fa', 0.02);

    this.createSky();

    this.blockTexture = this.createNoiseTexture();

    console.log('Container dimensions:', container.clientWidth, container.clientHeight);
    
    let width = container.clientWidth;
    let height = container.clientHeight;
    
    // Fallback to window dimensions if container is not yet sized
    if (width === 0 || height === 0) {
      width = window.innerWidth;
      height = window.innerHeight;
      console.log('Container not sized, using window dimensions:', width, height);
    }

    const aspect = width / height;
    const d = 12; // Slightly larger view
    this.camera = new THREE.OrthographicCamera(-d * aspect, d * aspect, d, -d, 1, 1000);
    
    this.camera.position.set(20, 20, 20);
    this.camera.lookAt(0, 0, 0);

    this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
    this.renderer.setSize(width, height);
    this.renderer.domElement.style.width = '100%';
    this.renderer.domElement.style.height = '100%';
    this.renderer.domElement.style.display = 'block';
    this.renderer.domElement.style.position = 'absolute';
    this.renderer.domElement.style.top = '0';
    this.renderer.domElement.style.left = '0';
    this.renderer.shadowMap.enabled = true;
    this.renderer.shadowMap.type = THREE.PCFShadowMap;
    this.renderer.outputColorSpace = THREE.SRGBColorSpace;
    container.appendChild(this.renderer.domElement);
    console.log('Canvas added to DOM:', this.renderer.domElement);

    // Post-processing - temporarily disabled to debug rendering
    const renderScene = new RenderPass(this.scene, this.camera);
    
    this.composer = new EffectComposer(this.renderer);
    this.composer.addPass(renderScene);
    // Temporarily skip bloom pass
    // const bloomPass = new UnrealBloomPass(new THREE.Vector2(width, height), 1.5, 0.4, 0.85);
    // bloomPass.threshold = 0.8;
    // bloomPass.strength = 0.8;
    // bloomPass.radius = 0.5;
    // this.composer.addPass(bloomPass);

    this.illusionSystem = new IllusionSystem(this.camera);

    // Lights
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.7);
    this.scene.add(ambientLight);

    const dirLight = new THREE.DirectionalLight(0xffffff, 1.2);
    dirLight.position.set(15, 30, 10);
    dirLight.castShadow = true;
    dirLight.shadow.mapSize.width = 2048;
    dirLight.shadow.mapSize.height = 2048;
    dirLight.shadow.camera.left = -20;
    dirLight.shadow.camera.right = 20;
    dirLight.shadow.camera.top = 20;
    dirLight.shadow.camera.bottom = -20;
    this.scene.add(dirLight);

    // Player
    const playerGeo = new THREE.CapsuleGeometry(0.25, 0.4, 4, 16);
    const playerMat = new THREE.MeshStandardMaterial({ 
      color: '#ff4757',
      roughness: 0.3,
      metalness: 0.2,
      map: this.blockTexture,
      emissive: '#ff4757',
      emissiveIntensity: 0.8
    });
    this.player = new THREE.Mesh(playerGeo, playerMat);
    this.player.castShadow = true;
    this.scene.add(this.player);

    // Player Trail
    const trailCount = 40;
    this.trailPositions = new Float32Array(trailCount * 3);
    const trailGeo = new THREE.BufferGeometry();
    trailGeo.setAttribute('position', new THREE.BufferAttribute(this.trailPositions, 3));
    
    // Initialize opacities
    const trailOpacities = new Float32Array(trailCount);
    trailGeo.setAttribute('alpha', new THREE.BufferAttribute(trailOpacities, 1));

    const trailMat = new THREE.PointsMaterial({
      color: '#ff4757',
      size: 0.15,
      transparent: true,
      opacity: 0.6,
      blending: THREE.AdditiveBlending,
      depthWrite: false
    });
    
    trailMat.onBeforeCompile = (shader) => {
      shader.vertexShader = `
        attribute float alpha;
        varying float vAlpha;
        ${shader.vertexShader}
      `.replace(
        `void main() {`,
        `void main() {
          vAlpha = alpha;`
      );
      shader.fragmentShader = `
        varying float vAlpha;
        ${shader.fragmentShader}
      `.replace(
        `vec4 diffuseColor = vec4( diffuse, opacity );`,
        `vec4 diffuseColor = vec4( diffuse, opacity * vAlpha );`
      );
    };

    this.playerTrail = new THREE.Points(trailGeo, trailMat);
    this.scene.add(this.playerTrail);

    this.createParticles();

    window.addEventListener('resize', this.handleResize);
    window.addEventListener('keydown', this.handleKeyDownBound);
    this.container.addEventListener('wheel', this.handleWheelBound, { passive: false });

    // Ensure canvas is sized correctly after DOM layout
    requestAnimationFrame(() => {
      this.handleResize();
      // Initial render
      console.log('Initial render');
      console.log('Camera position:', this.camera.position);
      console.log('Camera frustum:', {
        left: this.camera.left,
        right: this.camera.right,
        top: this.camera.top,
        bottom: this.camera.bottom
      });
      console.log('Scene fog:', this.scene.fog);
      console.log('Scene background:', this.scene.background);
      console.log('Scene children count:', this.scene.children.length);
      console.log('Rendering with composer');
      this.composer.render();
      
      // Also test direct render
      console.log('Testing direct render');
      this.renderer.render(this.scene, this.camera);
    });
  }

  public dispose() {
    window.removeEventListener('resize', this.handleResize);
    window.removeEventListener('keydown', this.handleKeyDownBound);
    this.container.removeEventListener('wheel', this.handleWheelBound);
    this.renderer.dispose();
  }

  private handleWheel(e: WheelEvent) {
    e.preventDefault();
    const zoomSpeed = 0.002;
    this.camera.zoom -= e.deltaY * zoomSpeed;
    this.camera.zoom = Math.max(0.5, Math.min(this.camera.zoom, 4.0));
    this.camera.updateProjectionMatrix();
  }

  private createSky() {
    const skyGeo = new THREE.SphereGeometry(100, 32, 32);
    const skyMat = new THREE.ShaderMaterial({
      uniforms: {
        topColor: { value: new THREE.Color('#e0f7fa') },
        bottomColor: { value: new THREE.Color('#ffffff') },
        offset: { value: 33 },
        exponent: { value: 0.6 }
      },
      vertexShader: `
        varying vec3 vWorldPosition;
        void main() {
          vec4 worldPosition = modelMatrix * vec4(position, 1.0);
          vWorldPosition = worldPosition.xyz;
          gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
        }
      `,
      fragmentShader: `
        uniform vec3 topColor;
        uniform vec3 bottomColor;
        uniform float offset;
        uniform float exponent;
        varying vec3 vWorldPosition;
        void main() {
          float h = normalize(vWorldPosition + offset).y;
          gl_FragColor = vec4(mix(bottomColor, topColor, max(pow(max(h, 0.0), exponent), 0.0)), 1.0);
        }
      `,
      side: THREE.BackSide
    });
    const sky = new THREE.Mesh(skyGeo, skyMat);
    this.scene.add(sky);
  }

  private createNoiseTexture(): THREE.Texture {
    const size = 256;
    const canvas = document.createElement('canvas');
    canvas.width = size;
    canvas.height = size;
    const ctx = canvas.getContext('2d')!;

    // Fill with white
    ctx.fillStyle = '#ffffff';
    ctx.fillRect(0, 0, size, size);

    // Add noise
    const imageData = ctx.getImageData(0, 0, size, size);
    const data = imageData.data;
    for (let i = 0; i < data.length; i += 4) {
      const noise = (Math.random() - 0.5) * 15;
      data[i] = Math.min(255, Math.max(0, data[i] + noise));
      data[i + 1] = Math.min(255, Math.max(0, data[i + 1] + noise));
      data[i + 2] = Math.min(255, Math.max(0, data[i + 2] + noise));
    }
    ctx.putImageData(imageData, 0, 0);

    // Add subtle grid pattern
    ctx.strokeStyle = 'rgba(0, 0, 0, 0.05)';
    ctx.lineWidth = 1;
    for (let i = 0; i <= size; i += size / 4) {
      ctx.beginPath();
      ctx.moveTo(i, 0);
      ctx.lineTo(i, size);
      ctx.stroke();
      ctx.beginPath();
      ctx.moveTo(0, i);
      ctx.lineTo(size, i);
      ctx.stroke();
    }

    const texture = new THREE.CanvasTexture(canvas);
    texture.wrapS = THREE.RepeatWrapping;
    texture.wrapT = THREE.RepeatWrapping;
    texture.repeat.set(1, 1);
    return texture;
  }

  private createParticles() {
    const particleCount = 100;
    const geometry = new THREE.BufferGeometry();
    const positions = new Float32Array(particleCount * 3);
    const colors = new Float32Array(particleCount * 3);

    for (let i = 0; i < particleCount; i++) {
      positions[i * 3] = (Math.random() - 0.5) * 30;
      positions[i * 3 + 1] = Math.random() * 15;
      positions[i * 3 + 2] = (Math.random() - 0.5) * 30;

      colors[i * 3] = 1;
      colors[i * 3 + 1] = 1;
      colors[i * 3 + 2] = 1;
    }

    geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
    geometry.setAttribute('color', new THREE.BufferAttribute(colors, 3));

    const material = new THREE.PointsMaterial({
      size: 0.08,
      vertexColors: true,
      transparent: true,
      opacity: 0.4,
      sizeAttenuation: true
    });

    const particles = new THREE.Points(geometry, material);
    particles.name = 'particles';
    this.scene.add(particles);
  }

  private createPlant(): THREE.Group {
    const group = new THREE.Group();
    group.name = 'plant';
    
    // Pot
    const potGeo = new THREE.CylinderGeometry(0.12, 0.08, 0.15, 8);
    const potMat = new THREE.MeshStandardMaterial({ color: '#d35400' });
    const pot = new THREE.Mesh(potGeo, potMat);
    pot.position.y = 0.075;
    pot.castShadow = true;
    group.add(pot);

    // Plant/Leaves
    const leafGeo = new THREE.SphereGeometry(0.1, 8, 8);
    const leafMat = new THREE.MeshStandardMaterial({ color: '#27ae60' });
    
    for (let i = 0; i < 3; i++) {
      const leaf = new THREE.Mesh(leafGeo, leafMat);
      leaf.scale.set(0.8, 1.2, 0.8);
      leaf.position.set(
        Math.sin(i * 2) * 0.05,
        0.2 + i * 0.05,
        Math.cos(i * 2) * 0.05
      );
      leaf.rotation.set(Math.random(), Math.random(), Math.random());
      group.add(leaf);
    }

    return group;
  }

  private handleKeyDown(e: KeyboardEvent) {
    if (this.movePath.length > 0) return;

    const key = e.key.toLowerCase();
    const cameraDir = new THREE.Vector3();
    this.camera.getWorldDirection(cameraDir);
    
    // Calculate movement directions relative to camera
    const up = new THREE.Vector3(0, 1, 0);
    const right = new THREE.Vector3().crossVectors(cameraDir, up).normalize();
    const forward = new THREE.Vector3().crossVectors(up, right).normalize();

    let targetDir = new THREE.Vector3();
    if (key === 'w') targetDir.copy(forward);
    else if (key === 's') targetDir.copy(forward).negate();
    else if (key === 'a') targetDir.copy(right).negate();
    else if (key === 'd') targetDir.copy(right);
    else return;

    // Find best adjacent node in that direction
    this.moveInDirection(targetDir);
  }

  private moveInDirection(dir: THREE.Vector3) {
    const currentNode = this.nodes.find(n => n.id === this.currentNodeId);
    if (!currentNode) return;

    // Combine real and illusion connections
    const illusionEdges = this.illusionSystem.getIllusionConnections(this.nodes);
    const possibleNextIds = new Set(currentNode.connections);
    illusionEdges.forEach(([idA, idB]) => {
      if (idA === this.currentNodeId) possibleNextIds.add(idB);
      if (idB === this.currentNodeId) possibleNextIds.add(idA);
    });

    let bestNode: LevelNode | null = null;
    let maxDot = 0.5; // Minimum alignment threshold

    possibleNextIds.forEach(id => {
      const node = this.nodes.find(n => n.id === id);
      if (node) {
        // Validate distance for real connections (should be ~1 unit apart)
        const isIllusion = illusionEdges.some(edge => 
          (edge[0] === this.currentNodeId && edge[1] === id) ||
          (edge[1] === this.currentNodeId && edge[0] === id)
        );
        
        const dist = node.position.distanceTo(currentNode.position);
        if (!isIllusion && dist > 1.5) {
          return; // Too far for a real connection (e.g., rotated away)
        }

        const nodeDir = node.position.clone().sub(currentNode.position).normalize();
        const dot = nodeDir.dot(dir);
        if (dot > maxDot) {
          maxDot = dot;
          bestNode = node;
        }
      }
    });

    if (bestNode) {
      this.navigateTo((bestNode as LevelNode).id);
    }
  }

  public loadLevel(level: LevelData) {
    console.log('Loading level:', level.id, 'with', level.blocks.length, 'blocks');
    // Clear previous level - properly remove children
    const childrenToKeep = new Set<THREE.Object3D>();
    childrenToKeep.add(this.player);
    
    this.scene.children.forEach(c => {
      if (c instanceof THREE.Light || c.name === 'sky' || c.name === 'particles') {
        childrenToKeep.add(c);
      }
    });
    
    // Remove all children not in the keep set
    const childrenToRemove = this.scene.children.filter(c => !childrenToKeep.has(c));
    childrenToRemove.forEach(c => this.scene.remove(c));
    
    this.nodes = [];
    this.rotatingBlocks = [];

    level.blocks.forEach(block => {
      const geo = new THREE.BoxGeometry(1, 1, 1);
      
      // Create a slightly "textured" look with procedural map and color jitter
      const baseColor = new THREE.Color(block.color || '#a8d8ea');
      const jitter = (Math.random() - 0.5) * 0.05;
      baseColor.r = Math.min(1, Math.max(0, baseColor.r + jitter));
      baseColor.g = Math.min(1, Math.max(0, baseColor.g + jitter));
      baseColor.b = Math.min(1, Math.max(0, baseColor.b + jitter));

      const mat = new THREE.MeshStandardMaterial({ 
        color: baseColor,
        roughness: 0.8,
        metalness: 0.1,
        map: this.blockTexture,
        flatShading: false
      });
      
      let mainObject: THREE.Object3D;
      const mesh = new THREE.Mesh(geo, mat);
      
      if (block.scale) {
        mesh.scale.set(...block.scale);
      } else if (block.type === 'rotating') {
        mesh.scale.set(3, 1, 1);
      }

      // Add edge carvings (subtle trim)
      const edgeGeo = new THREE.EdgesGeometry(geo);
      const edgeMat = new THREE.LineBasicMaterial({ color: '#ffffff', transparent: true, opacity: 0.15 });
      const edges = new THREE.LineSegments(edgeGeo, edgeMat);
      mesh.add(edges);

      // Add a subtle "cap" or detail to blocks to make them more attractive
      if (block.type === 'cube' || block.type === 'rotating') {
        const detailGeo = new THREE.BoxGeometry(0.9, 0.1, 0.9);
        const detailMat = new THREE.MeshStandardMaterial({ color: '#ffffff', opacity: 0.1, transparent: true });
        const detail = new THREE.Mesh(detailGeo, detailMat);
        detail.position.set(0, 0.51, 0);
        mesh.add(detail);

        // Randomly add a plant to some blocks that are NOT nodes
        if (block.nodes.length === 0 && Math.random() > 0.7) {
          const plant = this.createPlant();
          plant.position.set(0, 0.5, 0);
          // Inverse scale for plant so it doesn't stretch
          plant.scale.set(1 / mesh.scale.x, 1 / mesh.scale.y, 1 / mesh.scale.z);
          mesh.add(plant);
        }
      }

      if (block.type === 'rotating') {
        const group = new THREE.Group();
        group.position.set(...block.position);
        group.userData.baseY = block.position[1];
        
        // Add the scaled mesh to the group
        group.add(mesh);

        // Add a visual indicator that it rotates (unscaled)
        const indicatorGeo = new THREE.TorusGeometry(0.4, 0.05, 8, 24);
        const indicatorMat = new THREE.MeshStandardMaterial({ color: '#ff9ff3', emissive: '#ff9ff3', emissiveIntensity: 2.0 });
        const indicator = new THREE.Mesh(indicatorGeo, indicatorMat);
        indicator.rotation.x = Math.PI / 2;
        indicator.position.y = 0.52;
        group.add(indicator);

        this.scene.add(group);
        mainObject = group;

        this.rotatingBlocks.push({
          mesh: group as any, // We rotate the group
          blockData: block,
          nodeIds: block.nodes.map(n => n.id)
        });
      } else {
        mesh.position.set(...block.position);
        mesh.userData.baseY = block.position[1];
        this.scene.add(mesh);
        console.log('Added block at position:', block.position, 'Color:', block.color);
        mainObject = mesh;
      }

      mesh.receiveShadow = true;
      mesh.castShadow = true;

      block.nodes.forEach(nodeData => {
        const worldPos = new THREE.Vector3(...block.position).add(new THREE.Vector3(...nodeData.offset));
        this.nodes.push({
          id: nodeData.id,
          position: worldPos,
          connections: nodeData.connections
        });

        // Visual for goal node
        if (nodeData.id === level.goalNodeId) {
          const goalGeo = new THREE.TorusGeometry(0.4, 0.05, 16, 100);
          const goalMat = new THREE.MeshStandardMaterial({ 
            color: '#f1c40f', 
            emissive: '#f1c40f', 
            emissiveIntensity: 2.5 
          });
          const goalMesh = new THREE.Mesh(goalGeo, goalMat);
          goalMesh.position.copy(worldPos);
          goalMesh.userData.baseY = worldPos.y;
          goalMesh.rotation.x = Math.PI / 2;
          goalMesh.name = 'goal_visual';
          this.scene.add(goalMesh);
          
          // Add a glow light to the goal
          const goalLight = new THREE.PointLight('#f1c40f', 5, 3);
          goalLight.position.copy(worldPos);
          this.scene.add(goalLight);
        }

        // Visual for start node
        if (nodeData.id === level.startNodeId) {
          const startGeo = new THREE.RingGeometry(0.3, 0.4, 32);
          const startMat = new THREE.MeshStandardMaterial({ color: '#ffffff', transparent: true, opacity: 0.5 });
          const startMesh = new THREE.Mesh(startGeo, startMat);
          startMesh.position.copy(worldPos);
          startMesh.position.y -= 0.45; // Just above the block surface
          startMesh.rotation.x = -Math.PI / 2;
          this.scene.add(startMesh);
        }
      });
    });

    this.currentNodeId = level.startNodeId;
    const startNode = this.nodes.find(n => n.id === this.currentNodeId);
    if (startNode) {
      this.player.position.copy(startNode.position);
    }
    console.log('Level loaded. Nodes created:', this.nodes.length, 'Scene children:', this.scene.children.length);
  }

  public rotateCamera(direction: number) {
    // direction: 1 for clockwise, -1 for counter-clockwise
    const radius = 34.64; // Distance from origin (sqrt(20^2 + 20^2 + 20^2))
    const currentAngle = Math.atan2(this.camera.position.z, this.camera.position.x);
    const newAngle = currentAngle + (Math.PI / 2) * direction;
    
    // Smooth rotation would be better, but let's start with instant
    this.camera.position.x = Math.cos(newAngle) * radius;
    this.camera.position.z = Math.sin(newAngle) * radius;
    this.camera.lookAt(0, 0, 0);
    this.camera.updateProjectionMatrix();
  }

  public handlePointerDown(event: MouseEvent | TouchEvent, container: HTMLElement) {
    const rect = container.getBoundingClientRect();
    const x = (event instanceof MouseEvent ? event.clientX : event.touches[0].clientX) - rect.left;
    const y = (event instanceof MouseEvent ? event.clientY : event.touches[0].clientY) - rect.top;
    
    const mouse = new THREE.Vector2(
      (x / container.clientWidth) * 2 - 1,
      -(y / container.clientHeight) * 2 + 1
    );

    const raycaster = new THREE.Raycaster();
    raycaster.setFromCamera(mouse, this.camera);

    // We want to detect which node was clicked. 
    // Since nodes are invisible, we can check distance to projected nodes or use invisible hitboxes.
    // For simplicity, let's find the closest node to the ray.
    let closestNode: LevelNode | null = null;
    let minDistance = Infinity;

    this.nodes.forEach(node => {
      const screenPos = node.position.clone().project(this.camera);
      const dist = mouse.distanceTo(new THREE.Vector2(screenPos.x, screenPos.y));
      if (dist < 0.1 && dist < minDistance) {
        minDistance = dist;
        closestNode = node;
      }
    });

    if (closestNode) {
      this.navigateTo(closestNode.id);
    }
  }

  private navigateTo(targetId: string) {
    if (this.currentNodeId === targetId) return;

    const path = this.findPath(this.currentNodeId, targetId);
    if (path.length > 0) {
      this.movePath = path;
      this.targetNodeId = targetId;
      this.soundManager.playMove();
    }
  }

  private findPath(startId: string, endId: string): LevelNode[] {
    // BFS to find shortest path
    const queue: { id: string; path: LevelNode[] }[] = [{ id: startId, path: [] }];
    const visited = new Set<string>();
    
    // Get illusion connections
    const illusionEdges = this.illusionSystem.getIllusionConnections(this.nodes);

    while (queue.length > 0) {
      const { id, path } = queue.shift()!;
      if (id === endId) return path;
      if (visited.has(id)) continue;
      visited.add(id);

      const node = this.nodes.find(n => n.id === id);
      if (!node) continue;

      // Real connections
      node.connections.forEach(connId => {
        const connNode = this.nodes.find(n => n.id === connId);
        if (connNode) {
          // Validate distance (should be ~1 unit apart, allow up to 1.5 for diagonals/stairs)
          if (node.position.distanceTo(connNode.position) < 1.5) {
            queue.push({ id: connId, path: [...path, connNode] });
          }
        }
      });

      // Illusion connections
      illusionEdges.forEach(([idA, idB]) => {
        if (idA === id) {
          const nodeB = this.nodes.find(n => n.id === idB);
          if (nodeB) queue.push({ id: idB, path: [...path, nodeB] });
        } else if (idB === id) {
          const nodeA = this.nodes.find(n => n.id === idA);
          if (nodeA) queue.push({ id: idA, path: [...path, nodeA] });
        }
      });
    }

    return [];
  }

  public update() {
    const time = performance.now() * 0.001;

    // Update rotating blocks
    this.rotatingBlocks.forEach(rb => {
      const cycle = 4;
      const phase = (time % cycle) / cycle;
      let angle = Math.floor(time / cycle) * (Math.PI / 2);
      
      if (phase > 0.75) {
        const t = (phase - 0.75) * 4;
        const smoothT = t * t * (3 - 2 * t);
        angle += smoothT * (Math.PI / 2);
      }

      rb.mesh.rotation.y = angle;

      // Update associated nodes
      rb.blockData.nodes.forEach(nodeData => {
        const node = this.nodes.find(n => n.id === nodeData.id);
        if (node) {
          const offset = new THREE.Vector3(...nodeData.offset);
          offset.applyAxisAngle(new THREE.Vector3(0, 1, 0), angle);
          node.position.copy(new THREE.Vector3(...rb.blockData.position).add(offset));
        }
      });
    });

    // Player idle animation
    if (this.movePath.length === 0) {
      const currentNode = this.nodes.find(n => n.id === this.currentNodeId);
      if (currentNode) {
        this.player.position.x = currentNode.position.x;
        this.player.position.z = currentNode.position.z;
        // Bobbing relative to the node's base position, matching the block's bobbing
        this.player.position.y = currentNode.position.y + Math.sin(time * 1.5 + currentNode.position.x * 0.5) * 0.05;
      }
      // Breathing scale effect
      const scale = 1 + Math.sin(time * 2) * 0.02;
      this.player.scale.set(scale, scale, scale);
      // Subtle rotation
      this.player.rotation.y = Math.sin(time * 0.5) * 0.1;
    } else {
      // Reset scale when moving
      this.player.scale.set(1, 1, 1);
      this.player.rotation.y = 0;
    }

    // Scene animations
    this.scene.traverse(child => {
      if (child instanceof THREE.Mesh) {
        // Subtle block floating
        if (child.userData.baseY !== undefined && child !== this.player && child.name !== 'particles' && child.name !== 'sky') {
          child.position.y = child.userData.baseY + Math.sin(time * 1.5 + child.position.x * 0.5) * 0.05;
        }
        
        // Animate goal visual
        if (child.name === 'goal_visual') {
          child.rotation.z += 0.02;
          if (child.userData.baseY !== undefined) {
            child.position.y = child.userData.baseY + Math.sin(time * 4) * 0.1;
          }
          // Pulsing glow
          const mat = child.material as THREE.MeshStandardMaterial;
          mat.emissiveIntensity = 0.5 + Math.sin(time * 3) * 0.3;
        }
      }

      // Animate plants
      if (child.name === 'plant') {
        // Gentle swaying using parent's position for spatial variation
        const seed = child.parent ? child.parent.position.x + child.parent.position.z : 0;
        child.rotation.z = Math.sin(time * 1.2 + seed) * 0.05;
        child.rotation.x = Math.cos(time * 1.0 + seed) * 0.03;
      }

      // Animate particles with more dynamic movement
      if (child.name === 'particles') {
        const positions = (child as THREE.Points).geometry.attributes.position.array as Float32Array;
        for (let i = 0; i < positions.length / 3; i++) {
          positions[i * 3 + 0] += Math.sin(time * 0.3 + i) * 0.002;
          positions[i * 3 + 1] += Math.sin(time * 0.5 + i) * 0.002;
          positions[i * 3 + 2] += Math.cos(time * 0.4 + i) * 0.002;
          
          // Wrap around if they go too high/far
          if (positions[i * 3 + 1] > 15) positions[i * 3 + 1] = -5;
        }
        (child as THREE.Points).geometry.attributes.position.needsUpdate = true;
      }
    });

    if (this.movePath.length > 0) {
      const nextNode = this.movePath[0];
      const dist = this.player.position.distanceTo(nextNode.position);

      if (dist < 0.05) {
        this.player.position.copy(nextNode.position);
        this.currentNodeId = nextNode.id;
        this.movePath.shift();
        
        if (this.movePath.length === 0 && this.currentNodeId === this.targetNodeId) {
          // Check win condition
          // (Handled in App.tsx)
        }
      } else {
        const dir = nextNode.position.clone().sub(this.player.position).normalize();
        this.player.position.add(dir.multiplyScalar(this.moveSpeed));
      }
    }

    // Update Trail
    this.trailPositions[this.trailIndex * 3] = this.player.position.x;
    this.trailPositions[this.trailIndex * 3 + 1] = this.player.position.y - 0.1;
    this.trailPositions[this.trailIndex * 3 + 2] = this.player.position.z;
    this.trailIndex = (this.trailIndex + 1) % (this.trailPositions.length / 3);
    this.playerTrail.geometry.attributes.position.needsUpdate = true;

    // Fade trail
    const trailOpacities = this.playerTrail.geometry.attributes.alpha.array as Float32Array;
    for (let i = 0; i < trailOpacities.length; i++) {
      const age = (this.trailIndex - i + trailOpacities.length) % trailOpacities.length;
      trailOpacities[i] = age / trailOpacities.length;
    }
    this.playerTrail.geometry.attributes.alpha.needsUpdate = true;

    this.composer.render();
  }

  public getCurrentNodeId() {
    return this.currentNodeId;
  }
}
